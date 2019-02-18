#ifndef  SCIVIS_INC
#define  SCIVIS_INC

#include "Datasets/SubDataset.h"
#include "Graphics/GameObject.h"
#include "Graphics/Color.h"
#include "ColorMode.h"

namespace sereno
{
    /** \brief  SciVis interface, proposing an interface for the SciVis Dataset to draw */
    class SciVis : public GameObject
    {
        public:
            /**
             * \brief  Constructor
             * \param parent The parent GameObject 
             * \param renderer the GLRenderer containing the OpenGL Context
             * \param mtl the material to use
             * \paremt model the model to use
             * \param tfTexture the transfert function texture to apply
             * \param tfTextureDim the transfert function texture dimension*/
            SciVis(GameObject* parent, GLRenderer* renderer, Material* mtl, SubDataset* model, GLuint tfTexture, uint8_t tfTextureDim) : 
                GameObject(parent, renderer, mtl), m_model(model), m_tfTexture(tfTexture), m_tfTextureDim(tfTextureDim) {}

            /** \brief  Virtual destructor */
            virtual ~SciVis() {};

            /* \brief Set the color range to display. All the color OUTSIDE [min, max] will be discarded (i.e transparent)
             * \param min the minimum value (ratio : 0.0, 1.0)
             * \param max the maximum value (ratio : 0.0, 1.0)*/
            virtual void setColorRange(float min, float max, ColorMode colorMode) = 0;

            /**
             * \brief  Get the SubDataset model in use
             * \return  The SubDataset model 
             */
            SubDataset* getModel() {return m_model;}
        protected:
            SubDataset* m_model = NULL; /*!< The model bound to this visualization*/
            GLuint      m_tfTexture;
            uint8_t     m_tfTextureDim;
    };
}

#endif
