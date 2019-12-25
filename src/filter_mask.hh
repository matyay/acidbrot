#ifndef FILTER_MASK_HH
#define FILTER_MASK_HH

#include <cstddef>
#include <cstdint>

#include <vector>

// ============================================================================

class FilterMask
{
public:

    /// Sample offset type
    struct Ofs {
        float x;
        float y;
    };

    /// Constructor
    FilterMask (size_t nx, size_t ny);

    /// Weight access
    float& w (size_t i, size_t j);

    /// Normalizes all weights
    void normalizeWeights ();
    /// Computes offsets for given texture size
    void computeOffsets (size_t a_Width, size_t a_Height);

    /// Returns weight and offset count
    size_t       getCount   () const;
    /// Returns the weights vector
    const float* getWeights ();
    /// Returns the offsets vector
    const float* getOffsets ();

protected:

    /// Dimensions
    size_t  m_Width;
    size_t  m_Height;

    /// Weights
    std::vector<float> m_Weights;
    /// Offsets
    std::vector<Ofs>   m_Offsets;
};

#endif // FILTER_MASK_HH
