#ifndef  SCIVIS_INC
#define  SCIVIS_INC

#include "Datasets/SubDataset.h"
#include "Graphics/GameObject.h"
#include "Color.h"
#include "ColorMode.h"

namespace sereno
{
    /** \brief  SciVis interface, proposing an interface for the SciVis Dataset to draw */
    class SciVis : public GameObject
    {
        public:
            /**
             * \brief  Constructor.
             * \param parent The parent GameObject.
             * \param renderer the GLRenderer containing the OpenGL Context.
             * \param mtl the material to use.
             * \paremt model the model to use.*/
            SciVis(GameObject* parent, GLRenderer* renderer, Material* mtl, SubDataset* model) :
                GameObject(parent, renderer, mtl), m_model(model) {}

            /** \brief  Virtual destructor */
            virtual ~SciVis() {};

            /* \brief  Get the SubDataset model in use
             * \return  The SubDataset model */
            SubDataset* getModel() {return m_model;}

            /** \brief  Load the visualization once the data has been correctly loaded*/
            virtual void load() {};

            /* \brief Function to call when the transfer function attached to the model has changed.
             * If no TF is used, it depends on the sub class and surely on the shader in use*/
            virtual void onTFChanged() = 0;
        protected:
            SubDataset* m_model = NULL; /*!< The model bound to this visualization*/
    };
}

#endif
