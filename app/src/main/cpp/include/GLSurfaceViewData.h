#ifndef  GLSURFACEVIEWDATA_INC
#define  GLSURFACEVIEWDATA_INC

#include "ConcurrentVector.h"
#include "Event.h"
#include "GLRenderer.h"

namespace sereno
{
    /* \brief Data managed by both Java and C++ regarding GLSurfaceView */
    struct GLSurfaceViewData
    {
        ConcurrentVector<Event*> events;   /*!< Array of events*/
        GLRenderer               renderer; /*!< The renderer */
    };
}

#endif
