#pragma once

#include <Ogre.h>
#include "base/GameManager.h"
#include "base/GameState.h"
#include "base/PauseState.h"
#include "base/PlayState.h"
#include "base/IntroState.h"

#include "RenderToTexture.h"

#include "SkyManager.h"
#include "HydraxManager.h"

class IntroState : public GameState
{
  public:
    void enter();
    void exit();

    void pause();
    void resume();

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    void keyPressed(const OIS::KeyEvent &e);
    void keyReleased(const OIS::KeyEvent &e);

    void mouseMoved(const OIS::MouseEvent &e);
    void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    static IntroState* getInstance()
    {
      return &mIntroState;
    }

    Ogre::String model;
  protected:
    IntroState() { }

    Ogre::Root *mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Viewport* mViewport;
    Ogre::Camera* mCamera;

    Ogre::OverlayManager *mOverlayMgr;
    Ogre::Overlay        *mIntroOverlay;
    Ogre::Overlay        *mMouseOverlay;
    Ogre::OverlayElement *mMousePointer;

    //  GuiManager      *mGuiMgr;
    SkyManager      *Sky;
    HydraxManager   *Hydrax;
    RTTManager      *RttMgr;
    Ogre::SceneNode   *fNode;
    Ogre::Light     *omni_light;
    int SelPos;

    bool mExitGame;
  private:
    static IntroState mIntroState;
};
