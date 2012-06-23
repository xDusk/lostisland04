//////////////////////////////////////////////////////////////////////////
///////Класс расширеной камеры. камера вращается вокруг цели.   //////////
//////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
#pragma once

#include <Ogre.h>

using namespace Ogre;

//Класс расширенной камеры
class ExtendedCamera
{
  protected:
    SceneNode     *mTargetNode;   // Цель камеры
    Camera        *mCamera;     // Наша камера
    SceneNode     *mCameraNode;   // узел камеры
    SceneManager    *mSceneMgr;     // манагер сцены
    String        mName;        // имя  хз зачем.
    Real        r;          // радиус (расстояние от камеры до цели)
    Real        angleX;     // угол X с мыши
    Real        angleY;     // угол Y с мыши
    bool mOwnCamera;// Для того чтобы знать была ли камера создана вне этого класса
    Real mTightness;//Удерживать передвижение камеры 1 - значит  плотное передвижение, 0 значит не передвигаться вообще

  public:
    ExtendedCamera(String name, SceneManager *sceneMgr, Camera *camera = 0, Vector3 target = Vector3(0, 0, 0))
    {
      // Установка базовых членов ссылки
      mName = name;
      mSceneMgr = sceneMgr;
      mTargetNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mName, target);

      // Создание нашей камеры, если она не прошла как параметр
      if(camera == 0)
      {
        mCamera = mSceneMgr->createCamera(mName);
        mOwnCamera = true;
      }
      else
      {
        mCamera = camera;
        mOwnCamera = false;
      }

      // значения по умолчанию
      r = 100;
      angleX = -1.57;
      angleY = 0;
    }

    ~ExtendedCamera()
    {
      if(mOwnCamera)
      {
        delete mCamera;
      }
    }

    void setAngles(Real Xrel = 0, Real Yrel = 0)
    {
      if((angleY + Yrel * 0.01571 > -1.5) && (angleY + Yrel * 0.01571 < 1.5)) // проверка на углы!
      {
        angleY = angleY + Yrel * 0.01571;
      }

      angleX = angleX + Xrel * 0.01571;
    }

    void setRadius(Real rad)
    {
      r = r + rad / 100;
    }

    float getAngleX()
    {
      return angleX;
    }
    float getAngleY()
    {
      return angleY;
    }

    SceneNode *getTargetNode()
    {
      return mTargetNode;
    }
    Camera *getCamera()
    {
      return mCamera;
    }

    Vector3 getCameraPosition()
    {
      return mCamera->getPosition();
    }

    void update(Real elapsedTime, Vector3 targetPosition , bool lockcam)
    {
      mTargetNode->setPosition(targetPosition);
      lockcam = false;

      if(lockcam == false)
      {
        Real cX = cos(angleY) * cos(angleX);
        Real cY = sin(angleY);
        Real cZ = cos(angleY) * sin(angleX);
        mCamera->setPosition(mTargetNode->_getDerivedPosition().x + cX * r, mTargetNode->_getDerivedPosition().y + r * cY, mTargetNode->_getDerivedPosition().z + r * cZ);
        mCamera->lookAt(mTargetNode->_getDerivedPosition());
      }
      else
      {
        Real cX = cos(angleY) * cos(angleX);
        Real cY = sin(angleY);
        Real cZ = cos(angleY) * sin(angleX);
        mCamera->setPosition(mTargetNode->_getDerivedPosition().x + cX * r, mTargetNode->_getDerivedPosition().y + r * cY, mTargetNode->_getDerivedPosition().z + r * cZ);
      }

      mCamera->lookAt(targetPosition);
    }
};
