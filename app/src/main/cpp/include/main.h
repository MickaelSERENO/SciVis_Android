#ifndef  MAIN_INC
#define  MAIN_INC

#include "GLSurfaceViewData.h"
#include "unistd.h"
#include "time.h"
using namespace sereno;

extern "C"
{
    void GLSurface_main(GLSurfaceViewData* data, void* arg);
};

#endif
