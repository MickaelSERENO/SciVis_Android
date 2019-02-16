#include "main.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "utils.h"
#include "Graphics/SciVis/TransfertFunction/TransfertFunction.h"
#include "Graphics/SciVis/TransfertFunction/GTF.h"

using namespace sereno;

/*----------------------------------------------------------------------------*/
/*---------------------------Redirection management---------------------------*/
/*----------------------------------------------------------------------------*/

static int errPfd[2];
static int outPfd[2];

static pthread_t outThread;
static pthread_t errThread;

static void *redirectThread(void* param)
{
    int stdID = (int)param;
    int pfd   = (stdID == 0 ? outPfd[0] : errPfd[0]);

    ssize_t rdsz;
    char buf[256];
    while((rdsz = read(pfd, buf, sizeof buf - 1)) > 0) 
    {
        if(buf[rdsz - 1] == '\n')
            rdsz--;
        buf[rdsz] = 0;

        if(stdID == 0)
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", buf);
        else
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", buf);
    }
    return 0;
}


int startLogger()
{
    /* make stdout and stderr line-buffered*/
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IOLBF, 0);

    /* create the pipe and redirect stdout and stderr*/
    pipe(outPfd);
    dup2(outPfd[1], STDOUT_FILENO);
    pipe(errPfd);
    dup2(outPfd[1], STDERR_FILENO);

    /* spawn the logging thread */
    if(pthread_create(&outThread, 0, redirectThread, (void*)1) == -1)
        return -1;
    if(pthread_create(&errThread, 0, redirectThread, 0) == -1)
        return -1;
    pthread_detach(outThread);
    pthread_detach(errThread);

    return 0;
}

/*----------------------------------------------------------------------------*/
/*------------------------------------Main------------------------------------*/
/*----------------------------------------------------------------------------*/

void GLSurface_main(GLSurfaceViewData* data, void* arg)
{
    startLogger();
    MainVFV mainVFV(data, (VFVData*)arg);
    mainVFV.run();
}
