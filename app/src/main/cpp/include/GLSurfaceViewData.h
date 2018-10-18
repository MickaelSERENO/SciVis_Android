#ifndef  GLSURFACEVIEWDATA_INC
#define  GLSURFACEVIEWDATA_INC

#include "ConcurrentVector.h"
#include "Event.h"
#include "Graphics/GLRenderer.h"

namespace sereno
{
    /* \brief Data managed by both Java and C++ regarding GLSurfaceView */
    class GLSurfaceViewData
    {
        public:
            GLSurfaceViewData(const std::string& _dataPath) : dataPath(_dataPath), renderer(this)
            {}
            ConcurrentVector<Event*> events;   /*!< Array of events*/
            const std::string        dataPath; /*!< The data path*/
            GLRenderer               renderer; /*!< The renderer */
    };
}

#endif
