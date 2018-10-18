#include "main.h"

using namespace sereno;

void GLSurface_main(GLSurfaceViewData* data, void* arg)
{
    MainVFV mainVFV(data, (VFVData*)arg);
    mainVFV.run();
}
