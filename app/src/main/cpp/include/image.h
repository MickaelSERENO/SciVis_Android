#ifndef  IMAGE_INC
#define  IMAGE_INC

#include <cstdint>

namespace sereno
{
    /** \brief get the PNG bytes array in a RGBA, 8bits for each component, format from a file. The array needs to be free afterward
     * \param filePath[in] the file to read
     * \param width[out] the width value
     * \param height[out] the height value
     * \return the bytes array in a RGBA 8bits format. NULL if it could not have been read */
    uint8_t* getPNGRGBABytesFromFiles(const char* filePath, uint32_t* width, uint32_t* height);
}

#endif
