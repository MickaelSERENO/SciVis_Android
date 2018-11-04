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
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        //List of dataset modified
        std::vector<FluidDataset*> modelChanged;

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
                        glViewport(0, 0, event->sizeEvent.width, event->sizeEvent.height);
                        break;
                    case TOUCH_MOVE:
                    {
                        if(m_currentVF)
                        {
                            float roll  = event->touchEvent.x - event->touchEvent.oldX;
                            float pitch = event->touchEvent.y - event->touchEvent.oldY;
                            modelChanged.push_back(m_currentVF->getModel());
                            m_currentVF->getModel()->setGlobalRotate(Quaternionf(roll, pitch, 0)*m_currentVF->getRotate());

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
                switch(event->type)
                {
                    case VFV_ADD_DATA:
                        if(m_arrowMesh)
                        {
                            m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_arrowMtl, NULL,
                                                                     event->fluidData.dataset, m_arrowMesh));
                            if(m_currentVF == NULL)
                                m_currentVF      = m_vectorFields.back();
                            break;
                        }

                    case VFV_DEL_DATA:
                        {
                            if(m_currentVF->getModel() == event->fluidData.dataset)
                                m_currentVF = NULL;

                            for(std::vector<VectorField*>::iterator it = m_vectorFields.begin(); it != m_vectorFields.end(); it++)
                                if((*it)->getModel() == event->fluidData.dataset)
                                {
                                    delete (*it);
                                    m_vectorFields.erase(it);
                                }
                        }
                        break;
                    //Change color event. The color will be changed ONCE below (only once because it is heavy to change the color multiple times. We do it once before rendering)
                    case VFV_COLOR_RANGE_CHANGED:
                        modelChanged.push_back(event->colorRange.currentData);
                        updateColor = true;
                        break;
                    default:
                        LOG_ERROR("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
            }

            if(m_currentVF != NULL)
            {
                //Apply the model changement (rotation + color)
                for(FluidDataset* fd : modelChanged)
                {
                    if(m_currentVF->getModel() == fd)
                    {
                        if(updateColor)
                            m_currentVF->setColorRange(fd->getMinClamping(), fd->getMaxClamping(), fd->getColorMode());
                        m_currentVF->setRotate(fd->getGlobalRotate());
                        break;
                    }
                }
            }

            //Draw the scene
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if(m_currentVF != NULL)
                m_currentVF->update(&m_surfaceData->renderer);
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
