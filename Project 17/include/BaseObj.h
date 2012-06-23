#pragma once

#include <Ogre.h>
#include "ActionManager.h"
#include "configAnims.h"
#include "PhysxManager.h"
#include "PhysX/MyCharacterController.h"
#include "Ragdoll.h"

using namespace Ogre;

//Базовый для всех объектов
class BaseObj
{
  protected:
    String        mName;
    Entity        *mEntity;
    SceneManager    *mSceneMgr;
    PhysxManager    *mPxMgr;
    NxPhysicsSDK    *mPxSDK;        // ядро физиха
    NxScene       *mPxScene;        // физическая сцена
    SceneNode     *mMainNode;
    Vector3       mPosition;
    Quaternion      mOrientation;
    NxActor       *mActor;
    NxActorDesc     mActorDesc;
    NxBodyDesc      mBodyDesc;

  public:

    ~BaseObj()
    {
      mEntity = nullptr;
      mSceneMgr = nullptr;
      mPxMgr = nullptr;
      mPxSDK = nullptr;
      mPxScene = nullptr;
      mMainNode = nullptr;
      mActor = nullptr;
    }

    String getName()
    {
      return mName;
    }

    virtual String getType()
    {
      return "BaseObj";
    }

    virtual SceneNode *getMainNode()
    {
      return mMainNode;
    }

    virtual Vector3 getWorldPosition()
    {
      return mMainNode->_getDerivedPosition();
    }

    virtual Quaternion getWorldOrientation()
    {
      return mMainNode->_getDerivedOrientation();
    }

    virtual Vector3 getPosition()
    {
      return mMainNode->getPosition();
    }

    virtual Entity *getEntity()
    {
      return mEntity;
    }

    virtual void loadResources()
    {
      //загрузка в фоне если огр собран с #define OGRE_THREAD_SUPPORT 1:
      //ResourceBackgroundQueue::getSingleton().load("Texture", "MyTexture.jpg", "MyResGroup", false, NULL, &myListener);
      //когда ресурс загрузится в фоновом потоке, в слушателе вызовется метод operationCompleted
      //в нем уже делаешь что тебе нада с загруженным ресурсом.
    }

    virtual void unloadMesh()
    {
      Ogre::ResourceGroupManager::getSingleton()._getResourceManager("Mesh")->unload(mName);
    }

};

//базовый для игрока и нпц (контроллер анимация харки.)
class CharacterObj: public BaseObj
{

  public:
    bool        PxPushCharacter;    // нажата ли кнопка
    float       mBlock;         //флаг блока //также показывает длительность.
    BaseObj       *focusedTarget;
    bool        mIsDead;
    int         mHealth;

  protected:
    SceneNode       *OffsetNode;    // узел смещённый отосительно центра контроллера
    bool          PxJump;       // флаг прыжка
    NxVec3          horizontalDisp;
    NxF32         NxCharacterSpeed;
    NxVec3          disp;
    Vector3         MeshDisp;     // направление меша

    ActionManager     *mActionManager;

    NxControllerManager   *PxControllerMgr; // менеджер контролера
    MyControllerHitReport *mReport;     // сюда записывается реакция на столкновение
    UserAllocator     *mAllocator;    // юзер аллокатор. хз чё такое
    MyCharacterController *PxController;    // контроллер в виде капсулы

    NxF32         PxV0;
    NxF32         PxJumpTime;
    NxF32         G;
    NxU32         collisionFlags;

    MyRagdoll       *doll;        //регдолл

    int  mStrength;
    int  mEndurance;
    int  mAgility;
    int  mIntelegent;
    int  mDam;
    bool inBattle;    //флаг боя
    CharacterObj  *target;  //кого щас бьём

  public:
    virtual void CreateCharacterController(Vector3 position)
    {
      //чарактер контроллер
      mAllocator = new UserAllocator;
      PxControllerMgr = NxCreateControllerManager(mAllocator);
      PxController = new MyCharacterController(PxControllerMgr, mPxScene, mPxMgr->toNx(position), 6, 18);
      PxJump = false;   //флаг прыжка
      PxPushCharacter = false;  //флаг движения
      G = -98.1f;         //гравитация
      NxCharacterSpeed = 300;   //скорость
      //    AddUserDataToActors( mPxScene);
      mActor = PxController->GetCharacterActor();
      mActor->setGroup(CollisionGroup::GROUP_COLLIDABLE_PUSHABLE);
      mActor->setMass(75);
      mPxMgr->addAtom(mName, mActor, mMainNode);
    }

    virtual void Hit(CharacterObj* enemy, int dam)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::Hit");
      Vector3 enemypos = enemy->getWorldPosition();
      Vector3 mypos = mMainNode->_getDerivedPosition();
      float dir = mypos.distance(enemypos);

      if(dir < 80)
      {
        enemy->damageHandler(dam, static_cast<CharacterObj*>(this));
      }

      target = enemy;
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(dir));
    }

    virtual bool damageHandler(int dam, CharacterObj* enemy)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::damageHandler");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(dam));

      if(mBlock == 0)
      {
        mHealth = mHealth - dam;
        target = enemy;
        inBattle = true;

        if(!mIsDead)
          if(mHealth <= 0)
          {
            mIsDead = true;
          };
      }

      return mIsDead;
    }

    virtual String getType()
    {
      return "CharacterObj";
    }

    virtual void update(Real elapsedTime)
    {
      return;
    }

    NxF32 GetHeight(NxF32 elapsedTime)
    {
      if(!PxJump)
      {
        return 0.0f;
      }

      NxF32 Vt = PxV0 + G * PxJumpTime; // Vt = Vo + GT
      PxJumpTime += elapsedTime;
      return Vt * elapsedTime + 1 / 2 * G * elapsedTime * elapsedTime; // S = VtT + 1/2GT^2
    }

    inline ActionManager *getActionManager()
    {
      return mActionManager;
    }

    inline bool IsDead()
    {
      return mIsDead;
    }

    inline int getDamage()
    {
      return mDam;
    }

    inline void setDamage(int dam)
    {
      mDam = dam;
    }

    inline void setBlock(bool b)
    {
      mBlock = b;
    }

    inline int getHealth()
    {
      return mHealth;
    }

    inline int getSpeed()
    {
      return NxCharacterSpeed;
    }

    inline void setSpeed(int speed)
    {
      NxCharacterSpeed = speed;
    }

    Vector3 getDispVector()
    {
      return mPxMgr->toOgre(disp);
    }

    void addAnimationState(String animName, String  mainSkel)
    {
      /*
      Функция добавляет анимацию animName из файла animName.skeleton на скелет из файла mainSkel.
      Экспортишь просто модельки со скелетом, безо всяких анимаций. Потом в Максе(например) добавляешь на скелет анимации и экспортишь.
      Получаешь еще один *.skeleton файл, только уже с анимацией. Называешь его так же как анимация(ну или можешь чуть переделать функицю,
      там совсем просто, и называть как угодно). Юзаешь функцию выше на основной скелет и у скелета появляется анимация.
      Только функцию надо юзать ДО ТОГО как ты создал Entity объекта со скелетом, на который анимации добавляешь.
      */
      SkeletonPtr pSkeletonMaster = SkeletonManager::getSingleton().getByName(mainSkel);

      if(!pSkeletonMaster->hasAnimation(animName))
      {
        SkeletonPtr pNewSkeleton =  SkeletonManager::getSingleton().load(animName + ".skeleton", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Skeleton::BoneHandleMap boneHandleMap;
        pNewSkeleton->_buildMapBoneByHandle(pNewSkeleton.getPointer(), boneHandleMap);
        pSkeletonMaster->_mergeSkeletonAnimations(pNewSkeleton.getPointer(), boneHandleMap);
        SkeletonManager::getSingleton().remove(animName + ".skeleton");
      }
    }

    virtual void getConfigPlayer(std::vector<configAnims> &ca)
    {
      configAnims mConfigAnims;
      mConfigAnims.mActions = IDLE;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "idle";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = WALKF;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "walk_forward";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = WALKL;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "walk_left";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Limit;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = WALKR;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "walk_right";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Limit;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = WALKB;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "walk_backward";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = SLOWF;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "idle";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = SLOWB;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "idle";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = RUNF;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.2;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "walk_fast_forward";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = ONEHANDIDLE;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "one_hand_idle";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = ATTACK1;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = false;
      mConfigAnims.mName = "one_hand_attack_left";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Limit;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = ATTACK2;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = false;
      mConfigAnims.mName = "handtohand_hook";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Limit;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = ONEHANDBLOCK;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "one_hand_block";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
      mConfigAnims.mActions = ONEHANDFORWARD;
      mConfigAnims.mSpeed = 1;
      mConfigAnims.mDuration = 0.1;
      mConfigAnims.mLooped = true;
      mConfigAnims.mName = "one_hand_forward";
      mConfigAnims.mSoundEnabled = false;
      mConfigAnims.mBreak = false;
      mConfigAnims.mActionTermination = Normal;
      mConfigAnims.mTransition = BlendThenAnimate;
      ca.push_back(mConfigAnims);
    }
};