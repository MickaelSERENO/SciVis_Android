#ifndef SHADER_INCLUDE
#define SHADER_INCLUDE

#define ERROR_MAX_LENGTH 500

#define GL_GLEXT_PROTOTYPES

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h> //Extension for ES 2.0/3.0
#include <iostream>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define MATERIAL_VPOSITION 0
#define MATERIAL_VNORMAL   1
#define MATERIAL_VUV       2
#define MATERIAL_VCOLOR    3 

namespace sereno
{
    /** \brief A graphic program.*/
    class Shader
    {
        public:
            /** \brief the shader constructor. Should never be called alone (use loader functions)*/
            Shader();

            /* \brief Destructor. Destroy the shader component created */
            ~Shader();

            /** \brief get the program ID stored in the graphic memory of this shader.
             * \return the program ID */
            int getProgramID() const;

            /** \brief get the vertex ID stored in the graphic memory of this shader.
             * \return the vertex ID */
            int getVertexID() const;

            /** \brief get the fragment ID stored in the graphic memory of this shader.
             * \return the fragment ID */
            int getFragID() const;

            /** \brief create a shader from a vertex and a fragment file.
             * \param vertFile the vertex file.
             * \param fragFile the fragment file.
             * \param geomFile the geometry file. Can be NULL, in this case we do not compile a geometry shader
             *
             * \return the Shader constructed or NULL if error*/
            static Shader* loadFromFiles(FILE* vertexFile, FILE* fragFile, FILE* geomFile = NULL);

            /** \brief create a shader from a vertex and a fragment string.
             * \param vertString the vertex string.
             * \param fragString the fragment string.
             * \param geomString the geometry string. Can be empty (in this case we do not compile a geometry shader)
             *
             * \return the Shader constructed or NULL if error
             * */
            static Shader* loadFromStrings(const std::string& vertexString, const std::string& fragString, const std::string& geomString="");
        private:
            GLuint m_programID; /*!< The shader   program ID*/
            GLuint m_vertexID;  /*!< The vertex   shader  ID*/
            GLuint m_fragID;    /*!< The fragment shader  ID*/
            GLuint m_geomID;    /*!< The geometry shader  ID*/

            bool m_hasGeom = false; /*!< Have we used a geometry shader ?*/

            void bindAttributes();

            /** \brief Bind the attributes key string by an ID 
             * \param code the attribute name
             * \param type the type of this attribute*/
            static int loadShader(const std::string& code, int type);
    };
}

#endif
