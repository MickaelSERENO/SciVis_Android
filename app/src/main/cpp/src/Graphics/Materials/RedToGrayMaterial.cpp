#include "Graphics/Materials/RedToGrayMaterial.h"

namespace sereno
{
    RedToGrayMaterial::RedToGrayMaterial(GLRenderer* renderer) : Material(renderer, renderer->getShader("redToGray"))
    {
        if(!getShader())
            return;
    }
}
