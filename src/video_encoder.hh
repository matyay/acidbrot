#ifndef VIDEO_ENCODER_HH
#define VIDEO_ENCODER_HH

#include <spdlog/spdlog.h>
#include "utils/param_dict.hh"

#include <x264.h>

#include <cstddef>
#include <cstdint>

#include <vector>
#include <queue>
#include <thread>
#include <atomic>

// ============================================================================

/// Video encoder class that uses libx264
class VideoEncoder
{
public:

    /// Buffer type
    typedef std::vector<uint8_t> Buffer;

    /// Constructor / destructor
     VideoEncoder (size_t a_Width, size_t a_Height, const ParamDict& a_Params);
    ~VideoEncoder ();

    // ................................

    /// Passes an image data to the encoder
    int  encode  (const Buffer& a_ImageData);
    /// Retrieves a block of encoded data
    bool getData (Buffer& a_Buffer);

    /// Flushes the encoder
    void flush ();

    /// Returns true while flushing
    bool isFlushing ();
    /// Returns true when the flushing is finished
    bool isFinished ();

protected:

    /// Logger
    std::shared_ptr<spdlog::logger> m_Logger;

    /// Resolution
    size_t m_Width;
    size_t m_Height;

    /// Encoder parameters
    x264_param_t   m_Params;
    /// The encoder
    x264_t*        m_x264 = nullptr;

    /// Input picture
    x264_picture_t m_InpPic;
    /// Output picture
    x264_picture_t m_OutPic;
    /// Encoded NAL
    x264_nal_t*    m_Nal = nullptr;

    /// Picture PTS
    size_t m_Pts = 0;

    /// The worker thread
    std::thread m_Worker;

    /// Input queue
    std::queue<Buffer> m_InpQueue;
    /// Output queue
    std::queue<Buffer> m_OutQueue;

    /// Flushing in progress flag
    std::atomic_bool m_Flushing;
    /// Encoding finished flag
    std::atomic_bool m_Finished;

    // ................................
    
    /// Worker thread proc
    void workerProc ();
};

#endif // VIDEO_ENCODER_HH
