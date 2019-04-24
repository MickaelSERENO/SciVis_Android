#ifndef  TRANSFERTFUNCTION_INC
#define  TRANSFERTFUNCTION_INC

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h> //Extension for ES 2.0/3.0
#include <cstdint>
#include <cstdlib>

namespace sereno
{
    template<uint8_t Dim>
    struct TF
    {
        /**
         * \brief  Get the dimension of this texture
         * \return  The Dim parameter 
         */
        static constexpr uint8_t getDimensions() {return Dim;};

        virtual ~TF(){}
    };

    template<uint8_t N, typename T>
    /** \brief  Compute the transfert function for N dimension */
    struct ComputeTF
    {
        /**
         * \brief  Function to compute the transfert function
         *
         * \param arr the array of value to fill
         * \param loopSize the for loop size
         * \param tf the transfert function to use
         */
        static void compute(uint8_t* arr, const uint32_t* loopSize, const T& tf)
        {
            float ind[N];
            ComputeTF::_compute(arr, 0, loopSize, ind, tf);
        }

        /**
         * \brief  Create nested loop for
         *
         * \param arr the array to update
         * \param ind the indice to update in the array
         * \param loopSize the size of the loop
         * \param indArr the normalized indice
         * \param tf the transfer function
         */
        static void _compute(uint8_t* arr, uint32_t ind, 
                             const uint32_t* loopSize,
                             float* indArr,
                             const T& tf)
        {
            //Creates nested loop for 
            for(uint32_t i = 0; i < loopSize[N-1]; i++)
            {
                //Update the normalized arr indice
                indArr[N-1] = ((float)i)/loopSize[N-1];
                uint32_t shift = 1;
                for(uint32_t j = 0; j < N-1; j++)
                    shift*=loopSize[j];
                ComputeTF<N-1, T>::_compute(arr, ind+i*shift, loopSize, indArr, tf);
            }
        }
    };

    //Specialization of ComputeTF<N, T> with N==0
    template<typename T>
    struct ComputeTF<0, T>
    {
        static void _compute(uint8_t* arr, uint32_t ind, 
                             const uint32_t* loopSize,
                             float* indArr,
                             const T& tf)
        {
            //Compute the color
            uint8_t col[3];
            tf.computeColor(indArr, col);
            for(uint32_t i = 0; i < 3; i++)
                arr[4*ind+i] = col[i];

            arr[4*ind+3]   = tf.computeAlpha(indArr);
        }
    };

    template<typename T>
    struct TFTexture : public TF<T::getDimensions()>
    {
        static_assert(T::getDimensions() >= 1 || T::getDimensions() <= 3, "Dim must be between 1 and 3 included");

        /**
         * \brief  Generate the transfert function texture
         * \param texSize the texture size in pixels
         * \param tf the transfert function to use
         * \return  The texture 
         */
        static GLuint generateTexture(const uint32_t* texSize, const T& tf);

        /**
         * \brief  Get the texture target associated with Dim
         * \return  the GLenum texture target 
         */
        static GLenum getTextureTarget();
    };

    template <typename T>
    GLuint TFTexture<T>::generateTexture(const uint32_t* texSize,
                                         const T& tf)
    {

        size_t totalSize = 1;
        for(uint32_t i = 0; i < T::getDimensions(); i++)
            totalSize *= texSize[i];

        uint8_t* texels = (uint8_t*)malloc(sizeof(uint8_t)*4*totalSize);
        ComputeTF<T::getDimensions(), T>::compute(texels, texSize, tf);

        //Generate the texure
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(getTextureTarget(), tex);
            //Parameterize it
            glTexParameteri(getTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(getTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_OES);
            if(T::getDimensions() > 1)
                glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_OES);
            if(T::getDimensions() > 2)
                glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER_OES);

            //Send the pixel data
            if(T::getDimensions() == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else if(T::getDimensions() == 2)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], texSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, texSize[0], texSize[1], texSize[2], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            glTexParameteri(getTextureTarget(), GL_GENERATE_MIPMAP_HINT, GL_TRUE);
        glBindTexture(getTextureTarget(), 0);
        return tex;
    }

    template <typename T>
    GLenum TFTexture<T>::getTextureTarget()
    {
        switch(T::getDimensions())
        {
            case 1:
               return GL_TEXTURE_2D; 

            case 2:
               return GL_TEXTURE_2D; 

            case 3:
               return GL_TEXTURE_3D; 
        }
    }
}

#endif
