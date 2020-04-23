#ifndef  LASSO_INC
#define  LASSO_INC

#include "Graphics/GameObject.h"

namespace sereno
{
    class Lasso : public GameObject
    {
        public:
            /** 
             * \brief Constructor. Initialize this GameObject with a default material
             * \param parent the parent of this GameObject (can be NULL)
             * \param renderer the OpenGL context object
             * \param mtl the material to use. */
            Lasso(GameObject* parent, GLRenderer* renderer, Material* mtl);
            void draw(const Render& render);
            void startLasso(float x, float y, float z);
            void continueLasso(float x, float y, float z);
            void endLasso();
        protected:
            GLuint   m_vboID = 0;
            GLuint   m_vaoID = 0;
            uint32_t m_nbVertices = 0;
            GLenum   m_mode  = GL_LINES;

            std::vector<float> data;
    };
}

#endif