#ifndef  GPUDATA_INC
#define  GPUDATA_INC

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace sereno
{
    class GPUData
    {
        public:
            GPUData()
            {}

            virtual ~GPUData()
            {
                glDeleteBuffers(1, &m_vboID);
                glDeleteVertexArrays(1, &m_vaoID);
            }

            /* \brief  Get the VBO ID containing the mesh information
             * \return   The VBO ID*/
            GLuint getVBO() {return m_vboID;}

            /* \brief  Get the VAO ID containing draw calls parameters
             * \return  The VAO ID */
            GLuint getVAO() {return m_vaoID;}

            /* \brief  Get the number of vertices
             * \return   The number of vertices this GPU Data contains*/
            uint32_t getNbVertices() {return m_nbVertices;}

            /* \brief  Get the drawing mode (GL_TRIANGLES, etc.)
             * \return  The drawing mode associated with this data */
            GLenum getMode() {return m_mode;}
        protected:
            GLuint   m_vboID = 0;
            GLuint   m_vaoID = 0;
            uint32_t m_nbVertices = 0;
            GLenum   m_mode  = GL_TRIANGLES;
    };
}

#endif
