#pragma once

#include <Ogre.h>
#include "OgreTerrainGroup.h"
//Include PagedGeometry headers that will be needed
#include "PagedGeometry.h"
#include "GrassLoader.h"
#include "BatchPage.h"
//#include "WindBatchPage.h"
#include "ImpostorPage.h"
#include "TreeLoader2D.h"
#include "TreeLoader3D.h"
#include "stdio.h"
#include "PhysxManager.h"

using namespace Ogre;   // Using Ogre namespace

using namespace Forests;  // Using Forests namespace

static TerrainGroup *PgTerrainGroup;

class GamePGManager : public FrameListener
{
  protected:
    Camera *pgCam;
    SceneManager *pgScene;
    PhysxManager *mPxManager;

    //  TerrainGroup *mTerrainGroup;

    //Pointers to PagedGeometry class instances:
    std::vector<PagedGeometry*> mAllPagedGeometry;
    GrassLoader*        mGrassLoader;

  public:
    // Define constructor and destructor
    GamePGManager() {}
    GamePGManager(Camera *cam, SceneManager *scene, PhysxManager* mPxMgr, TerrainGroup *terrainGroup);
    ~GamePGManager();

    // Define FrameListener methods
    bool frameStarted(const FrameEvent& evt);
    bool frameEnded(const FrameEvent& evt);

    void addTreeLayer(String posfile, Vector4 bounds, int batchdist, int pagesize, int impostordist, String model);
    void addGrassLayer(String densityMap, Vector3 terrainPosition, Vector2 terrainSize, Real density, Real maxSize, Real minSize, int fadeTech, int grassTech, String material, int numLayer);

};
