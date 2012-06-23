#pragma once

#include <Ogre.h>
#include "BaseObj.h"
#include "Player.h"
#include <vector>
#include "Items.h"

#include "SharedData.h"

using namespace std;

using namespace Ogre;

using namespace mySharedData;

class Creature : public CharacterObj
{
  private:
    bool            inWork;   //���� ��������
    Vector3     targetPos;  //���� ��� ���
    PathFinding     *PF;    //������ ���������
    Vector3     To;     //���� ������
    bool      isGo;   //��� ���
    Real      hittime;  //����� ����� ������� � ��������
    Real      ElapsedTime;//����� �����

    enum actions
    {
      moving,
      battle,
      doAction
    };

    std::vector<actions> actionsVec;

  public:

    Inventory *CharacterInv;      //���������

    Creature(String name, SceneManager* sceneMgr, String meshName, Player *player, Vector3 pos)
    {
      inBattle = false;
      inWork = false;
      isGo = false;
      mName = name;
      mSceneMgr = sceneMgr;
      mPosition = pos;
      mOrientation = Quaternion(0, 1, 0, 0);
      mPxMgr = mPxManager;
      mPxSDK = mPxManager->PxSDK;
      mPxScene = mPxManager->PxScene;
      hittime = 3;
      CharacterInv = new Inventory(4);
      //��������� ������
      mHealth = 100;
      mIsDead = false;
      mBlock  = false;
      inWork = false;
      //�������
      mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, mPosition);
      mEntity = mSceneMgr->createEntity(mName, meshName);
      mMainNode->scale(25, 25, 25);
      OffsetNode = mMainNode->createChildSceneNode(mName + "Child", Vector3(0, -0.91, 0));
      OffsetNode->attachObject(mEntity);
      OffsetNode->setOrientation(mMainNode->getOrientation());
      //������
      CreateCharacterController(mPosition);
      disp = NxVec3(0, -98.1f, 0);
      To = mMainNode->getPosition();
      LogManager::getSingletonPtr()->logMessage(mName + "::Created");
    };

    String getType()
    {
      return "Creature";
    }

    void createDoll()
    {
      //    mEntity->getSkeleton()->getBone( "Bip01 Head")->getPosition();
      //    doll->head->setGlobalPosition( )
    };

    Inventory* getInvebtory()
    {
      return CharacterInv;
    }

    void Battle(CharacterObj *target)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::In Battle");
      hittime += ElapsedTime;
      //������� � ����. � ���������)
      Vector3 tmp, tmp2;
      tmp = mMainNode->getPosition();
      tmp.y = 0;
      tmp2 = target->getPosition();
      tmp2.y = 0;

      if(target->getHealth() > 0 && tmp.distance(tmp2) < 2000)
      {
        if(tmp.distance(tmp2) > 50.0)
        {
          // ����  50<���������<1000 ��������� ����������� �� ������������ ������. ���� �� ��������� �� ���� ��� ���� �����
          if(!isGo)
          {
            GoTo(mMainNode->getPosition(), target->getWorldPosition());
          }
        }
        else
        {
          StopWalk();

          if(hittime > 5)    //1 ���� � 5 ���.
          {
            this->getActionManager()->DoAction(ATTACK1);
            this->Hit(target, 10);
            hittime = 0;
          }
        }
      }
      else
      {
        inBattle = false;
      }

      return;
    };

    void OnTimeAction(Vector2 time)//� ����������� �� ������� ���������� �������� �� ������ ���������� ��������� �������� (������).
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction");

      if(time.x < 20.0f && time.x > 8.0f)
      {
        LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction = �������");
        To = Vector3(Math::RangeRandom(0, 0), 0, Math::RangeRandom(20000, 20000));
        GoTo(mMainNode->getPosition(), To);
        inWork = true;
      }
      else
      {
        LogManager::getSingletonPtr()->logMessage(mName + "::OnTimeAction = ������");
        disp = NxVec3(0, -98, 0);
        inWork = true;
      }
    };

    void GoTo(Vector3 From, Vector3 To)
    {
      LogManager::getSingletonPtr()->logMessage(mName + "::GoTo");
      LogManager::getSingletonPtr()->logMessage("From");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(From));
      LogManager::getSingletonPtr()->logMessage("To");
      LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(To));
      /*    targetPos = To; // �� ������ ���������� ��������
          PF = new PathFinding();
          Vector2 F,T;
          std::vector<Vector2> Path;
          F.x = From.x;
          F.y = From.z;
          T.x = To.x;
          T.y = To.z;
          Path = PF->FindPath( F, T);
          delete PF;
          for (unsigned int i=0;i<Path.size();i++)
          {
          LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString(Path[i]));
          mMainNode->translate(Path[i].x, 0,Path[i].y); //��� �� ���� ����� � �������� ������� ���� ���������.
          if(mMainNode->getPosition().x == T.x && mMainNode->getPosition().y == T.y )
            break;
          //if �������� �� ������ ���!!!!!!!!
          }*/
      isGo = true;
      horizontalDisp = mPxMgr->toNx(To - From);
      disp.x = horizontalDisp.x;
      disp.z = horizontalDisp.z;
      disp.normalize();
      disp.y = -98.1f;
      //disp *= ElapsedTime; �������� ������� ��������
      //    mActionManager->DoAction( WALKF);
      return;
    };

    void StopWalk()
    {
      isGo = false;
      disp.x = 0;
      disp.z = 0;
      //    mActionManager->addTime(IDLE);
    }

    void update(Real elapsedTime, Vector2 time)
    {
      ElapsedTime = elapsedTime;

      //���������� ��
      if(!mIsDead)
      {
        if(!inBattle)
        {
          if(time.x < 20.0f && time.x > 8.0f)
          {
            inWork = false;
          }

          Vector3 tmp, tmp2;
          tmp = mMainNode->getPosition();
          tmp.y = 0;
          tmp2 = To;
          tmp2.y = 0;

          if(tmp.distance(tmp2) < 50.0)
          {
            if(inWork != true) //�������� �� ������� ��������, ���� ���� ���������� ���� ���� �������� ����� � ����������� �� �������
            {
              OnTimeAction(time);
            }
          }
          else
          {
            inWork = false;
          }
        }
        else
        {
          Battle(target);
        }
      }
      else
      {
        disp = NxVec3(0, -98.1, 0);
        createDoll();
      }

      //���������� ����������� � ����
      PxController->Move(disp, collisionFlags);
      PxControllerMgr->updateControllers();
      MeshDisp = mPxMgr->toOgre(disp) * (-1);
      MeshDisp.y = 0;
      mMainNode->setDirection(MeshDisp);
      //    mActionManager->addTime( elapsedTime);
    };
    ~Creature(void)
    {
      mActionManager;
      mMainNode->detachAllObjects();
      mSceneMgr->destroyEntity(mEntity);
      mMainNode->removeAndDestroyAllChildren();
      mSceneMgr->destroySceneNode(mName);
    };
};
