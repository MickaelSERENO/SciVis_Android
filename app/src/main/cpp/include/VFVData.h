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
#include <glm/glm.hpp>
#include "HeadsetStatus.h"
#include "Datasets/VectorFieldDataset.h"
#include "Datasets/VTKDataset.h"
#include "Datasets/CloudPointDataset.h"
#include "ColorMode.h"
#include "Datasets/DatasetMetaData.h"

namespace sereno
{
    /* \brief Enumeration representing the possible events from the Model modification */
    enum VFVEventType
    {
        VFV_ADD_VECTOR_FIELD_DATA, /*!< VectorField Data added*/
        VFV_ADD_VTK_DATA,          /*!< VTK Data added*/
        VFV_ADD_CLOUD_POINT_DATA,  /*!< CloudPoint Data added*/
        VFV_SET_CURRENT_DATA,      /*!< Current Data setted*/
        VFV_SET_ROTATION_DATA,     /*!< A SubDataset rotation changing*/
        VFV_SET_POSITION_DATA,     /*!< A SubDataset position changing*/
        VFV_SET_SCALE_DATA,        /*!< A SubDataset scale changing*/
        VFV_SET_TF_DATA,           /*!< A SubDataset transfer function changing*/
        VFV_REMOVE_DATASET,        /*!< Remove a Dataset from memory*/
        VFV_REMOVE_SUBDATASET,     /*!< Remove a SubDataset from memory*/
        VFV_ADD_SUBDATASET,        /*!< Add a new SubDataset*/
        VFV_SET_LOCATION,          /*!< Update the tablet's location*/
        VFV_SET_TABLET_SCALE,      /*!< Update the tablet's location*/
        VFV_SET_SELECTION,         /*!< Set selection event*/
        VFV_SET_TB_USER_STUDY_MODE, /*!< Set the tangible brush user study mode*/
    };

    /* \brief Enumeration representing the different current actions the multi-touch device can enter*/
    enum VFVCurrentAction
    {
        VFV_CURRENT_ACTION_NOTHING             = 0,
        VFV_CURRENT_ACTION_MOVING              = 1,
        VFV_CURRENT_ACTION_SCALING             = 2,
        VFV_CURRENT_ACTION_ROTATING            = 3,
        VFV_CURRENT_ACTION_SKETCHING           = 4,
        VFV_CURRENT_ACTION_LASSO               = 6,
        VFV_CURRENT_ACTION_SELECTING           = 7,
        VFV_CURRENT_ACTION_REVIEWING_SELECTION = 8,
    };

    /* \brief Enumeration representing the different mode for the tangible brush user study*/
    enum VFVTBUserStudyMode
    {
        VFV_TB_USER_STUDY_AR = 0,
        VFV_TB_USER_STUDY_2D = 1,
    };

    struct SubDatasetEvent
    {
        SubDataset* sd; /*!< The subdataset being updated*/
    };

    /* \brief vectorField data event information (add) */
    struct VectorFieldDataEvent
    {
        std::shared_ptr<VectorFieldDataset> dataset; /*!< The dataset associated*/
    };

    /* \brief VTK data event information (add) */
    struct VTKDataEvent
    {
        std::shared_ptr<VTKDataset> dataset; /*!< The dataset associated*/
    };

    /* \brief CloudPoint data event information (add) */
    struct CloudPointDataEvent
    {
        std::shared_ptr<CloudPointDataset> dataset; /*!< The dataset associated*/
    };

    /** \brief general dataset event information (delete, set current data) */
    struct DatasetEvent
    {
        std::shared_ptr<Dataset> dataset; /*!< The dataset associated */
    };

    /** \brief location event information */
    struct SetLocationEvent
    {
        glm::vec3 pos;          /*!< Tablet's position */
        Quaternionf rot;    /*!< Tablet's rotation */
    };

    /** \brief tablet scale event information */
    struct SetTabletScaleEvent
    {
        float scale, width, height, posx, posy;
    };


    /* \brief Describes the start or end of a selection*/
    struct SelectionEvent
    {
        bool starting; /*!< True if start of a selecton*/
    };

    struct TBUserStudyEvent
    {
        VFVTBUserStudyMode tbMode; /*!< The tangible brush user study mode*/
    };

    /* \brief The Event that can be sent from JNI */
    struct VFVEvent
    {
        union
        {
            DatasetEvent          dataset;        /*!< General dataset event*/
            VectorFieldDataEvent  vectorFieldData;/*!< VectorField dataset event*/
            VTKDataEvent          vtkData;        /*!< VTK dataset event*/
            CloudPointDataEvent   cloudPointData; /*!< CloudPoint dataset event */
            SubDatasetEvent       sdEvent;        /*!< SubDataset general event information*/
            SetLocationEvent      setLocation;    /*!< location event information */
            SetTabletScaleEvent   setTabletScale; /*!< location event information */
            SelectionEvent        selection;      /*!< Selection event*/
            TBUserStudyEvent      tbUserStudy;    /*!< Tangible Brush User Study*/
        };

        VFVEvent(VFVEventType t) : type(t)
        {
            switch(type)
            {
                case VFV_ADD_VECTOR_FIELD_DATA:
                    new(&vectorFieldData) VectorFieldDataEvent;
                    break;
                case VFV_ADD_VTK_DATA:
                    new(&vtkData) VTKDataEvent;
                    break;
                case VFV_REMOVE_DATASET:
                    new(&dataset) DatasetEvent;
                    break;
                case VFV_ADD_CLOUD_POINT_DATA:
                    new(&cloudPointData) CloudPointDataEvent;
                    break;
                case VFV_SET_LOCATION:
                    new(&setLocation) SetLocationEvent;
                    break;
                case VFV_SET_TABLET_SCALE:
                    new(&setTabletScale) SetTabletScaleEvent;
                    break;
                case VFV_SET_SELECTION:
                    new(&selection) SelectionEvent;
                    break;
                case VFV_SET_TB_USER_STUDY_MODE:
                    new(&tbUserStudy) TBUserStudyEvent;
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
                case VFV_ADD_VECTOR_FIELD_DATA:
                    vectorFieldData.~VectorFieldDataEvent();
                    break;
                case VFV_ADD_VTK_DATA:
                    vtkData.~VTKDataEvent();
                    break;
                case VFV_ADD_CLOUD_POINT_DATA:
                    cloudPointData.~CloudPointDataEvent();
                    break;
                case VFV_REMOVE_DATASET:
                    dataset.~DatasetEvent();
                    break;
                case VFV_SET_LOCATION:
                    setLocation.~SetLocationEvent();
                    break;
                case VFV_SET_TABLET_SCALE:
                    setTabletScale.~SetTabletScaleEvent();
                    break;
                case VFV_SET_SELECTION:
                    selection.~SelectionEvent();
                    break;
                case VFV_SET_TB_USER_STUDY_MODE:
                    tbUserStudy.~TBUserStudyEvent();
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

            /* \brief Add a new VectorField Dataset in this application
             * \param dataset the VectorField dataset to add
             * \param jVectorField the Java VectorFieldDataset object*/
            void addVectorFieldData(std::shared_ptr<VectorFieldDataset> dataset, jobject jVectorField);

            /* \brief Add a new CloudPoint Dataset in this application
             * \param dataset the CloudPoint dataset to add
             * \param jCloudPoint the Java CloudPointDataset object*/
            void addCloudPointData(std::shared_ptr<CloudPointDataset> dataset, jobject jCloudPoint);

            /* \brief  Add a new VTK Dataset in this application
             * \param dataset the VTK dataset to add
             * \param jVTK the Java VTKDataset object*/
            void addVTKData(std::shared_ptr<VTKDataset> dataset, jobject jVTK);

            /* \brief Remove the dataset "subdataset"
             * \param sd the subdataset to remove*/
            void onRemoveSubDataset(SubDataset* sd);

            /* \brief Remove the dataset "dataset"
             * \param dataset the ataset to remove*/
            void onRemoveDataset(std::shared_ptr<Dataset> dataset);

            /* \brief Set whether or not the tablet is in selection mode
             * \param selection the new selection mode*/
            void setSelection(bool selection);

            /* \brief Update the tablet's location
             * \param pos the tablet's position
             * \param pos the tablet's rotation */
            void onSetLocation(const glm::vec3& pos, const Quaternionf& rot);
            
            /* \brief Update the tablet scale */
            void onSetTabletScale(float scale, float width, float height, float posx, float posy);

            /* \brief Function called when a SubDataset rotation has changed
             * \param data the SubDataset changing */
            void onRotationChange(SubDataset* data);

            /* \brief Function called when a SubDataset position has changed
             * \param data the SubDataset changing */
            void onPositionChange(SubDataset* data);

            /* \brief Function called when a SubDataset scaling has changed
             * \param data the SubDataset changing */
            void onScaleChange(SubDataset* data);

            /* \brief Function called when a SubDataset transfer function has changed
             * \param data the SubDataset changing */
            void onTFChange(SubDataset* data);

            /* \brief Function called when the mode of the tangible brush user study has changed
             * \param mode the new mode to use*/
            void onSetTBUserStudyMode(int mode);

            /* \brief Set the current data displayed in the application
            * \param sd the new SubDataset to display*/
            void setCurrentSubDataset(SubDataset* sd);

            /* \brief Get the headsets status. You should probably lock this object before calling this function 
             * \return  Pointer of an array of headsets status*/
            std::shared_ptr<std::vector<HeadsetStatus>> getHeadsetsStatus() {return m_headsetsStatus;}

            /* \brief  Set the current action the device is performing
             * \param action */
            void setCurrentAction(VFVCurrentAction action);

            /* \brief  Set the lasso
             * \param data lasso data */
            void setLasso(const std::vector<float> data);

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

            /* \brief Bind a C++ SubDataset to its Java counter part
             * \param sd the SubDataset C++ object to bind
             * \param publicJObjectSD the Java object to bind*/
            void bindSubDatasetJava(SubDataset* sd, jobject publicJObjectSD);

            /* \brief Make the application know that a new SubDataset has been added via the Java interface
             * \param sd the SubDataset already added*/
            void addSubDatasetFromJava(SubDataset* sd);

            /* \brief Send a new snapshot available event to the Java UI
             * \param subDataset the subDataset bound to this snapshot*/
            void sendSnapshotEvent(SubDataset* subDataset);

            /* \brief  Send a rotation event.
             * \param subDataset the subDataset being modified.
             * \param q the rotation to apply*/
            void sendRotationEvent(SubDataset* subDataset, const Quaternionf& q);

            /* \brief  Send a translation event.
             * \param subDataset the subDataset being modified.
             * \param position the position to apply*/
            void sendPositionEvent(SubDataset* subDataset, const glm::vec3& position);

            /* \brief  Send a scaling event.
             * \param subDataset the subDataset being modified.
             * \param scale the scale to apply*/
            void sendScaleEvent(SubDataset* subDataset, const glm::vec3& scale);

            /** \brief  Send the event "on Dataset loaded"
             * This function will call asynchronously the method Dataset::onLoadDataset (which is private)
             * \param pDataset the Dataset loaded
             * \param success true on success, false on failure */
            void sendOnDatasetLoaded(std::shared_ptr<Dataset> pDataset, bool success);

            /** \brief Send a CPCP Texture bound to a Dataset and its point field IDs
             * \param pDataset the Dataset which possess the Data
             * \param pixels the texture pixel array. Size: width*height. Format: ARGB32
             * \param width the texture width
             * \param height the texture height
             * \param pIDLeft the left axis of the parallel coordinate plot point field ID
             * \param pIDRight the right axis of the parallel coordinate plot point field ID */
            void sendCPCPTexture(std::shared_ptr<Dataset> pDataset, uint32_t* pixels, uint32_t width, uint32_t height,
                                 uint32_t pIDLeft, uint32_t pIDRight);

            /** \brief Send a 1D Histogram data bound to a Dataset and its point field ID
             * \param pDataset the Dataset which possess the Data
             * \param values The normalized floating values. Size: width
             * \param width the histogram width
             * \param pID the point field ID*/
            void send1DHistogram(std::shared_ptr<Dataset> pDataset, float* values, uint32_t width, uint32_t pID);

            /** \brief  Lock this object*/
            void lock() {pthread_mutex_lock(&m_mutex);}

            /** \brief  unlock this object */
            void unlock() {pthread_mutex_unlock(&m_mutex);}

            /* \brief Get the Java object bound to this model
             * \return the Java object*/
            jobject getJavaObj() {return m_javaObj;}

            /** \brief  Return the stored std::shared_ptr from a Dataset raw pointer
             * \param ptr a Dataset known by VFVData object
             * \return   default std::shared_ptr if not found, the corresponding std::shared_ptr otherwise */
            std::shared_ptr<Dataset> getDatasetSharedPtr(const Dataset* ptr)
            {
                for(auto& it : m_datas)
                    if(it.get() == ptr)
                        return it;
                return std::shared_ptr<Dataset>();
            }

            /** \brief  Get the DatasetMetaData bound to a given dataset
             * \param dataset the dataset to look at
             *
             * \return   the MetaData of the dataset given in parameter. Default std::shared_ptr if not found */
            std::shared_ptr<DatasetMetaData> getDatasetMetaData(const std::shared_ptr<Dataset>& dataset)
            {
                auto it = m_datasetMetaDatas.find(dataset);
                return (it == m_datasetMetaDatas.end() ? std::shared_ptr<DatasetMetaData>() : it->second);
            }

            /** \brief Tells whether a giving SubDataset can be modified or not
             * \param sd the SubDataset to evaluate
             * \return true if it can be modified, false otherwise*/
            bool canSubDatasetBeModified(SubDataset* sd);
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

            std::map<std::shared_ptr<Dataset>, std::shared_ptr<DatasetMetaData>> m_datasetMetaDatas; /*!< MetaData of Loaded dataset*/

            int m_headsetID = -1; /*!< The headset ID this device is bound to*/
            VFVTBUserStudyMode m_tbUserStudyMode = VFV_TB_USER_STUDY_AR; /*!< The current tangible mode for the associated user study*/

            jobject                  m_javaObj = 0;    /*!< The java object linked to this model object*/
            std::deque<VFVEvent*>    m_events;         /*!< The events from Java*/
            pthread_mutex_t          m_mutex;          /*!< The mutex for handling communication between Java and Cpp*/
            pthread_mutex_t          m_eventMutex;     /*!< The event mutex*/
    };
}

#endif
