#include "filter_mask.hh"

#include <sys/types.h>

#include <cstdint>
#include <cmath>
#include <cstring>

// ============================================================================

FilterMask::FilterMask (size_t nx, size_t ny) :
    m_Width (nx),
    m_Height(ny)
{
    // Allocate data
    m_Weights.resize(nx*ny);
    m_Offsets.resize(nx*ny);

    // Clear it
    memset(m_Weights.data(), 0, nx*ny*sizeof(float));
    memset(m_Offsets.data(), 0, nx*ny*sizeof(Ofs));
}

// ============================================================================

void FilterMask::normalizeWeights () {
    float sum = 0.0f;

    for (size_t i=0; i<getCount(); ++i) {
        sum += fabs(m_Weights[i]);
    }

    for (size_t i=0; i<getCount(); ++i) {
        m_Weights[i] /= sum;
    }
}

void FilterMask::computeOffsets (size_t a_Width, size_t a_Height) {

    for (size_t j=0; j<m_Height; ++j) {
        ssize_t jj = j - m_Height / 2;
        for (size_t i=0; i<m_Width; ++i) {
            ssize_t ii  = i - m_Width / 2;
            size_t  idx = j * m_Width + i;

            m_Offsets[idx].x = (float)ii / (float)a_Width;
            m_Offsets[idx].y = (float)jj / (float)a_Height;
        }
    }
}

// ============================================================================

float& FilterMask::w (size_t i, size_t j) {
    return m_Weights[j * m_Width + i];
}

size_t FilterMask::getCount () const {
    return m_Width * m_Height;
}

const float* FilterMask::getWeights () {
    return m_Weights.data();
}

const float* FilterMask::getOffsets () {
    return (float*)m_Offsets.data();
}

