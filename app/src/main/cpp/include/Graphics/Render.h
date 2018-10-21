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

            /* \brief Returns the width of the render
             * \return the width of the render */
            int getWidth() const {return m_width;}

            /* \brief Returns the height of the render
             * \return the height of the render */
            int getHeight() const {return m_height;}
        protected:
            std::vector<Drawable*> m_currentDrawable; /*!< What are the current drawable being drawn ?*/
            glm::mat4              m_cameraMatrix;    /*!< The camera matrix to apply*/

            int m_width  = 0; /*!< The width of the surface (pixels)*/
            int m_height = 0; /*!< The height of the surface (pixels)*/
    };
}

#endif
