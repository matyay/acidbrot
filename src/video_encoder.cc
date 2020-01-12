#include "video_encoder.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <stdexcept>

// ============================================================================

VideoEncoder::VideoEncoder (size_t a_Width, size_t a_Height, const ParamDict& a_Params) :
    m_Width (a_Width),
    m_Height(a_Height)
{

    // Create the logger
    m_Logger = spdlog::get("encoder");
    if (!m_Logger) {
        m_Logger = spdlog::stderr_color_mt("encoder");
    }

    // Setup the encoder parameters
    int res = x264_param_default_preset(&m_Params, "medium", nullptr);
    if (res < 0) {
        throw std::runtime_error("Error initializing x264 encoder parameters");
    }

    // FIXME: Use a_Params !!

    m_Params.i_threads      = 2;
    m_Params.i_log_level    = X264_LOG_NONE;

    m_Params.i_csp          = X264_CSP_I444;
    m_Params.i_width        = m_Width;
    m_Params.i_height       = m_Height;
    m_Params.i_fps_num      = 30;
    m_Params.i_fps_den      = 1;

    m_Params.i_keyint_max       = 15;
    m_Params.b_intra_refresh    = 1;

    m_Params.rc.i_rc_method     = X264_RC_CQP;
    m_Params.rc.i_qp_constant   = 30; // FIXME: make a parameter

    m_Params.b_vfr_input        = 0;
    m_Params.b_repeat_headers   = 1;
    m_Params.b_annexb           = 1;
    m_Params.b_aud              = 1;

    // Apply profile restrictions
    res = x264_param_apply_profile(&m_Params, "high444");
    if (res < 0) {
        throw std::runtime_error("x264_param_apply_profile() Failed!");
    }

    // Allocate the input buffer
    res = x264_picture_alloc(&m_InpPic, m_Params.i_csp, m_Params.i_width, m_Params.i_height);
    if (res < 0) {
        throw std::runtime_error("x264_picture_alloc() Failed!");
    }

    // Open the encoder
    m_x264 = x264_encoder_open(&m_Params);
    if (m_x264 == nullptr) {
        throw std::runtime_error("x264_encoder_open() Failed!");
    }

    m_Logger->info("Video encoder initialized");

    // Start the worker thread
    m_Flushing = false;
    m_Finished = false;
    m_Worker   = std::thread([this] { this->workerProc(); });
}

VideoEncoder::~VideoEncoder() {

    // Terminate the worker thread
    m_Finished = true;

    if (m_Worker.joinable()) {
        m_Worker.join();
    }

    // Close the encoder
    if (m_x264) {
        x264_encoder_close(m_x264);
    }

    x264_picture_clean(&m_InpPic);
}

// ============================================================================

int VideoEncoder::encode (const Buffer& a_ImageData) {

    // The encoder is flushing
    if (m_Flushing) {
        return -1;
    }

    // Put the copy of the data buffer into the queue
    m_InpQueue.push(Buffer(a_ImageData));

    return 0;
}

bool VideoEncoder::getData (VideoEncoder::Buffer& a_Buffer) {

    // No data
    if (m_OutQueue.empty()) {
        return false;
    }

    // Get data
    a_Buffer = m_OutQueue.front();
    m_OutQueue.pop();

    return true;
}

void VideoEncoder::flush () {
    m_Logger->debug("Flushing began.");
    m_Flushing = true;
}

bool VideoEncoder::isFlushing () {
    return m_Flushing;
}

bool VideoEncoder::isFinished () {
    return m_Finished;
}

// ============================================================================

void VideoEncoder::workerProc () {

    // Loop until finished
    while (!m_Finished) {

        int nal = 0;
        int res = 0;

        // Get a regular frame
        if (!m_Flushing) {

            // Input queue empty
            if (m_InpQueue.empty()) {
                usleep(100); // FIXME: make platform agnostic
                continue;
            }

            // Get a picture from the queue.
            const Buffer& buffer = m_InpQueue.front();

            // Copy data to the x264 picture buffer
            const size_t lumaSize   = m_Width * m_Height;
            const size_t chromaSize = lumaSize;//lumaSize >> 2;
            const size_t frameSize  = lumaSize + (chromaSize << 1);

            // Incorrect buffer size
            if (buffer.size() != frameSize) {
                m_Logger->error("Incorrect buffer size! ({} instead of {})",
                        buffer.size(), frameSize);

                m_InpQueue.pop();
                continue;
            }

            // Copy data
            uint8_t const* ptr = (uint8_t*)buffer.data();
            memcpy(m_InpPic.img.plane[0], ptr, lumaSize);   ptr += lumaSize;
            memcpy(m_InpPic.img.plane[1], ptr, chromaSize); ptr += chromaSize;
            memcpy(m_InpPic.img.plane[2], ptr, chromaSize); ptr += chromaSize;

            // Pop the picture
            m_InpQueue.pop();

            // Encode the picture
            m_InpPic.i_pts = m_Pts;
            m_Pts++;

            res = x264_encoder_encode(m_x264, &m_Nal, &nal, &m_InpPic, &m_OutPic);            
            if (res < 0) {
                m_Logger->error("x264_encoder_encode() Failed!");
                continue;
            }
        }
        // Flush frames
        else {

            // Flush a frame
            res = x264_encoder_delayed_frames(m_x264);
            if (!res) {
                m_Flushing = false;
                m_Finished = true;
                m_Logger->debug("Flushing finished.");
                break;
            }

            res = x264_encoder_encode(m_x264, &m_Nal, &nal, nullptr, &m_OutPic);            
            if (res < 0) {
                m_Logger->error("x264_encoder_encode() Failed!");
                continue;
            }
        }

        // If there is data then put it into the output queue
        if (res > 0) {
            Buffer nalBuffer(res);
            memcpy(nalBuffer.data(), m_Nal->p_payload, res);

            m_OutQueue.push(nalBuffer);
        }
    }
}

