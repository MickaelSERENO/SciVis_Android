#ifndef  REDTOGRAYMATERIAL_INC
#define  REDTOGRAYMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief  Material used to convert red texture to grayscale textures*/
    class RedToGrayMaterial : public Material
    {
        public:
            /** \brief  Constructor
             * \param renderer the Opengl context object*/ 
            RedToGrayMaterial(GLRenderer* renderer);
        private:
    };
}

#endif
