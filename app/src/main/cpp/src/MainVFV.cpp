#include "MainVFV.h"

namespace sereno
{
    MainVFV::MainVFV(GLSurfaceViewData* surfaceData, VFVData* mainData) : m_surfaceData(surfaceData), m_mainData(mainData)
    {
        surfaceData->renderer.initializeContext();
        if(mainData)
            mainData->setCallback(this);

        //Load arrow mesh and material
        m_arrowMesh = MeshLoader::loadFrom3DS(m_surfaceData->dataPath + "/Models/arrow.3ds");
        m_arrowMtl  = new ColorMaterial(&surfaceData->renderer);
    }

    MainVFV::~MainVFV()
    {
        for(VectorField* vf : m_vectorFields)
            if(vf)
                delete vf;
        delete m_arrowMesh;
        delete m_arrowMtl;
    }

    void MainVFV::run()
    {
        glViewport(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight());

        //Initialize the snapshot pixels
        m_snapshotWidth  = m_surfaceData->renderer.getWidth();
        m_snapshotHeight = m_surfaceData->renderer.getHeight();
        m_snapshotPixels = (uint32_t*)malloc(sizeof(uint32_t*)*m_snapshotWidth*m_snapshotHeight);
        memset(m_snapshotPixels, 0, sizeof(uint32_t*)*m_snapshotWidth*m_snapshotHeight);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        //List of dataset modified
        std::vector<SubDataset*> modelChanged;

        //Should we update the color ?
        //We do not keep WHICH dataset has seen its color changed, in most condition it is the CURRENT DATA which has seen its color changed
        //If not, we update something for nothing, but the visualization is not broken (because bound to the model which is set in the Java thread)
        bool updateColor = false;

        while(true)
        {
            //Handles event received from the surface view 
            while(Event* event = m_surfaceData->pollEvent())
            {
                switch(event->type)
                {
                    case RESIZE:
                        //Redo the viewport and the snapshot pixels sizes
                        glViewport(0, 0, event->sizeEvent.width, event->sizeEvent.height);
                        free(m_snapshotPixels);

                        m_snapshotWidth  = event->sizeEvent.width;
                        m_snapshotHeight = event->sizeEvent.height;
                        m_snapshotPixels = (uint32_t*)malloc(sizeof(uint32_t*)*m_snapshotWidth*m_snapshotHeight);
                        memset(m_snapshotPixels, 0, sizeof(uint32_t*)*m_snapshotWidth*m_snapshotHeight);
                        m_snapshotCnt = 0; //Reset the counter. By doing this we normally do not need any synchronization (we are good for MAX_SNAPSHOT_COUNTER frame, hence can have again a resize event if needed)
                        break;
                    case TOUCH_MOVE:
                    {
                        if(m_currentVF)
                        {
                            float roll  = event->touchEvent.x - event->touchEvent.oldX;
                            float pitch = event->touchEvent.y - event->touchEvent.oldY;
                            modelChanged.push_back(m_currentVF->getModel()->getSubDataset(0));
                            m_currentVF->getModel()->getSubDataset(0)->setGlobalRotate(Quaternionf(roll, pitch, 0)*m_currentVF->getRotate());

                            LOG_INFO("Rotating about %f %f", pitch, roll);
                        }
                        break;
                    }
                    default:
                        LOG_ERROR("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
            }

            //Handle events sent from JNI for our application (application wise)
            while(VFVEvent* event = m_mainData->pollEvent())
            {
                switch(event->getType())
                {
                    case VFV_ADD_BINARY_DATA:
                        if(m_arrowMesh)
                        {
                            m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_arrowMtl, NULL,
                                                                     event->binaryData.dataset, m_arrowMesh));
                            if(m_currentVF == NULL)
                                m_currentVF      = m_vectorFields.back();
                            break;
                        }

                    case VFV_DEL_DATA:
                        {
                            if(m_currentVF->getModel() == event->dataset.dataset)
                                m_currentVF = NULL;

                            //Check vector field
                            for(std::vector<VectorField*>::iterator it = m_vectorFields.begin(); it != m_vectorFields.end(); it++)
                                if((*it)->getModel() == event->dataset.dataset)
                                {
                                    delete (*it);
                                    m_vectorFields.erase(it);
                                }
                        }
                        break;
                    case VFV_COLOR_RANGE_CHANGED:
                        //Change color event. The color will be changed ONCE below (only once because it is heavy to change the color multiple times. We do it once before rendering)
                        modelChanged.push_back(event->colorRange.currentData->getSubDataset(event->colorRange.subDataID));
                        updateColor = true;
                        break;
                    default:
                        LOG_ERROR("type %d still has to be done\n", event->getType());
                        break;
                }
                delete event;
            }

            if(m_currentVF != NULL)
            {
                //Apply the model changement (rotation + color)
                for(auto dataset : modelChanged)
                {
                    if(m_currentVF->getModel().get() == dataset->getParent())
                    {
                        if(updateColor)
                            m_currentVF->setColorRange(dataset->getMinClamping(), dataset->getMaxClamping(), dataset->getColorMode());
                        m_currentVF->setRotate(dataset->getGlobalRotate());
                        break;
                    }
                }
            }

            //Draw the scene
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if(m_currentVF != NULL)
                m_currentVF->update(&m_surfaceData->renderer);

            if(m_snapshotCnt == MAX_SNAPSHOT_COUNTER)
            {
                m_snapshotCnt = 0;
                glReadPixels(0, 0, m_snapshotWidth, m_snapshotHeight, GL_RGBA, GL_BYTE, m_snapshotPixels);
                m_mainData->setSnapshotPixels(m_snapshotPixels, m_snapshotWidth, m_snapshotHeight);
            }
            m_surfaceData->renderer.render();
        }
    }

    void MainVFV::onRemoveData(const std::string& dataPath)
    {
        //TODO
    }

    void MainVFV::onAddData(const std::string& dataPath)
    {
        //TODO
    }
}
