#include "MainVFV.h"
#include "jniData.h"
#include "utils.h"

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

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
        m_3dTextureMtl  = (SimpleTextureMaterial*)malloc(sizeof(SimpleTextureMaterial)*8);
        m_gpuTexVBO     = new TextureRectangleData();
        m_cpcpMtl       = new CPCPMaterial(&m_surfaceData->renderer, HISTOGRAM_WIDTH*sqrt(2));
        m_normalizeMtl  = new NormalizeMaterial(&m_surfaceData->renderer);
        m_redToGrayMtl  = new RedToGrayMaterial(&m_surfaceData->renderer);
        m_lassoMaterial = new UniColorMaterial(&m_surfaceData->renderer, Color::YELLOW_COLOR);
        m_currentVisFBOMtl = new SimpleTextureMaterial(&surfaceData->renderer);

        //Load 3D images used to translate/rotate/scale the 3D datasets
        m_3dImageManipTex = (Texture2D*)malloc(sizeof(Texture2D)*8);
        m_3dImageManipGO = (DefaultGameObject*)malloc(sizeof(DefaultGameObject)*8);
        for(int i = 0; i < 8; i++)
        {
            uint32_t texWidth, texHeight;
            std::string texPath = surfaceData->dataPath + "/Images/" + volumeImageManipPath[i];
            uint8_t* texData = getPNGRGBABytesFromFiles(texPath.c_str(), &texWidth, &texHeight);
            new(m_3dImageManipTex+i) Texture2D(texWidth, texHeight, texData);
            free(texData);
            new(m_3dTextureMtl+i) SimpleTextureMaterial(&surfaceData->renderer);
            m_3dTextureMtl[i].bindTexture(m_3dImageManipTex[i].getTextureID(), 2, 0);
            new(m_3dImageManipGO+i) DefaultGameObject(NULL, &surfaceData->renderer, m_3dTextureMtl+i, m_gpuTexVBO);
        }

        //Not connected texture logo
        uint32_t texWidth, texHeight;
        std::string texPath = surfaceData->dataPath + "/Images/" + "notConnected.png";
        uint8_t* texData    = getPNGRGBABytesFromFiles(texPath.c_str(), &texWidth, &texHeight);
        m_notConnectedTex   = new Texture2D(texWidth, texHeight, texData);
        m_notConnectedTextureMtl = new SimpleTextureMaterial(&surfaceData->renderer);
        m_notConnectedGO         = new DefaultGameObject(NULL, &surfaceData->renderer, m_notConnectedTextureMtl, m_gpuTexVBO);
        m_notConnectedTextureMtl->bindTexture(m_notConnectedTex->getTextureID(), 2, 0);
        m_notConnectedTextureMtl->setBlend(transparency);
        m_notConnectedTextureMtl->setDepthWrite(false);
        free(texData);

        //Volume selection lasso
        m_lasso = new Lasso(NULL, &surfaceData->renderer, m_lassoMaterial);
        m_selecting = false;
        m_tabletPos = glm::vec3(0.0f, 0.0f, 0.0f);
        m_tabletRot = Quaternionf(0.0f, 0.0f, 0.0f, 1.0f);
        m_tabletScale = 1;

        //Load CPCP data
        m_rawCPCPFBO      = new FBO(CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, GL_R32F, false);
        m_cpcpFBORenderer = new FBORenderer(m_rawCPCPFBO);

        //Current visualization data
        m_currentVisFBO         = new FBO(VIS_FBO_WIDTH, VIS_FBO_HEIGHT, GL_RGBA8, false);
        m_currentVisFBORenderer = new FBORenderer(m_currentVisFBO);
        m_currentVisFBOMtl->bindTexture(m_currentVisFBO->getColorBuffer(), 2, 0);
        m_currentVisFBOGO       = new DefaultGameObject(NULL, &surfaceData->renderer, m_currentVisFBOMtl, m_gpuTexVBO);
        m_currentVisFBOGO->scale(glm::vec3(2.0f, 2.0f, 2.0f));
        m_currentVisFBOGO->setEnableCamera(false);
    }

    MainVFV::~MainVFV()
    {
        /*----------------------------------------------------------------------------*/
        /*----------------------------Delete visualization----------------------------*/
        /*----------------------------------------------------------------------------*/
        for(VectorField* vf : m_vectorFields)
            if(vf)
                delete vf;
        for(VTKStructuredGridPointSciVis* vis : m_vtkStructuredGridPoints) //This ensure that the parallel threads have finished (because of a join call)
            delete vis;

        delete m_currentVisFBO;
        delete m_currentVisFBORenderer;
        delete m_currentVisFBOGO;

        /*----------------------------------------------------------------------------*/
        /*------------------------------Delete materials------------------------------*/
        /*----------------------------------------------------------------------------*/
        delete m_vfMtl;
        delete m_colorGridMtl;
        delete m_notConnectedTextureMtl;
        delete m_colorPhongMtl;
        delete m_cpcpMtl;
        delete m_redToGrayMtl;
        delete m_lassoMaterial;
        delete m_currentVisFBOMtl;

        /*----------------------------------------------------------------------------*/
        /*-----------------Delete the 4 DOF manipulation texture data-----------------*/
        /*----------------------------------------------------------------------------*/
        delete m_arrowMesh;
        delete m_gpuTexVBO;
        for(int i = 0; i < 8; i++)
        {
            m_3dTextureMtl[i].~SimpleTextureMaterial();
            m_3dImageManipTex[i].~Texture2D();
            m_3dImageManipGO[i].~DefaultGameObject();
        }
        free(m_3dImageManipTex);
        free(m_3dImageManipGO);
        delete m_notConnectedGO;
        delete m_notConnectedTex;

        delete m_lasso;

        /*----------------------------------------------------------------------------*/
        /*------------------------------Delete FBO data-------------------------------*/
        /*----------------------------------------------------------------------------*/
        delete m_rawCPCPFBO;
        delete m_cpcpFBORenderer;
    }

    void MainVFV::runOnMainThread(const MainThreadFunc& func)
    {
        std::lock_guard<std::mutex> lock(m_mainThreadFuncsMutex);
        m_mainThreadFuncs.push(func);
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

        if(m_selecting){
                m_surfaceData->renderer.setOrthographicMatrix(-m_tabletScale/2.0f*m_surfaceData->renderer.getWidth(), m_tabletScale/2.0f*m_surfaceData->renderer.getWidth(),
                                                              -m_tabletScale/2.0f * ((float)m_surfaceData->renderer.getHeight()),
                                                               m_tabletScale/2.0f * ((float)m_surfaceData->renderer.getHeight()),
                                                              0.0f, 100.0f, true);
                m_surfaceData->renderer.getCameraTransformable().setPosition(m_tabletPos);
                m_surfaceData->renderer.getCameraTransformable().setRotate(m_tabletRot);
        }
        else if((m_currentWidgetAction == NO_IMAGE && numberFinger <= 1) || !m_curSDCanBeModified)
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
        m_surfaceData->renderer.setViewport(Rectangle2i(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight()));

        glEnable(GL_DEPTH_TEST);
        bool     visible = true;

        while(!m_surfaceData->isClosed())
        {
            m_mainData->lock();
            //Handle events sent from JNI for our application (application wise)
            handleVFVDataEvent();

            //Check the status about the modificability of our current subdataset
            if(!m_currentVis || !visible)
                m_curSDCanBeModified = false;
            else
                m_curSDCanBeModified = m_mainData->canSubDatasetBeModified(m_currentVis->getModel());

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
                            if(m_selecting)
                            {
                                if(m_lasso->endLasso()){
                                    m_mainData->setLasso(m_lasso->getData());
                                    m_mainData->setCurrentAction(VFV_CURRENT_ACTION_SELECTING);
                                }
                            }
                            else
                            {
                                m_mainData->setCurrentAction(VFV_CURRENT_ACTION_NOTHING);
                            }
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

                            if(m_selecting)
                            {
                                m_lasso->startLasso(x, y/ratio, 0);
                            }
                            else
                            {
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
                        }
                        else
                        {
                            m_mainData->setCurrentAction(VFV_CURRENT_ACTION_MOVING);
                        }
                        break;
                    }

                    case RESIZE:
                        //Redo the viewport
                        m_surfaceData->renderer.setViewport(Rectangle2i(0, 0, m_surfaceData->renderer.getWidth(), m_surfaceData->renderer.getHeight()));
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
                    case SELECTION:
                    {
                        m_selecting = event->selection.starting;
                        if(event->selection.starting)
                        {
                            m_mainData->setCurrentAction(VFV_CURRENT_ACTION_LASSO);
                        }
                        else
                        {
                            m_mainData->setCurrentAction(VFV_CURRENT_ACTION_NOTHING);
                            m_lasso->clearLasso();
                        }
                    }
                    default:
                        LOG_WARNING("type %d still has to be done\n", event->type);
                        break;
                }
                delete event;
            }

            //Update the snapshot. It will have "one frame" behind the current one... never mind!
            if(visible && m_currentVis)
            {
                if(m_snapshotCnt == MAX_SNAPSHOT_COUNTER)
                {
                    SubDataset* sd  = m_currentVis->getModel();

                    if(sd != NULL)
                    {
                        //Enlarge the pixel array
                        uint32_t snapWidth  = m_surfaceData->renderer.getWidth();
                        uint32_t snapHeight = m_surfaceData->renderer.getHeight();
                        std::shared_ptr<const Snapshot> curSnapshot = sd->getSnapshot();

                        if((curSnapshot == NULL || curSnapshot->width != snapWidth || curSnapshot->height != snapHeight)
                            && (snapWidth * snapHeight != 0))
                        {
                            Snapshot* newSnapshot = new Snapshot(snapWidth, snapHeight, (uint32_t*)malloc(sizeof(uint32_t)*snapWidth*snapHeight));
                            m_snapshots[m_currentVis] = std::shared_ptr<Snapshot>(newSnapshot);
                            curSnapshot = m_snapshots[m_currentVis];
                            sd->setSnapshot(m_snapshots[m_currentVis]);
                        }

                        //Read pixels
                        glReadPixels(0, 0, snapWidth, snapHeight, GL_RGBA, GL_UNSIGNED_BYTE, curSnapshot->pixels);
                        m_mainData->sendSnapshotEvent(sd);
                        m_snapshotCnt = 0;
                    }
                }
            }

            //Clear GL state
            glDepthMask(true);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //Run jobs in main thread
            {
                while(!m_mainThreadFuncs.empty())
                {
                    //Cut the job like this for not blocking the other thread too much
                    m_mainThreadFuncsMutex.lock();
                        auto func = m_mainThreadFuncs.front();
                        m_mainThreadFuncs.pop();
                    m_mainThreadFuncsMutex.unlock();
                    func();
                }
            }

            m_mainData->unlock();

            if(m_currentVis)
            {
                //Apply the model changement for the current visualization (transform + color)
                //Change only the current one because changing current visualization also will update its color/transformation (see set_current_subdataset)
                for(auto& it : m_modelChanged)
                {
                    const SubDataset* sd = it.first;
                    if(m_currentVis && m_currentVis->getModel() == sd)
                    {
                        if(it.second.updateColor)
                            m_currentVis->onTFChanged();
                        if(it.second.updateRotation)
                            m_currentVis->setRotate(sd->getGlobalRotate());
                        if(it.second.updateScale)
                            m_currentVis->setScale(glm::vec3(sd->getScale().x*-1, sd->getScale().y, sd->getScale().z));
                        break;
                    }
                }

                if(m_surfaceData->renderer.getCameraParams().w == 0.0f && m_selecting)
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

                //Draw the scene
                if(m_surfaceData->renderer.getCameraParams().w == 1.0 && !m_selecting) //Orthographic mode
                {
                    for(int i = 0; i < 8; i++)
                        m_3dImageManipGO[i].update(&m_surfaceData->renderer);
                    if(!m_curSDCanBeModified)
                        m_notConnectedGO->update(&m_surfaceData->renderer);
                }

                m_lasso->update(&m_surfaceData->renderer);

                if(m_currentVis != NULL)
                {
                    m_currentVisFBORenderer->setCameraData(m_surfaceData->renderer.getCameraTransformable(), m_surfaceData->renderer.getProjectionMatrix(), m_surfaceData->renderer.getCameraParams());
                    m_currentVis->update(m_currentVisFBORenderer);
                    m_currentVisFBORenderer->render();
                    m_currentVisFBOGO->update(&m_surfaceData->renderer);
                }

                m_surfaceData->renderer.render();
            }

            if(visible && m_currentVis)
                m_snapshotCnt++;

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
        if(!sd || !m_curSDCanBeModified) //We want to modify a SubDataset...
            return;

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
                m_mainData->sendScaleEvent(sd, currentScale);
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
                
                //selection
                if(m_selecting){
                    m_lasso->continueLasso(event->x, event->y, 0);
                }

                //Z Translation
                else if(numberFinger >= 2)
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

                    Quaternionf q = Quaternionf(roll, pitch, 0)*m_currentVis->getRotate();
                    m_mainData->sendRotationEvent(sd, q);
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
            m_mainData->sendPositionEvent(sd, sd->getPosition() + newPos);
        }
    }

    void MainVFV::onLoadVTKDataset(VTKDataset* dataset, uint32_t status)
    {
        onLoadDataset(dataset, status);
    }

    void MainVFV::onLoadDataset(Dataset* dataset, uint32_t status)
    {
        LOG_INFO("Creating dataset's histograms");
        //Create every possible 2D histograms
        uint32_t ptDescSize = dataset->getPointFieldDescs().size();
        bool     loaded     = true;
        if(ptDescSize > 1)
        {
            for(uint32_t i = 0; i < ptDescSize-1; i++)
            {
                for(uint32_t j = i+1; j < ptDescSize; j++)
                {
                    uint32_t* histogram = (uint32_t*)malloc(sizeof(uint32_t)*HISTOGRAM_WIDTH*HISTOGRAM_HEIGHT);
                    if(!dataset->create2DHistogram(histogram, HISTOGRAM_WIDTH, HISTOGRAM_HEIGHT, dataset->getPointFieldDescs()[i].id, dataset->getPointFieldDescs()[j].id))
                    {
                        LOG_WARNING("Could not create histogram between property %s and %s\n", dataset->getPointFieldDescs()[i].name.c_str(), dataset->getPointFieldDescs()[j].name.c_str());
                        loaded = false;
                        free(histogram);
                        continue;
                    }

                    LOG_INFO("Creating an histogram between %s and %s\n", dataset->getPointFieldDescs()[i].name.c_str(), dataset->getPointFieldDescs()[j].name.c_str());

                    //Normalize values in a float array object
                    //Check type. This is used to earn time regarding histogram* size

                    //First get max element
                    static_assert(sizeof(uint32_t) == sizeof(float), "sizeof(float) != sizeof(uint32_t)");
                    uint32_t maxVal = 0;
#if defined(_OPENMP)
                    #pragma omp parallel for reduction(max:maxVal)
#endif
                    for(uint32_t k = 0; k < HISTOGRAM_WIDTH*HISTOGRAM_HEIGHT; k++)
                        maxVal = (histogram[k] > maxVal ? histogram[k] : maxVal);

#if defined(_OPENMP)
                    #pragma omp parallel for
#endif
                    for(uint32_t k = 0; k < HISTOGRAM_WIDTH*HISTOGRAM_HEIGHT; k++)
                        ((float*)histogram)[k] = ((float)histogram[k])/(float)maxVal; //This work because sizeof(float) == sizeof(uint32_t)

                    //Run a job to make them as Texture objects
                    runOnMainThread([this, dataset, histogram, i, j]()
                    {
                        //Remember that the data of histogram is now "float" (even if declared as a uint32_t, VIVA CASTING OPERATION!).
                        Texture2D* text  = new Texture2D(HISTOGRAM_WIDTH, HISTOGRAM_HEIGHT, histogram, GL_RED, GL_RED, GL_FLOAT);

                        //Draw the corresponding CPCP (Continuous Parallel Coordinate Plot) in a Texture
                        //This texture will not be normalized (i.e., pixels values will not be clamped to [0,1])
                        m_cpcpMtl->bindTexture(text->getTextureID(), 2, 0);
                        m_cpcpFBORenderer->setFBO(m_rawCPCPFBO);
                        DefaultGameObject rawGO(NULL, &m_surfaceData->renderer, m_cpcpMtl, m_gpuTexVBO);
                        rawGO.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
                        rawGO.update(m_cpcpFBORenderer);
                        m_cpcpFBORenderer->render();

                        //Get the min and max values of this newly created texture
                        //And normalize this texture
                        //
                        //TODO optimize this using compute shader if they are available (OpenGL ES 3.X)
                        GLint curFBO;
                        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFBO);
                        glBindFramebuffer(GL_FRAMEBUFFER, m_rawCPCPFBO->getBuffer());
                            void* pixels = malloc(sizeof(float)*CPCP_TEXTURE_WIDTH*CPCP_TEXTURE_HEIGHT);
                            glReadPixels(0, 0, CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, GL_RED, GL_FLOAT, pixels);
                        glBindFramebuffer(GL_FRAMEBUFFER, curFBO);

                        //Parallel reduction to get the max
                        float maxVal = 0;
#ifdef _OPENMP
                        #pragma omp parallel for reduction(max:maxVal)
#endif
                        for(uint32_t k = 0; k < CPCP_TEXTURE_WIDTH*CPCP_TEXTURE_HEIGHT; k++)
                            maxVal = (((float*)pixels)[k] > maxVal ? ((float*)pixels)[k] : maxVal);

                        //Normalize the texture
                        FBO normalizeFBO(CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, GL_RGBA8, false);
                        m_cpcpFBORenderer->setFBO(&normalizeFBO);
                        m_normalizeMtl->setRange(0, maxVal);
                        m_normalizeMtl->bindTexture(m_rawCPCPFBO->getColorBuffer(), 2, 0);
                        DefaultGameObject go(NULL, &m_surfaceData->renderer, m_normalizeMtl, m_gpuTexVBO);
                        go.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
                        go.update(m_cpcpFBORenderer);
                        m_cpcpFBORenderer->render();

                        //Set color to grayscale
                        FBO redToGrayFBO(CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, GL_RGBA8, false);
                        m_cpcpFBORenderer->setFBO(&redToGrayFBO);
                        m_redToGrayMtl->bindTexture(normalizeFBO.getColorBuffer(), 2, 0);
                        go.setMaterial(m_redToGrayMtl);
                        go.update(m_cpcpFBORenderer);
                        m_cpcpFBORenderer->render();

                        m_cpcpFBORenderer->setFBO(NULL); //Reset the FBO to NULL

                        //Generate and add a 2DHistogram to the Dataset's meta data
                        Dataset2DHistogram hist2D;
                        hist2D.texture    = std::shared_ptr<Texture2D>(text);
                        hist2D.pcpTexture = std::shared_ptr<Texture2D>(new Texture2D(redToGrayFBO.stealColorBuffer(), CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT));
                        hist2D.ptFieldID1 = i;
                        hist2D.ptFieldID2 = j;

                        std::shared_ptr<DatasetMetaData> metaData = m_mainData->getDatasetMetaData(m_mainData->getDatasetSharedPtr(dataset));
                        if(metaData.get())
                            metaData->add2DHistogram(hist2D);

                        //Read back the 2DHistogram and send it to the Java thread
                        glBindFramebuffer(GL_FRAMEBUFFER, redToGrayFBO.getBuffer());
                            glReadPixels(0, 0, CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels); //This work because sizeof(float) = =sizeof(uint32_t)
                        glBindFramebuffer(GL_FRAMEBUFFER, curFBO);
                        m_mainData->sendCPCPTexture(m_mainData->getDatasetSharedPtr(dataset), (uint32_t*)pixels, CPCP_TEXTURE_WIDTH, CPCP_TEXTURE_HEIGHT, i, j);

                        //Free allocations
                        free(pixels);
                        free(histogram);
                    });
                }
            }
        }

        //Create every possible 1D histograms
        for(uint32_t i = 0; i < ptDescSize; i++)
        {
            uint32_t* histogram = (uint32_t*)malloc(sizeof(uint32_t)*HISTOGRAM_WIDTH);
            if(!dataset->create1DHistogram(histogram, HISTOGRAM_WIDTH, dataset->getPointFieldDescs()[i].id))
            {
                LOG_WARNING("Could not create histogram of property %s\n", dataset->getPointFieldDescs()[i].name.c_str());
                loaded = false;
                free(histogram);
                continue;
            }

            //Normalize values in a float array object
            //Check type. This is used to earn time regarding histogram* size
            //First get max element
            static_assert(sizeof(uint32_t) == sizeof(float), "sizeof(float) != sizeof(uint32_t)");
            uint32_t maxVal = 0;
#if defined(_OPENMP)
            #pragma omp parallel for reduction(max:maxVal)
#endif
            for(uint32_t k = 0; k < HISTOGRAM_WIDTH; k++)
                maxVal = (histogram[k] > maxVal ? histogram[k] : maxVal);

#if defined(_OPENMP)
            #pragma omp parallel for
#endif
            for(uint32_t k = 0; k < HISTOGRAM_WIDTH; k++)
                ((float*)histogram)[k] = (float)histogram[k]/(float)maxVal;

            //Run a job to make them as Texture objects
            runOnMainThread([this, dataset, histogram, i]()
            {
                //Remember that the data of histogram is now "float" (even if declared as a uint32_t, VIVA casting type!).
                //The histogram is already normalized
                Texture2D* text  = new Texture2D(HISTOGRAM_WIDTH, 1, histogram, GL_RED, GL_RED, GL_FLOAT);

                //Generate and add a 1DHistogram to the Dataset's meta data
                Dataset1DHistogram hist1D;
                hist1D.texture   = std::shared_ptr<Texture2D>(text);
                hist1D.ptFieldID = i;

                std::shared_ptr<DatasetMetaData> metaData = m_mainData->getDatasetMetaData(m_mainData->getDatasetSharedPtr(dataset));
                if(metaData.get())
                    metaData->add1DHistogram(hist1D);

                //Send the 1D histogram to the Java thread.
                m_mainData->send1DHistogram(m_mainData->getDatasetSharedPtr(dataset), (float*)histogram, HISTOGRAM_WIDTH, i);

                free(histogram);
            });
        }

        //Notify Java side that everything is loaded.
        //We can do this in this way because every thread are called one after the other
        runOnMainThread([this, dataset, loaded]()
        {
            //Load first the dataset graphical object
            for(SubDataset* sd : dataset->getSubDatasets())
            {
                SciVis* vis = createVisualization(sd);
                if(vis)
                    vis->load();
                if(m_currentVis == NULL)
                    m_currentVis = vis;
            }

            m_mainData->sendOnDatasetLoaded(m_mainData->getDatasetSharedPtr(dataset), loaded);
        });
    }

    void MainVFV::handleVFVDataEvent()
    {
        while(VFVEvent* event = m_mainData->pollEvent())
        {
            switch(event->getType())
            {
                //Add binary dataset
                case VFV_ADD_VECTOR_FIELD_DATA:
                    if(m_arrowMesh)
                    {
                       /*//Create the visualization
                        m_vectorFields.push_back(new VectorField(&m_surfaceData->renderer, m_vfMtl, NULL,
                                                                 event->binaryData.dataset, m_arrowMesh, 
                                                                 texture, 2));
                        m_sciVis.push_back(m_vectorFields.back());

                        //Set the snapshot
                        std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(NULL));
                        m_snapshots.insert(snap);
                        m_sciVis.back()->getModel()->setSnapshot(snap.second);
                        addSubDataChangement(m_sciVis.back()->getModel(), SubDatasetChangement(true, true, true, true));


                        if(m_currentVis == NULL)
                            m_currentVis = m_sciVis[0];*/
                        break;
                    }

                case VFV_ADD_CLOUD_POINT_DATA:
                {
                    LOG_WARNING("CLOUD POINT DATASET NOT YET HANDLED\n");
                    break;
                }

                //Add VTK Dataset
                case VFV_ADD_VTK_DATA:
                {
                    m_vtkStructuredGridPoints.push_back(new VTKStructuredGridPointSciVis(&m_surfaceData->renderer, m_colorGridMtl, event->vtkData.dataset, VTK_STRUCTURED_POINT_VIS_DENSITY));
                    m_colorGridMtl->setSpacing(m_vtkStructuredGridPoints.back()->vbo->getSpacing());
                    float dim[3];
                    for(uint8_t i = 0; i < 3; i++)
                        dim[i] = m_vtkStructuredGridPoints.back()->vbo->getDimensions()[i];
                    m_colorGridMtl->setDimension(dim);

                    //Load VTK data in a separate thread
                    event->vtkData.dataset->loadValues([](Dataset* dataset, uint32_t status, void* data)
                    {
                        ((MainVFV*)data)->onLoadVTKDataset(reinterpret_cast<VTKDataset*>(dataset), status);
                    }, this);

                    break;
                }

                case VFV_REMOVE_DATASET:
                {
                    for(uint32_t i = 0; i < event->dataset.dataset.get()->getNbSubDatasets(); i++)
                        removeSubDataset(event->dataset.dataset.get()->getSubDatasets()[i]);

                    //Check if there is "empty" VTKStructuredGridPointSciVis Object to delete
                    for(auto it = m_vtkStructuredGridPoints.begin(); it != m_vtkStructuredGridPoints.end(); it++)
                    {
                        if((*it)->dataset == event->dataset.dataset)
                        {
                            delete *it;
                            m_vtkStructuredGridPoints.erase(it);
                            break;
                        }
                    }

                    break;
                }

                case VFV_REMOVE_SUBDATASET:
                {
                    removeSubDataset(event->sdEvent.sd);
                    break;
                }

                case VFV_ADD_SUBDATASET:
                {
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

                case VFV_SET_TF_DATA:
                {
                    addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(true, false, false, false));
                    break;
                }

                case VFV_SET_CURRENT_DATA:
                {
                    //Find which SciVis this sub dataset belongs to and change the current sci vis
                    m_currentVis = createVisualization(event->sdEvent.sd);
                    if(event->sdEvent.sd)
                        addSubDataChangement(event->sdEvent.sd, SubDatasetChangement(true, true, true, true));
                    break;
                }

                case VFV_SET_LOCATION:
                {
                    m_tabletPos = event->setLocation.pos;
                    m_tabletRot = event->setLocation.rot;
                    m_lasso->setPosition(event->setLocation.pos);
                    m_lasso->setRotate(event->setLocation.rot);
                    break;
                }

                case VFV_SET_TABLET_SCALE:
                {
                    m_tabletScale = event->setTabletScale.scale;
                    m_lasso->setScale(glm::vec3(m_tabletScale*m_surfaceData->renderer.getWidth()/2,m_tabletScale*m_surfaceData->renderer.getHeight()/2,m_tabletScale));
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
                it->second._data[i] |= sdChangement._data[i];
    }

    SciVis* MainVFV::createVisualization(SubDataset* sd)
    {
        if(sd == NULL)
            return NULL;
        //First check if this SubDataset was already registered
        for(auto it : m_sciVis)
            if(it->getModel() == sd)
                return it;

        //Check what type of Dataset this SubDataset is (the action will not be the same)
        for(auto it : m_vtkStructuredGridPoints)
        {
            if(it->dataset.get() == sd->getParent())
            {
                //If no property to look at, discard
                if(it->dataset->getPtFieldValues().size() == 0)
                    break;

                VTKStructuredGridPointGameObject* go = new VTKStructuredGridPointGameObject(NULL, &m_surfaceData->renderer, m_colorGridMtl, it->vbo,
                                                                                            it->gameObjects.size(), sd);
                it->gameObjects.push_back(go);
                m_sciVis.push_back(go);

                //Set the transfer function
                m_sciVis.back()->onTFChanged();

                //Update the snapshot
                std::pair<SciVis*, std::shared_ptr<Snapshot>> snap(m_sciVis.back(), std::shared_ptr<Snapshot>(nullptr));
                m_snapshots.insert(snap);
                m_sciVis.back()->getModel()->setSnapshot(snap.second);

                addSubDataChangement(m_sciVis.back()->getModel(), SubDatasetChangement(true, true, true, true));

                //Return it
                return m_sciVis.back();
            }
        }

        LOG_WARNING("Trying to create a SubDataset visualization but its Dataset is not found...");
        return NULL; //Dataset not found...
    }

    void MainVFV::removeSubDataset(SubDataset* sd)
    {
        //Remove the bound scientific visualizations
        for(uint32_t i = 0; i < m_sciVis.size(); i++)
            if(m_sciVis[i]->getModel() == sd)
            {
                removeSciVis(m_sciVis[i]);
                break;
            }
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
            for(int i = 0; i < it->gameObjects.size(); i++)
                if(it->gameObjects[i] == sciVis)
                {
                    it->gameObjects.erase(it->gameObjects.begin()+i);
                    goto endForVTK;
                }
        }
endForVTK:

        //Remove the link of this scivis object
        for(auto it = m_sciVis.begin(); it != m_sciVis.end(); it++)
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
