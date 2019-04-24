#ifndef  MAIN_INC
#define  MAIN_INC

#include "GLSurfaceViewData.h"
#include "VFVData.h"
#include "MainVFV.h"

using namespace sereno;

extern "C"
{
    void GLSurface_main(GLSurfaceViewData* data, ANativeWindow* nativeWindow, void* arg);
};

#endif
