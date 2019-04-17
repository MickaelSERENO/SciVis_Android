#ifndef  RENDER_INC
#define  RENDER_INC

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

            /* \brief Render all the Drawable */
            virtual void render();

            /* \brief Add the Drawable in parameter in the draw list
             * \param d the Drawable to draw later */
            void addToDraw(Drawable* d);

            /* \brief Get the render camera matrix
             * \return the render camera matrix */
            const glm::mat4& getCameraMatrix() const {return m_cameraMatrix;}

            /* \brief Get the projection camera matrix
             * \return the projection camera matrix*/
            const glm::mat4& getProjectionMatrix() const {return m_projMatrix;}

            /* \brief  Get the camera parameters
             * \return  The camera parameters. Only the w component is used. w == 0.0 -> perspective, w == 1.0 -> orthographic */
            const glm::vec4& getCameraParams() const {return m_cameraParams;}

            /* \brief Set the camera matrix (model-view)
             * \param cameraMat the new view camera matrix*/
            void setCameraMatrix(const glm::mat4& cameraMat) {m_cameraMatrix = cameraMat;}

            /* \brief Set the projection camera matrix as being orthographic. See glm::ortho for more details
             * \param left the left clipping plane
             * \param right the right clipping plane
             * \param bottom the bottom clipping plane
             * \param top the top clipping plane
             * \param near the near clipping plane
             * \param far the far clipping plane*/
            void setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far);

            /* \brief  Set the projection camera matrix as being perspective. See glm::perspective for more details
             * \param fovY the Y field of view
             * \param aspect the aspect ratio x/y
             * \param near the near clipping plane
             * \param far the far clipping plane */
            void setPerspectiveMatrix(float fovY, float aspect, float near, float far);

            /* \brief Returns the width of the render
             * \return the width of the render */
            int getWidth() const {return m_width;}

            /* \brief Returns the height of the render
             * \return the height of the render */
            int getHeight() const {return m_height;}
        protected:
            std::vector<Drawable*> m_currentDrawable; /*!< What are the current drawable being drawn ?*/
            glm::mat4              m_cameraMatrix;    /*!< The camera matrix to apply*/
            glm::mat4              m_projMatrix;      /*!< The perspective matrix to apply*/
            glm::vec4              m_cameraParams;    /*!< The camera parameters*/

            int m_width  = 0; /*!< The width of the surface (pixels)*/
            int m_height = 0; /*!< The height of the surface (pixels)*/
    };
}

#endif
