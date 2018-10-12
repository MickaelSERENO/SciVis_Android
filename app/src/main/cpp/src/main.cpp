#include "main.h"

void GLSurface_main(GLSurfaceViewData* data, void* arg)
{
    data->renderer.initializeContext();
    while(true)
    {
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        data->renderer.swapBuffers();
        usleep(1000);
    }
}