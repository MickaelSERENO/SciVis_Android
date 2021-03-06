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
             * \param render the render used to draw this drawable*/
            virtual void draw(const Render& render);

            /* Function called for post processing drawing. 
             * This is called after EVERY object of the render has been drawned
             * \param render the render used to draw this drawable*/
            virtual void postDraw(const Render& render);

            /** \brief Set the Drawable Material
             * \param mtl the new Material to use*/
            void setMaterial(Material* mtl);

            /** \brief Enable the camera component
             * \param enable true if the camera should exist, false otherwise*/
            void setEnableCamera(bool enable) {m_enableCamera = enable;};

            /* \brief   Get wether the camera is enabled or not
             * \return  true if the camera component is taken into account, false otherwise*/
            bool isCameraEnabled() const {return m_enableCamera;}
        protected:
            GLRenderer* m_glRenderer = NULL;   /*! The GLRenderer containing the opengl renderer information*/
            Material*   m_mtl        = NULL;   /*!< The material to use*/
            bool        m_enableCamera = true; /*!< Enable the camera matrix component*/
    };
}

#endif
