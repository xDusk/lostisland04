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

    // ��������� � �������� �� ��� �������������� �������
    //  std::vector<AiCharacter*> mAllRemovableObjects;
    // ��������� � �������� �� ��� �����
    std::vector<ItemStruct>   mAllItemsVector;

    std::vector<BaseObj*>   mAllObjects;

    //������� �� ������� �������� �����.
    //������� ������� ������ ������ ��� ���� ��������� � ������� ��� ������� � �������
    Vector3       StartPosition;
    Quaternion      StartOrientation;

    //�����
    Player        *mPlayer;
    //������ ������
    ExtendedCamera    *mPlayerCamera;

    //�����
    MyTerrain     *mMyTerrain;

    //���
    GuiListener     *mGuiMgr;

    //�������
    //WoodyListener *wTrees;

    //��������� ���������
    DeferredShading   *mDeferredShading;
    DeferredShadingSystem *mDS;

    //���� ��� ������������ ���������
    int mSceneDetailIndex;

    //���������� ��� ������������ ������� ������
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