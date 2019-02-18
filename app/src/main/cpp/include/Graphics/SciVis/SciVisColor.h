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
    inline Color SciVis_computeColor(ColorMode mode, float t)
    {
        Color c;
        switch(mode)
        {
            case RAINBOW:
            {
                HSVColor hsvColor(260.0*(1.0f-t), 1.0f, 1.0f, 1.0f);
                c = hsvColor.toRGB();
                break;
            }
            case GRAYSCALE:
            {
                c = Color(t, t, t, 1.0f);
                break;
            }
            case WARM_COLD_CIELUV:
            {
                if(t < 0.5f)
                {
                    LUVColor luv = LUVColor::COLD_COLOR*(1.0f-2.0f*t) + LUVColor::WHITE*2.0f*t;
                    c = luv.toRGB();
                }
                else
                {
                    LUVColor luv = LUVColor::WHITE*(2.0f-2.0f*t) + LUVColor::WARM_COLOR*(2.0f*t-1.0f);
                    c = luv.toRGB();
                }
                break;
            }
            case WARM_COLD_CIELAB:
            {
                if(t < 0.5f)
                {
                    LABColor lab = LABColor::COLD_COLOR*(1.0-2.0*t) + LABColor::WHITE*2.0*t;
                    c = lab.toRGB();
                }
                else
                {
                    LABColor lab = LABColor::WHITE*(2.0f-2.0f*t) + LABColor::WARM_COLOR*(2.0f*t-1.0f);
                    c = lab.toRGB();
                }
                break;
            }
            case WARM_COLD_MSH:
            {
                c = MSHColor::fromColorInterpolation(Color::COLD_COLOR, Color::WARM_COLOR, t).toRGB();
                break;
            }
        }

        return c;
    }
}

#endif
