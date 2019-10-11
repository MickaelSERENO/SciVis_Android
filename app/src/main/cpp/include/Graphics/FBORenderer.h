#ifndef  FBORENDERER_INC
#define  FBORENDERER_INC

#include "Graphics/Render.h"
#include "Graphics/FBO.h"

namespace sereno
{
    /** \brief  Renderer based on a FBO */
    class FBORenderer : public Render
    {
        public:
            /** \brief  Constructor, bind this renderer to a FBO
             * \param fbo the FBO to use */
            FBORenderer(FBO* fbo);

            virtual void render();
        private:
            FBO* m_fbo = NULL; /*!< The FBO to draw into*/
    };
}

#endif
