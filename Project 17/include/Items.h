#pragma once

#include "BaseObj.h"
#include "PhysxManager.h"
#include "SceneLoader.h"

using namespace Ogre;

enum ItemType
{
  MELE_1H,
  BOW,
  FOOD
};

class Item : public BaseObj
{
  protected:
    ItemStruct    *ItemStr;

  public:

    Item() {};

    Item(SceneManager *scene, PhysxManager *pxMgr, ItemStruct* istr)
    {
      ItemStr   = istr;
      mSceneMgr = scene;
      mPxMgr    = pxMgr;
      mPxScene  = mPxMgr->PxScene;
      mPxSDK    = mPxMgr->PxSDK;
      mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName + "_Item");
      mEntity   = mSceneMgr->createEntity(ItemStr->name, ItemStr->meshname);
    }

    void setPosition(Vector3 pos)
    {
      if(!mEntity->isAttached())
      {
        mMainNode->attachObject(mEntity);
      }

      mMainNode->_setDerivedPosition(pos);
    }

    void setOrientation(Quaternion orient)
    {
      if(!mEntity->isAttached())
      {
        mMainNode->attachObject(mEntity);
      }

      mMainNode->setOrientation(orient);
    }

    String getId()
    {
      return ItemStr->id;
    }

    String getType()
    {
      return "Item";
    }


    void createPxBody()
    {
      mActor = mPxMgr->LoadXmlShape(ItemStr->meshname + ".xml");
      mPxMgr->addAtom(mName, mActor, mMainNode);
    }

    String getMesh()
    {
      return ItemStr->meshname;
    }

    int getDamage()
    {
      return ItemStr->damage;
    }

    void unloadMesh()
    {
      mSceneMgr->destroyEntity(mEntity->getName());
    }

    void setSceneNode(SceneNode* node)
    {
      mMainNode = node;
      mMainNode->attachObject(mEntity);
    }

    void createMesh()
    {
      mEntity = mSceneMgr->createEntity(ItemStr->name + "ent" , ItemStr->meshname);
    }

    ItemStruct* getItemStruct()
    {
      return ItemStr;
    }

    ~Item()
    {
      ItemStr = nullptr;
      mSceneMgr->destroyEntity(mEntity->getName());
      mMainNode->removeAndDestroyAllChildren();
      mSceneMgr->destroySceneNode(mMainNode->getName());
    };
};

class Inventory
{
  protected:
    uint mInvMaxSize;       //размер инвентаря
  public:
    std::vector<ItemStruct*>  mItemsVector; //вектор с объектами лежащими в инвентаре

    Inventory(int size)
    {
      mInvMaxSize = size;
    }

    //если нет места возвращает фалс
    bool addItem(ItemStruct *newitem)
    {
      if(mItemsVector.size() < mInvMaxSize)
      {
        mItemsVector.push_back(newitem);
        return true;
      }
      else
      {
        return false;
      }
    }

    ItemStruct* getItem(String name)
    {
      for(std::vector<ItemStruct*>::iterator iter = mItemsVector.begin(); iter != mItemsVector.end(); ++iter)
        if((*iter)->name == name)
        {
          return *iter;
        }
        else
        {
          return false;
        }
    }

    void removeItemName(String name)
    {
      for(std::vector<ItemStruct*>::iterator iter = mItemsVector.begin(); iter != mItemsVector.end(); ++iter)
        if((*iter)->name == name)
        {
          mItemsVector.erase(iter);
        }
    }

    ~Inventory() {};
};
