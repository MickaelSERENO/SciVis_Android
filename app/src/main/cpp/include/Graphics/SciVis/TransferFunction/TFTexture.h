#ifndef  TFTEXTURE_INC
#define  TFTEXTURE_INC

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h> //Extension for ES 2.0/3.0
#include <cstdint>
#include <cstdlib>
#include "TransferFunction/TransferFunction.h"

namespace sereno
{
    inline GLenum getTextureTarget(uint32_t dim)
    {
        switch(dim)
        {
            case 1:
               return GL_TEXTURE_2D; 

            case 2:
               return GL_TEXTURE_2D; 

            case 3:
               return GL_TEXTURE_3D; 
        }
        return 0;
    }

    template <typename T>
    GLuint generateTexture(const uint32_t* texSize, const T& tf)
    {
        size_t totalSize = 1;
        for(uint32_t i = 0; i < tf.getDimension(); i++)
            totalSize *= texSize[i];

        uint8_t* texels = (uint8_t*)malloc(sizeof(uint8_t)*4*totalSize);
        computeTFTexels(texels, texSize, tf);

        //Generate the texure
        GLuint tex;
        GLenum target = getTextureTarget(tf.getDimension());
        glGenTextures(1, &tex);
        glBindTexture(target, tex);
            //Parameterize it
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_OES);
            if(tf.getDimension() > 1)
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_OES);
            if(tf.getDimension() > 2)
                glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER_OES);

            //Send the pixel data
            if(tf.getDimension() == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else if(tf.getDimension() == 2)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], texSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, texSize[0], texSize[1], texSize[2], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            glTexParameteri(target, GL_GENERATE_MIPMAP_HINT, GL_TRUE);
        glBindTexture(target, 0);

        free(texels);
        return tex;
    }
}

#endif
