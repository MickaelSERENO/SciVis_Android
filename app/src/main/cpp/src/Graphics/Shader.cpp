#include "Graphics/Shader.h"

namespace sereno
{
    Shader::Shader() : m_programID(0), m_vertexID(0), m_fragID(0)
    {}

    Shader::~Shader()
    {
        glDeleteProgram(m_programID);
        glDeleteShader(m_vertexID);
        glDeleteShader(m_fragID);
        if(m_hasGeom)
            glDeleteShader(m_geomID);
    }

    Shader* Shader::loadFromFiles(FILE* vertexFile, FILE* fragFile, FILE* geomFile)
    {
        uint32_t vertexFileSize = 0;
        uint32_t fragFileSize   = 0;
        uint32_t geomFileSize   = 0;
        char* vertexCodeC;
        char* fragCodeC;
        char* geomCodeC = NULL;

        /* Determine the vertex and fragment shader sizes */
        fseek(vertexFile, 0, SEEK_END);
        vertexFileSize = ftell(vertexFile);
        fseek(vertexFile, 0, SEEK_SET);

        fseek(fragFile, 0, SEEK_END);
        fragFileSize = ftell(fragFile);
        fseek(fragFile, 0, SEEK_SET);

        if(geomFile)
        {
            fseek(geomFile, 0, SEEK_END);
            geomFileSize = ftell(geomFile);
            fseek(geomFile, 0, SEEK_SET);
        }

        vertexCodeC = (char*)malloc(vertexFileSize+1);
        fragCodeC   = (char*)malloc(fragFileSize+1);

        /* Read the files */
        fread(vertexCodeC, 1, vertexFileSize, vertexFile);
        vertexCodeC[vertexFileSize] = '\0';
        fread(fragCodeC, 1, fragFileSize, fragFile);
        fragCodeC[fragFileSize] = '\0';

        if(geomFile)
        {
            geomCodeC = (char*)malloc(geomFileSize+1);
            fread(geomCodeC, 1, geomFileSize, geomFile);
            geomCodeC[geomFileSize] = '\0';
        }

        /* Return the shader and free everything*/
        Shader* s = loadFromStrings(std::string(vertexCodeC), std::string(fragCodeC), 
                                    (geomCodeC ? std::string(geomCodeC) : ""));

        free(vertexCodeC);
        free(fragCodeC);
        if(geomCodeC)
            free(geomCodeC);

        return s;
    }

    Shader* Shader::loadFromStrings(const std::string& vertexString, const std::string& fragString, const std::string& geomString)
    {
        Shader* shader = new Shader();
        shader->m_programID = glCreateProgram();
        shader->m_vertexID = loadShader(vertexString, GL_VERTEX_SHADER);
        shader->m_fragID = loadShader(fragString, GL_FRAGMENT_SHADER);

        //Load geometry shader
        if(geomString.size() != 0)
        {
            shader->m_geomID = loadShader(geomString, GL_GEOMETRY_SHADER);
            glAttachShader(shader->m_programID, shader->m_geomID);
            shader->m_hasGeom = true;
        }

        glAttachShader(shader->m_programID, shader->m_vertexID);
        glAttachShader(shader->m_programID, shader->m_fragID);

        shader->bindAttributes();

        glLinkProgram(shader->m_programID);

        //Display log
        int length=0;
        glGetProgramiv(shader->m_programID, GL_INFO_LOG_LENGTH, &length);
        if(length > 1)
        {
            char* error = (char*) malloc((length + 1)* sizeof(char));
            glGetProgramInfoLog(shader->m_programID, length, 0, error);
            LOG_ERROR("Shader log : %s", error);
            free(error);
        }

        int linkStatus;
        glGetProgramiv(shader->m_programID, GL_LINK_STATUS, &linkStatus);

        if(linkStatus == GL_FALSE)
        {
            LOG_ERROR("Could not link the shader...");
            delete shader;
            return NULL;
        }

        return shader;
    }

    int Shader::loadShader(const std::string& code, int type)
    {
        LOG_INFO("Shader: \n%s\n", code.c_str());
        int shader = glCreateShader(type);
        const GLchar* s = code.c_str();
        GLint l = code.length();
        glShaderSource(shader, 1, &s, &l);
        glCompileShader(shader);

        //Display log
        int length=0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if(length > 1)
        {
            char* error = (char*) malloc((length + 1)* sizeof(char));
            glGetShaderInfoLog(shader, length, 0, error);
            LOG_ERROR("Shader log : %s", error);
            free(error);
        }

        int compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if(compiled == GL_FALSE)
        {
            LOG_ERROR("Could not compile shader typed %d...", type);
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    int Shader::getProgramID() const
    {
        return m_programID;
    }

    int Shader::getVertexID() const
    {
        return m_vertexID;
    }

    int Shader::getFragID() const
    {
        return m_fragID;
    }

    void Shader::bindAttributes()
    {
        glBindAttribLocation(m_programID, MATERIAL_VPOSITION, "vPosition");
        glBindAttribLocation(m_programID, MATERIAL_VNORMAL,   "vNormal");
        glBindAttribLocation(m_programID, MATERIAL_VCOLOR,    "vColor");
        glBindAttribLocation(m_programID, MATERIAL_VUV0,      "vUV0");
        glBindAttribLocation(m_programID, MATERIAL_VUV1,      "vUV1");
        glBindAttribLocation(m_programID, MATERIAL_VUV2,      "vUV2");
        glBindAttribLocation(m_programID, MATERIAL_VUV3,      "vUV3");
    }
}
