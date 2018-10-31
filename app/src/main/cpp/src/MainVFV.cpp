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

        while(true)
        {
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
                            m_currentVF->setRotate(Quaternionf(roll, pitch, 0)*m_currentVF->getRotate());

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
                                m_currentVF = m_vectorFields.back();
                            break;
                        }

                    case VFV_DEL_DATA:
                        if(m_vectorFields.size() < event->fluidData.fluidID && 
                           m_vectorFields[event->fluidData.fluidID])
                        {
                            delete m_vectorFields[event->fluidData.fluidID];
                            if(m_currentVF == m_vectorFields[event->fluidData.fluidID])
                                m_currentVF = NULL;
                            m_vectorFields[event->fluidData.fluidID] = NULL;
                        }
                        break;
                    case VFV_COLOR_RANGE_CHANGED:
                        if(m_currentVF)
                            m_currentVF->setColorRange(event->colorRange.currentData, event->colorRange.min, event->colorRange.max, event->colorRange.mode);
                        break;
                    default:
                        LOG_ERROR("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
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
