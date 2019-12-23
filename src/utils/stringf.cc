#include "stringf.hh"

#include <cstddef>
#include <cstdarg>
#include <memory>

///////////////////////////////////////////////////////////////////////////////

std::string stringf (const std::string a_Format, ...) {

    ssize_t bufSize = 120+1;
    
    va_list args;
    va_list org_args;

    // Try formatting the string
    va_start(args, a_Format);
    va_copy(org_args, args);
    std::unique_ptr<char[]> buffer(new char[bufSize]);
    int res = vsnprintf(buffer.get(), bufSize, a_Format.c_str(), args);
    va_end(args);

    // The buffer was too small. Re-allocate it and try again.
    if (res >= bufSize) {
        bufSize = res + 1;
        buffer.reset(new char[bufSize]);
        res = vsnprintf(buffer.get(), bufSize, a_Format.c_str(), org_args);
    }

    // Error
    if (res < 0) {
        return std::string();
    }

    // Convert to std::string
    return std::string(buffer.get());
}

