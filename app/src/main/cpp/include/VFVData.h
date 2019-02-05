#ifndef  VFVDATA_INC
#define  VFVDATA_INC

#include <string>
#include <vector>
#include <pthread.h>
#include <deque>
#include <memory>
#include <cstdint>
#include "Datasets/BinaryDataset.h"
#include "Datasets/VTKDataset.h"
#include "ColorMode.h"

namespace sereno
{
    /* \brief Enumeration representing the possible events from the Model modification */
    enum VFVEventType
    {
        VFV_ADD_BINARY_DATA,     /*!< Binary Data added*/
        VFV_ADD_VTK_DATA,        /*!< VTK Data added*/
        VFV_DEL_DATA,            /*!< Data removed*/
        VFV_SET_CURRENT_DATA,    /*!< Current Data setted*/
        VFV_COLOR_RANGE_CHANGED  /*!< The color range has changed for the current dataset*/
    };

    /* \brief Color range information */
    struct ColorRangeEvent
    {
        float min;                            /*!< the minimum range (ratio : 0.0, 1.0)*/
        float max;                            /*!< the maximum range (ratio : 0.0, 1.0)*/
        ColorMode mode;                       /*!< The color mode to apply*/
        uint32_t  subDataID;                  /*!< The sub dataset ID*/
        std::shared_ptr<Dataset> currentData; /*!< The current dataset*/
    };

    /* \brief binary data event information (add) */
    struct BinaryDataEvent
    {
        std::shared_ptr<BinaryDataset> dataset; /*!< The dataset associated*/
    };

    /* \brief VTK data event information (add) */
    struct VTKDataEvent
    {
        std::shared_ptr<VTKDataset> dataset; /*!< The dataset associated*/
    };

    /** \brief general dataset event information (delete, set current data) */
    struct DatasetEvent
    {
        std::shared_ptr<Dataset> dataset; /*!< The dataset associated */
    };

    /* \brief The Event that can be sent from JNI */
    struct VFVEvent
    {
        union
        {
            DatasetEvent    dataset;    /*!< General dataset event*/
            BinaryDataEvent binaryData; /*!< Binary  dataset event*/
            VTKDataEvent    vtkData;    /*!< VTK    dataset event*/
            ColorRangeEvent colorRange; /*!< Color range event information */
        };

        VFVEvent(VFVEventType t) : type(t)
        {
            if(type == VFV_ADD_BINARY_DATA)
                new(&binaryData) BinaryDataEvent;
            else if(type == VFV_ADD_VTK_DATA)
                new(&vtkData) VTKDataEvent;
            else if(type == VFV_COLOR_RANGE_CHANGED)
                new(&colorRange) ColorRangeEvent;
        }

        ~VFVEvent()
        {
            if(type == VFV_ADD_BINARY_DATA)
                binaryData.~BinaryDataEvent();
            else if(type == VFV_ADD_VTK_DATA)
                vtkData.~VTKDataEvent();
            else if(type == VFV_COLOR_RANGE_CHANGED)
                colorRange.~ColorRangeEvent();
        }

        /* \brief  Get the type of this event
         * \return the type of this event */
        VFVEventType getType() {return type;}

        private:
            VFVEventType type; /*!< The type of the event*/
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

            /* \brief Add a new Binary Dataset in this application
             * \param dataset the Binary dataset to add*/
            void addBinaryData(std::shared_ptr<BinaryDataset> dataset);

            /* \brief  Add a new VTK Dataset in this application
             * \param dataset the VTK dataset to add*/
            void addVTKData(std::shared_ptr<VTKDataset> dataset);

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

            /* \brief lock the snapshot update from the main thread */
            void lockSnapshot() {pthread_mutex_lock(&m_snapshotMutex);}

            /* \brief Unlock the snapshot update from the main thread */
            void unlockSnapshot() {pthread_mutex_lock(&m_snapshotMutex);}

            /**
             * \brief  Set the snapshot pixels
             * \param pixels the pixels
             * \param width the snapshot width
             * \param height the snapshot height
             */
            void setSnapshotPixels(uint32_t* pixels, uint32_t width, uint32_t height);

            /* \brief Get the snapshot pixels ARGB8888. Use getSnapshotWidth and getSnapshotHeight in order to get the correct layout
             * \return the snapshot pixels. */
            const uint32_t* getSnapshotPixels() const {return m_snapshotPixels;}

            /* \brief Get the snapshot image width 
             * \return the snapshot image width*/
            uint32_t getSnapshotWidth() const {return m_snapshotWidth;}

            /* \brief Get the snapshot image height 
             * \return the snapshot image height*/
            uint32_t getSnapshotHeight() const {return m_snapshotHeight;}
        private:
            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(VFVEvent* ev);

            std::vector<std::shared_ptr<Dataset>> m_datas;    /*!< The data paths */

            std::shared_ptr<Dataset> m_currentData       = NULL; /*!< The current data*/
            int                      m_currentSubDataIdx = -1;   /*!< The current subdata index*/
            IVFVCallback*            m_clbk              = NULL; /*!< The callback interface */
            std::deque<VFVEvent*>    m_events;                   /*!< The events from Java*/
            pthread_mutex_t          m_mutex;                    /*!< The mutex for handling communication between Java and Cpp*/
            uint32_t                 m_snapshotWidth;
            uint32_t                 m_snapshotHeight;
            uint32_t*                m_snapshotPixels    = NULL; /*!< The snapshot pixels*/
            pthread_mutex_t          m_snapshotMutex;            /*!< The mutex for handling the snapshot generation*/
    };
}

#endif
