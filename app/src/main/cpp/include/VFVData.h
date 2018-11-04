#ifndef  VFVDATA_INC
#define  VFVDATA_INC

#include <string>
#include <vector>
#include <pthread.h>
#include <deque>
#include "FluidDataset.h"
#include "ColorMode.h"

namespace sereno
{
    /* \brief Enumeration representing the possible events from the Model modification */
    enum VFVEventType
    {
        VFV_ADD_DATA,            /*!< Data added*/
        VFV_DEL_DATA,            /*!< Data removed*/
        VFV_SET_CURRENT_DATA,    /*!< Current Data setted*/
        VFV_COLOR_RANGE_CHANGED  /*!< The color range has changed for the current dataset*/
    };

    /* \brief Color range information */
    struct ColorRangeEvent
    {
        float min;                 /*!< the minimum range (ratio : 0.0, 1.0)*/
        float max;                 /*!< the maximum range (ratio : 0.0, 1.0)*/
        ColorMode mode;            /*!< The color mode to apply*/
        FluidDataset* currentData; /*!< The current dataset*/
    };

    /* \brief The Event that can be sent from JNI */
    struct VFVEvent
    {
        VFVEventType type; /*!< The type of the event*/
        union
        {
            /* \brief fluid data event information (add, del, set current data) */
            struct
            {
                FluidDataset* dataset; /*!< The dataset associated*/
            }fluidData;
            ColorRangeEvent colorRange; /*!< Color range event information */
        };
    };


    /* \brief Callback interface for communication between JNI and CPP applications 
     * Note that the most part of the communication will not be in the OpenGL thread*/
    class IVFVCallback
    {
        public:
            /* \brief Function called when a new data has been added
             * \param dataPath the dataPath asked */
            virtual void onAddData   (const std::string& dataPath) = 0;

            /* \brief Functon called when a data is asked of being removed
             * \param dataPath the dataPath to remove */
            virtual void onRemoveData(const std::string& dataPath) = 0;
    };

    /* \brief Class containing the VFV data application to send to the main function */
    class VFVData
    {
        public:
            /* \brief Constructor. Initialize everything at default value */
            VFVData();

            ~VFVData();

            /* \brief Set the callback interface.
             * The aim is that the JNI application can set arguments and the cpp application can receive through callbacks
             * what has changed. Note that the most part of the communication will not be in the OpenGL thread
             *
             * \param clbk the new callback to discuss with
             * \param data data to send to this callback*/
            void setCallback(IVFVCallback* clkb);

            /* \brief Poll the next event. If the event is not null, it has to be freed
             * \return the next event or NULL if no event exists */
            VFVEvent* pollEvent();

            /* \brief Add a new Dataset in this application
             * \param dataset the dataset to add*/
            void addData(FluidDataset* dataset);

            /* \brief Remove the dataset "dataID"
             * \param dataID the id of the dataset to remove*/
            void removeData(int dataID);

            /* \brief Function called when the range color has changed
             * \param min the minimum range color
             * \param max the maximum range color 
             * \param mode the current color mode*/
            void onRangeColorChange(float min, float max, ColorMode mode);

            /* \brief Set the current data displayed in the application
             * \param dataID the dataID*/
            void setCurrentData(int dataID);
        private:
            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(VFVEvent* ev);

            std::vector<FluidDataset*> m_datas;              /*!< The data paths */
            FluidDataset*              m_currentData = NULL; /*!< The current data*/
            IVFVCallback*              m_clbk        = NULL; /*!< The callback interface */
            std::deque<VFVEvent*>      m_events;             /*!< The events from Java*/
            pthread_mutex_t            m_mutex;              /*!< The mutex for handling communication between Java and Cpp*/
    };
}

#endif
