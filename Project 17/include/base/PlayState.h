#pragma once

#include <Ogre.h>
#include <vector>

#include "base/GameManager.h"
#include "base/GameState.h"
#include "base/PauseState.h"
#include "base/IntroState.h"
#include "base/InputManager.h"

#include "base/SharedData.h"

#include <Rocket/Core/Core.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include "RenderInterfaceOgre3D.h"
#include "SystemInterfaceOgre3D.h"

#include "AiCharacter.h"
#include "ProgressBar.h"
#include "TerrainManager.h"
#include "EngineDeferredShading.h"
#include "DeferredShading.h"
#include "debugOut.h"
#include "ExtendedCamera.h"
#include "GuiManager.h"
#include "RenderToTexture.h"
#include "Creature.h"

#include "GuiInventory.h"

#include "OgreShaderParticleRenderer.h"
#include "OgreBoxEmitterFactory.h"

#include "WoodyListener.h"

class PlayState : public GameState
{

  public:
    ~PlayState(void) {}

    void enter(void);
    void exit(void);

    void pause(void);
    void resume(void);

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    void keyPressed(const OIS::KeyEvent &e);
    void keyReleased(const OIS::KeyEvent &e);

    void mouseMoved(const OIS::MouseEvent &e);
    void mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    static PlayState* getInstance()
    {
      return &mPlayState;
    }

  private:

    PlayState(void) { }

    Ogre::Root           *mRoot;
    Ogre::SceneManager   *mSceneMgr;
    Ogre::Viewport       *mViewport;

    RTTManager      *RttMgr;

    static PlayState     mPlayState;

    bool mExitGame;

  protected:

    Rocket::Core::Context* context;

    void ConfigureRenderSystem();
    void BuildProjectionMatrix(Ogre::Matrix4& projection_matrix);

    // Контейнер с ссылками на все перемещающиеся объекты
    //  std::vector<AiCharacter*> mAllRemovableObjects;
    // Контейнер с ссылками на все итемы
    std::vector<ItemStruct>   mAllItemsVector;

    std::vector<BaseObj*>   mAllObjects;

    //позиция на которой появится игрок.
    //наверно придётся делать вектор для всех объекитов и хранить имя объекта и позицию
    Vector3       StartPosition;
    Quaternion      StartOrientation;

    //игрок
    Player        *mPlayer;
    //камера игрока
    ExtendedCamera    *mPlayerCamera;

    //теран
    MyTerrain     *mMyTerrain;

    //гуй
    GuiListener     *mGuiMgr;

    //деревья
    //WoodyListener *wTrees;

    //отложеное освещение
    DeferredShading   *mDeferredShading;
    DeferredShadingSystem *mDS;

    //флаг для переключения верфрейма
    int mSceneDetailIndex;

    //переменные для отслеживания нажатий клавиш
    bool mKeyPressed_UP;
    bool mKeyPressed_DOWN;
    bool mKeyPressed_LEFT;
    bool mKeyPressed_RIGHT;
    bool mKeyPressed_SHIFT;
    bool mKeyPressed_CTRL;
    bool mKeyPressed_ENTER;
    bool mKeyPressed_SPACE;
    bool mKeyPressed_LMOUSE;
    bool mKeyPressed_RMOUSE;

    void keyHandler();
    void createScene(ProgressBar *pb);
};