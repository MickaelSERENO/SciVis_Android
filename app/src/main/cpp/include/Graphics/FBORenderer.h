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

            /** \brief  Set the FBO being bound
             * \param fbo the new FBO to use 
             * \param resetViewport should the viewport be reset?*/
            void setFBO(FBO* fbo, bool resetViewport=true);
        private:
            FBO* m_fbo = NULL; /*!< The FBO to draw into*/
    };
}

#endif
