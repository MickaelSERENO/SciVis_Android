#ifndef  DEFAULTGAMEOBJECT_INC
#define  DEFAULTGAMEOBJECT_INC

#include "Graphics/GameObject.h"
#include "Graphics/GPUData.h"

namespace sereno
{
    class TextureRectangleData : public GPUData
    {
        public:
            TextureRectangleData();
    };

    class DefaultGameObject : public GameObject
    {
        public:
            /* \brief Constructor. Initialize this DefaultGameObject with a default material
             * \param parent the parent of this DefaultGameObject (can be NULL)
             * \param renderer the OpenGL context object
             * \param mtl the material to use. 
             * \param gpuData which GPUData to use?*/
            DefaultGameObject(GameObject* parent, GLRenderer* renderer, Material* mtl, GPUData* gpuData);

            void draw(const glm::mat4& cameraMat, const glm::mat4& projMat);
        private:
            GPUData* m_gpuData; /*!< Pointer to the GPU Data (VBO, VAO)*/
    };
}

#endif
