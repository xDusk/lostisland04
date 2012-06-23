#pragma once

#include <vector>
#include <Ogre.h>
#include <OgreSingleton.h>
#include <OIS/OIS.h>

#include "base/GameState.h"
#include "base/InputManager.h"

#include "OgreShaderParticleRenderer.h"
#include "OgreBoxEmitterFactory.h"

class GameState;

class GameManager : public Ogre::FrameListener, public Ogre::Singleton<GameManager>, public OIS::KeyListener, OIS::MouseListener
{
  public:
    GameManager();
    ~GameManager();
    void start(GameState* state);
    void changeState(GameState* state);
    void pushState(GameState* state);
    void popState();
    static GameManager& getSingleton(void);
    static GameManager* getSingletonPtr(void);


  protected:
    Ogre::Root* mRoot;
    Ogre::RenderWindow* mRenderWindow;

    void setupResources(void);
    bool configure(void);

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
  private:
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    InputManager       *mInputMgr;
    std::vector<GameState*> mStates;
};
