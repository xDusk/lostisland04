#pragma once

#include <Ogre.h>
#include "OgrePage.h"
#include "OgrePagedWorld.h"
#include "OgrePageManager.h"
#include "OgrePaging.h"
#include "OgreTerrain.h"
#include "OgreTerrainPaging.h"
#include "OgreTerrainGroup.h"
#include "myTerrainMaterialGenerator.h"
#include "xml/tinyxml.h"

using namespace Ogre;

class MyTerrain
{
  protected:
    SceneManager    *mSceneMgr;

    Terrain       *mTerrain;
    TerrainGlobalOptions  *mTerrainGlobalOptions;

    TerrainGroup    *mTerrainGroup;
    TerrainPaging   *mTerrainPaging;
    PageManager     *mPageManager;

    Real mUpdateCountDown;

    Vector3 mTerrainPos;
    Light *light;

    String      m_sGroupName;
    String      m_sPrependNode;
  public:

    MyTerrain::MyTerrain(SceneManager *sceneMgr, Light *l)
    {
      mSceneMgr = sceneMgr;
      light = l;
      parseScene("sc.scene", "Bootstrap", mSceneMgr, mSceneMgr->getRootSceneNode());
    }

  protected:

    void parseScene(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, SceneNode *pAttachNode)
    {
      // set up shared object values
      m_sGroupName = groupName;
      mSceneMgr = yourSceneMgr;
      TiXmlDocument   *XMLDoc = 0;
      TiXmlElement   *XMLRoot;

      try
      {
        // Strip the path
        Ogre::String basename, path;
        Ogre::StringUtil::splitFilename(SceneName, basename, path);
        DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(basename, groupName);
        String data = pStream->getAsString();
        // Open the .scene File
        XMLDoc = new TiXmlDocument();
        XMLDoc->Parse(data.c_str());
        pStream->close();
        pStream.setNull();

        if(XMLDoc->Error())
        {
          //We'll just log, and continue on gracefully
          LogManager::getSingleton().logMessage("[TerrainLoader] The TiXmlDocument reported an error");
          delete XMLDoc;
          return;
        }
      }
      catch(...)
      {
        //We'll just log, and continue on gracefully
        LogManager::getSingleton().logMessage("[TerrainLoader] Error creating TiXmlDocument");
        delete XMLDoc;
        return;
      }

      // Validate the File
      XMLRoot = XMLDoc->RootElement();

      if(String(XMLRoot->Value()) != "scene")
      {
        LogManager::getSingleton().logMessage("[TerrainLoader] Error: Invalid .scene File. Missing ");
        delete XMLDoc;
        return;
      }

      // Process the scene
      TiXmlElement *pElement = XMLRoot->FirstChildElement("terrain");

      if(pElement)
      {
        processTerrain(pElement);
      }

      // Close the XML File
      delete XMLDoc;
    };



    void MyTerrain::processTerrain(TiXmlElement* XMLNode)
    {
      Ogre::Real worldSize = Ogre::StringConverter::parseReal(getAttrib(XMLNode, "worldSize"));
      int mapSize = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "mapSize"));
      bool colourmapEnabled = Ogre::StringConverter::parseBool(getAttrib(XMLNode, "colourmapEnabled"));
      int colourMapTextureSize = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "colourMapTextureSize"));
      int compositeMapDistance = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "tuningCompositeMapDistance"));
      int maxPixelError = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "tuningMaxPixelError"));
      mTerrainGlobalOptions = OGRE_NEW TerrainGlobalOptions();
      mTerrainGlobalOptions->setMaxPixelError((Ogre::Real)maxPixelError);
      mTerrainGlobalOptions->setCompositeMapDistance((Ogre::Real)compositeMapDistance);
      mTerrainGlobalOptions->setLightMapDirection(light->getDirection());
      mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
      mTerrainGlobalOptions->setCompositeMapDiffuse(light->getDiffuseColour());
      mTerrainGlobalOptions->setDefaultMaterialGenerator(SharedPtr<Ogre::TerrainMaterialGenerator>(OGRE_NEW myTerrainMaterialGenerator()));
      myTerrainMaterialGenerator::SM2Profile *sm2 = static_cast<myTerrainMaterialGenerator::SM2Profile* >(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
      sm2->setLayerNormalMappingEnabled(true);
      sm2->setLayerParallaxMappingEnabled(true);
      sm2->setCompositeMapEnabled(false);
      sm2->setGlobalColourMapEnabled(false);
      sm2->setLayerSpecularMappingEnabled(true);
      sm2->setLightmapEnabled(false);
      mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, mapSize, worldSize);
      mTerrainGroup->setOrigin(Ogre::Vector3(worldSize / 2, 0, worldSize / 2));
      //      mTerrainGroup->setOrigin( Vector3::ZERO);
      mTerrainGroup->setResourceGroup("General");
      TiXmlElement* pElement;
      TiXmlElement* pPageElement;
      // Process terrain pages
      pElement = XMLNode->FirstChildElement("terrainPages");

      if(pElement)
      {
        pPageElement = pElement->FirstChildElement("terrainPage");

        while(pPageElement)
        {
          processTerrainPage(pPageElement);
          pPageElement = pPageElement->NextSiblingElement("terrainPage");
        }
      }

      mTerrainGroup->loadAllTerrains(true);
      mTerrainGroup->freeTemporaryResources();
    }

    void MyTerrain::processTerrainPage(TiXmlElement* XMLNode)
    {
      Ogre::String name = getAttrib(XMLNode, "name");
      int pageX = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "pageX"));
      int pageY = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "pageY"));

      if(Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), name))
      {
        mTerrainGroup->defineTerrain(pageX, pageY, name);
      }
    }

  protected:
    String getAttrib(TiXmlElement *XMLNode, const String &attrib, const String &defaultValue = "")
    {
      if(XMLNode->Attribute(attrib.c_str()))
      {
        return XMLNode->Attribute(attrib.c_str());
      }
      else
      {
        return defaultValue;
      }
    };

  public:

    MyTerrain::MyTerrain(SceneManager *sceneMgr, String terName)
    {
      mSceneMgr = sceneMgr;
      mUpdateCountDown = 0;
      mTerrainGlobalOptions = OGRE_NEW TerrainGlobalOptions();
      mTerrainGlobalOptions->setMaxPixelError(8);
      mTerrainGlobalOptions->setCompositeMapDistance(2000);
      mTerrainGlobalOptions->setLightMapDirection(Vector3(0, -1, 0));
      mTerrainGlobalOptions->setCompositeMapAmbient(ColourValue(0.7, 0.7, 0.7));
      mTerrainGlobalOptions->setCompositeMapDiffuse(ColourValue(0.7, 0.7, 0.7));
      mTerrain = OGRE_NEW Ogre::Terrain(sceneMgr);
      mTerrain->load(terName);
    }
    /*
    MyTerrain::MyTerrain( SceneManager *sceneMgr, Light *l, bool hz)  :mTerrain(0)
    {
      mSceneMgr = sceneMgr;
      light = l;

      bool loadTerrain = true;
      bool saveTerrain = true;

      mUpdateCountDown = 0;

      mTerrainGlobalOptions = OGRE_NEW TerrainGlobalOptions();
      mTerrainGlobalOptions->setMaxPixelError(8);
      mTerrainGlobalOptions->setCompositeMapDistance(2000);
    //   mTerrainGlobalOptions->setLightMapDirection(light->getDirection());
      mTerrainGlobalOptions->setLightMapDirection( Vector3(0,-1,0));
      mTerrainGlobalOptions->setCompositeMapAmbient( ColourValue( 0.7, 0.7, 0.7));
      mTerrainGlobalOptions->setCompositeMapDiffuse(light->getDiffuseColour());

      mTerrain = OGRE_NEW Ogre::Terrain(sceneMgr);
      if(loadTerrain)
      {
        mTerrain->load("terrain.dat");
      }
      else
      {
    //     Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("isl_h.raw", "General" );
    //     size_t size = stream.get()->size();
    //     if(size != 129 * 129 * 4)//для *.raw почему ещё *4???????!!
    //     {
    //       OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Size of stream does not match terrainsize!", "TerrainPage" );
    //     }
    //     float* buffer = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_GENERAL);
    //     stream->read(buffer, size);

        Image img;
        img.load( "isl_h.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //     img.loadRawData( stream, 129, 129, PF_FLOAT32_R );
        Ogre::Terrain::ImportData imp;
        imp.terrainSize = TERRAIN_SIZE;
        imp.worldSize = TERRAIN_WORLD_SIZE;
        imp.inputScale = 1500;
    //     imp.inputScale = 1;
        imp.minBatchSize = 65;
        imp.maxBatchSize = 129;
    //     imp.inputFloat = buffer;
        imp.inputImage = &img;

        //textures
        imp.layerList.resize(3);
        imp.layerList[0].textureNames.push_back("dirt_01.png");
        imp.layerList[0].textureNames.push_back("dirt_01_n.png");

        imp.layerList[1].worldSize = 80;
        imp.layerList[1].textureNames.push_back("sand_01.jpg");
        imp.layerList[1].textureNames.push_back("sand_01_n.png");

        imp.layerList[2].worldSize = 80;
        imp.layerList[2].textureNames.push_back("rock_03.png");
        imp.layerList[2].textureNames.push_back("rock_03_n.png");

        mTerrain->prepare(imp);
        mTerrain->load();

        TerrainLayerBlendMap *blendMap0 = mTerrain->getLayerBlendMap(1);
        TerrainLayerBlendMap *blendMap1 = mTerrain->getLayerBlendMap(2);

        Image bm;
        bm.load("TreeMap.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        blendMap0->loadImage(bm);

    //     bm.load("BushMap.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        blendMap1->loadImage(bm);

        Real minHeight0 = 650;
        Real fadeDist0 = 15;
        Real minHeight1 = 800;
        Real fadeDist1 = 15;

        float *pBlend0 = blendMap0->getBlendPointer();
        float *pBlend1 = blendMap1->getBlendPointer();

        for (Ogre::uint16 y = 0; y < mTerrain->getLayerBlendMapSize(); ++y) {
          for (Ogre::uint16 x = 0; x < mTerrain->getLayerBlendMapSize(); ++x) {
            Real tx, ty;

            blendMap0->convertImageToTerrainSpace( x, y, &tx, &ty);
            Real height = mTerrain->getHeightAtTerrainPosition(tx, ty);
            Real val = (height - minHeight0) / fadeDist0;
            val = Math::Clamp(val, (Real)0, (Real)1);

            val = (height - minHeight1) / fadeDist1;
            val = Math::Clamp(val, (Real)0, (Real)1);

            *pBlend1++ = val;
          }
        }

        blendMap0->dirty();
        blendMap1->dirty();

        blendMap0->update();
        blendMap1->update();

        mTerrain->freeTemporaryResources();

        mTerrain->setPosition(Ogre::Vector3( TERRAIN_WORLD_SIZE/2, 0, TERRAIN_WORLD_SIZE/2));
    //     mTerrain->setVisibilityFlags(3);
      }
      if(saveTerrain)
        mTerrain->save("terrain.dat");
    }

    //создаёт теран по хейтмапу и сохраняет в файл.
    void MyTerrain::createTerrain()
    {
      mTerrain = OGRE_NEW Terrain(mSceneMgr);
      Image img;
      img.load("isl_h.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

      Terrain::ImportData imp;
      imp.inputImage = &img;
      imp.terrainSize = TERRAIN_SIZE;
      imp.worldSize = TERRAIN_WORLD_SIZE;
      imp.inputScale = 4800;
      imp.minBatchSize = 33;
      imp.maxBatchSize = 65;

      imp.layerList.resize(3);
      imp.layerList[0].worldSize = 500;
      imp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
      imp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");

      imp.layerList[1].worldSize = 500;
      imp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
      imp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");

      imp.layerList[2].worldSize = 500;
      imp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
      imp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

      mTerrain->setPosition( Vector3( TERRAIN_WORLD_SIZE/2, 0, TERRAIN_WORLD_SIZE/2));
      mTerrain->prepare(imp);
      mTerrain->load();

      TerrainLayerBlendMap *blendMap0 = mTerrain->getLayerBlendMap(1);
      TerrainLayerBlendMap *blendMap1 = mTerrain->getLayerBlendMap(2);

      Real minHeight0 = 70;
      Real fadeDist0 = 40;
      Real minHeight1 = 70;
      Real fadeDist1 = 15;

      float *pBlend0 = blendMap0->getBlendPointer();
      float *pBlend1 = blendMap1->getBlendPointer();

      for (Ogre::uint16 y = 0; y < mTerrain->getLayerBlendMapSize(); ++y) {
        for (Ogre::uint16 x = 0; x < mTerrain->getLayerBlendMapSize(); ++x) {
          Real tx, ty;

          blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
          Real height = mTerrain->getHeightAtTerrainPosition(tx, ty);
          Real val = (height - minHeight0) / fadeDist0;
          val = Math::Clamp(val, (Real)0, (Real)1);

          val = (height - minHeight1) / fadeDist1;
          val = Math::Clamp(val, (Real)0, (Real)1);

          *pBlend1++ = val;
        }
      }

      blendMap0->dirty();
      blendMap1->dirty();

      blendMap0->update();
      blendMap1->update();

      mTerrain->save( "islandterrain.dat");

      mTerrain->freeTemporaryResources();

    }
    */

    MyTerrain::MyTerrain(SceneManager *sceneMgr, TerrainGroup *terrainGroup, Light *l)
    {
      mSceneMgr = sceneMgr;
      mTerrainGroup = terrainGroup;
      light = l;
      mTerrain = mTerrainGroup->getTerrain(0, 0);
    }

    MyTerrain::~MyTerrain()
    {
      //    if( mTerrainPaging)
      //    {
      //      OGRE_DELETE mTerrainPaging;
      //      OGRE_DELETE mPageManager;
      //    }
      //    else
      if(mTerrainGroup)
      {
        OGRE_DELETE mTerrainGroup;
      }
      else if(mTerrain != 0)
      {
        OGRE_DELETE mTerrain;
        mTerrain = 0;
      }

      OGRE_DELETE mTerrainGlobalOptions;
    }

    void update(Real timeSinceLastFrame)
    {
      mTerrainGlobalOptions->setLightMapDirection(light->getDirection());
      mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
      mTerrainGlobalOptions->setCompositeMapDiffuse(light->getDiffuseColour());

      //    if ( mUpdateCountDown > 0)
      //    {
      //      mUpdateCountDown -= timeSinceLastFrame;
      //      if ( mUpdateCountDown <= 0)
      //      {
      if(mTerrainGroup)
      {
        mTerrainGroup->update();
      }

      //        mUpdateCountDown = 0;
      //      }
      //    }
    }
};