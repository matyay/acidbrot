#ifndef FILTER_MASK_HH
#define FILTER_MASK_HH

#include <cstddef>
#include <cstdint>

#include <vector>

// ============================================================================

class FilterMask
{
public:

    /// Constructor
    FilterMask (size_t nx, size_t ny, size_t nc = 1);

    /// Set all weights for the given channel
    void setWeights (const std::vector<float>& a_Weights, size_t c = 0);
    /// Normalizes all weights
    void normalizeWeights ();

    /// Computes offsets for given texture size
    void computeOffsets (size_t a_Width, size_t a_Height);

    /// Returns weight and offset count for shader
    size_t       getCountForShader   ();
    /// Returns the weights vector for shader
    const float* getWeightsForShader ();
    /// Returns the offsets vector for shader
    const float* getOffsetsForShader ();

protected:

    /// Sample offset type
    struct Ofs {
        float x;
        float y;
    };

    /// Dimensions
    size_t  m_Width;
    size_t  m_Height;
    /// Channel count
    size_t  m_Channels;

    /// Dirty flag
    bool    m_Dirty = true;

    /// Weights
    std::vector<float>  m_Weights;
    /// Offsets
    std::vector<Ofs>    m_Offsets;

    /// Weights for shader
    std::vector<float>  m_WeightsForShader;
    /// Offsets for shader
    std::vector<Ofs>    m_OffsetsForShader;

    /// Prepares weights for the shader by discarding weights of value 0.0 for
    /// all channels
    void prepareDataForShader ();
};

#endif // FILTER_MASK_HH
