#ifndef  TRANSFERTFUNCTION_INC
#define  TRANSFERTFUNCTION_INC

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h> //Extension for ES 2.0/3.0
#include <cstdint>
#include <cstdlib>
#include "SciVisColor.h"

namespace sereno
{
    /** \brief  Basic class for transfer function computation */
    class TF
    {
        public:
            /* \brief  Constructor of Basic class of transfer functions
             * \param dim the dimension of the transfer function
             * \param mode the color mode*/
            TF(uint32_t dim, ColorMode mode) : m_dim(dim), m_mode(mode)
            {}

            TF(const TF& copy)
            {
                *this = copy;
            }

            TF& operator=(const TF& copy)
            {
                m_dim  = copy.m_dim;
                m_mode = copy.m_mode;

                return *this;
            }

            virtual ~TF(){}

            /* \brief Compute the alpha component of this transfer function 
             * \param ind the normalized indice of the transfer function. Must be at least of size = getDim
             * \return the alpha component computed*/
            virtual uint8_t computeAlpha(float* ind) const
            {
                return 0xff;
            }

            /* \brief  Compute the RGB color of this transfer function
             * \param ind the normalized indice of the transfer function. Must be at least of size = getDim
             * \param colOut[out] the RGB color output. Minimum size: 3*/
            virtual void computeColor(float* ind, uint8_t* colOut) const
            {
                float mag = 0;
                for(uint32_t i = 0; i < m_dim; i++)
                    mag += ind[i]*ind[i];
                mag = sqrt(mag);
                Color c = SciVis_computeColor(m_mode, mag);
                for(int i = 0; i < 3; i++)
                    colOut[i] = std::min(255.0f, std::max(0.0f, 255.0f*c[i]));
            }

            /* \brief  Get the transfer function dimension
             * \return   The transfer function dimension*/
            uint32_t getDimension() const {return m_dim;}
        protected:
            uint32_t  m_dim;  /*!< The transfer function dimension*/
            ColorMode m_mode; /*!< The color mode*/
    };

    /* \brief  Compute the transfer function texels. The dimension of the transfer function must be inferior at 1024
     * Use this function if you prefer parallelism. Otherwise use computeTFTexelsRec
     * \param texels[out] the texels to compute
     * \param texSize the size of the texture
     * \param tf the transfer function in use. Its dimension must be greater or equal to 1 */
    template <typename T>
    void computeTFTexels(uint8_t* texels, const uint32_t* texSize, const T& tf)
    {
        #pragma omp parallel
        {
            float indArr[1024];
            int32_t ind = tf.getDimension()-1;

            uint32_t shift = 1;
            for(uint32_t i = 0; i < ind; i++)
                shift*=texSize[i];
            #pragma omp for
            {
                for(uint32_t i = 0; i < texSize[ind]; i++)
                {
                    indArr[ind] = ((float)i)/texSize[ind];
                    computeTFTexelsRec(texels, texSize, indArr, tf, ind-1, shift);
                }
            }
        }
    }

    /* \brief  Compute the transfer function texture by recursion. Some values are needed to be initialize at default value for the recursion to work
     *
     * \param texels[out] the texels to compute
     * \param texSize the size of the texture
     * \param indArr array of the stored indice (x, y, z, ...) while we iterate. Its size must be at least Dim. No needed to initialize it at the first call
     * \param tf the transfer function in use
     * \param ind current indice in the dimension. Go through dim-1 to 0. Must be dim-1 at the first call
     * \param off the offset of the texels array. Must be 0 at the first call*/
    template <typename T>
    void computeTFTexelsRec(uint8_t* texels, const uint32_t* texSize, float* indArr, 
                            const T& tf, int32_t ind, uint32_t off)
    {
        //Do the recursion
        if(ind > 0)
        {
            uint32_t shift = 1;
            for(uint32_t i = 0; i < ind; i++)
                shift*=texSize[i];
            for(uint32_t i = 0; i < texSize[ind]; i++)
            {
                indArr[ind] = ((float)i)/texSize[ind];
                computeTFTexelsRec(texels, texSize, indArr, tf, ind-1, off+shift);
            }
        }

        //Compute (finally) the RGBA components of the last dimension
        else
        {
            for(uint32_t i = 0; i < texSize[0]; i++)
            {
                indArr[0] = ((float)i)/texSize[0];

                //Compute the color
                uint8_t col[3];
                tf.computeColor(indArr, col);
                for(uint8_t j = 0; j < 3; j++)
                    texels[4*(off+i)+j] = col[j];

                //Compute the alpha component
                texels[4*(off+i)+3] = tf.computeAlpha(indArr);
            }
        }
    }

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
