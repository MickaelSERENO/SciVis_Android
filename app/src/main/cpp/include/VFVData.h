#ifndef  VFVDATA_INC
#define  VFVDATA_INC

#include <string>
#include <vector>
#include <pthread.h>
#include <deque>
#include <map>
#include <memory>
#include <cstdint>
#include <jni.h>
#include <vector>
#include "HeadsetStatus.h"
#include "Datasets/BinaryDataset.h"
#include "Datasets/VTKDataset.h"
#include "Datasets/SubDatasetMetaData.h"
#include "ColorMode.h"

namespace sereno
{
    /* \brief Enumeration representing the possible events from the Model modification */
    enum VFVEventType
    {
        VFV_ADD_BINARY_DATA,     /*!< Binary Data added*/
        VFV_ADD_VTK_DATA,        /*!< VTK Data added*/
        VFV_SET_CURRENT_DATA,    /*!< Current Data setted*/
        VFV_SET_ROTATION_DATA,   /*!< A SubDataset rotation changing*/
        VFV_SET_POSITION_DATA,   /*!< A SubDataset position changing*/
        VFV_SET_SCALE_DATA,      /*!< A SubDataset scale changing*/
        VFV_COLOR_RANGE_CHANGED, /*!< The color range has changed for the current dataset*/
        VFV_SET_VISIBILITY_DATA, /*!< The visibility data has changed for the current dataset*/
        VFV_REMOVE_DATASET,      /*!< Remove a Dataset from memory*/
        VFV_REMOVE_SUBDATASET,   /*!< Remove a SubDataset from memory*/
    };

    /* \brief Enumeration representing the different current actions the multi-touch device can enter*/
    enum VFVCurrentAction
    {
        VFV_CURRENT_ACTION_NOTHING   = 0,
        VFV_CURRENT_ACTION_MOVING    = 1,
        VFV_CURRENT_ACTION_SCALING   = 2,
        VFV_CURRENT_ACTION_ROTATING  = 3,
        VFV_CURRENT_ACTION_SKETCHING = 4
    };

    struct SubDatasetEvent
    {
        SubDataset* sd; /*!< The subdataset being updated*/
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
            SubDatasetEvent sdEvent;    /*!< SubDataset general event information*/
        };

        VFVEvent(VFVEventType t) : type(t)
        {
            switch(type)
            {
                case VFV_ADD_BINARY_DATA:
                    new(&binaryData) BinaryDataEvent;
                    break;
                case VFV_ADD_VTK_DATA:
                    new(&vtkData) VTKDataEvent;
                    break;
                case VFV_REMOVE_DATASET:
                    new(&dataset) DatasetEvent;
                    break;
                default:
                    new(&sdEvent) SubDatasetEvent;
                    break;
            }
        }

        ~VFVEvent()
        {
            switch(type)
            {
                case VFV_ADD_BINARY_DATA:
                    binaryData.~BinaryDataEvent();
                    break;
                case VFV_ADD_VTK_DATA:
                    vtkData.~VTKDataEvent();
                    break;
                case VFV_REMOVE_DATASET:
                    dataset.~DatasetEvent();
                    break;
                default:
                    sdEvent.~SubDatasetEvent();
                    break;
            }
        }

        /* \brief  Get the type of this event
         * \return the type of this event */
        VFVEventType getType() {return type;}

        private:
            VFVEventType type; /*!< The type of the event*/
    };

    /* \brief Class containing the VFV data application to send to the main function */
    class VFVData
    {
        public:
            /* \brief Constructor. Initialize everything at default value 
             * \param instance the java object calling this constructor*/
            VFVData(jobject instance);

            ~VFVData();

            /* \brief Poll the next event. If the event is not null, it has to be freed
             * \return the next event or NULL if no event exists */
            VFVEvent* pollEvent();

            /* \brief  Update the headsets status known
             * \param status pointer to the list of status*/
            void updateHeadsetsStatus(std::shared_ptr<std::vector<HeadsetStatus>> status);

            /* \brief Add a new Binary Dataset in this application
             * \param dataset the Binary dataset to add*/
            void addBinaryData(std::shared_ptr<BinaryDataset> dataset);

            /* \brief  Add a new VTK Dataset in this application
             * \param dataset the VTK dataset to add*/
            void addVTKData(std::shared_ptr<VTKDataset> dataset);

            /* \brief Remove the dataset "subdataset"
             * \param sd the subdataset to remove*/
            void onRemoveSubDataset(SubDataset* sd);

            /* \brief Remove the dataset "dataset"
             * \param dataset the ataset to remove*/
            void onRemoveDataset(std::shared_ptr<Dataset> dataset);

            /* \brief Function called when the clipping range has changed
             * \param min the minimum range color
             * \param max the maximum range color 
             * \param data the SubDataset changing*/
            void onClampingChange(float min, float max, SubDataset* data);

            /* \brief Function called when a SubDataset rotation has changed
             * \param data the SubDataset changing */
            void onRotationChange(SubDataset* data);

            /* \brief Function called when a SubDataset position has changed
             * \param data the SubDataset changing */
            void onPositionChange(SubDataset* data);

            /* \brief Function called when a SubDataset scaling has changed
             * \param data the SubDataset changing */
            void onScaleChange(SubDataset* data);

            /* \brief Set the current data displayed in the application
             * \param sd the new SubDataset to display*/
            void setCurrentSubDataset(SubDataset* sd);

            /* \brief Get the headsets status. You should probably lock this object before calling this function 
             * \return  Pointer of an array of headsets status*/
            std::shared_ptr<std::vector<HeadsetStatus>> getHeadsetsStatus() {return m_headsetsStatus;}

            /* \brief  Set the current action the device is performing
             * \param action */
            void setCurrentAction(VFVCurrentAction action);

            /* \brief  Set the headset ID this device is bound to. -1 == no headset bound
             * \param headsetID the new headset ID */
            void setHeadsetID(int headsetID)
            {
                lock();
                    m_headsetID = headsetID;
                unlock();
            }

            /* \brief  Get the headset ID this device is bound to. -1 == no headset bound
             * \return   The headset ID*/
            int getHeadsetID() const
            {
                return m_headsetID;
            }

            /* \brief  Add a new SubDataset metaData
             * \param metaData the subdataset meta data to add
             * \param publicJObjectSD the public states java object subdataset
             * \param privateJObjectSD the private states java object subdataset*/
            void addSubDatasetMetaData(const SubDatasetMetaData& metaData, jobject publicJObjectSD, jobject privateJObjectSD);

            /* \brief  Get the SubDatasetMetaData associated to a SubDataset
             * \param sd the subdataset to look at
             * \return    the associated SubDatasetMetaData or NULL if not found */
            const SubDatasetMetaData* getSubDatasetMetaData(const SubDataset* sd) const;

            /* \brief  Get the SubDatasetMetaData associated to a SubDataset
             * \param sd the subdataset to look at
             * \return    the associated SubDatasetMetaData or NULL if not found */
            SubDatasetMetaData* getSubDatasetMetaData(const SubDataset* sd);

            /* \brief  Set a SubDataset visibility. This function does something only if a SubDatasetMetaData is associated to the given SubDataset
             *
             * \param sd the SubDataset being modified
             * \param visibility the new visibility to apply. 
             *
             * \return true on success (found SubDatasetMetaData counterpart), false otherwise*/
            bool setSubDatasetVisibility(const SubDataset* sd, int visibility);

            /* \brief Send a new snapshot available event to the Java UI
             * \param subDataset the subDataset bound to this snapshot*/
            void sendSnapshotEvent(SubDataset* subDataset);

            /* \brief  Send a rotation event. Must be called after subDataset has been rotated
             * \param subDataset the subDataset being modified.*/ 
            void sendRotationEvent(SubDataset* subDataset);

            /* \brief  Send a translation event. Must be called after subDataset has been moved
             * \param subDataset the subDataset being modified.*/ 
            void sendPositionEvent(SubDataset* subDataset);

            /* \brief  Send a scaling event. Must be called after subDataset has been scaled
             * \param subDataset the subDataset being modified.*/ 
            void sendScaleEvent(SubDataset* subDataset);

            /** \brief  Lock this object*/
            void lock() {pthread_mutex_lock(&m_mutex);}

            /** \brief  unlock this object */
            void unlock() {pthread_mutex_unlock(&m_mutex);}

            /* \brief Get the Java object bound to this model
             * \return the Java object*/
            jobject getJavaObj() {return m_javaObj;}
        private:
            /* \brief  Add a subdataset event without parameter (update only)
             * \param sd the subdataset 
             * \param type the event type */
            void addSubDatasetEvent(SubDataset* sd, VFVEventType type);

            /* \brief  Add a dataset event without parameter (update only)
             * \param dataset the dataset
             * \param type the event type */
            void addDatasetEvent(std::shared_ptr<Dataset> dataset, VFVEventType type);

            /* \brief Add an event 
             * \param ev the event to add */
            void addEvent(VFVEvent* ev);

            /* \brief Remove a SubDataset from memory
             * \param sd the subdataset to remove*/
            void removeSubDataset(SubDataset* sd);

            std::shared_ptr<std::vector<HeadsetStatus>> m_headsetsStatus; /*!< The headsets status*/
            std::vector<std::shared_ptr<Dataset>> m_datas;   /*!< The data paths */
            std::map<SubDataset*, jobject> m_jSubDatasetMap; /*!< Map permitting to look up the java SubDataset objects*/
            std::map<const SubDataset*, SubDatasetMetaData*> m_sdMetaDatas; /*!< Map linking SubDataset to their meta data counter part*/

            int m_headsetID = -1; /*!< The headset ID this device is bound to*/

            jobject                  m_javaObj = 0;    /*!< The java object linked to this model object*/
            std::deque<VFVEvent*>    m_events;         /*!< The events from Java*/
            pthread_mutex_t          m_mutex;          /*!< The mutex for handling communication between Java and Cpp*/
            pthread_mutex_t          m_eventMutex;     /*!< The event mutex*/
    };
}

#endif
