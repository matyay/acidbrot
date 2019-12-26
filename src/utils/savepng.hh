#ifndef SAVEPNG_HH
#define SAVEPNG_HH

#include <string>
#include <memory>

#include <cstddef>
#include <cstdint>

// ============================================================================

/// Saves a data buffer to PNG
int savePNG (const std::string& a_FileName, size_t a_Width, size_t a_Height, const uint8_t* a_Data, bool a_Flip = false);

#endif // SAVEPNG_HH
