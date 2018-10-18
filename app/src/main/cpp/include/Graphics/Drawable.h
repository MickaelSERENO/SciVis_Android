#ifndef  DRAWABLE_INC
#define  DRAWABLE_INC

#include "Graphics/Materials/Material.h"
#include <glm/glm.hpp>

namespace sereno
{
    class Render;
    class GLRenderer;

    /* \brief Drawable class. Represents an object that can be drawned on screen*/
    class Drawable
    {
        public:
            /* \brief Constructor. Initialize this Drawable with a default material
             * \param glRenderer the opengl renderer which contains information about the current opengl context
             * \param mtl the material to use. */
            Drawable(GLRenderer* glRenderer, Material* mtl);

            virtual ~Drawable();

            /* \brief Draw this drawable on the current framebuffer. 
             * \param cameraMat the current camera matrix*/
            virtual void draw(const glm::mat4& cameraMat);

            /* Function called for post processing drawing. 
             * This is called after EVERY children has been drawned
             * \param cameraMat the camera matrix  */
            virtual void postDraw(const glm::mat4& cameraMat);
        protected:
            GLRenderer* m_glRenderer = NULL; /*! The GLRenderer containing the opengl renderer information*/
            Material*   m_mtl        = NULL; /*!< The material to use*/
    };
}

#endif
