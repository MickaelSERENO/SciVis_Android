#include "MainVFV.h"
#include "jniData.h"

namespace sereno
{
    struct SubDatasetChangement
    {
        bool updateColor    = false;;
        bool updateRotation = false;
    };

    MainVFV::MainVFV(GLSurfaceViewData* surfaceData, VFVData* mainData) : m_surfaceData(surfaceData), m_mainData(mainData)
    {
        surfaceData->renderer.initializeContext();
        if(mainData)
            mainData->setCallback(this);

        //Load arrow mesh and material
        m_arrowMesh    = MeshLoader::loadFrom3DS(m_surfaceData->dataPath + "/Models/arrow.3ds");
        m_vfMtl        = new Material(&surfaceData->renderer, surfaceData->renderer.getShader("vectorField"));
        m_colorGridMtl = new ColorGridMaterial(&surfaceData->renderer);

        uint32_t texSize[2] = {256, 256};
        for(uint32_t i = 0; i < SciVisTFEnum_End; i++)
            m_sciVisTFs.push_back(sciVisTFGenTexture((SciVisTFEnum)i, texSize));
    }

    MainVFV::~MainVFV()
    {
        for(VectorField* vf : m_vectorFields)
            if(vf)
                delete vf;
        for(GLuint tex : m_sciVisTFs)
            glDeleteTextures(1, &tex);
        delete m_arrowMesh;
        delete m_vfMtl;
    }

    void MainVFV::run()
    {
        glViewport(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight());

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        std::map<const SubDataset*, SubDatasetChangement> modelChanged;

        while(!m_surfaceData->isClosed())
        {
            //Handles event received from the surface view 
            while(Event* event = m_surfaceData->pollEvent())
            {
                switch(event->type)
                {
                    case RESIZE:
                        //Redo the viewport
                        glViewport(0, 0, event->sizeEvent.width, event->sizeEvent.height);
                        m_surfaceData->renderer.setProjectionMatrix(glm::ortho(-1.0f, 1.0f,
                                                                               -(float)(event->sizeEvent.height/event->sizeEvent.width),
                                                                                (float)(event->sizeEvent.height/event->sizeEvent.width),
                                                                               -1.0f, 1.0f));
                        break;
                    case TOUCH_MOVE:
                    {
                        if(m_currentVis)
                        {
                            float roll  = event->touchEvent.x - event->touchEvent.oldX;
                            float pitch = event->touchEvent.y - event->touchEvent.oldY;

                            auto it = modelChanged.find(m_currentVis->getModel());
                            if(it == modelChanged.end())
                                modelChanged.insert(std::pair<const SubDataset*, SubDatasetChangement>(m_currentVis->getModel(), SubDatasetChangement{false, true}));
                            else
                                modelChanged[m_currentVis->getModel()].updateRotation = true;
                            m_currentVis->getModel()->setGlobalRotate(Quaternionf(roll, pitch, 0)*m_currentVis->getRotate());
                            m_mainData->sendRotationEvent(jniMainThread, m_currentVis->getModel(), roll, pitch, 0);

                            LOG_INFO("Rotating about %f %f", pitch, roll);
                        }
                        break;
                    }
                    default:
                        LOG_WARNING("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
            }

            //Handle events sent from JNI for our application (application wise)
            while(VFVEvent* event = m_mainData->pollEvent())
            {
                switch(event->getType())
                {
                    //Add binary dataset
                    case VFV_ADD_BINARY_DATA:
                        if(m_arrowMesh)
                        {
                            //Create the visualization
                            m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_vfMtl, NULL,
                                                                     event->binaryData.dataset, m_arrowMesh, 
                                                                     m_sciVisTFs[RAINBOW_DefaultTF], sciVisTFGetDimension(RAINBOW_DefaultTF)));
                            m_sciVis.push_back(m_vectorFields.back());
                            m_sciVisDefaultTF.insert(SciVisPair(m_vectorFields.back(), RAINBOW_DefaultTF));

                            //Set state
                            m_sciVis.back()->setTFTexture(m_sciVisTFs[m_sciVis.back()->getModel()->getColorMode() + RAINBOW_DefaultTF]);
                            std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(NULL));
                            m_snapshots.insert(snap);
                            m_sciVis.back()->getModel()->setSnapshot(snap.second);

                            if(m_currentVis == NULL)
                                m_currentVis = m_sciVis[0];
                            break;
                        }

                    //Add VTK Dataset
                    case VFV_ADD_VTK_DATA:
                        m_vtkStructuredGridPoints.push_back(new VTKStructuredGridPointSciVis(&m_surfaceData->renderer, m_colorGridMtl, event->vtkData.dataset, VTK_STRUCTURED_POINT_VIS_DENSITY,
                                                                                             m_sciVisTFs[RAINBOW_TriangularGTF], sciVisTFGetDimension(RAINBOW_TriangularGTF)));
                        m_colorGridMtl->setSpacing(m_vtkStructuredGridPoints.back()->vbo->getSpacing());
                        float dim[3];
                        for(uint8_t i = 0; i < 3; i++)
                            dim[i] = m_vtkStructuredGridPoints.back()->vbo->getDimensions()[i];
                        m_colorGridMtl->setDimension(dim);

                        for(uint32_t i = 0; i < event->vtkData.dataset->getNbSubDatasets(); i++)
                        {
                            m_sciVis.push_back(m_vtkStructuredGridPoints.back()->gameObjects[i]);
                            m_sciVisDefaultTF.insert(SciVisPair(m_sciVis.back(), RAINBOW_TriangularGTF));

                            //Set internal state
                            m_sciVis.back()->setTFTexture(m_sciVisTFs[m_sciVis.back()->getModel()->getColorMode() + RAINBOW_TriangularGTF]);
                            std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(nullptr));
                            m_snapshots.insert(snap);
                            m_sciVis.back()->getModel()->setSnapshot(snap.second);
                        }
                        if(m_currentVis == NULL)
                            m_currentVis = m_sciVis[0];
                        break;

                    case VFV_DEL_DATA:
                    {
                        //Check if the current sci vis is being deleted
                        if(m_currentVis->getModel()->getParent() == event->dataset.dataset.get())
                            m_currentVis = NULL;

                        //Check vector field
                        for(std::vector<VectorField*>::iterator it = m_vectorFields.begin(); it != m_vectorFields.end(); it++)
                            if((*it)->getModel()->getParent() == event->dataset.dataset.get())
                            {
                                delete (*it);
                                m_vectorFields.erase(it);
                                break;
                            }
                        break;
                    }

                    case VFV_COLOR_RANGE_CHANGED:
                    {
                        auto it = modelChanged.find(event->sdEvent.sd);
                        if(it == modelChanged.end())
                            modelChanged.insert(std::pair<const SubDataset*, SubDatasetChangement>(event->sdEvent.sd, SubDatasetChangement{true, false}));
                        else
                            modelChanged[event->sdEvent.sd].updateColor = true;
                        break;
                    }
                    case VFV_SET_ROTATION_DATA:
                    {
                        auto it = modelChanged.find(event->sdEvent.sd);
                        if(it == modelChanged.end())
                            modelChanged.insert(std::pair<const SubDataset*, SubDatasetChangement>(event->sdEvent.sd, SubDatasetChangement{false, true}));
                        else
                            modelChanged[event->sdEvent.sd].updateRotation = true;
                        break;
                    }
                    case VFV_SET_CURRENT_DATA:
                    {
                        //Find which SciVis this sub dataset belongs to and change the current sci vis
                        for(auto it : m_sciVis)
                            if(it->getModel() == event->sdEvent.sd)
                            {
                                m_currentVis = it;
                                break;
                            }
                        break;
                    }
                    default:
                        LOG_ERROR("type %d still has to be done\n", event->getType());
                        break;
                }
                delete event;
            }

            //Apply the model changement (rotation + color)
            for(auto& it : modelChanged)
            {
                for(auto sciVis : m_sciVis)
                {
                    if(sciVis->getModel() == it.first)
                    {
                        if(it.second.updateColor)
                        {
                            m_currentVis->setColorRange(it.first->getMinClamping(), it.first->getMaxClamping(), it.first->getColorMode());
                            m_currentVis->setTFTexture(m_sciVisTFs[it.first->getColorMode() + m_sciVisDefaultTF[sciVis]]);
                        }
                        if(it.second.updateRotation)
                            m_currentVis->setRotate(it.first->getGlobalRotate());
                    }
                }
            }
            modelChanged.clear();

            //Draw the scene
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if(m_currentVis != NULL)
                m_currentVis->update(&m_surfaceData->renderer);

            m_surfaceData->renderer.render();

            //Update the snapshot
            if(m_currentVis)
                m_snapshotCnt++;
            if(m_snapshotCnt == MAX_SNAPSHOT_COUNTER)
            {
                //Enlarge the pixel array
                uint32_t snapWidth  = m_surfaceData->renderer.getWidth();
                uint32_t snapHeight = m_surfaceData->renderer.getHeight();
                Snapshot* curSnapshot = m_currentVis->getModel()->getSnapshot();

                if(curSnapshot == NULL || curSnapshot->width != snapWidth || curSnapshot->height != snapHeight)
                {
                    Snapshot* newSnapshot = new Snapshot(snapWidth, snapHeight, (uint32_t*)malloc(sizeof(uint32_t)*snapWidth*snapHeight));
                    m_snapshots[m_currentVis] = std::shared_ptr<Snapshot>(newSnapshot);
                    curSnapshot = newSnapshot;
                    m_currentVis->getModel()->setSnapshot(m_snapshots[m_currentVis]);
                }
                //Read pixels
                glReadPixels(0, 0, snapWidth, snapHeight, GL_RGBA, GL_UNSIGNED_BYTE, curSnapshot->pixels);
                m_mainData->sendSnapshotEvent(jniMainThread, m_currentVis->getModel());
                m_snapshotCnt = 0;
            }

            m_surfaceData->renderer.swapBuffers();
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
