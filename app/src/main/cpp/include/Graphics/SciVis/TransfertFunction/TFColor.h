#ifndef  TFCOLOR_INC
#define  TFCOLOR_INC

#include "SciVisColor.h"
#include <cstdint>

namespace sereno
{
    template<ColorMode Mode>
    class TFColor
    {
        public:
            /**
             * \brief  Compute the SciVis color. The color is only based on the first dimension value
             *
             * \param ind the current Dim indice (i, j, k, ...)
             * \param outCol[output] the rgb 8 bits color.*/
            void computeColor(float* ind, uint8_t* outCol) const
            {
                Color c = SciVis_computeColor(Mode, ind[0]);
                for(uint32_t i = 0; i < 3; i++)
                    outCol[i] = std::min(255.0f, 255.0f*c[i]);
            }
    };
}

#endif
