#include "PGManager.h"
#include <fstream>

using namespace std;

Real getTerrainHeight(Real x, Real z, void* userData)
{
  return PgTerrainGroup->getHeightAtWorldPosition(x, 0.f, z);
}

GamePGManager::GamePGManager(Camera *cam, SceneManager *scene, PhysxManager *mPxMgr, TerrainGroup *terrainGroup)
{
  pgCam = cam;
  pgScene = scene;
  mPxManager = mPxMgr;
  PgTerrainGroup = terrainGroup;
  //Create and configure a new PagedGeometry instance for grass
  PagedGeometry *grass = new PagedGeometry(pgCam, 1000);
  grass->addDetailLevel<GrassPage>(2000);
  mGrassLoader = new GrassLoader(grass);
  grass->setPageLoader(mGrassLoader);
  mGrassLoader->setBuildEdgesEnabled(false);
  mGrassLoader->setHeightFunction(&getTerrainHeight, NULL);
  mAllPagedGeometry.push_back(grass);
}

GamePGManager::~GamePGManager()
{
}

void GamePGManager::addTreeLayer(String posfile, Vector4 bounds, int batchdist, int pagesize, int impostordist, String model)
{
  PagedGeometry *trees = new PagedGeometry();
  trees->setCamera(pgCam);
  trees->setPageSize(pagesize);
  //  trees->setInfinite();             //Use infinite paging mode
  //  trees->addDetailLevel<WindBatchPage>(1000, 50); //хм что то с ветром мб.
  trees->addDetailLevel<BatchPage>(batchdist, 100);  //расстояние на котором начинают рисоватся спрайты
  trees->addDetailLevel<ImpostorPage>(impostordist, 100);  //макс. расстояние на котором деревья рисуются вообще
  //Create a new TreeLoader2D object
  TreeLoader3D *treeLoader = new TreeLoader3D(trees, TBounds(-10000, -10000, 10000, 10000));   //размеры области. в даном случае совпадает с размером тарана
  trees->setPageLoader(treeLoader);  //Assign the "treeLoader" to be used to load geometry for the PagedGeometry instance
  Forests::ImpostorPage::setImpostorResolution(256);
  //Load a tree entity
  Entity *myEntity = pgScene->createEntity(model, model + ".mesh");
  myEntity->setCastShadows(true);
  treeLoader->setMinimumScale(1);
  treeLoader->setMaximumScale(1);
  String EntityName;
  Vector3 pos;
  String aa;
  Ogre::DataStreamPtr dataStream = Ogre::ResourceGroupManager::getSingleton().openResource(posfile, "General");
  Ogre::MemoryDataStream stream(dataStream);

  while(!stream.eof())
  {
    aa = stream.getLine();
    aa = aa.erase(0, aa.find_first_of(";") + 1);
    pos = Ogre::StringConverter::parseVector3(aa);
    treeLoader->addTree(myEntity, pos, Degree(0), 1.0);
    mPxManager->createBox(pos, Vector3(20, 100, 20), true);
  }

  stream.close();
  mAllPagedGeometry.push_back(trees);
}

void GamePGManager::addGrassLayer(String densityMap, Vector3 terrainPosition, Vector2 terrainSize, Real density, Real maxSize, Real minSize, int fadeTech, int grassTech, String material, int numLayer)
{
  //Add some grass to the scene with GrassLoader::addLayer()
  GrassLayer *grassLayer = mGrassLoader->addLayer(material);
  //Configure the grass layer properties (size, density, animation properties, fade settings, etc.)
  grassLayer->setMinimumSize(minSize, minSize);
  grassLayer->setMaximumSize(maxSize, maxSize);
  grassLayer->setDensityMapFilter(MAPFILTER_NONE);
  //  l->setAnimationEnabled( true);    //Enable animations
  grassLayer->setLightingEnabled(true);    //Enable dynamic lighting
  //  l->setSwayDistribution( 10.0f);   //Sway fairly unsynchronized
  //  l->setSwayLength( 0.5f);      //Sway back and forth 0.5 units in length
  //  l->setSwaySpeed( 0.5f);       //Sway 1/2 a cycle every second
  grassLayer->setDensity(0.01f);     //плотность травы. (насколько часто стоят травинки=))
  //  grassLayer->setFadeTechnique( (FadeTechnique)fadeTech);
  //  grassLayer->setRenderTechnique( (FadeTechnique)grassTech);
  grassLayer->setFadeTechnique(FADETECH_ALPHAGROW);
  grassLayer->setRenderTechnique(GRASSTECH_CROSSQUADS);

  switch(numLayer)
  {
    case 0:
      grassLayer->setDensityMap(densityMap, CHANNEL_RED);
      break;
    case 1:
      grassLayer->setDensityMap(densityMap, CHANNEL_GREEN);
      break;
    case 2:
      grassLayer->setDensityMap(densityMap, CHANNEL_BLUE);
      break;
    case 3:
      grassLayer->setDensityMap(densityMap, CHANNEL_ALPHA);
      break;
    default:
      break;
  }

  grassLayer->setMapBounds(TBounds(terrainPosition.x - (terrainSize.x / 2), terrainPosition.z - (terrainSize.y / 2), terrainPosition.x + (terrainSize.x / 2), terrainPosition.z + (terrainSize.y / 2)));
}


bool GamePGManager::frameStarted(const FrameEvent& evt)
{
  OgreProfileBegin("PG Update");

  for(std::vector<PagedGeometry*>::iterator iter = mAllPagedGeometry.begin(); iter != mAllPagedGeometry.end(); ++iter)
  {
    (*iter)->update();
  }

  OgreProfileEnd("PG Update");
  return true;
}

bool GamePGManager::frameEnded(const FrameEvent& evt)
{
  return true;
}
