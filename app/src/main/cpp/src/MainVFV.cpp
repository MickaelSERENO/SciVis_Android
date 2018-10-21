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
        m_arrowMtl  = new UniColorMaterial(&surfaceData->renderer, Color::WHITE);
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
            //Handle events sent from JNI for our application (application wise)
            while(VFVEvent* event = m_mainData->pollEvent())
            {
                switch(event->type)
                {
                    case VFV_ADD_DATA:
                        m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_arrowMtl, NULL,
                                                                 event->fluidData.dataset, m_arrowMesh));
                        if(m_currentVF == NULL)
                            m_currentVF = m_vectorFields.back();
                        break;

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
                    default:
                        LOG_ERROR("type %d style has to be done\n", event->type);
                        break;
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
