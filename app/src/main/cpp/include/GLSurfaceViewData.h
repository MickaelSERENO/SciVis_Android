#ifndef  GLSURFACEVIEWDATA_INC
#define  GLSURFACEVIEWDATA_INC

#include <deque>
#include <pthread.h>
#include "Event.h"
#include "Graphics/GLRenderer.h"

namespace sereno
{
    /* \brief Data managed by both Java and C++ regarding GLSurfaceView */
    class GLSurfaceViewData
    {
        public:
            /* \brief Constructor.
             * \param _dataPath the path of the data for this application (something like /sdcard/Android/data/com.sereno.vfs/files) */
            GLSurfaceViewData(const std::string& _dataPath);

            /* \brief Destructor.*/
            ~GLSurfaceViewData();

            /* \brief Poll the next event. If the event is not null, it has to be freed
             * \return the next event or NULL if no event exists */
            Event* pollEvent();

            /* \brief Function to call when the surface size is changing
             * \param width the surface width
             * \param height the surface height */
            void changeSurfaceSize(uint32_t width, uint32_t height);

            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(Event* ev);

            const std::string  dataPath; /*!< The data path*/
            GLRenderer         renderer; /*!< The renderer */
        private:
            std::deque<Event*> m_events; /*!< Array of events*/
            pthread_mutex_t    m_mutex;  /*!< The mutex for handling communication between Java and Cpp*/
    };
}

#endif
