#ifndef  TRIANGULARGTF_INC
#define  TRIANGULARGTF_INC


#include "Graphics/SciVis/TransfertFunction/TransfertFunction.h"
#include "Graphics/SciVis/TransfertFunction/TFColor.h"
#include "Graphics/SciVis/SciVisColor.h"
#include <algorithm>


namespace sereno
{
    /** \brief  Triangular Gaussian Transfert Function. The
     *
     * @tparam Dim
     * @tparam Mode */
    template<uint8_t Dim, ColorMode Mode>
    class TriangularGTF : public TF<Dim>, public TFColor<Mode>
    {
        static_assert(Dim >= 2, "Needs at least two dimensions (the last one is for the gradient)");
        public:
            /** \brief  Constructor. Set scale at 1.0f, center at 0.0 and alphaMax at 1.0 */
            TriangularGTF() : TF<Dim>(), m_alphaMax(1.0f)
            {
                for(uint8_t i = 0; i < Dim; i++)
                {
                    m_scale[i]  = 0.5f;
                    m_center[i] = 0.5f;
                }
            }

            /**
             * \brief  Compute the alpha component of the transfer function. The last value "ind" if for the gradient magnitude
             * The algorithm comes from "Gaussian Transfert Function for Multiple-Field Volume Visualization" by Kniss et al. in 2003 (IEEE Vis)
             *
             * \param ind the current Dim indice (i, j, k, ...). The last one is the gradient
             *
             * \return   the alpha computed
             */
            uint8_t computeAlpha(float* ind) const
            {
                if(ind[Dim-1] == 0)
                    return 0;

                float r0 = 1.f/ind[Dim-1];
                float r1[Dim-1] = {0};
                float r1Mag = 0;

                for(uint32_t i = 0; i < Dim-1; i++)
                {
                    r1[i] = r0*m_scale[i]*(ind[i] - m_center[i]);
                    r1Mag += r1[i]*r1[i];
                }

                return std::min(m_alphaMax*exp(-r1Mag)*255, 255.0f);
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
             * \brief  Set the scaling along each axis of the TriangularGTF
             * \param scale the scaling along each axis of the TriangularGTF
             */
            void setScale(float* scale) {for(uint8_t i = 0; i < Dim; i++) m_scale[i] = scale[i];}
            /**
             * \brief  Set the center of the TriangularGTF
             * \param center the center of the TriangularGTF
             */
            void setCenter(float* center) {for(uint8_t i = 0; i < Dim; i++) m_center[i] = center[i];}
            /**
             * \brief  Set the alpha max of the TriangularGTF
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
