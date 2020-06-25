#ifndef  CLOUDPOINTGAMEOBJECT_INC
#define  CLOUDPOINTGAMEOBJECT_INC

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <mutex>
#include <thread>
#include "Graphics/SciVis/SciVis.h"
#include "Graphics/GLRenderer.h"
#include "Graphics/Materials/Material.h"
#include "Datasets/CloudPointDataset.h"

namespace sereno
{
    /** \brief  GameObject permitting to display cloud point datasets */
    class CloudPointGameObject : public SciVis
    {
        public:
            /** \brief  Constructor, initialize the VBO with enough space
             * \param renderer the OpenGL context
             * \param parent the parent GameObject
             * \param mtl the Material to use
             * \param cloudPoint the dataset to use. This object will display points corresponding to this dataset
             * \param sd the SubDataset containing the parameters of the bound CloudPoint dataset (e.g., transfer function)*/
            CloudPointGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, std::shared_ptr<CloudPointDataset> cloudPoint, SubDataset* sd);

            /** \brief  Destructor, delete the OpenGL object loaded */
            ~CloudPointGameObject();

            void draw(const Render& render);
            void onTFChanged();
            void load();
        private:
            GLuint m_vboID;
            GLuint m_vaoID;
            std::shared_ptr<CloudPointDataset> m_dataset;
            uint8_t* m_newCols = nullptr;
            bool m_isPositionInit = false;

            bool     m_isWaitingTF = false; /*!< Is the object waiting to compute the colors?*/
            std::mutex m_updateTFLock;      /*!< The mutex locking the updateTF call*/
            std::mutex m_updateColorLock;              /*!< The mutex used to update the color*/

    };
}


#endif
