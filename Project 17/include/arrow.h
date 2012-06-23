#include "header.h"

class Arrow: public BaseObj
{
  protected:
    Vector3 mDestination; //����� ���� ������� ������
    bool mNotUse;
    Real mTimer;
    Real mSpeed;

    SceneManager *mSceneMgr;
    CharacterObj *mCo;    //��������� �� �������
    CharacterObj *mRo;    //��������� ��������

    NxActor *actor;

  public:
    Arrow(String name, SceneManager *sceneMgr, Real nscale, CharacterObj *pl, CharacterObj *pObject)
    {
      mName = name;
      mSceneMgr = sceneMgr;
      mRo = pl;
      mCo = pObject;
      mDestination = mRo->getMainNode()->getPosition();
      Vector3 zmPos = mCo->getMainNode()->getPosition();
      Vector3 dir = mDestination - zmPos;
      dir.normalise();
      mDestination += dir * 6000;
      mDestination.y += 100;
      mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, Vector3(zmPos.x, zmPos.y + 100, zmPos.z));
      mEntity = mSceneMgr->createEntity(mName, "Arrow.mesh");
      mMainNode->attachObject(mEntity);
      dir = mMainNode->getPosition();
      dir.y = 0;
      dir -= Vector3(mDestination.x, 0, mDestination.z);
      mMainNode->setDirection(dir);
      mNotUse = false;
      mTimer = 0;
      mSpeed = 0.1;
      //    actor->addForce( toNx( mCo->getDispVector().x, mCo->getDispVector().y, 0));
    }
    ~Arrow()
    {
      mMainNode->removeAndDestroyAllChildren();
      mSceneMgr->destroySceneNode(mName);
      mSceneMgr->destroyEntity(mName);
    }
    void update(Real elapsedTime)
    {
      if(mTimer > 30) //����� ����� ������ (���.) �������
      {
        mNotUse = true;
        mCo->setDamage(0);
      }

      mTimer += elapsedTime;
      Vector3 campos = mRo->getMainNode()->getPosition();
      Vector3 ourpos = mMainNode->getPosition();
      Vector3 dir = ourpos - campos;  //���������� ���������� �� ������
      dir.y = 0;
      Real dist = dir.normalise();

      if(dist < 50)   //��������� � ������
      {
        mNotUse = true;
        Vector3 orient = mMainNode->getOrientation() * Vector3::UNIT_Z;
        //mRo->damageHandler(ourpos,orient,30);
        mCo->setDamage(0);
      }

      //���������� ���������� ����������� � ����� ����������
      dir = mDestination - ourpos;
      mMainNode->translate(dir * elapsedTime * mSpeed);
    }
    inline bool NotUse()
    {
      return mNotUse;
    }
    inline SceneNode *getMainNode()
    {
      // ���������� ���� � �������� ����������
      return mMainNode;
    }
    inline String getName()
    {
      return mName;
    }
};