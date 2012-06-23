
#include "base/IntroState.h"

using namespace Ogre;

IntroState IntroState::mIntroState;

void IntroState::enter()
{
  LogManager::getSingletonPtr()->logMessage("********************************* IntroState::enter() *********************************");
  mRoot         = Root::getSingletonPtr();
  mOverlayMgr   = OverlayManager::getSingletonPtr();
  mSceneMgr     = mRoot->createSceneManager(ST_GENERIC);
  mCamera       = mSceneMgr->createCamera("IntroCamera");
  mViewport     = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
  mIntroOverlay     = mOverlayMgr->getByName("Overlay/Info");
  mMouseOverlay     = mOverlayMgr->getByName("Overlay/MousePointer");
  mMousePointer     = mOverlayMgr->getOverlayElement("MousePointer/Pointer");
  SelPos = 0;
  Ogre::TextureManager::getSingleton().reloadAll();
  RttMgr = new RTTManager(mCamera, mRoot->getAutoCreatedWindow());
  mRoot->addFrameListener(RttMgr);
  //  mGuiMgr = new GuiManager(mRoot->getAutoCreatedWindow());
  //  mRoot->addFrameListener(mGuiMgr);
  //буквы меню
  Entity *ent0 = mSceneMgr->createEntity("MenuContinue", "menu_continue.mesh");
  Entity *ent1 = mSceneMgr->createEntity("MenuNew", "menu_new.mesh");
  Entity *ent2 = mSceneMgr->createEntity("MenuBattle", "menu_battle.mesh");
  Entity *ent3 = mSceneMgr->createEntity("MenuExit", "menu_exit.mesh");
  ent0->setMaterialName("Menu/Text");
  ent0->setCastShadows(true);
  ent1->setMaterialName("Menu/Text");
  ent1->setCastShadows(true);
  ent2->setMaterialName("Menu/Text");
  ent2->setCastShadows(true);
  ent3->setMaterialName("Menu/Text");
  ent3->setCastShadows(true);
  SceneNode *node0 =  mSceneMgr->createSceneNode();
  SceneNode *node1 =  mSceneMgr->createSceneNode();
  SceneNode *node2 =  mSceneMgr->createSceneNode();
  SceneNode *node3 =  mSceneMgr->createSceneNode();
  node0->attachObject(ent0);
  node1->attachObject(ent1);
  node2->attachObject(ent2);
  node3->attachObject(ent3);
  node0->scale(Vector3(0.08, 0.08, 0.08));
  node1->scale(Vector3(0.08, 0.08, 0.08));
  node2->scale(Vector3(0.08, 0.08, 0.08));
  node3->scale(Vector3(0.08, 0.08, 0.08));
  node0->pitch(Degree(90));
  node1->pitch(Degree(90));
  node2->pitch(Degree(90));
  node3->pitch(Degree(90));
  mIntroOverlay->add3D(node0);
  mIntroOverlay->add3D(node1);
  mIntroOverlay->add3D(node2);
  mIntroOverlay->add3D(node3);
  node0->setPosition(0, 15, -90);
  node1->setPosition(0, 5, -90);
  node2->setPosition(0, -5, -90);
  node3->setPosition(0, -15, -90);
  mIntroOverlay->show();
  mIntroOverlay->setZOrder(0);
  //камера
  mCamera->setPosition(0, 0, 100);
  mCamera->setNearClipDistance(0.1);
  mCamera->lookAt(node2->getPosition());
  //свет
  mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));
  omni_light = mSceneMgr->createLight("Light1");
  omni_light->setType(Light::LT_DIRECTIONAL);
  omni_light->setDiffuseColour(1, 1, 1);
  omni_light->setSpecularColour(1, 1, 1);
  omni_light->setCastShadows(true);
  //самозатенение
  mSceneMgr->setShadowTextureSelfShadow(true);
  mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
  //частицы
  // огонь типо
  ParticleSystem* pSys2 = mSceneMgr->createParticleSystem("test", "Menu/Fire");
  fNode = mSceneMgr->createSceneNode();
  mIntroOverlay->add3D(fNode);
  fNode->attachObject(pSys2);
  //курсор. отображаем и помещаем в центр экрана
  mMouseOverlay->show();
  //композитор
  //  CompositorManager::getSingleton().addCompositor(mViewport, "DepthBlur");
  //  CompositorManager::getSingleton().setCompositorEnabled(mViewport, "DepthBlur", true);
  //sky
  Sky = new SkyManager(mSceneMgr, mCamera);
  Sky->VolumeCloudsCreate();
  Sky->SimpleCloudsCreate();
  Sky->initSkyXManager();
  Sky->getSkyXSystem()->setTimeMultiplier(0.1);
  Sky->setLightingModeHDR();
  Sky->setCurrentTime(Vector3(8, 6, 14));
  //water
  Hydrax = new HydraxManager(mSceneMgr, mCamera, mRoot->getAutoCreatedWindow(), Sky, Vector3(0, -100, 0), MENU_WATER);
  mRoot->addFrameListener(Hydrax);
  mRoot->getAutoCreatedWindow()->getViewport(0)->setVisibilityMask(2);
  mExitGame = false;
}

void IntroState::exit()
{
  //  TextureManager::getSingleton().destroyAllResourcePools();
  //  TextureManager::getSingleton().unloadAll();
  //  mGuiMgr->~GuiManager();
  Sky->~SkyManager();
  mRoot->removeFrameListener(Hydrax);
  Hydrax->~HydraxManager();
  mRoot->removeFrameListener(RttMgr);
  RttMgr->~RTTManager();
  mMouseOverlay->hide();
  mIntroOverlay->hide();
  mSceneMgr->clearScene();
  mSceneMgr->destroyAllCameras();
  mRoot->getAutoCreatedWindow()->removeAllViewports();
  TextureManager::getSingleton().destroyAllResourcePools();
  TextureManager::getSingleton().unloadAll();
  MeshManager::getSingleton().destroyAllResourcePools();
  MeshManager::getSingleton().unloadAll();
  MaterialManager::getSingleton().destroyAllResourcePools();
  MaterialManager::getSingleton().unloadAll();
  //  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("General");
  //  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("Hydrax");
  //  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("SkyX");
  mRoot = nullptr;
  mSceneMgr->~SceneManager();
  //  mOverlayMgr->destroyAll();
  mMousePointer->~OverlayElement();
  mIntroOverlay->~Overlay();
  mMouseOverlay->~Overlay();
  //  mOverlayMgr->~OverlayManager();
}

void IntroState::pause()
{
}

void IntroState::resume()
{
  mMouseOverlay->show();
  mIntroOverlay->show();
}

bool IntroState::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  return true;
}
bool IntroState::frameStarted(const FrameEvent& evt)
{
  Sky->update(evt.timeSinceLastFrame);

  switch(SelPos)
  {
    case(0):
      fNode->setPosition(0, 15, -100);
      break;
    case(1):
      fNode->setPosition(0, 5, -100);
      break;
    case(2):
      fNode->setPosition(0, -5, -100);
      break;
    case(3):
      fNode->setPosition(0, -15, -100);
      break;
    default:
      break;
  }

  return true;
}

bool IntroState::frameEnded(const FrameEvent& evt)
{
  if(mExitGame)
  {
    return false;
  }

  return true;
}

void IntroState::keyPressed(const OIS::KeyEvent &e)
{
  if(e.key == OIS::KC_UP)
  {
    SelPos = SelPos - 1;

    if(SelPos < 0)
    {
      SelPos = 3;
    }
  }

  if(e.key == OIS::KC_DOWN)
  {
    SelPos = SelPos + 1;

    if(SelPos > 3)
    {
      SelPos = 0;
    }
  }

  if(e.key == OIS::KC_RETURN)
    switch(SelPos)
    {
      case(0):
        this->changeState(PlayState::getInstance());
        break;
      case(1):
        this->changeState(PlayState::getInstance());
        break;
      case(2):
        break;
      case(3):
        mExitGame = true;
        break;
      default:
        break;
    }

  if(e.key == OIS::KC_ESCAPE)
  {
    mExitGame = true;
  }
}

void IntroState::keyReleased(const OIS::KeyEvent &e)
{
}

void IntroState::mouseMoved(const OIS::MouseEvent &e)
{
  mMousePointer->setPosition(e.state.X.abs, e.state.Y.abs);
}

void IntroState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void IntroState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}
