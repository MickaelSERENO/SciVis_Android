#ifndef  VFVDATA_INC
#define  VFVDATA_INC

#include <string>
#include <vector>
#include <pthread.h>
#include <deque>
#include "FluidDataset.h"

namespace sereno
{
    /* \brief Enumeration representing the possible events from the Model modification */
    enum VFVEventType
    {
        VFV_ADD_DATA,        /*!< Data added*/
        VFV_DEL_DATA,        /*!< Data removed*/
        VFV_SET_CURRENT_DATA /*!< Current Data setted*/
    };

    /* \brief The Event that can be sent from JNI */
    struct VFVEvent
    {
        VFVEventType type; /*!< The type of the event*/
        union
        {
            struct
            {
                uint32_t      fluidID; /*!< Indice of this opened file (always incremental)*/
                FluidDataset* dataset; /*!< The dataset associated*/
            }fluidData;
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

            /* \brief Set the callback interface.
             * The aim is that the JNI application can set arguments and the cpp application can receive through callbacks
             * what has changed. Note that the most part of the communication will not be in the OpenGL thread
             *
             * \param clbk the new callback to discuss with
             * \param data data to send to this callback*/
            void setCallback(IVFVCallback* clkb);

            /* \brief Poll the next event
             * \return the next event or NULL if no event exists */
            VFVEvent* pollEvent();

            /* \brief Add a new Dataset in this application
             * \param dataset the dataset to add*/
            void addData(FluidDataset* dataset);

            /* \brief Remove the dataset "dataID"
             * \param dataID the id of the dataset to remove*/
            void removeData(int dataID);

            /* \brief Set the current data displayed in the application
             * \param dataID the dataID*/
            void setCurrentData(int dataID);
        private:
            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(VFVEvent* ev);

            std::vector<FluidDataset*> m_datas;          /*!< The data paths */
            IVFVCallback*              m_clbk    = NULL; /*!< The callback interface */
            std::deque<VFVEvent*>      m_events;         /*!< The events from Java*/
            pthread_mutex_t            m_mutex;          /*!< The mutex for handling communication between Java and Cpp*/
    };
}

#endif
