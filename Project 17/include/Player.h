//////////////////////////////////////////////////////////////////////////
///////Класс игрока.                      //////////
//////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
#pragma once

#include "BaseObj.h"
#include "Items.h"
#include "configAnims.h"
#include "ActionManager.h"
#include "OgreTextAreaOverlayElement.h"

class Player : public CharacterObj
{
  protected:
    int       go;             // 1 вперёд 2 назад 3 влево 4 вправо
    ItemStruct    *EquipedWeapon; // текущее оружие
    Entity      *WeaponEntity;
    Entity      *HeadEntity;
    Entity      *LeftEyeEntity;
    Entity      *RightEyeEntity;
    Entity      *EarsEntity;
    SceneNode   *HeadNode;
    SceneNode   *WeaponNode;
    ItemStruct    *EquipedArmour; //
    bool      battle;     // вытащено ли оружие=)
    bool      isCurrentWeapon;
    bool      ragdoll;

  public:
    Inventory *PlayerInv;     //инвентарь

    Player::Player(String name, PhysxManager *pxMgr, SceneManager *sceneMgr, NxPhysicsSDK *pxSdk, NxScene *pxScene, Vector3 location, Quaternion orientation)
    {
      mName = name;
      mPxMgr = pxMgr;
      mSceneMgr = sceneMgr;
      mPxSDK = pxSdk;
      mPxScene = pxScene;
      mPosition = location;
      mOrientation = orientation;
      //инвентарь. 10 слотов.
      PlayerInv = new Inventory(10);
      focusedTarget = NULL;
      mHealth = 1000;   //здоровье
      mIsDead = false;  //флаг смерти
      mBlock = false;   //флаг блока
      battle = false;   //флаг битвы
      ragdoll = false;  //рагдол сначало вырублен
      //Добавляем анимации к скелету
      SkeletonManager::getSingleton().load("skeleton.skeleton", "General");
      addAnimationState("idle", "skeleton.skeleton");
      addAnimationState("walk_forward", "skeleton.skeleton");
      addAnimationState("walk_left", "skeleton.skeleton");
      addAnimationState("walk_right", "skeleton.skeleton");
      addAnimationState("walk_backward", "skeleton.skeleton");
      addAnimationState("walk_fast_forward", "skeleton.skeleton");
      //
      addAnimationState("one_hand_attack_left", "skeleton.skeleton");
      addAnimationState("handtohand_hook", "skeleton.skeleton");
      addAnimationState("one_hand_idle", "skeleton.skeleton");
      addAnimationState("one_hand_block", "skeleton.skeleton");
      addAnimationState("one_hand_forward", "skeleton.skeleton");
      //узел игрока
      mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, mPosition);
      //    mMainNode->setOrientation( mOrientation);
      //меш игрока
      //так генерить тангент вектор. если он небыл экспортирован
      //    MeshPtr pMesh = MeshManager::getSingleton().load( "Guard.mesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      //    unsigned short src, dest;
      //    if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
      //      pMesh->buildTangentVectors(VES_TANGENT, src, dest);
      mEntity = mSceneMgr->createEntity("Guard.mesh");
      mMainNode->scale(10, 10, 10);
      mMainNode->yaw(Degree(180));
      OffsetNode = mMainNode->createChildSceneNode(mName + "Child", Vector3(0, -1.5, 0));
      OffsetNode->attachObject(mEntity);
      OffsetNode->setOrientation(mMainNode->getOrientation());
      HeadEntity = mSceneMgr->createEntity(mName, "head.mesh");
      HeadNode  = OffsetNode->createChildSceneNode(mName + "Head", Vector3(0, mEntity->getBoundingBox().getSize().y - 0.1, 0));
      HeadNode->attachObject(HeadEntity);
      LeftEyeEntity = mSceneMgr->createEntity("leftEye", "EyeLeftHuman.mesh");
      LeftEyeEntity->setMaterialName("humaneyel");
      RightEyeEntity = mSceneMgr->createEntity("rigthEye", "EyeRightHuman.mesh");
      RightEyeEntity->setMaterialName("humaneyer");
      HeadNode->attachObject(LeftEyeEntity);
      HeadNode->attachObject(RightEyeEntity);
      EarsEntity = mSceneMgr->createEntity("ears", "ears.mesh");
      HeadNode->attachObject(EarsEntity);
      disp = NxVec3(0, -98.1f, 0);
      //создаём контроллер
      CreateCharacterController(mPosition);
      //вектор со всеми анимациями
      std::vector<configAnims> ca;
      getConfigPlayer(ca);
      mActionManager = new ActionManager(mEntity, ca);
      mActionManager->init();
      //оружие
      isCurrentWeapon = false;
    }

    Player::~Player()
    {
      delete mActionManager;
      mMainNode->detachAllObjects();
      mSceneMgr->destroyEntity(mEntity);
      mMainNode->removeAndDestroyAllChildren();
      mSceneMgr->destroySceneNode(mName);
    }
    ItemStruct* getCurentWeapon()
    {
      return EquipedWeapon;
    }

    void setBattle(bool b)
    {
      battle = b;
    }

    bool getBattle()
    {
      return battle;
    }

    Vector3 getWorldPosition()
    {
      return  mMainNode->_getDerivedPosition();
    }
    Vector3 getOffsetPosition()
    {
      return OffsetNode->_getDerivedPosition();
    }

    void setCurentWeapon(ItemStruct* weapon)
    {
      isCurrentWeapon = true;
      EquipedWeapon = weapon;
      WeaponEntity = mSceneMgr->createEntity(EquipedWeapon->name + "_ent" , EquipedWeapon->meshname);
      WeaponNode = OffsetNode->createChildSceneNode("Wnode");
      WeaponNode->setPosition(mEntity->getSkeleton()->getBone("Weapon")->_getDerivedPosition());
      //..
      WeaponNode->setScale(0.1, 0.1, 0.1);
    }

    Inventory* getInventory()
    {
      return PlayerInv;
    }

    Entity* getEntity()
    {
      return mEntity;
    }

    void goAcross()
    {
      go = 2;
      NxCharacterSpeed = 200;
      PxPushCharacter = true;
    }

    void goAlongFast()
    {
      go = 1;
      NxCharacterSpeed = 300;
      PxPushCharacter = true;
    }

    void goAlong()
    {
      go = 1;
      NxCharacterSpeed = 200;
      PxPushCharacter = true;
    }

    void goLeft()
    {
      go = 3;
      NxCharacterSpeed = 200;
      PxPushCharacter = true;
    }

    void goRight()
    {
      go = 4;
      NxCharacterSpeed = 200;
      PxPushCharacter = true;
    }

    void StartJump(NxF32 v0)
    {
      if(PxJump)
      {
        return;
      }

      PxJumpTime = 0.001f;
      PxV0 = v0;
      PxJump = true;
    }

    void StopJump()
    {
      PxJump = false;
    }

    void ragdollOn()
    {
      doll = new MyRagdoll(mPxMgr->getPxScene(), mPxMgr->PxSDK , OffsetNode->_getDerivedPosition(), OffsetNode->_getDerivedOrientation(), mEntity->getSkeleton(), mPxMgr);
      ragdoll = true;
      //сносим Чк  ///удаление актора добить
      //    mPxMgr->destroyAtom( PxController->GetCharacterActor());
      //    PxController->~MyCharacterController();
      //ручное управление вкл.
      mEntity->getSkeleton()->getBone("Bip01")->setManuallyControlled(true);
      LogManager::getSingleton().logMessage(StringConverter::toString(mEntity->getSkeleton()->getBone("Bip01")->_getDerivedPosition()));
      HeadEntity->getSkeleton()->getBone("Bip01 Head")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Neck1")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Neck")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Spine2")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Spine1")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Spine")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 Pelvis")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L Calf")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R Calf")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L Foot")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R Foot")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L Thigh")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R Thigh")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L Forearm")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R Forearm")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L Hand")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R Hand")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 L UpperArm")->setManuallyControlled(true);
      mEntity->getSkeleton()->getBone("Bip01 R UpperArm")->setManuallyControlled(true);
    }

    void ragdollUpdate()
    {
      mMainNode->_setDerivedPosition(OffsetNode->_getDerivedPosition());
      mMainNode->_setDerivedOrientation(mPxMgr->toOgre(doll->pelvis->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 Head")->_setDerivedPosition( mPxMgr->toOgre( doll->head->getGlobalPosition()));
      HeadEntity->getSkeleton()->getBone("Bip01 Head")->_setDerivedOrientation(mPxMgr->toOgre(doll->head->getGlobalOrientationQuat()));
      mEntity->getSkeleton()->getBone("Bip01 Neck")->_setDerivedOrientation(mPxMgr->toOgre(doll->neck->getGlobalOrientationQuat()));
      mEntity->getSkeleton()->getBone("Bip01 Spine2")->_setDerivedOrientation(mPxMgr->toOgre(doll->spine2->getGlobalOrientationQuat()));
      mEntity->getSkeleton()->getBone("Bip01 Spine1")->_setDerivedOrientation(mPxMgr->toOgre(doll->spine1->getGlobalOrientationQuat()));
      mEntity->getSkeleton()->getBone("Bip01 Spine")->_setDerivedOrientation(mPxMgr->toOgre(doll->spine->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Calf")->_setDerivedPosition( mPxMgr->toOgre( doll->leftCalf->getGlobalPosition()));
      mEntity->getSkeleton()->getBone("Bip01 L Calf")->_setDerivedOrientation(mPxMgr->toOgre(doll->leftCalf->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Calf")->_setDerivedPosition( mPxMgr->toOgre( doll->rightCalf->getGlobalPosition()));
      mEntity->getSkeleton()->getBone("Bip01 R Calf")->_setDerivedOrientation(mPxMgr->toOgre(doll->rightCalf->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Foot")->_setDerivedPosition( mPxMgr->toOgre( doll->leftFoot->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Foot")->_setDerivedOrientation( mPxMgr->toOgre( doll->leftFoot->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Foot")->_setDerivedPosition( mPxMgr->toOgre( doll->rightFoot->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Foot")->_setDerivedOrientation( mPxMgr->toOgre( doll->rightFoot->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Thigh")->_setDerivedPosition( mPxMgr->toOgre( doll->leftThigh->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Thigh")->_setDerivedOrientation( mPxMgr->toOgre( doll->leftThigh->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Thigh")->_setDerivedPosition( mPxMgr->toOgre( doll->rightThigh->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Thigh")->_setDerivedOrientation( mPxMgr->toOgre( doll->rightThigh->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 Pelvis")->_setDerivedPosition( mPxMgr->toOgre( doll->pelvis->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 Pelvis")->_setDerivedOrientation( mPxMgr->toOgre( doll->pelvis->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Forearm")->_setDerivedPosition( mPxMgr->toOgre( doll->leftForeArm->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Forearm")->_setDerivedOrientation( mPxMgr->toOgre( doll->leftForeArm->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Forearm")->_setDerivedPosition( mPxMgr->toOgre( doll->rightForeArm->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Forearm")->_setDerivedOrientation( mPxMgr->toOgre( doll->rightForeArm->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Hand")->_setDerivedPosition( mPxMgr->toOgre( doll->leftHand->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L Hand")->_setDerivedOrientation( mPxMgr->toOgre( doll->leftHand->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Hand")->_setDerivedPosition( mPxMgr->toOgre( doll->rightHand->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R Hand")->_setDerivedOrientation( mPxMgr->toOgre( doll->rightHand->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L UpperArm")->_setDerivedPosition( mPxMgr->toOgre( doll->leftUpperArm->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 L UpperArm")->_setDerivedOrientation( mPxMgr->toOgre( doll->leftUpperArm->getGlobalOrientationQuat()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R UpperArm")->_setDerivedPosition( mPxMgr->toOgre( doll->rightUpperArm->getGlobalPosition()));
      //    mEntity->getSkeleton()->getBone( "Bip01 R UpperArm")->_setDerivedOrientation( mPxMgr->toOgre( doll->rightUpperArm->getGlobalOrientationQuat()));
    }

    bool getPushChar()
    {
      return PxPushCharacter;
    }

    void update(Real elapsedTime, Vector3 camPos)
    {
      HeadNode->setPosition(mEntity->getSkeleton()->getBone("Bip01 Head")->_getDerivedPosition());
      HeadNode->setOrientation(mEntity->getSkeleton()->getBone("Bip01 Head")->_getDerivedOrientation());
      HeadNode->roll(Degree(-90));

      if(ragdoll == true)
      {
        ragdollUpdate();
      }
      else
      {
        //проверяем длительность блока. если слишком долго, отжимаем=)
        if(mBlock > 0)
        {
          mBlock = mBlock - elapsedTime;
        }

        if(mBlock < 0)
        {
          mBlock = 0;
        }

        //обновляем оружие
        if(battle == true)
        {
          if(isCurrentWeapon)
          {
            WeaponNode->setPosition(mEntity->getSkeleton()->getBone("Weapon")->_getDerivedPosition());
            WeaponNode->setOrientation(mEntity->getSkeleton()->getBone("Weapon")->_getDerivedOrientation());
            WeaponNode->pitch(Degree(-90));
          }
        }
        else
        {
          if(isCurrentWeapon)
          {
            WeaponNode->setPosition(mEntity->getSkeleton()->getBone("Bip01 Spine2")->_getDerivedPosition());
            WeaponNode->translate(Vector3(-8, 15, -10));
            WeaponNode->setOrientation(mEntity->getSkeleton()->getBone("Bip01 Spine2")->_getDerivedOrientation());
            WeaponNode->pitch(Degree(90));
            WeaponNode->roll(Degree(-10));
            WeaponNode->yaw(Degree(-110));
          }
        }

        //обнуляем промежуточнй вектор направления
        horizontalDisp.x = 0;
        horizontalDisp.z = 0;

        //если чарактер двигается выщитываем вектор направления
        if(PxPushCharacter)
        {
          horizontalDisp = mPxMgr->toNx(mMainNode->_getDerivedPosition() - camPos);
          horizontalDisp.y = 0.0f;
          horizontalDisp.normalize();
        }

        //обнуляем конечный вектор направления
        disp.x = 0;
        disp.z = 0;

        //если чарактер двигается выщитываем конечный вектор движения
        if(PxPushCharacter)
        {
          SceneNode *nodeTmp;

          switch(go)
          {
            case 1:
            {
              disp += horizontalDisp * NxCharacterSpeed;
            }
            break;
            case 2:
            {
              disp += horizontalDisp * NxCharacterSpeed;
              disp = disp * (-1);
            }
            break;
            case 3:
            {
              disp += horizontalDisp * NxCharacterSpeed;
              Vector3 tmp = mPxMgr->toOgre(disp);
              tmp.y = 0;
              tmp = Vector3(tmp.z, 0, -tmp.x);
              tmp = tmp.reflect(Vector3(0, 1, 0));
              disp = mPxMgr->toNx(tmp);
            }
            break;
            case 4:
            {
              disp += horizontalDisp * NxCharacterSpeed;
              Vector3 tmp = mPxMgr->toOgre(disp);
              tmp.y = 0;
              tmp = Vector3(-tmp.z, 0, tmp.x);
              disp = mPxMgr->toNx(tmp);
            }
            break;
          }
        }

        if(!PxJump)
        {
          disp.y = -98;
        }
        else
        {
          disp.y = 0;
        }

        disp *= elapsedTime;
        disp.y *= 2;
        NxF32 height = GetHeight(elapsedTime * 2);

        if(height != 0.0f)
        {
          disp.y += height;
        }

        PxController->Move(disp, collisionFlags);

        if(collisionFlags & NXCC_COLLISION_DOWN)
        {
          StopJump();
        }

        // если чарактер двигается поворачиваем меш соответсвенно направлению движения
        if(PxPushCharacter)
        {
          MeshDisp = mPxMgr->toOgre(disp) * (-1);
          MeshDisp.y = 0;
        }

        mMainNode->setDirection(MeshDisp);
        //обновляем контроллер
        PxControllerMgr->updateControllers();
        //и анимацию
        mActionManager->addTime(elapsedTime);
      }
    }
};
