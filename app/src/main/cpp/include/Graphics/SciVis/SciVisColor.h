#ifndef  SCIVISCOLOR_INC
#define  SCIVISCOLOR_INC

#include "ColorMode.h"
#include "Graphics/Color.h"

namespace sereno
{

    /**
     * \brief  Compute the color from the color mode and the advancement t
     *
     * \param mode the color mode
     * \param t the advancement (between 0.0 and 1.0)
     *
     * \return   the desired color in RGB space
     */
    Color SciVis_computeColor(ColorMode mode, float t);
}

#endif
