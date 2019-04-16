#include "Graphics/Materials/SimpleTextureMaterial.h"

namespace sereno
{
    SimpleTextureMaterial::SimpleTextureMaterial(GLRenderer* renderer) : Material(renderer, renderer->getShader("simpleTexture"))
    {
        if(!getShader())
            return;
        getAttributs();
    }

    SimpleTextureMaterial::~SimpleTextureMaterial()
    {
    }
}
