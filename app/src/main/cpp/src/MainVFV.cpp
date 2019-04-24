#include "MainVFV.h"
#include "jniData.h"

namespace sereno
{
    MainVFV::MainVFV(GLSurfaceViewData* surfaceData, ANativeWindow* nativeWindow, VFVData* mainData) : m_surfaceData(surfaceData), m_mainData(mainData)
    {
        surfaceData->renderer.initializeContext(nativeWindow);
        if(mainData)
            mainData->setCallback(this);

        //Load arrow mesh
        m_arrowMesh    = MeshLoader::loadFrom3DS(m_surfaceData->dataPath + "/Models/arrow.3ds");

        //Load images data
        const char* volumeImageManipPath[8];
        volumeImageManipPath[TOP_IMAGE]          = "negativeYTranslation.png";
        volumeImageManipPath[BOTTOM_IMAGE]       = "positiveYTranslation.png";
        volumeImageManipPath[RIGHT_IMAGE]        = "negativeXTranslation.png";
        volumeImageManipPath[LEFT_IMAGE]         = "positiveXTranslation.png";
        volumeImageManipPath[TOP_LEFT_IMAGE]     = "topLeftNegativeScaling.png";
        volumeImageManipPath[TOP_RIGHT_IMAGE]    = "topRightNegativeScaling.png";
        volumeImageManipPath[BOTTOM_LEFT_IMAGE]  = "bottomLeftPositiveScaling.png";
        volumeImageManipPath[BOTTOM_RIGHT_IMAGE] = "bottomRightPositiveScaling.png";

        //Load every materials
        m_vfMtl        = new Material(&surfaceData->renderer, surfaceData->renderer.getShader("vectorField"));
        m_colorGridMtl = new ColorGridMaterial(&surfaceData->renderer);
        m_textureMtl   = (SimpleTextureMaterial*)malloc(sizeof(SimpleTextureMaterial)*8);
        m_gpuTexVBO    = new TextureRectangleData();

        //Load 3D images used to translate/rotate/scale the 3D datasets
        m_3dImageManipTex = (Texture*)malloc(sizeof(Texture)*8);
        m_3dImageManipGO = (DefaultGameObject*)malloc(sizeof(DefaultGameObject)*8);
        for(int i = 0; i < 8; i++)
        {
            uint32_t texWidth, texHeight;
            std::string texPath = surfaceData->dataPath + "/Images/" + volumeImageManipPath[i];
            uint8_t* texData = getPNGRGBABytesFromFiles(texPath.c_str(), &texWidth, &texHeight);
            new(m_3dImageManipTex+i) Texture(texWidth, texHeight, texData);
            free(texData);
            new(m_textureMtl+i) SimpleTextureMaterial(&surfaceData->renderer);
            m_textureMtl[i].bindTexture(m_3dImageManipTex[i].getTextureID(), 2, 0);
            new(m_3dImageManipGO+i) DefaultGameObject(NULL, &surfaceData->renderer, m_textureMtl+i, m_gpuTexVBO);
        }

        //Load the default scientific vis transfer functions
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
        delete m_gpuTexVBO;
        for(int i = 0; i < 8; i++)
        {
            m_textureMtl[i].~SimpleTextureMaterial();
            m_3dImageManipTex[i].~Texture();
            m_3dImageManipGO[i].~DefaultGameObject();
        }
        free(m_3dImageManipGO);
    }

    void MainVFV::placeWidgets()
    {
        //Nothing to do
        if(m_surfaceData->renderer.getWidth() == 0.0)
            return;

        float width  = m_surfaceData->renderer.getWidth();
        float height = m_surfaceData->renderer.getHeight();
        float ratio  = height/width;

        float widgetWidth  = 2.0f*WIDGET_WIDTH_PX/width;
        float widgetHeight = 2.0*ratio - 2.0*widgetWidth;

        m_3dImageManipGO[LEFT_IMAGE].setScale(glm::vec3(widgetWidth, widgetHeight, 1.0f));
        m_3dImageManipGO[LEFT_IMAGE].setPosition(glm::vec3(-1.0f+widgetWidth*0.5f, 0.0f, 0.0f));

        m_3dImageManipGO[RIGHT_IMAGE].setScale(glm::vec3(widgetWidth, widgetHeight, 1.0f));
        m_3dImageManipGO[RIGHT_IMAGE].setPosition(glm::vec3(1.0f-widgetWidth*0.5f, 0.0f, 0.0f));

        m_3dImageManipGO[TOP_IMAGE].setScale(glm::vec3(2.0f-2.0*widgetWidth, widgetWidth, 1.0f));
        m_3dImageManipGO[TOP_IMAGE].setPosition(glm::vec3(0.0f, ratio-widgetWidth*0.5f, 0.0f));
        m_3dImageManipGO[BOTTOM_IMAGE].setScale(glm::vec3(2.0f-2.0*widgetWidth, widgetWidth, 1.0f));
        m_3dImageManipGO[BOTTOM_IMAGE].setPosition(glm::vec3(0.0f, -ratio+widgetWidth*0.5f, 0.0f));

        int corners[4] {TOP_LEFT_IMAGE, TOP_RIGHT_IMAGE, BOTTOM_RIGHT_IMAGE, BOTTOM_LEFT_IMAGE};
        for(int i = 0; i < 4; i++)
            m_3dImageManipGO[corners[i]].setScale(glm::vec3(widgetWidth, widgetWidth, 1.0));

        m_3dImageManipGO[TOP_LEFT_IMAGE].setPosition(glm::vec3(-1.0f+widgetWidth*0.5f, ratio-widgetWidth*0.5f, 0.0));
        m_3dImageManipGO[TOP_RIGHT_IMAGE].setPosition(glm::vec3(1.0f-widgetWidth*0.5f, ratio-widgetWidth*0.5f, 0.0));

        m_3dImageManipGO[BOTTOM_LEFT_IMAGE].setPosition(glm::vec3(-1.0f+widgetWidth*0.5f, -ratio+widgetWidth*0.5f, 0.0));
        m_3dImageManipGO[BOTTOM_RIGHT_IMAGE].setPosition(glm::vec3(1.0f-widgetWidth*0.5f, -ratio+widgetWidth*0.5f, 0.0));
    }

    bool MainVFV::findHeadsetCameraTransformation(glm::vec3* outPos, Quaternionf* outRot)
    {
        bool found = false;
        m_mainData->lock();
        {
            std::shared_ptr<std::vector<HeadsetStatus>> headsetsStatus = m_mainData->getHeadsetsStatus();
            int headsetID = m_mainData->getHeadsetID();

            if(headsetID == -1 || headsetsStatus.get() == NULL)
                goto end;

            for(HeadsetStatus& hs : *headsetsStatus)
            {
                if(hs.id == headsetID)
                {
                    if(outPos)
                        *outPos = hs.position;
                    if(outRot)
                        *outRot = hs.rotation;
                    found = true;
                    break;
                }
            }
        }
    end:
        m_mainData->unlock();
        return found;
    }

    void MainVFV::placeCamera(bool forceReset)
    {
        //Reset the animation
        if(forceReset)
        {
            m_animationTimer = 0;
            m_inAnimation    = false;
        }

        switch(m_currentWidgetAction)
        {
            //Cancel the animation
            case NO_IMAGE:
            {
                if(m_inAnimation || forceReset)
                {
                    m_animationTimer = 0;

                    m_surfaceData->renderer.setOrthographicMatrix(-1.0f, 1.0f,
                                                                  -((float)m_surfaceData->renderer.getHeight())/m_surfaceData->renderer.getWidth(),
                                                                   ((float)m_surfaceData->renderer.getHeight())/m_surfaceData->renderer.getWidth(),
                                                                  -10.0f, 10.0f, false);

                    //Reset camera position
                    m_surfaceData->renderer.getCameraTransformable().setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
                    m_surfaceData->renderer.getCameraTransformable().setRotate(Quaternionf(0.0f, 0.0f, 0.0f, 1.0f));
                }
                m_inAnimation = false;
                break;
            }

            //Go into an animation
            default:
            {
                if(!m_inAnimation || forceReset)
                {
                    m_surfaceData->renderer.setPerspectiveMatrix(45.0f*3.14f/180.0f, ((float)m_surfaceData->renderer.getWidth())/m_surfaceData->renderer.getHeight(), 0.1, 100.0f, false);
                    if(m_currentVis)
                    {
                        m_animationStartingPoint = m_currentVis->getPosition();
                        if(!findHeadsetCameraTransformation(&m_animationEndingPoint, NULL))
                            m_animationEndingPoint = glm::vec3(0, 0, -10);
                    }
                    m_animationTimer = 0;
                }

                if(m_currentVis && m_animationTimer <= MAX_CAMERA_ANIMATION_TIMER)
                {
                    glm::vec3 dir = m_animationEndingPoint - m_animationStartingPoint;
                    lookAt(m_surfaceData->renderer.getCameraTransformable(), glm::vec3(0.0f, 1.0f, 0.0f), ((float)m_animationTimer)/MAX_CAMERA_ANIMATION_TIMER*dir, m_animationStartingPoint, false);
                    m_animationTimer++;
                }
                m_inAnimation = true;
                break;
            }
        }
    }

    void MainVFV::run()
    {
        glViewport(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight());

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        bool     visible = true;

        while(!m_surfaceData->isClosed())
        {
            //Handles event received from the surface view 
            while(Event* event = m_surfaceData->pollEvent())
            {
                switch(event->type)
                {
                    case TOUCH_UP:
                    {
                        uint32_t numberFinger = 0;
                        TouchCoord* tc = NULL;
                        for(uint32_t i = 0; NULL != (tc = m_surfaceData->getTouchCoord(i++));)
                            if(tc->type != TOUCH_TYPE_UP)
                                numberFinger++;

                        if(numberFinger == 0)
                        {
                            m_currentWidgetAction = NO_IMAGE;
                            m_mainData->setCurrentAction(VFV_CURRENT_ACTION_NOTHING);
                        }
                        break;
                    }

                    case TOUCH_DOWN:
                    {
                        uint32_t numberFinger = 0;
                        TouchCoord* tc = NULL;
                        for(uint32_t i = 0; NULL != (tc = m_surfaceData->getTouchCoord(i++));)
                            if(tc->type != TOUCH_TYPE_UP)
                                numberFinger++;

                        if(numberFinger == 1)
                        {
                            float width  = m_surfaceData->renderer.getWidth();
                            float height = m_surfaceData->renderer.getHeight();
                            float ratio  = height/width;
                            float y = event->touchEvent.y*ratio; //Determine where y is
                            float x = event->touchEvent.x;

                            float widgetWidth  = 2.0f*WIDGET_WIDTH_PX/width;
                            float widgetHeight = 2.0*ratio - 2.0*widgetWidth;

                            //Determine current widget touched

                            //Left
                            if(x <= -1.0+widgetWidth)
                            {
                                if(y <= ratio-widgetWidth && y >= -ratio+widgetWidth)
                                    m_currentWidgetAction = LEFT_IMAGE;
                                else if(y > ratio-widgetWidth)
                                    m_currentWidgetAction = TOP_LEFT_IMAGE;
                                else
                                    m_currentWidgetAction = BOTTOM_LEFT_IMAGE;
                            }

                            //Right
                            else if(x >= 1.0f-widgetWidth)
                            {
                                if(y <= ratio-widgetWidth && y >= -ratio+widgetWidth)
                                    m_currentWidgetAction = RIGHT_IMAGE;
                                else if(y > ratio-widgetWidth)
                                    m_currentWidgetAction = TOP_RIGHT_IMAGE;
                                else
                                    m_currentWidgetAction = BOTTOM_RIGHT_IMAGE;
                            }

                            //Center (top/bottom)
                            else
                            {
                                if(y > ratio-widgetWidth)
                                    m_currentWidgetAction = TOP_IMAGE;
                                else if(y < -ratio+widgetWidth)
                                    m_currentWidgetAction = BOTTOM_IMAGE;
                            }

                            if(m_currentWidgetAction == TOP_IMAGE ||
                               m_currentWidgetAction == BOTTOM_IMAGE ||
                               m_currentWidgetAction == LEFT_IMAGE ||
                               m_currentWidgetAction == RIGHT_IMAGE)
                            {
                                m_mainData->setCurrentAction(VFV_CURRENT_ACTION_MOVING);
                            }

                            else if(m_currentWidgetAction == NO_IMAGE)
                                m_mainData->setCurrentAction(VFV_CURRENT_ACTION_ROTATING);
                            else
                                m_mainData->setCurrentAction(VFV_CURRENT_ACTION_SCALING);
                        }
                        else
                        {
                            m_mainData->setCurrentAction(VFV_CURRENT_ACTION_MOVING);
                        }
                        break;
                    }

                    case RESIZE:
                        //Redo the viewport
                        glViewport(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight());
                        placeWidgets();
                        placeCamera(true);
                        break;

                    case TOUCH_MOVE:
                    {
                        if(m_currentVis)
                        {
                            m_surfaceData->lock();
                                handleTouchAction(&event->touchEvent);
                            m_surfaceData->unlock();
                        }
                        break;
                    }
                    case VISIBILITY:
                    {
                        visible = event->visibility.visibility;
                        m_snapshotCnt = 0;
                        break;
                    }
                    default:
                        LOG_WARNING("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
            }

            //Handle events sent from JNI for our application (application wise)
            handleVFVDataEvent();

            //Apply the model changement (rotation + color)
            for(auto& it : m_modelChanged)
            {
                for(auto sciVis : m_sciVis)
                {
                    if(sciVis->getModel() == it.first)
                    {
                        if(it.second.updateColor)
                        {
                            sciVis->setColorRange(it.first->getMinClamping(), it.first->getMaxClamping(), it.first->getColorMode());
                            sciVis->setTFTexture(m_sciVisTFs[it.first->getColorMode() + m_sciVisDefaultTF[sciVis]]);
                        }
                        if(it.second.updateRotation)
                            sciVis->setRotate(it.first->getGlobalRotate());
                        if(it.second.updateScale)
                            sciVis->setScale(it.first->getScale());
                    }
                }
            }
            

            if(m_currentVis)
            {
                if(m_surfaceData->renderer.getCameraParams().w == 0.0f)
                    m_currentVis->setPosition(m_currentVis->getModel()->getPosition());
                else
                    m_currentVis->setPosition(glm::vec3(0, 0, 0));
            }
            m_modelChanged.clear();

            if(visible)
            {
                placeCamera();

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //Draw the scene
                if(m_currentVis != NULL)
                    m_currentVis->update(&m_surfaceData->renderer);
                if(m_surfaceData->renderer.getCameraParams().w == 1.0) //Orthographic mode
                    for(int i = 0; i < 8; i++)
                        m_3dImageManipGO[i].update(&m_surfaceData->renderer);
                m_surfaceData->renderer.render();

                //Update the snapshot
                if(m_currentVis)
                {
                    m_snapshotCnt++;
                }
                if(m_snapshotCnt == MAX_SNAPSHOT_COUNTER)
                {
                    //Enlarge the pixel array
                    uint32_t snapWidth  = m_surfaceData->renderer.getWidth();
                    uint32_t snapHeight = m_surfaceData->renderer.getHeight();
                    Snapshot* curSnapshot = m_currentVis->getModel()->getSnapshot();

                    if((curSnapshot == NULL || curSnapshot->width != snapWidth || curSnapshot->height != snapHeight)
                        && (snapWidth * snapHeight != 0))
                    {
                        Snapshot* newSnapshot = new Snapshot(snapWidth, snapHeight, (uint32_t*)malloc(sizeof(uint32_t)*snapWidth*snapHeight));
                        m_snapshots[m_currentVis] = std::shared_ptr<Snapshot>(newSnapshot);
                        curSnapshot = newSnapshot;
                        m_currentVis->getModel()->setSnapshot(m_snapshots[m_currentVis]);
                    }

                    //Read pixels
                    glReadPixels(0, 0, snapWidth, snapHeight, GL_RGBA, GL_UNSIGNED_BYTE, curSnapshot->pixels);
                    m_mainData->sendSnapshotEvent(m_currentVis->getModel());
                    m_snapshotCnt = 0;
                }
            }
            else
                usleep(2.0e3);
            m_surfaceData->renderer.swapBuffers();
        }
    }

    void MainVFV::handleTouchAction(TouchEvent* event)
    {
        switch(m_currentWidgetAction)
        {
            case TOP_RIGHT_IMAGE:
            case BOTTOM_RIGHT_IMAGE:
            case TOP_LEFT_IMAGE:
            case BOTTOM_LEFT_IMAGE:
            {
                if(m_currentVis)
                {
                    float factor = sqrt((event->y - event->oldY)*(event->y - event->oldY)+
                                        (event->x - event->oldX)*(event->x - event->oldX));

                    float inv    = (((event->y - event->startY)*(event->y - event->startY)+
                                     (event->x - event->startX)*(event->x - event->startX)) > ((event->startY - event->oldY)*(event->startY - event->oldY)+
                                                                                               (event->startX - event->oldX)*(event->startX - event->oldX))) ? 1.0f : -1.0f;
                    factor *= inv;

                    if(m_currentWidgetAction == TOP_RIGHT_IMAGE ||
                       m_currentWidgetAction == TOP_LEFT_IMAGE)
                        factor *= -1.0f;

                    //Modify the scale
                    glm::vec3 currentScale = m_currentVis->getModel()->getScale();
                    currentScale.x = currentScale.y = currentScale.z = fmax((float)currentScale.x+factor*2.0f, 0.0f);
                    m_currentVis->getModel()->setScale(currentScale);
                    m_mainData->sendScaleEvent(m_currentVis->getModel());
                }
                break;
            }

            case TOP_IMAGE:
            case BOTTOM_IMAGE:
            {
                if(m_currentVis)
                {
                    m_currentVis->getModel()->setPosition(m_currentVis->getModel()->getPosition() + 3.0f*glm::vec3(0.0f, event->y - event->oldY, 0.0f));
                    m_mainData->sendPositionEvent(m_currentVis->getModel());
                }
                break;
            }
            case RIGHT_IMAGE:
            case LEFT_IMAGE:
            {
                if(m_currentVis)
                {
                    m_currentVis->getModel()->setPosition(m_currentVis->getModel()->getPosition() + 3.0f*glm::vec3(event->x - event->oldX, 0.0f, 0.0f));
                    m_mainData->sendPositionEvent(m_currentVis->getModel());
                }
                break;
            }

            case NO_IMAGE:
            {
                if(m_currentVis)
                {
                    //Get the number of fingers down
                    TouchCoord* tc1 = NULL;
                    TouchCoord* tc2 = NULL;

                    uint32_t numberFinger = 0;
                    TouchCoord* tc = NULL;
                    for(uint32_t i = 0; NULL != (tc = m_surfaceData->getTouchCoord(i++));)
                    {
                        if(tc->type != TOUCH_TYPE_UP)
                        {
                            if(tc1 == NULL)
                                tc1 = tc;
                            else if(tc2 == NULL)
                                tc2 = tc;
                            numberFinger++;
                        }
                    }

                    //Z Translation
                    if(numberFinger >= 2)
                    {
                        //Determine if we are having a pitch
                        glm::vec2 vec1(tc1->x - tc1->oldX,
                                       tc1->y - tc1->oldY);

                        glm::vec2 vec2(tc2->x - tc2->oldX,
                                       tc2->y - tc2->oldY);

                        if((vec1.x != 0 || vec1.x != 0) &&
                                (vec2.x != 0 || vec2.x != 0))
                        {
                            vec1 = glm::normalize(vec1);
                            vec2 = glm::normalize(vec2);

                            float cosVec = vec1.x*vec2.x + vec1.y*vec2.y;

                            if(cosVec <= -0.8f)
                            {
                                //Determine the scaling factor
                                float distanceAfter = sqrt((tc1->x - tc2->x) * (tc1->x - tc2->x) +
                                        (tc1->y - tc2->y) * (tc1->y - tc2->y) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                                float distanceBefore = sqrt((tc1->oldX - tc2->oldX) * (tc1->oldX - tc2->oldX) +
                                        (tc1->oldY - tc2->oldY) * (tc1->oldY - tc2->oldY) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                                float distanceOrigin = sqrt((tc1->startX - tc2->startX) * (tc1->startX - tc2->startX) +
                                        (tc1->startY - tc2->startY) * (tc1->startY - tc2->startY) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                                float factor = distanceAfter - distanceBefore;

                                m_currentVis->getModel()->setPosition(m_currentVis->getModel()->getPosition() + 3.0f*glm::vec3(0.0f, 0.0f, factor));
                                m_mainData->sendPositionEvent(m_currentVis->getModel());
                            }
                        }
                    }

                    //Rotation
                    else
                    {
                        float roll  = event->x - event->oldX;
                        float pitch = event->y - event->oldY;
                        pitch = 0; //Disable pitch rotation

                        m_currentVis->getModel()->setGlobalRotate(Quaternionf(roll, pitch, 0)*m_currentVis->getRotate());
                        m_mainData->sendRotationEvent(m_currentVis->getModel());
                    }
                }
                break;
            }

            default:
            {}
        }
    }

    void MainVFV::handleVFVDataEvent()
    {
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
                    addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(true, false, false, false));
                    break;
                }

                case VFV_SET_ROTATION_DATA:
                {
                    addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(false, true, false, false));
                    break;
                }

                case VFV_SET_POSITION_DATA:
                {
                    addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(false, false, false, true));
                    break;
                }

                case VFV_SET_SCALE_DATA:
                {
                    addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(false, false, true, false));
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
    }

    void MainVFV::addSubDataChangement(const SubDataset* sd, const SubDatasetChangement& sdChangement)
    {
        auto it = m_modelChanged.find(sd);
        if(it == m_modelChanged.end())
            m_modelChanged.insert(std::pair<const SubDataset*, SubDatasetChangement>(sd, sdChangement));
        else
            for(int i = 0; i < sizeof(sdChangement._data)/sizeof(sdChangement._data[0]); i++)
                m_modelChanged[m_currentVis->getModel()]._data[i] |= sdChangement._data[i];
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
