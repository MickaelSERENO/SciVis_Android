#ifndef  GTF_INC
#define  GTF_INC

#include "Graphics/SciVis/TransfertFunction/TransfertFunction.h"
#include "Graphics/SciVis/SciVisColor.h"

template<uint8_t Dim, ColorMode Mode>
class GTF : public TF<Dim>
{
    public:
        /**
         * \brief  Compute the alpha component of the transfer function
         *
         * \param minVals the minimum values to consider (outside == transparent)
         * \param maxVals the maximum values to consider (outsize == transparent)
         * \param ind the current Dim indice (i, j, k, ...)
         *
         * \return   the alpha computed
         */
        uint8_t computeAlpha(float* minVals, float* maxVals, float* ind)
        {
            float r[Dim] = {0};
            for(uint32_t i = 0; i < Dim; i++)
                r[i] = (ind[i] - minVals[i])/(maxVals[i]-minVals[i]);

            float rMag = 0;
            for(uint32_t i = 0; i < Dim; i++)
                rMag += r[i]*r[i];

           return exp(-r)*255;
        }

        /**
         * \brief  Compute the HSV GTF color. The color is only based on the first dimension value
         *
         * \param minVals the minimum values to consider (outside == transparent)
         * \param maxVals the maximum values to consider (outsize == transparent)
         * \param ind the current Dim indice (i, j, k, ...)
         * \param outCol[output] the rgb 8 bits color.*/
        void computeColor(float* minVals, float* maxVals, float* ind, uint8_t* outCol)
        {
            Color c = SciVis_computeColor(Mode, ind[0]);
            for(uint32_t i = 0; i < 3; i++)
                outCol[i] = c[i];
        }
};

#endif
