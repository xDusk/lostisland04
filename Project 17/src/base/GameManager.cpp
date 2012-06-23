#include "base/GameManager.h"

using namespace Ogre;

template<> GameManager* Singleton<GameManager>::ms_Singleton = 0;

GameManager::GameManager()
{
  mRoot = 0;
}

GameManager::~GameManager()
{
  // clean up all the states
  while(!mStates.empty())
  {
    mStates.back()->exit();
    mStates.pop_back();
  }

  //краш при выходе тут
  //  if (mRoot)
  //    delete mRoot;
}

void GameManager::start(GameState* state)
{
  mRoot = OGRE_NEW Root("plugins.cfg", "settings.cfg", "log.log");
  setupResources();

  if(!configure())
  {
    return;
  }

  mInputMgr = InputManager::getSingletonPtr();
  mInputMgr->initialise(mRenderWindow);
  mInputMgr->addKeyListener(this, "GameManager");
  mInputMgr->addMouseListener(this, "GameManager");
  mRoot->addFrameListener(this);
  changeState(state);
  mRoot->startRendering();
}

void GameManager::changeState(GameState* state)
{
  // cleanup the current state
  if(!mStates.empty())
  {
    mStates.back()->exit();
    mStates.pop_back();
  }

  // store and init the new state
  mStates.push_back(state);
  mStates.back()->enter();
}

void GameManager::pushState(GameState* state)
{
  // pause current state
  if(!mStates.empty())
  {
    mStates.back()->pause();
  }

  // store and init the new state
  mStates.push_back(state);
  mStates.back()->enter();
}

void GameManager::popState()
{
  // cleanup the current state
  if(!mStates.empty())
  {
    mStates.back()->exit();
    mStates.pop_back();
  }

  // resume previous state
  if(!mStates.empty())
  {
    mStates.back()->resume();
  }
}

void GameManager::setupResources(void)
{
  // load resource paths from config file
  ConfigFile cf;
  std::string ResourcePath;
  ResourcePath = "..\\media\\";
  cf.load(ResourcePath + "resources.cfg");
  // go through all settings in the file
  ConfigFile::SectionIterator seci = cf.getSectionIterator();
  String secName, typeName, archName;

  while(seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    ConfigFile::SettingsMultiMap *settings = seci.getNext();
    ConfigFile::SettingsMultiMap::iterator i;

    for(i = settings->begin() ; i != settings->end() ; ++i)
    {
      typeName = i->first;
      archName = i->second;
      ResourceGroupManager::getSingleton().addResourceLocation(
        ResourcePath + archName, typeName, secName);
    }
  }

  ParticleSystemRendererFactory*  mParticleSystemRendererFact;
  ParticleEmitterFactory*     mParticleEmitterFact;
  mParticleSystemRendererFact = OGRE_NEW ShaderParticleRendererFactory();
  mParticleEmitterFact        = OGRE_NEW BoxEmitterFactory();
  ParticleSystemManager::getSingleton().addRendererFactory(mParticleSystemRendererFact);
  ParticleSystemManager::getSingleton().addEmitterFactory(mParticleEmitterFact);
}

bool GameManager::configure(void)
{
  // грузим настройки из settings.cfg
  if(!mRoot->restoreConfig())
  {
    // если не загружены то выводим окно настройки
    if(!mRoot->showConfigDialog())
    {
      return false;
    }
  }

  // инициализация и создание дефолтового окна
  mRenderWindow = mRoot->initialise(true, "LI Tech Demo v0.0004 unstable");
  //устанавливаем фильтрацию текстур
  MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
  MaterialManager::getSingleton().setDefaultAnisotropy(16);
  // устанавливаем уповень мипмэпинга
  TextureManager::getSingleton().setDefaultNumMipmaps(5);
  //инициализация всех ресурсгрупп
  ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  return true;
}
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  return true;
}

bool GameManager::frameStarted(const FrameEvent& evt)
{
  mInputMgr->capture();
  return mStates.back()->frameStarted(evt);
}

bool GameManager::frameEnded(const FrameEvent& evt)
{
  // call frameEnded of current state
  return mStates.back()->frameEnded(evt);
}

GameManager* GameManager::getSingletonPtr(void)
{
  return ms_Singleton;
}

GameManager& GameManager::getSingleton(void)
{
  assert(ms_Singleton);
  return *ms_Singleton;
}

bool GameManager::keyPressed(const OIS::KeyEvent &e)
{
  // Call keyPressed of current state
  mStates.back()->keyPressed(e);
  return true;
}

bool GameManager::keyReleased(const OIS::KeyEvent &e)
{
  // Call keyReleased of current state
  mStates.back()->keyReleased(e);
  return true;
}

bool GameManager::mouseMoved(const OIS::MouseEvent &e)
{
  // Call mouseMoved of current state
  mStates.back()->mouseMoved(e);
  return true;
}

bool GameManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  // Call mousePressed of current state
  mStates.back()->mousePressed(e, id);
  return true;
}

bool GameManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  // Call mouseReleased of current state
  mStates.back()->mouseReleased(e, id);
  return true;
}