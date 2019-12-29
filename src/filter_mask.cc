#include "filter_mask.hh"

#include <sys/types.h>

#include <cstdint>
#include <cmath>
#include <cstring>

// ============================================================================

FilterMask::FilterMask (size_t nx, size_t ny, size_t nc) :
    m_Width   (nx),
    m_Height  (ny),
    m_Channels(nc)
{
    // Allocate data
    m_Weights.resize(nx * ny * nc);
    m_Offsets.resize(nx * ny);

    // Clear it
    memset(m_Weights.data(), 0, ny * nx * nc * sizeof(float));
    memset(m_Offsets.data(), 0, ny * nx *      sizeof(Ofs));
}

// ============================================================================

void FilterMask::setWeights (const std::vector<float>& a_Weights, size_t c) {
    size_t N = m_Width * m_Height;

    // Size mismatch
    if (a_Weights.size() < N) {
        return;
    }

    // Store weights
    for (size_t i=0; i<N; ++i) {
        m_Weights[i * m_Channels + c] = a_Weights[i];
    }

    m_Dirty = true;
}

void FilterMask::normalizeWeights () {
    size_t N = m_Width * m_Height;

    // Normalize each channel independently
    for (size_t c=0; c<m_Channels; ++c) {
        float  sum = 0.0f;

        // Compute sum
        for (size_t i=0; i<N; ++i) {
            sum += fabs(m_Weights[i * m_Channels + c]);
        }

        // Normalize
        if (sum == 0.0f) {
            for (size_t i=0; i<N; ++i) {
                m_Weights[i * m_Channels + c]  = 0.0f;
            }
        }
        else {
            for (size_t i=0; i<N; ++i) {
                m_Weights[i * m_Channels + c] /= sum;
            }
        }
    }

    m_Dirty = true;
}

void FilterMask::computeOffsets (size_t a_Width, size_t a_Height) {

    // Compute sample offsets in texture coordinates given its size.
    for (size_t j=0; j<m_Height; ++j) {
        ssize_t jj = j - m_Height / 2;
        for (size_t i=0; i<m_Width; ++i) {
            ssize_t ii  = i - m_Width / 2;
            size_t  idx = j * m_Width + i;

            m_Offsets[idx].x = ((float)ii + 0.5f) / (float)a_Width;
            m_Offsets[idx].y = ((float)jj + 0.5f) / (float)a_Height;
        }
    }

    m_Dirty = true;
}

// ============================================================================

void FilterMask::prepareDataForShader () {
    size_t N = m_Width * m_Height;

    m_WeightsForShader.clear();
    m_OffsetsForShader.clear();

    std::vector<float> weightVec(m_Channels);

    // Process all spatial offsets
    for (size_t i=0; i<N; ++i) {
        bool useWeight = false;

        // Collect weights among all channels for the given spatial location
        for (size_t c=0; c<m_Channels; ++c) {
            float w = m_Weights[i * m_Channels + c];
            weightVec[c] = w;

            if (w != 0.0f) {
                useWeight = true;
            }
        }

        // Do not use this weights. Its 0 for all channels
        if (!useWeight) {
            continue;
        }

        // Store data
        m_WeightsForShader.insert(m_WeightsForShader.end(),
            weightVec.begin(), weightVec.end());

        // Store offset
        m_OffsetsForShader.push_back(m_Offsets[i]);
    }

    m_Dirty = false;
}

size_t FilterMask::getCountForShader () {

    if (m_Dirty) {
        prepareDataForShader();
    }

    return m_OffsetsForShader.size();
}

const float* FilterMask::getWeightsForShader () {

    if (m_Dirty) {
        prepareDataForShader();
    }

    return m_WeightsForShader.data();
}

const float* FilterMask::getOffsetsForShader () {

    if (m_Dirty) {
        prepareDataForShader();
    }

    return (float*)m_OffsetsForShader.data();
}

