#pragma once

#include "HydraxManager.h"
#include "SkyManager.h"
#include "PGManager.h"
#include "PhysxManager.h"
#include "OgrePageManager.h"

namespace mySharedData
{
  static HydraxManager  *mHydraxMgr;
  static GamePGManager  *mPGManager;
  static SkyManager   *mSkyManager;
  static PhysxManager   *mPxManager;
  static TerrainGroup   *mTerrainGroup;
  static PageManager    *mPageManager;
  static Ogre::Camera   *mCamera;
}