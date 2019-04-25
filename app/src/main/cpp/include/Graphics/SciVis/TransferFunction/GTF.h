#ifndef  GTF_INC
#define  GTF_INC

#include "Graphics/SciVis/TransferFunction/TransferFunction.h"
#include "SciVisColor.h"
#include <algorithm>

namespace sereno
{
    /** \brief  The Gaussian Transfer Function*/
    class GTF : public TF
    {
        public:
            /** \brief  Constructor. Set scale at 1.0f, center at 0.0 and alphaMax at 1.0 */
            GTF(uint32_t dim, ColorMode mode) : TF(dim, mode), m_alphaMax(1.0f)
            {
                m_scale  = (float*)malloc(sizeof(float)*dim);
                m_center = (float*)malloc(sizeof(float)*dim);
                for(uint32_t i = 0; i < dim; i++)
                {
                    m_scale[i]  = 0.5f;
                    m_center[i] = 0.5f;
                }
            }

            GTF(const GTF& copy) : TF(copy)
            {
                *this = copy;
            }

            GTF& operator=(const GTF& copy)
            {
                TF::operator=(copy);
                if(&copy != this)
                {
                    if(m_scale)
                        free(m_scale);
                    if(m_center)
                        free(m_center);
                    m_scale  = (float*)malloc(sizeof(float)*m_dim);    
                    m_center = (float*)malloc(sizeof(float)*m_dim);    

                    for(uint32_t i = 0; i < m_dim; i++)
                    {
                        m_scale[i]  = copy.m_scale[i];
                        m_center[i] = copy.m_center[i];
                    }
                }
                return *this;
            }

            ~GTF()
            {
                if(m_scale)
                    free(m_scale);
                if(m_center)
                    free(m_center);
            }

            /* \brief  Compute the alpha component of the transfer function
             * The algorithm comes from "Gaussian Transfer Function for Multiple-Field Volume Visualization" by Kniss et al. in 2003 (IEEE Vis)
             *
             * \param ind the current Dim indice (i, j, k, ...)
             *
             * \return   the alpha computed */
            uint8_t computeAlpha(float* ind) const
            {
                float rMag = 0;
                for(uint32_t i = 0; i < m_dim; i++)
                {
                    float r = m_scale[i]*(ind[i] - m_center[i]);
                    rMag += r*r;
                }

                return std::min(m_alphaMax*exp(-rMag)*255, 255.0f);
            }

            /* \brief  Get the scale applied
             * \return  the scale */
            const float* getScale()  const {return m_scale;}

            /* \brief  Get the center applied
             * \return  the center */
            const float* getCenter() const {return m_center;}

            /* \brief  Get the alpha max applied
             * \return  the alpha max */
            const float getAlphaMax() const {return m_alphaMax;}

            /* \brief  Set the scaling along each axis of the GTF
             * \param scale the scaling along each axis of the GTF */
            void setScale(float* scale) {for(uint32_t i = 0; i < m_dim; i++) m_scale[i] = scale[i];}

            /* \brief  Set the center of the GTF
             * \param center the center of the GTF */
            void setCenter(float* center) {for(uint32_t i = 0; i < m_dim; i++) m_center[i] = center[i];}

            /* \brief  Set the alpha max of the GTF
             * \param alphaMax the alpha max */
            void setAlphaMax(float alphaMax) {m_alphaMax = alphaMax;}

        private:
            float* m_scale    = NULL; /*!< The scaling factor of the GTF*/
            float* m_center   = NULL; /*!< The center of the GTF*/
            float  m_alphaMax;        /*!< The alpha max of the GTF*/
    };
}

#endif
