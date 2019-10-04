#include "MainVFV.h"
#include "jniData.h"

namespace sereno
{
    MainVFV::MainVFV(GLSurfaceViewData* surfaceData, ANativeWindow* nativeWindow, VFVData* mainData) : m_surfaceData(surfaceData), m_mainData(mainData)
    {
        surfaceData->renderer.initializeContext(nativeWindow);

        Blend transparency;
        transparency.enable = true;
        transparency.sFactor = GL_SRC_ALPHA;
        transparency.dFactor = GL_ONE_MINUS_SRC_ALPHA;

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
        m_colorGridMtl->setBlend(transparency);
        m_colorGridMtl->setDepthWrite(false);
        m_colorPhongMtl = new PhongMaterial(&surfaceData->renderer, Color::BLUE_COLOR, 0.3f, 0.7f, 0.1f, 100);
        m_3dTextureMtl = (SimpleTextureMaterial*)malloc(sizeof(SimpleTextureMaterial)*8);
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
            new(m_3dTextureMtl+i) SimpleTextureMaterial(&surfaceData->renderer);
            m_3dTextureMtl[i].bindTexture(m_3dImageManipTex[i].getTextureID(), 2, 0);
            new(m_3dImageManipGO+i) DefaultGameObject(NULL, &surfaceData->renderer, m_3dTextureMtl+i, m_gpuTexVBO);
        }

        //Not connected texture logo
        uint32_t texWidth, texHeight;
        std::string texPath = surfaceData->dataPath + "/Images/" + "notConnected.png";
        uint8_t* texData    = getPNGRGBABytesFromFiles(texPath.c_str(), &texWidth, &texHeight);
        m_notConnectedTex   = new Texture(texWidth, texHeight, texData);
        m_notConnectedTextureMtl = new SimpleTextureMaterial(&surfaceData->renderer);
        m_notConnectedGO         = new DefaultGameObject(NULL, &surfaceData->renderer, m_notConnectedTextureMtl, m_gpuTexVBO);
        m_notConnectedTextureMtl->bindTexture(m_notConnectedTex->getTextureID(), 2, 0);
        m_notConnectedTextureMtl->setBlend(transparency);
        m_notConnectedTextureMtl->setDepthWrite(false);
        free(texData);
    }

    MainVFV::~MainVFV()
    {
        for(VectorField* vf : m_vectorFields)
            if(vf)
                delete vf;
        for(GLuint tex : m_sciVisTFTextures)
            glDeleteTextures(1, &tex);
        for(auto& it : m_sciVisTFs)
            delete(it.second);

        delete m_arrowMesh;
        delete m_vfMtl;
        delete m_gpuTexVBO;
        for(int i = 0; i < 8; i++)
        {
            m_3dTextureMtl[i].~SimpleTextureMaterial();
            m_3dImageManipTex[i].~Texture();
            m_3dImageManipGO[i].~DefaultGameObject();
        }
        free(m_3dImageManipTex);
        free(m_3dImageManipGO);
        delete m_notConnectedGO;
        delete m_notConnectedTextureMtl;
        delete m_notConnectedTex;
    }

    void MainVFV::placeWidgets()
    {
        //Nothing to do
        if(m_surfaceData->renderer.getWidth() == 0.0)
            return;

        //Variables needed
        float width  = m_surfaceData->renderer.getWidth();
        float height = m_surfaceData->renderer.getHeight();
        float ratio  = height/width;

        float widgetWidth  = 2.0f*WIDGET_WIDTH_PX/width;
        float widgetHeight = 2.0*ratio - 2.0*widgetWidth;

        //3D texture manipulation widgets
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

        //Not connected logo
        m_notConnectedGO->setPosition(glm::vec3(0.0f, 0.0f, -1.0f));
        m_notConnectedGO->setScale(glm::vec3(2.0f-widgetWidth, 2.0f-widgetWidth, 2.0f));
    }

    bool MainVFV::findHeadsetCameraTransformation(glm::vec3* outPos, Quaternionf* outRot)
    {
        bool found = false;
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

        uint32_t numberFinger = 0;
        TouchCoord* tc = NULL;
        for(uint32_t i = 0; NULL != (tc = m_surfaceData->getTouchCoord(i++));)
            if(tc->type != TOUCH_TYPE_UP)
                numberFinger++;

        if(m_currentWidgetAction == NO_IMAGE && numberFinger <= 1)
        {
            //Cancel the animation
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
        }
        else
        {
            //Go into an animation
            if(!m_inAnimation || forceReset)
            {
                m_surfaceData->renderer.setPerspectiveMatrix(45.0f*3.14f/180.0f, ((float)m_surfaceData->renderer.getWidth())/m_surfaceData->renderer.getHeight(), 0.1, 100.0f, false);
                if(m_currentVis)
                {
                    m_animationStartingPoint = m_currentVis->getPosition();
                    if(!findHeadsetCameraTransformation(&m_animationEndingPoint, &m_animationRotation))
                    {
                        m_animationEndingPoint = glm::vec3(0, 0, -10);
                        m_animationRotation    = Quaternionf();
                        m_surfaceData->renderer.getCameraTransformable().setRotate(m_animationRotation);
                    }
                    else
                    {
                        m_surfaceData->renderer.getCameraTransformable().setRotate(m_animationRotation);
                        glm::vec3 eulerAngles = m_animationRotation.toEulerAngles();
                        eulerAngles.x = eulerAngles.z = 0;
                        eulerAngles.y *= -1.0f;
                        m_animationRotation = Quaternionf::fromEulerAngles(eulerAngles);
                    }
                }
                m_animationTimer = 0;
            }

            if(m_currentVis && m_animationTimer <= MAX_CAMERA_ANIMATION_TIMER)
            {
                glm::vec3 dir = m_animationEndingPoint - m_animationStartingPoint;
                m_surfaceData->renderer.getCameraTransformable().setPosition(((float)m_animationTimer)/MAX_CAMERA_ANIMATION_TIMER*dir);

                //Old version using look at
                //lookAt(m_surfaceData->renderer.getCameraTransformable(), glm::vec3(0.0f, 1.0f, 0.0f), ((float)m_animationTimer)/MAX_CAMERA_ANIMATION_TIMER*dir, m_animationStartingPoint, false);
                m_animationTimer++;
            }
            m_inAnimation = true;
        }
    }

    void MainVFV::run()
    {
        glViewport(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight());

        glEnable(GL_DEPTH_TEST);

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
                        if(m_mainData->getHeadsetID() == -1)
                            break;
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

            m_mainData->lock();
            //Handle events sent from JNI for our application (application wise)
            handleVFVDataEvent();

            //Apply the model changement (rotation + color)
            for(auto& it : m_modelChanged)
            {
                const SubDataset* sd = it.first;

                for(auto sciVis : m_sciVis)
                {
                    if(sciVis->getModel() == sd)
                    {
                        if(it.second.updateColor)
                        {
                            sciVis->setColorRange(sd->getMinClamping(), sd->getMaxClamping());
                        }
                        if(it.second.updateRotation)
                            sciVis->setRotate(sd->getGlobalRotate());
                        if(it.second.updateScale)
                            sciVis->setScale(sd->getScale());
                    }
                }
            }

            if(m_currentVis)
            {
                if(m_surfaceData->renderer.getCameraParams().w == 0.0f)
                {
                    SubDataset* sd = m_currentVis->getModel();

                    if(sd != NULL)
                        m_currentVis->setPosition(sd->getPosition());
                }
                else
                    m_currentVis->setPosition(glm::vec3(0, 0, 0));
            }
            m_modelChanged.clear();

            if(visible)
            {
                placeCamera();

                glDepthMask(true);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //Draw the scene
                if(m_surfaceData->renderer.getCameraParams().w == 1.0) //Orthographic mode
                {
                    for(int i = 0; i < 8; i++)
                        m_3dImageManipGO[i].update(&m_surfaceData->renderer);
                    if(m_mainData->getHeadsetID() == -1)
                        m_notConnectedGO->update(&m_surfaceData->renderer);
                }

                if(m_currentVis != NULL)
                    m_currentVis->update(&m_surfaceData->renderer);

                m_surfaceData->renderer.render();

                //Update the snapshot. It will have "one frame" behind the current one... never mind!
                if(m_currentVis)
                {
                    m_snapshotCnt++;
                }
                if(m_snapshotCnt == MAX_SNAPSHOT_COUNTER)
                {
                    SubDataset* sd  = m_currentVis->getModel();

                    if(sd != NULL)
                    {
                        //Enlarge the pixel array
                        uint32_t snapWidth  = m_surfaceData->renderer.getWidth();
                        uint32_t snapHeight = m_surfaceData->renderer.getHeight();
                        Snapshot* curSnapshot = sd->getSnapshot();

                        if((curSnapshot == NULL || curSnapshot->width != snapWidth || curSnapshot->height != snapHeight)
                            && (snapWidth * snapHeight != 0))
                        {
                            Snapshot* newSnapshot = new Snapshot(snapWidth, snapHeight, (uint32_t*)malloc(sizeof(uint32_t)*snapWidth*snapHeight));
                            m_snapshots[m_currentVis] = std::shared_ptr<Snapshot>(newSnapshot);
                            curSnapshot = newSnapshot;
                            sd->setSnapshot(m_snapshots[m_currentVis]);
                        }

                        //Read pixels
                        glReadPixels(0, 0, snapWidth, snapHeight, GL_RGBA, GL_UNSIGNED_BYTE, curSnapshot->pixels);
                        m_mainData->sendSnapshotEvent(sd);
                        m_snapshotCnt = 0;
                    }
                }

            }
            m_mainData->unlock();
            if(!visible)
            {
                usleep(2.0e3);
            }
            m_surfaceData->renderer.swapBuffers();
        }
    }

    void MainVFV::handleTouchAction(TouchEvent* event)
    {
        if(m_mainData->getHeadsetID() == -1)
            return;
        bool inMovement = false;
        glm::vec3 movement;

        if(!m_currentVis)
            return;

        SubDataset* sd = m_currentVis->getModel();
        if(sd == NULL)
        {
            m_mainData->unlock();
            return;
        }

        switch(m_currentWidgetAction)
        {
            case TOP_RIGHT_IMAGE:
            case BOTTOM_RIGHT_IMAGE:
            case TOP_LEFT_IMAGE:
            case BOTTOM_LEFT_IMAGE:
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
                glm::vec3 currentScale = sd->getScale();
                currentScale.x = currentScale.y = currentScale.z = fmax((float)currentScale.x+factor*2.0f, 0.0f);
                sd->setScale(currentScale);
                m_mainData->sendScaleEvent(sd);
                break;
            }

            case TOP_IMAGE:
            case BOTTOM_IMAGE:
            {
                inMovement = true;
                movement = 3.0f*glm::vec3(0.0f, event->y - event->oldY, 0.0f);
                break;
            }
            case RIGHT_IMAGE:
            case LEFT_IMAGE:
            {
                inMovement = true;
                movement = 3.0f*glm::vec3(event->x - event->oldX, 0.0f, 0.0f);
                break;
            }

            case NO_IMAGE:
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

                    if((vec1.x != 0 || vec1.y != 0) &&
                       (vec2.x != 0 || vec2.y != 0))
                    {
                        vec1 = glm::normalize(vec1);
                        vec2 = glm::normalize(vec2);

                        //Determine the scaling factor
                        float distanceAfter = sqrt((tc1->x - tc2->x) * (tc1->x - tc2->x) +
                                (tc1->y - tc2->y) * (tc1->y - tc2->y) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                        float distanceBefore = sqrt((tc1->oldX - tc2->oldX) * (tc1->oldX - tc2->oldX) +
                                (tc1->oldY - tc2->oldY) * (tc1->oldY - tc2->oldY) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                        float distanceOrigin = sqrt((tc1->startX - tc2->startX) * (tc1->startX - tc2->startX) +
                                (tc1->startY - tc2->startY) * (tc1->startY - tc2->startY) * m_surfaceData->renderer.getHeight() / m_surfaceData->renderer.getWidth());

                        float factor = distanceAfter - distanceBefore;

                        inMovement = true;
                        movement   = 3.0f*glm::vec3(0.0f, 0.0f, factor);
                    }
                }

                //Rotation
                else
                {
                    //Allow a full rotation in only one movement
                    float roll  = (event->x - event->oldX)*M_PI;
                    float pitch = (event->y - event->oldY)*M_PI;
                    pitch = 0; //Disable pitch rotation

                    sd->setGlobalRotate(Quaternionf(roll, pitch, 0)*m_currentVis->getRotate());
                    m_mainData->sendRotationEvent(sd);
                }
                break;
            }

            default:
            {}
        }

        //Move the dataset if asked
        Quaternionf cameraRot(0, 0, 0, 1.0);
        if(inMovement)
        {
            findHeadsetCameraTransformation(NULL, &cameraRot);
            glm::vec3 newPos = m_animationRotation.rotateVector(movement);
            LOG_INFO("old Movement : %f %f %f, new Movement : %f %f %f", movement.x, movement.y, movement.z,
                                                                         newPos.x,   newPos.y,   newPos.z);
            sd->setPosition(sd->getPosition() + newPos);
            m_mainData->sendPositionEvent(sd);
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
                        //Compute the Transfer Function
                        TF *tf = new TF(2, WARM_COLD_CIELAB);
                        uint32_t texSize[2] = {256, 256};
                        GLuint texture = generateTexture(texSize, *tf);
                        m_sciVisTFTextures.push_back(texture);

                        //Create the visualization
                        m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_vfMtl, NULL,
                                                                 event->binaryData.dataset, m_arrowMesh, 
                                                                 texture, 2));
                        m_sciVis.push_back(m_vectorFields.back());
                        m_sciVis.back()->getModel()->setTransferFunction(tf);
                        m_sciVisTFs.insert(std::pair<SubDataset*, TF*>(m_sciVis.back()->getModel(), tf));

                        //Set the snapshot
                        std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(NULL));
                        m_snapshots.insert(snap);
                        m_sciVis.back()->getModel()->setSnapshot(snap.second);
                        addSubDataChangement(m_sciVis.back()->getModel(), SubDatasetChangement(true, true, true, true));


                        if(m_currentVis == NULL)
                            m_currentVis = m_sciVis[0];
                        break;
                    }

                //Add VTK Dataset
                case VFV_ADD_VTK_DATA:
                    m_vtkStructuredGridPoints.push_back(new VTKStructuredGridPointSciVis(&m_surfaceData->renderer, m_colorGridMtl, event->vtkData.dataset, VTK_STRUCTURED_POINT_VIS_DENSITY,
                                                                                         0, 2));
                    m_colorGridMtl->setSpacing(m_vtkStructuredGridPoints.back()->vbo->getSpacing());
                    float dim[3];
                    for(uint8_t i = 0; i < 3; i++)
                        dim[i] = m_vtkStructuredGridPoints.back()->vbo->getDimensions()[i];
                    m_colorGridMtl->setDimension(dim);

                    for(uint32_t i = 0; i < event->vtkData.dataset->getNbSubDatasets(); i++)
                    {
                        m_sciVis.push_back(m_vtkStructuredGridPoints.back()->gameObjects[i]);

                        //Set the transfer function
                        TriangularGTF* tGTF = new TriangularGTF(2, RAINBOW);
                        uint32_t texSize[2] = {256, 256};
                        GLuint texture = generateTexture(texSize, *tGTF);
                        m_sciVis.back()->setTFTexture(texture);
                        m_sciVis.back()->getModel()->setTransferFunction(tGTF);
                        m_sciVis.back()->onTFChange();
                        m_sciVisTFTextures.push_back(texture);
                        m_sciVisTFs.insert(std::pair<SubDataset*, TF*>(m_sciVis.back()->getModel(), tGTF));

                        //Update the snapshot
                        std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(nullptr));
                        m_snapshots.insert(snap);
                        m_sciVis.back()->getModel()->setSnapshot(snap.second);

                        addSubDataChangement(m_sciVis.back()->getModel(), SubDatasetChangement(true, true, true, true));
                    }
                    if(m_currentVis == NULL)
                        m_currentVis = m_sciVis[0];
                    break;

                case VFV_REMOVE_DATASET:
                {
                    for(uint32_t i = 0; i < event->dataset.dataset.get()->getNbSubDatasets(); i++)
                        removeSubDataset(event->dataset.dataset.get()->getSubDataset(i));

                    //Check if there is "empty" VTKStructuredGridPointSciVis Object to delete
                    for(uint32_t i = 0; i < m_vtkStructuredGridPoints.size(); i++)
                    {
                        if(m_vtkStructuredGridPoints[i]->nbGameObjects == 0)
                        {
                            delete m_vtkStructuredGridPoints[i];
                            auto it = m_vtkStructuredGridPoints.begin();
                            std::advance(it, i);
                            m_vtkStructuredGridPoints.erase(it);
                            i--;
                        }
                    }

                    break;
                }

                case VFV_REMOVE_SUBDATASET:
                {
                    removeSubDataset(event->sdEvent.sd);
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
                    m_currentVis = NULL;
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

    void MainVFV::removeSubDataset(SubDataset* sd)
    {
        //Remove the bound scientific visualizations
        for(uint32_t i = 0; i < m_sciVis.size(); i++)
            if(m_sciVis[i]->getModel() == sd)
                removeSciVis(m_sciVis[i]);
    }

    void MainVFV::removeSciVis(SciVis* sciVis)
    {
        //Check if the current sci vis is being deleted
        if(m_currentVis == sciVis)
            m_currentVis = NULL;

        //Check vector field
        for(std::vector<VectorField*>::iterator it = m_vectorFields.begin(); it != m_vectorFields.end(); it++)
            if((*it) == sciVis)
            {
                m_vectorFields.erase(it);
                break;
            }

        //Check default visualization
        for(std::vector<DefaultSciVis*>::iterator it = m_defaultSciVis.begin(); it != m_defaultSciVis.end(); it++)
            if((*it) == sciVis)
            {
                m_defaultSciVis.erase(it);
                break;
            }

        //Check VTK data
        //We do not "delete" the game Objects because it will be at the end of the function
        for(auto it : m_vtkStructuredGridPoints)
        {
            for(int i = 0; i < it->nbGameObjects; i++)
                if(it->gameObjects[i] == sciVis)
                {
                    for(int j = i; j < it->nbGameObjects-1; j++)
                        it->gameObjects[j] = it->gameObjects[j+1];

                    it->nbGameObjects--;
                    goto endForVTK;
                }
        }
endForVTK:

        //Remove the link of this scivis object
        for(std::vector<SciVis*>::iterator it = m_sciVis.begin(); it != m_sciVis.end(); it++)
            if((*it) == sciVis)
                {
                    m_sciVis.erase(it);
                    break;
                }

        //Delete the scivis object
        delete sciVis;

        //Re assign a new current visualization
        if(m_sciVis.size() > 0)
            m_currentVis = m_sciVis[0];
    }
}
