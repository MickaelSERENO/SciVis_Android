#ifndef  RENDER_INC
#define  RENDER_INC

#include "Transformable.h"
#include <vector>
#include <glm/glm.hpp>

namespace sereno
{
    class Drawable;

    /* \brief Render class. Permits to draw objects : initialize framebuffer, render object, can perform after effects rendering, etc. */
    class Render
    {
        public:
            /* \brief Constructor, does nothing */
            Render();

            /* \brief Destructor */
            virtual ~Render();

            /* \brief Render all the Drawable. The Viewport is set to this object values (see getViewport) */
            virtual void render();

            /* \brief Add the Drawable in parameter in the draw list
             * \param d the Drawable to draw later */
            void addToDraw(Drawable* d);

            /* \brief  Get the camera transformable
             * \return   The camera transformable*/
            const Transformable& getCameraTransformable() const {return m_cameraMatrix;}

            /* \brief  Get the camera transformable
             * \return   The camera transformable*/
            Transformable& getCameraTransformable() {return m_cameraMatrix;}

            /* \brief Get the render camera matrix
             * \return the render camera matrix */
            glm::mat4 getCameraMatrix() const {return glm::inverse(m_cameraMatrix.getMatrix());}

            /* \brief Get the projection camera matrix
             * \return the projection camera matrix*/
            const glm::mat4& getProjectionMatrix() const {return m_projMatrix;}

            /* \brief  Get the camera parameters
             * \return  The camera parameters. Only the w component is used. w == 0.0 -> perspective, w == 1.0 -> orthographic */
            const glm::vec4& getCameraParams() const {return m_cameraParams;}

            /* \brief Set the projection camera matrix as being orthographic. See glm::ortho for more details
             * \param left the left clipping plane
             * \param right the right clipping plane
             * \param bottom the bottom clipping plane
             * \param top the top clipping plane
             * \param near the near clipping plane
             * \param far the far clipping plane
             * \param rh true if right handed, false otherwise*/
            void setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far, bool rh);

            /* \brief  Set the projection camera matrix as being perspective. See glm::perspective for more details
             * \param fovY the Y field of view
             * \param aspect the aspect ratio x/y
             * \param near the near clipping plane
             * \param far the far clipping plane
             * \param rh true if right handed, false otherwise*/
            void setPerspectiveMatrix(float fovY, float aspect, float near, float far, bool rh);

            /* \brief Returns the width of the render
             * \return the width of the render */
            int getWidth() const {return m_width;}

            /* \brief Returns the height of the render
             * \return the height of the render */
            int getHeight() const {return m_height;}

            /** \brief  Set the Render viewport
             * \param rect the new viewport */
            void  setViewport(const Rectangle2i& rect) {m_viewport = rect;}

            /** \brief  Get the render viewport
             * \return  the render viewport */
            const Rectangle2i& getViewport() const {return m_viewport;}
        protected:
            std::vector<Drawable*> m_currentDrawable; /*!< What are the current drawable being drawn ?*/
            glm::mat4              m_projMatrix;      /*!< The perspective matrix to apply*/
            glm::vec4              m_cameraParams;    /*!< The camera parameters*/
            Transformable          m_cameraMatrix;    /*!< The camera transformable matrix*/
            Rectangle2i            m_viewport;        /*!< The render viewport*/

            int m_width  = 0; /*!< The width of the surface (pixels)*/
            int m_height = 0; /*!< The height of the surface (pixels)*/
    };
}

#endif
