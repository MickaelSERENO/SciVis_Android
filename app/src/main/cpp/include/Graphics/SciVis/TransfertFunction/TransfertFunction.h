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
        static uint8_t getDimensions() {return Dim;};

        virtual ~TF(){}
    };

    template<uint8_t N, typename T>
    /** \brief  Compute the transfert function for N dimension */
    class ComputeTF
    {
        public:
            /**
             * \brief  Function to compute the transfert function
             *
             * \param arr the array of value to fill
             * \param loopSize the for loop size
             * \param minVals the minimum values
             * \param maxVals the maximum values
             * \param tfAlpha the transfert alpha function to use
             */
            static void compute(uint8_t* arr, uint32_t* loopSize,
                                float* minVals, float* maxVals,
                                const T& tfAlpha)
            {
                float ind[N];
                ComputeTF::_compute(arr, 0, loopSize, minVals, maxVals, ind, tfAlpha);
            }

        private:
            static void _compute(uint8_t* arr, uint32_t ind, 
                                 uint32_t* loopSize,
                                 float* minVals, float* maxVals,
                                 float* indArr,
                                 const T& tf)
            {
                if(N == 0)
                {
                    uint8_t col[3];
                    tf.computeColor(minVals, maxVals, indArr, col);
                    for(uint32_t i = 0; i < 3; i++)
                        arr[4*ind+i] = col[i];
                    arr[4*ind+3]   = tf.computeAlpha(minVals, maxVals, indArr);
                    return;
                }

                for(uint32_t i = 0; i < loopSize[N]; i++)
                {
                    indArr[N] = ((float)i)/loopSize[N];
                    uint32_t shift = 1;
                    for(uint32_t j = 0; j < N-1; j++)
                        shift*=loopSize[j];
                    ComputeTF<N-1, T>::_compute(arr, ind+i*shift, loopSize, 
                                                minVals, maxVals, indArr, tf);
                }
            }
    };


    template<uint8_t Dim, typename T>
    class TFTexture : public TF<Dim>
    {
        static_assert(Dim < 1 || Dim > 3,        "Dim must be between 1 and 3 included");
        static_assert(Dim != T::getDimensions(), "The dimension asked and the transfert function T are incompatible");

        /**
         * \brief  Generate the transfert function texture
         * \param minVal the minimal value along each dimension
         * \param maxVal the maximum value along each dimension
         * \param texSize the texture size in pixels
         * \param tf the transfert function to use
         * \return  The texture 
         */
        static GLuint generateTexture(const float* minVal, 
                                      const float* maxVal,
                                      const uint32_t* texSize,
                                      const T& tf);

        /**
         * \brief  Get the texture target associated with Dim
         * \return  the GLenum texture target 
         */
        static GLenum getTextureTarget();
    };

    template <uint8_t Dim, typename T>
    GLuint TFTexture<Dim,T>::generateTexture(const float* minVal,
                                             const float* maxVal,
                                             const uint32_t* texSize,
                                             const T& tf)
    {

        size_t totalSize = 1;
        for(uint32_t i = 0; i < Dim; i++)
            totalSize *= texSize[i];

        uint8_t* texels = (uint8_t*)malloc(sizeof(uint8_t)*4*totalSize);
        ComputeTF<Dim, T>::compute(texSize, minVal, maxVal, tf);

        //Generate the texure
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(getTextureTarget(), tex);
            //Parameterize it
            glTexParameteri(getTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(getTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_S, GL_REPEAT);
            if(Dim > 1)
                glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_T, GL_REPEAT);
            if(Dim > 2)
                glTexParameteri(getTextureTarget(), GL_TEXTURE_WRAP_R, GL_REPEAT);

            //Send the pixel data
            if(Dim == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else if(Dim == 2)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize[0], texSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            else
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, texSize[0], texSize[1], texSize[2], 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
        glBindTexture(getTextureTarget(), 0);
        return tex;
    }

    template <uint8_t Dim, typename T>
    GLenum TFTexture<Dim, T>::getTextureTarget()
    {
        switch(Dim)
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
