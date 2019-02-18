#ifndef  GTF_INC
#define  GTF_INC

#include "Graphics/SciVis/TransfertFunction/TransfertFunction.h"
#include "Graphics/SciVis/TransfertFunction/TFColor.h"
#include "Graphics/SciVis/SciVisColor.h"
#include <algorithm>

namespace sereno
{
    /** \brief  The Gaussian Transfer Function
     *
     * @tparam Dim The dimension of the data
     * @tparam Mode The color mode to apply*/
    template<uint8_t Dim, ColorMode Mode>
    class GTF : public TF<Dim>, public TFColor<Mode>
    {
        public:
            /** \brief  Constructor. Set scale at 1.0f, center at 0.0 and alphaMax at 1.0 */
            GTF() : TF<Dim>(), m_alphaMax(1.0f)
            {
                for(uint8_t i = 0; i < Dim; i++)
                {
                    m_scale[i] = 1.0f;
                    m_center[i] = 0.0f;
                }
            }

            /**
             * \brief  Compute the alpha component of the transfer function
             * The algorithm comes from "Gaussian Transfert Function for Multiple-Field Volume Visualization" by Kniss et al. in 2003 (IEEE Vis)
             *
             * \param ind the current Dim indice (i, j, k, ...)
             *
             * \return   the alpha computed
             */
            uint8_t computeAlpha(float* ind) const
            {
                float r[Dim] = {0};
                float rMag = 0;
                for(uint32_t i = 0; i < Dim; i++)
                {
                    r[i] = m_scale[i]*(ind[i] - m_center[i]);
                    rMag += r[i]*r[i];
                }

                return std::min(m_alphaMax*exp(-r)*255, 255.0f);
            }

            /**
             * \brief  Get the scale applied
             * \return  the scale 
             */
            const float* getScale()  const {return m_scale;}

            /**
             * \brief  Get the center applied
             * \return  the center 
             */
            const float* getCenter() const {return m_center;}

            /**
             * \brief  Get the alpha max applied
             * \return  the alpha max 
             */
            const float getAlphaMax() const {return m_alphaMax;}

            /**
             * \brief  Set the scaling along each axis of the GTF
             * \param scale the scaling along each axis of the GTF
             */
            void setScale(float* scale) {for(uint8_t i = 0; i < Dim; i++) m_scale[i] = scale[i];}
            /**
             * \brief  Set the center of the GTF
             * \param center the center of the GTF
             */
            void setCenter(float* center) {for(uint8_t i = 0; i < Dim; i++) m_center[i] = center[i];}
            /**
             * \brief  Set the alpha max of the GTF
             * \param alphaMax the alpha max
             */
            void setAlphaMax(float alphaMax) {m_alphaMax = alphaMax;}

        private:
            float m_scale[Dim];  /*!< The scaling factor of the GTF*/
            float m_center[Dim]; /*!< The center of the GTF*/
            float m_alphaMax;    /*!< The alpha max of the GTF*/
    };
}

#endif
