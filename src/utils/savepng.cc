#include "savepng.hh"

#include <png.h>

// ============================================================================

int savePNG (const std::string& a_FileName, size_t a_Width, size_t a_Height, const uint8_t* a_Data, bool a_Flip) {

    // Create row pointers
    auto rowPointers = std::unique_ptr<png_bytep>(new png_bytep[a_Height]);

    if (a_Flip) {
        for (size_t y=0; y<a_Height; ++y) {
            size_t v = a_Height - 1 - y;
            rowPointers.get()[y] = (png_bytep)&a_Data[v*a_Width*4];
        }
    }
    else {
        for (size_t y=0; y<a_Height; ++y) {
            rowPointers.get()[y] = (png_bytep)&a_Data[y*a_Width*4];
        }
    }

    // Open the file
    FILE* fp = fopen(a_FileName.c_str(), "wb");
    if (!fp) {
        return -1;
    }

    // Initialize PNG
    png_structp png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop   info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);

    //png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    png_set_IHDR(png_ptr, info_ptr, a_Width, a_Height,
                 8, PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    // Write data
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, rowPointers.get());
    png_write_end(png_ptr, NULL);

    // Close the file
    fclose(fp);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}
