#ifndef  GLSURFACEVIEWDATA_INC
#define  GLSURFACEVIEWDATA_INC

#include <deque>
#include <pthread.h>
#include "Event.h"
#include "Graphics/GLRenderer.h"
#include <android/native_window.h>

#define MAX_TOUCH_FINGER  10

namespace sereno
{
    /* \brief Data managed by both Java and C++ regarding GLSurfaceView */
    class GLSurfaceViewData
    {
        public:
            /* \brief Constructor.
             * \param _dataPath the path of the data for this application (something like /sdcard/Android/data/com.sereno.vfs/files)*/
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

            /* \brief Function to call when the surface received a touch event
             * \param action the action performed (UP, DOWN, MOVE)
             * \param finger the finger ID
             * \param x the x position
             * \param y the y position */
            void touchEvent(int action, int finger, float x, float y);

            /* \brief Function to call when the surface visibility changes
             * \param visibility is the surface visible?*/
            void visibilityEvent(bool visibility);

            /* \brief start and end selection
             * \param starting true if start of selection*/
            void selectionEvent(bool starting);

            /* \brief Get a touch finger coordinate state
             * \param id the id of the finger to look at
             * \return the touch coordinate or NULL if id is not correct */
            TouchCoord* getTouchCoord(int id);

            /** \brief  Close the surface view */
            void closeEvent() {m_isClosed = true;}

            /* \brief  Is the surface view closed?
             * \return  true if closed, false otherwise */
            bool isClosed() const {return m_isClosed;}

            /* \brief Unlock the surface data threading communication*/
            void unlock() {pthread_mutex_unlock(&m_mutex);}

            /* \brief lock the surface data threading communication*/
            void lock() {pthread_mutex_lock(&m_mutex);}

            const std::string  dataPath; /*!< The data path*/
            GLRenderer         renderer; /*!< The renderer */
        private:
            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(Event* ev);

            TouchCoord m_touchCoords[MAX_TOUCH_FINGER]; /*!< The coordinate of all the fingers*/

            std::deque<Event*> m_events;           /*!< Array of events*/
            pthread_mutex_t    m_mutex;            /*!< The mutex for handling communication between Java and Cpp*/
            bool               m_isClosed = false; /*!< Is the surface view closed ?*/
    };
}

#endif
