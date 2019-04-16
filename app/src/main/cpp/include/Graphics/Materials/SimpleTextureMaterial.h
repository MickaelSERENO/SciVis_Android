#ifndef  SIMPLETEXTUREMATERIAL_INC
#define  SIMPLETEXTUREMATERIAL_INC

#include "Graphics/Materials/Material.h"

namespace sereno
{
    /** \brief SimpleTextureMaterial is a default material displaying a texture on screen
     * vUV0 and vPosition need to be provided by the Drawable*/
    class SimpleTextureMaterial : public Material
    {
        public:
            /** \brief Constructor associated with a SimpleTexture object
             * \param renderer the opengl context object*/
            SimpleTextureMaterial(GLRenderer* renderer);

            ~SimpleTextureMaterial();
    };
}

#endif
