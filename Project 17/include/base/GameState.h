#pragma once

#include "base/GameManager.h"
#include "base/GameState.h"
#include <OIS.h>

class GameState
{
  public:
    virtual void enter() = 0;
    virtual void exit() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void keyPressed(const OIS::KeyEvent &e)  = 0;
    virtual void keyReleased(const OIS::KeyEvent &e) = 0;

    virtual void mouseMoved(const OIS::MouseEvent &e) = 0;
    virtual void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)  = 0;
    virtual void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) = 0;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) = 0;
    virtual bool frameStarted(const Ogre::FrameEvent& evt) = 0;
    virtual bool frameEnded(const Ogre::FrameEvent& evt) = 0;

    void changeState(GameState* state);
    void pushState(GameState* state);
    void popState();

  protected:
    GameState() { }
};
