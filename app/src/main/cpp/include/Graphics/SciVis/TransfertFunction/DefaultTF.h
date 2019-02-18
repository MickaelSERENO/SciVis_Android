#ifndef  DEFAULTTF_INC
#define  DEFAULTTF_INC

#include "Graphics/SciVis/SciVisColor.h"
#include "Graphics/SciVis/TransfertFunction/TFColor.h"
#include <cstdint>

namespace sereno
{
    template<uint8_t Dim, ColorMode Mode>
    class DefaultTF : public TF<Dim>, public TFColor<Mode>
    {
        public:
            /**
             * \brief  Compute the alpha component. Returns 1.0f (default alpha)
             * \param ind the current Dim indice (i, j, k, ...)
             * \return 255
             */
            uint8_t computeAlpha(float* ind) const
            {
                return 255;
            }
    };
}

#endif
