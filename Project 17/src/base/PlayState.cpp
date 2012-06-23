#include "base/PlayState.h"

//
#include "psapi.h"
#pragma comment( lib,"psapi.lib")


using namespace Ogre;
using namespace mySharedData;

static Vector2  curtime;
static int    FPS;
static int    TringleCount;
static Vector3  currentPos;
static Vector3  PlayerDispVec;
static int    Playerhealth;
static int    npc2health;
static int    lightintense;
static Vector3  lightpos;
static Vector3  lightdir;
static Vector3  skelpos;

static SceneNode *target;

PlayState PlayState::mPlayState;

void PlayState::enter(void)
{
  LogManager::getSingletonPtr()->logMessage("********************************* PlayState::enter() *********************************");
  PROCESS_MEMORY_COUNTERS pmemory;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage play state enter " + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  mRoot     = Root::getSingletonPtr();
  ProgressBar *pb = new ProgressBar(mRoot->getAutoCreatedWindow(), 10);
  mSceneMgr = mRoot->createSceneManager(ST_GENERIC);
  mCamera = mSceneMgr->createCamera("Camera");
  mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
  mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));
  mSceneDetailIndex = 0;
  debug.add("Time", 30, 30, curtime);
  debug.add("currentFPS", 30, mViewport->getActualHeight() - 50, FPS);
  debug.add("tringleCount", 30, mViewport->getActualHeight() - 80, TringleCount);
  debug.add("Current Position", 30, mViewport->getActualHeight() - 110, currentPos);
  debug.add("disp", 30, mViewport->getActualHeight() - 150, PlayerDispVec);
  debug.add("Player Health", 30, mViewport->getActualHeight() - 180, Playerhealth);
  debug.add("Batch count", 30, mViewport->getActualHeight() - 210, npc2health);
  debug.add("LightIntense", 30, mViewport->getActualHeight() - 240, lightintense);
  debug.add("LightPos", 30, mViewport->getActualHeight() - 270, lightpos);
  debug.add("LightDir", 30, mViewport->getActualHeight() - 300, lightdir);
  debug.add("SkeletonRootPos", 30, mViewport->getActualHeight() - 330, skelpos);
  target = mSceneMgr->getRootSceneNode()->createChildSceneNode("target");
  Entity *dbox = mSceneMgr->createEntity("box", "Crate.mesh");
  target->attachObject(dbox);
  target->scale(50, 50, 50);
  mKeyPressed_UP = false;
  mKeyPressed_DOWN = false;
  mKeyPressed_LEFT = false;
  mKeyPressed_RIGHT = false;
  mKeyPressed_SHIFT = false;
  mKeyPressed_CTRL = false;
  mKeyPressed_ENTER = false;
  mKeyPressed_SPACE = false;
  mKeyPressed_LMOUSE = false;
  mKeyPressed_RMOUSE = false;
  //загрузка позиции
  //  DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource( "save", "General");
  ifstream input("save.cfg");

  if(input)
  {
    Ogre::String sa;
    char ee[255];
    //    sa = pStream->getLine();
    input.getline(ee, 254);

    if(ee)
    {
      sa = ee;
      StartPosition = Ogre::StringConverter::parseVector3(sa);
    }
    else
    {
      StartPosition = Vector3(10000, 350, 10000);
    }

    input.getline(ee, 254);

    if(ee)
    {
      Ogre::String sa = ee;
      StartOrientation = Ogre::StringConverter::parseQuaternion(sa);
    }
    else
    {
      StartOrientation = Quaternion(0, 1, 0, 0);
    }
  }
  else
  {
    StartPosition = Vector3(10000, 350, 10000);
    StartOrientation = Quaternion(0, 1, 0, 0);
  }

  input.close();
  //  delete input;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage before create scene" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  ///////////////////////////////////////////////////////////////////////
  //  SystemInterfaceOgre3D* ogre_system;
  //  RenderInterfaceOgre3D* ogre_renderer;
  //  Ogre::ResourceGroupManager::getSingleton().createResourceGroup( "Rocket");
  //  Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "", "FileSystem", "Rocket");
  // Rocket initialisation.
  //  ogre_renderer = new RenderInterfaceOgre3D( mViewport->getWidth(), mViewport->getHeight());
  //  Rocket::Core::SetRenderInterface( ogre_renderer);
  //  ogre_system = new SystemInterfaceOgre3D();
  //  Rocket::Core::SetSystemInterface( ogre_system);
  //  Rocket::Core::Initialise();
  //  Rocket::Controls::Initialise();
  //  Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");
  //  Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
  //  Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
  //  Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
  //  context = Rocket::Core::CreateContext( "main", Rocket::Core::Vector2i( mViewport->getWidth(), mViewport->getHeight()));
  //  Rocket::Debugger::Initialise( context);
  //  Rocket::Core::ElementDocument* cursor = context->LoadMouseCursor("cursor.rml");
  //  Rocket::Core::ElementDocument* document = context->LoadDocument("demo.rml");
  //  document->Show();
  //  document->RemoveReference();
  //  mGuiMgr = new GuiListener( mRoot->getAutoCreatedWindow(), mCamera, context, InputManager::getSingletonPtr()->getKeyboard(), InputManager::getSingletonPtr()->getMouse());
  //  mRoot->addFrameListener( mGuiMgr);
  // Load and show the inventory document.
  //  GuiInventory* inventory_1 = new GuiInventory( "Inventory 1", Rocket::Core::Vector2f(50, 200), context, mGuiMgr);
  //  GuiInventory* inventory_2 = new GuiInventory( "Inventory 2", Rocket::Core::Vector2f(540, 240), context, mGuiMgr);
  // Add items into the inventory.
  //  inventory_1->AddItem("BOT_01");
  //  inventory_1->AddItem("BOT_02");
  //  inventory_1->AddItem("CST_01");
  //  inventory_1->AddItem("CST_02");
  //  inventory_1->AddItem("EAT_01");
  //  inventory_1->AddItem("HLM_01");
  //////////////////////////////////////////////////////////////////
  createScene(pb);
  delete pb;
  mExitGame = false;
  return;
}

void PlayState::exit(void)
{
  //Ogre::Profiler::getSingleton().setEnabled(false);
  debug.remove("Time");
  debug.remove("currentFPS");
  debug.remove("tringleCount");
  debug.remove("Current Position");
  debug.remove("disp");
  debug.remove("Player Health");
  debug.remove("Batch count");
  StartPosition = mPlayer->getWorldPosition();
  StartOrientation = mPlayer->getWorldOrientation();
  mPlayer->~Player();

  if(!mAllObjects.empty())
  {
    for(std::vector<BaseObj*>::iterator iter = mAllObjects.begin(); iter != mAllObjects.end(); ++iter)
    {
      if((*iter)->getType() == "AiCharacter")
      {
        AiCharacter *obj = reinterpret_cast<AiCharacter*>(*iter);
        delete obj;
      }
      else if((*iter)->getType() == "Creature")
      {
        Creature *obj = reinterpret_cast<Creature*>(*iter);
        delete obj;
      }
      else if((*iter)->getType() == "Item")
      {
        Item *obj = reinterpret_cast<Item*>(*iter);
        delete obj;
      }
    }
  }

  mAllObjects.clear();
  //  mGuiMgr->~GuiManager();
  mSkyManager->~SkyManager();
  mHydraxMgr->~HydraxManager();
  mPGManager->~GamePGManager();
  mPxManager->~PhysxManager();
  //  mMyTerrain->~MyTerrain();
  mSceneMgr->clearScene();
  mSceneMgr->destroyAllCameras();
  mRoot->getAutoCreatedWindow()->removeAllViewports();
  LogManager::getSingletonPtr()->logMessage("********************************* PlayState::exit() *********************************");
  ofstream out("save.cfg");
  out << Ogre::StringConverter::toString(StartPosition) << endl;
  out << Ogre::StringConverter::toString(StartOrientation) << endl;
  out.close();
  //  delete out;
  LogManager::getSingletonPtr()->logMessage("********************************* Progress Saved *********************************");
  return;
}

void PlayState::pause(void)
{
  return;
}

void PlayState::resume(void)
{
}

bool PlayState::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  return true;
}

bool PlayState::frameStarted(const FrameEvent& evt)
{
  //  context->Update();
  //  ConfigureRenderSystem();
  //  context->Render();
  //обновление физической сцены
  mPxManager->updateAllAtoms(evt.timeSinceLastFrame);
  //обновление движимых объектов
  mPlayer->update(evt.timeSinceLastFrame, mPlayerCamera->getCameraPosition());

  for(std::vector<BaseObj*>::iterator iter = mAllObjects.begin(); iter != mAllObjects.end(); ++iter)
  {
    if((*iter)->getType() == "AiCharacter")
    {
      AiCharacter* obj = dynamic_cast<AiCharacter*>(*iter);
      obj->update(evt.timeSinceLastFrame, mSkyManager->getCurrentTime());

      if(obj->getPosition().distance(mPlayer->getPosition()) < 100)
      {
        mPlayer->focusedTarget = obj;
        target->getAttachedObject("box")->setVisible(true);
        target->setPosition(mPlayer->focusedTarget->getPosition());
      }
    }
    else if((*iter)->getType() == "Item")
    {
      Item* obj = dynamic_cast<Item*>(*iter);

      if(obj->getPosition().distance(mPlayer->getPosition()) < 100)
      {
        mPlayer->focusedTarget = obj;
        target->getAttachedObject("box")->setVisible(true);
        target->setPosition(mPlayer->focusedTarget->getPosition());
      }
    }
    else if((*iter)->getType() == "Creature")
    {
      Creature* obj = dynamic_cast<Creature*>(*iter);
      obj->update(evt.timeSinceLastFrame, mSkyManager->getCurrentTime());
      skelpos = obj->getPosition();

      if(obj->getPosition().distance(mPlayer->getPosition()) < 100)
      {
        mPlayer->focusedTarget = obj;
        target->getAttachedObject("box")->setVisible(true);
        target->setPosition(mPlayer->focusedTarget->getPosition());
      }
    }
    else
    {
    };

    if(mPlayer->focusedTarget)
      if(mPlayer->focusedTarget->getPosition().distance(mPlayer->getPosition()) > 100)
      {
        mPlayer->focusedTarget = NULL;
        target->getAttachedObject("box")->setVisible(false);
      }
  }

  //и камеры
  mPlayerCamera->update(evt.timeSinceLastFrame, mPlayer->getWorldPosition() + Vector3(0, 6, 0), mKeyPressed_UP);
  OgreProfileBegin("SkyX Update");
  //skyx update
  mSkyManager->update(evt.timeSinceLastFrame, mPlayer->getMainNode()->getPosition());
  OgreProfileEnd("SkyX Update");
  //  mRoot->getAutoCreatedWindow()->getViewport(0)->setVisibilityMask( 5);
  OgreProfileBegin("terrain Update");
  mTerrainGroup->update();
  OgreProfileEnd("terrain Update");
  //  mGuiMgr->getGui()->injectFrameEntered( evt.timeSinceLastFrame);
  //обновление дебуг вывода
  curtime = mSkyManager->getCurrentTime();
  FPS = mRoot->getAutoCreatedWindow()->getLastFPS();
  TringleCount = mRoot->getAutoCreatedWindow()->getTriangleCount();
  currentPos = mPlayer->getWorldPosition();
  PlayerDispVec = mPlayer->getDispVector();
  Playerhealth = mPlayer->getHealth();
  npc2health = mRoot->getAutoCreatedWindow()->getBatchCount();
  lightintense = mSkyManager->getLightIntense();
  lightpos = mSkyManager->getLightPosition();
  lightdir = mSkyManager->getLight()->getDirection();
  //  skelpos = mPlayer->getEntity()->getSkeleton()->getRootBone()->getPosition();
  return true;
}

bool PlayState::frameEnded(const FrameEvent& evt)
{
  if(mExitGame)
  {
    return false;
  }

  return true;
}


void PlayState::keyPressed(const OIS::KeyEvent &e)
{
  using namespace OIS;

  //  mGuiMgr->getGui()->injectKeyPress( MyGUI::KeyCode::Enum( e.key), e.text);

  switch(e.key)
  {
    case KC_UP:
    case KC_W:
      mPlayer->getEntity()->getSkeleton()->getBone("Bip01 Head")->scale(0.1, 0.1, 0.1);
      mKeyPressed_UP = true;
      keyHandler();
      break;
    case KC_DOWN:
    case KC_S:
      mKeyPressed_DOWN = true;
      keyHandler();
      break;
    case KC_LEFT:
    case KC_A:
      mKeyPressed_LEFT = true;
      keyHandler();
      break;
    case KC_RIGHT:
    case KC_D:
      mKeyPressed_RIGHT = true;
      keyHandler();
      break;
    case KC_LSHIFT:
    case KC_RSHIFT:
      mKeyPressed_SHIFT = true;
      keyHandler();
      break;
    case KC_LCONTROL:
    case KC_RCONTROL:
      mKeyPressed_CTRL = true;
      keyHandler();
      break;
    case KC_NUMPADENTER:
      mKeyPressed_ENTER = true;
      keyHandler();
      break;
    case KC_SPACE:
      mKeyPressed_SPACE = true;
      keyHandler();
      break;
    case KC_R:
      mSceneDetailIndex = (mSceneDetailIndex + 1) % 3 ;

      switch(mSceneDetailIndex)
      {
        case 0 :
          mCamera->setPolygonMode(PM_SOLID);
          break;
        case 1 :
          mCamera->setPolygonMode(PM_WIREFRAME);
          break;
        case 2 :
          mCamera->setPolygonMode(PM_POINTS);
          break;
          break;
      }
  }

  return;
}

void PlayState::keyReleased(const OIS::KeyEvent &e)
{
  using namespace OIS;

  //  mGuiMgr->getGui()->injectKeyRelease( MyGUI::KeyCode::Enum( e.key));

  switch(e.key)
  {
    case KC_F1:

      if(mSceneMgr->getAmbientLight() != ColourValue(0.0, 0.0, 0.0))
      {
        mSceneMgr->setAmbientLight(ColourValue(0.0, 0.0, 0.0));
      }
      else
      {
        mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));
      }

    case KC_UP:
    case KC_W:
      mKeyPressed_UP = false;
      keyHandler();
      break;
    case KC_DOWN:
    case KC_S:
      mKeyPressed_DOWN = false;
      keyHandler();
      break;
    case KC_LEFT:
    case KC_A:
      mKeyPressed_LEFT = false;
      keyHandler();
      break;
    case KC_RIGHT:
    case KC_D:
      mKeyPressed_RIGHT = false;
      keyHandler();
      break;
    case KC_LSHIFT:
    case KC_RSHIFT:
      mKeyPressed_SHIFT = false;
      keyHandler();
      break;
    case KC_LCONTROL:
    case KC_RCONTROL:
      mKeyPressed_CTRL = false;
      keyHandler();
      break;
    case KC_NUMPADENTER:
      mKeyPressed_ENTER = false;
      keyHandler();
      break;
    case KC_SPACE:
      mKeyPressed_SPACE = false;
      keyHandler();
      break;
    case KC_ESCAPE:
      changeState(IntroState::getInstance());
      break;
    case KC_PAUSE:
      pushState(PauseState::getInstance());
      break;
    case KC_T:
      break;
    case KC_Y:
      mPlayer->ragdollOn();
      break;
    case KC_U:
      break;
    case KC_I:
      break;
    case KC_G:
      break;
    case KC_H:
      break;
    case KC_J:
      //    mGuiMgr->closeIngameMenu();
      break;
    case KC_K:
      break;
  }

  return;
}

void PlayState::mouseMoved(const OIS::MouseEvent &e)
{
  using namespace OIS;
  //  mGuiMgr->getGui()->injectMouseMove( e.state.X.abs, e.state.Y.abs, e.state.Z.abs);
  //  if( mGuiMgr->isMenu() == false)
  //  {
  //задаём углы для камеры
  mPlayerCamera->setAngles(e.state.X.rel, e.state.Y.rel);
  mPlayerCamera->setRadius(-e.state.Z.rel);
  //  }
}

void PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  using namespace OIS;

  //  mGuiMgr->getGui()->injectMousePress( e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

  //  if( mGuiMgr->isMenu() == false)
  //  {
  switch(id)
  {
    case MB_Left:
      mKeyPressed_LMOUSE = true;
      keyHandler();
      break;
    case MB_Right:
      mKeyPressed_RMOUSE = true;
      keyHandler();
      break;
  }

  //  }
  return;
}

void PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  using namespace OIS;

  //  mGuiMgr->getGui()->injectMouseRelease( e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

  //  if( mGuiMgr->isMenu() == false)
  //  {
  switch(id)
  {
    case MB_Left:
      mKeyPressed_LMOUSE = false;
      keyHandler();
      break;
    case MB_Right:
      mKeyPressed_RMOUSE = false;
      keyHandler();
      break;
  }

  //  }
  return;
}

void PlayState::keyHandler()
{
  if(mKeyPressed_LMOUSE)
  {
    if(mPlayer->focusedTarget != NULL)
      if(mPlayer->focusedTarget->getType() == "AiCharacter")
      {
        if(mPlayer->getBattle() == true)
        {
          mPlayer->getActionManager()->DoAction(ATTACK1);
        }
        else
        {
          mPlayer->getActionManager()->DoAction(ATTACK2);
        }

        LogManager::getSingletonPtr()->logMessage("Hit");
        //        if( dynamic_cast<CharacterObj*>(mPlayer->focusedTarget)->IsDead()!=true)
        mPlayer->Hit(reinterpret_cast<AiCharacter*>(mPlayer->focusedTarget), mPlayer->getCurentWeapon()->damage);
        //        else
        //          mPlayer->
      }
      else if(mPlayer->focusedTarget->getType() == "Item")
      {
        LogManager::getSingletonPtr()->logMessage("Pick Item");
        Item* itm = reinterpret_cast<Item*>(mPlayer->focusedTarget);
        mPlayer->getInventory()->addItem(itm->getItemStruct());
        mPlayer->focusedTarget = NULL;

        for(std::vector<BaseObj*>::iterator iter = mAllObjects.begin(); iter != mAllObjects.end(); ++iter)
        {
          Item* obj = dynamic_cast<Item*>(*iter);

          if(obj == itm)
          {
            mAllObjects.erase(iter);
            break;
          }
        }

        itm->~Item();
      }
  }
  else if(mKeyPressed_RMOUSE)
  {
    mPlayer->getActionManager()->DoAction(ONEHANDBLOCK);
    mPlayer->mBlock = 3;
  }
  else
  {
    if(mKeyPressed_UP)
    {
      if(mKeyPressed_SHIFT)
      {
        mPlayer->goAlongFast();
        mPlayer->PxPushCharacter = true;
        mPlayer->getActionManager()->DoAction(RUNF);
      }
      else
      {
        mPlayer->goAlong();
        mPlayer->PxPushCharacter = true;

        if(mPlayer->getBattle() == true)
        {
          mPlayer->getActionManager()->DoAction(ONEHANDFORWARD);
        }
        else
        {
          mPlayer->getActionManager()->DoAction(WALKF);
        }
      }
    }
    else
    {
      if(mKeyPressed_DOWN)
      {
        mPlayer->goAcross();
        mPlayer->PxPushCharacter = true;
        mPlayer->getActionManager()->DoAction(WALKB);
      }
      else
      {
        if(mKeyPressed_LEFT)
        {
          mPlayer->PxPushCharacter = true;
          mPlayer->goLeft();
          mPlayer->getActionManager()->DoAction(WALKF);
        }
        else
        {
          if(mKeyPressed_RIGHT)
          {
            mPlayer->goRight();
            mPlayer->PxPushCharacter = true;
            mPlayer->getActionManager()->DoAction(WALKF);
          }
          else
          {
            if(mKeyPressed_SPACE)
            {
              if(mPlayer->getBattle() == false)
              {
                mPlayer->setBattle(true);
              }
              else
              {
                mPlayer->setBattle(false);
              }
            }
            else
            {
              mPlayer->PxPushCharacter = false;

              if(mPlayer->getBattle() == true)
              {
                mPlayer->getActionManager()->DoAction(ONEHANDIDLE);
              }
              else
              {
                mPlayer->getActionManager()->DoAction(IDLE);
              }
            }
          }
        }
      }
    }
  }

  if(mKeyPressed_CTRL)
  {
    mPlayer->StartJump(75);
  }
}

void PlayState::createScene(ProgressBar *pb)
{
  pb->update(0);
  //теран физика и плэер
  mViewport->setBackgroundColour(ColourValue(0.5, 0.5, 0.5));
  mSceneMgr->setAmbientLight(ColourValue(0.4, 0.4, 0.4));
  mPxManager = new PhysxManager(mSceneMgr);
  mCamera->setNearClipDistance(0.5f);  //если сделать меньше почемуто глючит тест глубины
  mCamera->setFarClipDistance(100000);
  pb->update(1);
  Light* L = mSceneMgr->createLight("Light1");
  L->setPosition(Vector3(3133.07, 1436.233, -4350.63));
  L->setType(Light::LT_DIRECTIONAL);
  L->setDiffuseColour(1, 1, 1);
  L->setSpecularColour(1, 1, 1);
  L->setCastShadows(true);
  L->setShadowFarDistance(2000);
  L->setDirection(Vector3(1, -1, 0));
  pb->update(2);
  //  wTrees = new WoodyListener();
  //  mRoot->addFrameListener( wTrees);
  pb->update(3);
  PROCESS_MEMORY_COUNTERS pmemory;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage before create skyX" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  //небо
  mSkyManager = new SkyManager(mSceneMgr, mCamera);
  mSkyManager->VolumeCloudsCreate();
  mSkyManager->SimpleCloudsCreate();
  mSkyManager->initSkyXManager();
  mSkyManager->setCurrentTime(Vector3(9, 6, 15));
  pb->update(4);
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory, sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage before ogscene loader" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  ogSceneLoader *ogscn = new ogSceneLoader(mSceneMgr, mRoot->getAutoCreatedWindow());
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory, sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage after ogscene loader" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  //трава и деревья
  mRoot->addFrameListener(mPGManager);
  //вода
  mRoot->addFrameListener(mHydraxMgr);
  pb->update(5);
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory, sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage before create boxes" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  mPxManager->addStaticBox("StaticBox", Vector3(3000, 450, -4000));
  mPxManager->addBox("Box6", Vector3(3000, 550, -4000));
  mPxManager->addBox("Box7", Vector3(3000, 650, -4000));
  mPxManager->addBox("Box8", Vector3(3000, 750, -4000));
  mPxManager->addBox("Box9", Vector3(3000, 850, -4000));
  mPxManager->addBox("Box10", Vector3(3000, 950, -4000));
  mPxManager->addBox("Box11", Vector3(3000, 1050, -4000));
  mPxManager->addBox("Box12", Vector3(3000, 1150, -4000));
  mPxManager->addBox("Box13", Vector3(3000, 1250, -4000));
  mPxManager->addBox("Box14", Vector3(3000, 1350, -4000));
  mPxManager->addBox("Box15", Vector3(3000, 1450, -4000));
  mPxManager->addBox("Box16", Vector3(3000, 1550, -4000));
  mPxManager->addBox("Box17", Vector3(3000, 1650, -4000));
  mPxManager->addBox("Box18", Vector3(3000, 1750, -4000));
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage after create boxes" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  pb->update(6);
  /*
    ParticleSystem* pSys = mSceneMgr->createParticleSystem("test", "Menu/Fire2");
    SceneNode *fNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("fire", mSceneMgr->getSceneNode("campfire")->_getDerivedPosition());
    fNode->attachObject(pSys);
    fNode->scale( 0.1, 0.1, 0.1);
  */
  // create particle system //из демки нвидиа файр. не компилится в огрешадерпартиклрендерер 2 ерора и блядт скрипт невидит рендерер и эмитер
  //  ParticleSystem* pParticleSys = mSceneMgr->createParticleSystem( "my_fire_ps", "nVidia/fire");
  //  SceneNode* pNode = mSceneMgr->getSceneNode( "campfire#0");
  //  pNode->attachObject( pParticleSys);
  pb->update(7);
  mPlayer = new Player("Player", mPxManager, mSceneMgr, mPxManager->PxSDK, mPxManager->PxScene, StartPosition, StartOrientation);
  mPlayerCamera = new ExtendedCamera("PlayerCamera", mSceneMgr, mCamera, mPlayer->getOffsetPosition());
  //дождь
  ParticleSystem* pRain = mSceneMgr->createParticleSystem("rain", "Examples/Rain");
  SceneNode* RainNode = mPlayer->getMainNode()->createChildSceneNode(Vector3(0, 50 , 0));
  RainNode->attachObject(pRain);
  //  pRain->getEmitter(0)->setEmitted(false);
  //нпц
  AiCharacter *mNpc_1, *mNpc_2, *mNpc_3, *mNpc_4, *mNpc_5;
  mNpc_1 = new AiCharacter("commoner_1", "hunter.mesh", mPxManager, mPlayer, mSceneMgr, Vector3(3000, 1000, -4000));
  mAllObjects.push_back(mNpc_1);
  mNpc_2 = new AiCharacter("commoner_2", "hunter.mesh", mPxManager, mPlayer, mSceneMgr, Vector3(3000, 1000, -5000));
  mAllObjects.push_back(mNpc_2);
  mNpc_3 = new AiCharacter("commoner_3", "hunter.mesh", mPxManager, mPlayer, mSceneMgr, Vector3(3000, 1000, -6000));
  mAllObjects.push_back(mNpc_3);
  mNpc_4 = new AiCharacter("commoner_4", "hunter.mesh", mPxManager, mPlayer, mSceneMgr, Vector3(3400, 1000, -6000));
  mAllObjects.push_back(mNpc_4);
  mNpc_5 = new AiCharacter("commoner_5", "hunter.mesh", mPxManager, mPlayer, mSceneMgr, Vector3(3400, 1000, -4000));
  mAllObjects.push_back(mNpc_5);
  Creature *mCreature_1, *mCreature_2, *mCreature_3;
  mCreature_1 = new Creature("Wolf_1", mSceneMgr, "wolf.mesh", mPlayer, Vector3(3400, 1000, -5000));
  mAllObjects.push_back(mCreature_1);
  mCreature_2 = new Creature("Wolf_2", mSceneMgr, "wolf.mesh", mPlayer, Vector3(3200, 1000, -5000));
  mAllObjects.push_back(mCreature_2);
  mCreature_3 = new Creature("Wolf_3", mSceneMgr, "wolf.mesh", mPlayer, Vector3(3100, 1000, -5000));
  mAllObjects.push_back(mCreature_3);
  GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage after create player and npc" + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  pb->update(8);
  ItemLoader *itmldr = new ItemLoader("itm.item", "Bootstrap");
  mAllItemsVector = itmldr->getItemsVector();
  delete itmldr;

  //инвентарь
  for(std::vector<ItemStruct>::iterator iter = mAllItemsVector.begin(); iter != mAllItemsVector.end(); ++iter)
  {
    if((*iter).id == "2")
    {
      mPlayer->getInventory()->addItem(&(*iter));
      Item* sw = new Item(mSceneMgr, mPxManager, &(*iter));
      sw->setPosition(Vector3(3074, 434, -4371));
      sw->getMainNode()->scale(0.1, 0.1, 0.1);
      mAllObjects.push_back(sw);
    }
  }

  mPlayer->setCurentWeapon(mPlayer->getInventory()->getItem("sword"));
  //  mPlayer->getInventory()->addItem( Item( mSceneMgr, "Apple", mPxMgr, FOOD, "Apple.mesh", 0, 0, 50));
  //Ogre::Profiler::getSingleton().setEnabled(true);
  /*GetProcessMemoryInfo(GetCurrentProcess(), &pmemory,
                       sizeof(PROCESS_MEMORY_COUNTERS));
  LogManager::getSingletonPtr()->logMessage("*************** Memory Usage after create scene " + StringConverter::toString(pmemory.WorkingSetSize / 1048575) + "mb");
  */
  pb->update(9);
  // Create MulteRenderTarget, Fill G-Buffer
  /*mDeferredShading = new DeferredShading("MRT", mCamera,
                                         Ogre::PixelFormat::PF_R8G8B8A8,
                                         Ogre::PixelFormat::PF_FLOAT32_RGBA,
                                         Ogre::PixelFormat::PF_FLOAT32_RGBA,
                                         Ogre::PixelFormat::PF_FLOAT32_RGBA);
  // Create Ambient Quad
  float fVDx = Ogre::Real(Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth());
  float fVDy = Ogre::Real(Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight());
  Ogre::ManualObject* quadGI = mDeferredShading->createAmbientQuad(mSceneMgr, "DeferredShadingQuad", "DeferredShadingGI", fVDx, fVDy, 2);
  mSceneMgr->getRootSceneNode()->createChildSceneNode("QuadGI")->attachObject(quadGI);
  mRoot->getAutoCreatedWindow()->getViewport(0)->setVisibilityMask(10); //нарисовать амбиент квад
  mRoot->addFrameListener(mDeferredShading);
  mSceneMgr->addRenderQueueListener(mDeferredShading);*/
  //
  //  mDS = new DeferredShadingSystem( mViewport, mSceneMgr, mCamera);
  //  mDS->initialize();
  pb->update(10);
}

// Configures Ogre's rendering system for rendering Rocket.
void PlayState::ConfigureRenderSystem()
{
  Ogre::RenderSystem* render_system = Ogre::Root::getSingleton().getRenderSystem();
  // Set up the projection and view matrices.
  Ogre::Matrix4 projection_matrix;
  BuildProjectionMatrix(projection_matrix);
  render_system->_setProjectionMatrix(projection_matrix);
  render_system->_setViewMatrix(Ogre::Matrix4::IDENTITY);
  // Disable lighting, as all of Rocket's geometry is unlit.
  render_system->setLightingEnabled(false);
  // Disable depth-buffering; all of the geometry is already depth-sorted.
  render_system->_setDepthBufferParams(false, false);
  // Rocket generates anti-clockwise geometry, so enable clockwise-culling.
  render_system->_setCullingMode(Ogre::CULL_CLOCKWISE);
  // Disable fogging.
  render_system->_setFog(Ogre::FOG_NONE);
  // Enable writing to all four channels.
  render_system->_setColourBufferWriteEnabled(true, true, true, true);
  // Unbind any vertex or fragment programs bound previously by the application.
  render_system->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
  render_system->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
  // Set texture settings to clamp along both axes.
  Ogre::TextureUnitState::UVWAddressingMode addressing_mode;
  addressing_mode.u = Ogre::TextureUnitState::TAM_CLAMP;
  addressing_mode.v = Ogre::TextureUnitState::TAM_CLAMP;
  addressing_mode.w = Ogre::TextureUnitState::TAM_CLAMP;
  render_system->_setTextureAddressingMode(0, addressing_mode);
  // Set the texture coordinates for unit 0 to be read from unit 0.
  render_system->_setTextureCoordSet(0, 0);
  // Disable texture coordinate calculation.
  render_system->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
  // Enable linear filtering; images should be rendering 1 texel == 1 pixel, so point filtering could be used
  // except in the case of scaling tiled decorators.
  render_system->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
  // Disable texture coordinate transforms.
  render_system->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
  // Reject pixels with an alpha of 0.
  render_system->_setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);
  // Disable all texture units but the first.
  render_system->_disableTextureUnitsFrom(1);
  // Enable simple alpha blending.
  render_system->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
  // Disable depth bias.
  render_system->_setDepthBias(0, 0);
}

// Builds an OpenGL-style orthographic projection matrix.
void PlayState::BuildProjectionMatrix(Ogre::Matrix4& projection_matrix)
{
  float z_near = -1;
  float z_far = 1;
  projection_matrix = Ogre::Matrix4::ZERO;
  // Set up matrices.
  projection_matrix[0][0] = 2.0f / mRoot->getAutoCreatedWindow()->getWidth();
  projection_matrix[0][3] = -1.0000000f;
  projection_matrix[1][1] = -2.0f / mRoot->getAutoCreatedWindow()->getHeight();
  projection_matrix[1][3] = 1.0000000f;
  projection_matrix[2][2] = -2.0f / (z_far - z_near);
  projection_matrix[3][3] = 1.0000000f;
}