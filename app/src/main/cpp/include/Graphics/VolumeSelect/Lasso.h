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
            void clearLasso();
            void draw(const Render& render);
            void startLasso(float x, float y, float z);

            /** \brief Continue the creation of the lasso
             * \param x the x position of the new point
             * \param y the y position of the new point
             * \param z the z position of the new point
             * \return true if the point should be added, false otherwise*/
            bool continueLasso(float x, float y, float z);

            bool endLasso();

            const std::vector<float>& getData() const;
        protected:
            GLuint   m_vboID = 0;
            GLuint   m_vaoID = 0;
            uint32_t m_nbVertices = 0;
            GLenum   m_mode  = GL_LINES;
            bool m_vboInitialized = false;

            std::vector<float> m_data;

            void buildVBO();
    };
}

#endif