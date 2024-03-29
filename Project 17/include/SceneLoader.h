#pragma once

#include <Ogre.h>
#include "xml/tinyxml.h"

#include "OgreTerrain.h"
#include "OgreTerrainPaging.h"
#include "OgreTerrainGroup.h"
#include "myTerrainMaterialGenerator.h"
#include "OgreTerrainMaterialGeneratorA.h"

#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"
#include "Modules/SimpleGrid/SimpleGrid.h"

#include "base/SharedData.h"

using namespace std;
using namespace Ogre;
using namespace mySharedData;

class TiXmlElement;
namespace Ogre
{
  //структура предмета
  struct ItemStruct
  {
    String  id;
    String  name;
    String  type;
    String  description;
    int   healing;
    int   damage;
    int   defense;
    int   weight;
    String  meshname;
  };

  class ItemLoader
  {
      //  protected:
      //вектор структур
      std::vector<ItemStruct> ItemsVec;

    public:
      ItemLoader(const String &SceneName, const String &groupName)
      {
        parseItems(SceneName, groupName);
      };
      ~ItemLoader()
      {
        ItemsVec.clear();
      };

      void parseItems(const String &SceneName, const String &groupName)
      {
        TiXmlDocument   *XMLDoc = 0;
        TiXmlElement   *XMLRoot;

        try
        {
          // Strip the path
          Ogre::String basename, path;
          Ogre::StringUtil::splitFilename(SceneName, basename, path);
          DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(basename, groupName);
          String data = pStream->getAsString();
          // Open the .item File
          XMLDoc = new TiXmlDocument();
          XMLDoc->Parse(data.c_str());
          pStream->close();
          pStream.setNull();

          if(XMLDoc->Error())
          {
            //We'll just log, and continue on gracefully
            String errdesc, errrow, errcol;
            errdesc = StringConverter::toString(XMLDoc->ErrorDesc());
            errrow = StringConverter::toString(XMLDoc->ErrorRow());
            errcol = StringConverter::toString(XMLDoc->ErrorCol());
            LogManager::getSingleton().logMessage("[ItemLoader] The TiXmlDocument reported an error " + errdesc + " in " + errrow + " " + errcol);
            delete XMLDoc;
            return;
          }
        }
        catch(...)
        {
          //We'll just log, and continue on gracefully
          LogManager::getSingleton().logMessage("[ItemLoader] Error creating TiXmlDocument");
          delete XMLDoc;
          return;
        }

        // Validate the File
        XMLRoot = XMLDoc->RootElement();

        if(String(XMLRoot->Value()) != "items")
        {
          LogManager::getSingleton().logMessage("[ItemLoader] Error: Invalid .item File. Missing ");
          delete XMLDoc;
          return;
        }

        // Process the items
        processItems(XMLRoot);
        // Close the XML File
        delete XMLDoc;
      };

      void processItems(TiXmlElement *XMLRoot)
      {
        // Process the scene parameters
        String message = "[ItemLoader] Parsing Item file";
        LogManager::getSingleton().logMessage(message);
        TiXmlElement *pElement;
        // Process items
        pElement = XMLRoot->FirstChildElement("item");

        while(pElement)
        {
          processItem(pElement);
          pElement = pElement->NextSiblingElement("item");
        }
      };

      void processItem(TiXmlElement *XMLNode)
      {
        ItemStruct  itemTmp;
        itemTmp.id = String(XMLNode->Attribute("id"));
        LogManager::getSingletonPtr()->logMessage(itemTmp.id);
        itemTmp.name = String(XMLNode->Attribute("name"));
        LogManager::getSingletonPtr()->logMessage(itemTmp.name);
        itemTmp.type = String(XMLNode->Attribute("type"));
        LogManager::getSingletonPtr()->logMessage(itemTmp.type);
        itemTmp.description = String(XMLNode->Attribute("description"));
        LogManager::getSingletonPtr()->logMessage(itemTmp.description);
        itemTmp.meshname = String(XMLNode->Attribute("mesh_name"));
        LogManager::getSingletonPtr()->logMessage(itemTmp.meshname);
        itemTmp.weight = StringConverter::parseInt(XMLNode->Attribute("weight"));
        LogManager::getSingletonPtr()->logMessage(StringConverter::toString(itemTmp.weight));

        if(itemTmp.type == "FOOD")
        {
          itemTmp.healing = StringConverter::parseInt(XMLNode->Attribute("healing"));
          LogManager::getSingletonPtr()->logMessage(StringConverter::toString(itemTmp.healing));
        }
        else
        {
          itemTmp.healing = 0;
        }

        if(itemTmp.type == "MELE_1H")
        {
          itemTmp.damage = StringConverter::parseInt(XMLNode->Attribute("damage"));
          LogManager::getSingletonPtr()->logMessage(StringConverter::toString(itemTmp.damage));
        }
        else
        {
          itemTmp.damage = 0;
        }

        if(itemTmp.type == "ARMOUR")
        {
          itemTmp.defense = StringConverter::parseInt(XMLNode->Attribute("defense"));
          LogManager::getSingletonPtr()->logMessage(StringConverter::toString(itemTmp.defense));
        }
        else
        {
          itemTmp.defense = 0;
        }

        LogManager::getSingletonPtr()->logMessage("item readed");
        ItemsVec.push_back(itemTmp);
      };

      std::vector<ItemStruct> getItemsVector()
      {
        return ItemsVec;
      };
  };

  class ogSceneLoader
  {

      SceneManager  *mSceneMgr;
      RenderWindow  *mWindow;

      TerrainGlobalOptions *mTerrainGlobalOptions;

      String pagePrefix;

    public:
      ogSceneLoader(SceneManager* scene, RenderWindow *win)
      {
        mSceneMgr = scene;
        mWindow = win;
        TiXmlDocument   *XMLDoc = 0;
        TiXmlElement   *XMLRoot;

        try
        {
          // Strip the path
          Ogre::String basename, path;
          Ogre::StringUtil::splitFilename("myscene.ogscene", basename, path);
          DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(basename, "Bootstrap");
          String data = pStream->getAsString();
          // Open the .scene File
          XMLDoc = new TiXmlDocument();
          XMLDoc->Parse(data.c_str());
          pStream->close();
          pStream.setNull();

          if(XMLDoc->Error())
          {
            //We'll just log, and continue on gracefully
            LogManager::getSingleton().logMessage("[OgSceneLoader] The XmlDocument reported an error");
            delete XMLDoc;
            return;
          }
        }
        catch(...)
        {
          //We'll just log, and continue on gracefully
          LogManager::getSingleton().logMessage("[OgSceneLoader] Error creating XmlDocument");
          delete XMLDoc;
          return;
        }

        // Validate the File
        XMLRoot = XMLDoc->RootElement();

        if(String(XMLRoot->Value()) != "OGITORSCENE")
        {
          LogManager::getSingleton().logMessage("[OgSceneLoader] Error: Invalid .ogscene File. Missing ");
          delete XMLDoc;
          return;
        }

        // Process the scene parameters
        String version = getAttrib(XMLRoot, "version", "unknown");
        String message = "Parsing OgScene file with version " + version;
        LogManager::getSingleton().logMessage(message);
        //      pElement = XMLRoot->FirstChildElement("PROJECT");
        //      if(pElement)
        //        processProject( pElement);
        LogManager::getSingleton().logMessage("[OgSceneLoader] start read objects ");
        TiXmlElement *pElement = XMLRoot->FirstChildElement("OBJECT");

        do
        {
          if(pElement)
          {
            String objectType = pElement->Attribute("typename");

            if(objectType == "Light Object")
            {
              parseLight(pElement);
            }
            else if(objectType == "Terrain Group Object")
            {
              parseTerrainGroup(pElement);
            }
            else if(objectType == "Terrain Page Object")
            {
              parseTerrainPage(pElement);
            }
            else if(objectType == "SkyX Object")
            {
              parseSkyX(pElement);
            }

            LogManager::getSingleton().logMessage("[OgSceneLoader] object done " + objectType);
          }

          pElement = pElement->NextSiblingElement("OBJECT");
        }
        while(pElement);

        mTerrainGroup->loadAllTerrains(true);
        mTerrainGroup->freeTemporaryResources();
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();

        while(ti.hasMoreElements())
        {
          Ogre::Terrain* t = ti.getNext()->instance;
          mPxManager->CookeTerrain(t);
        }

        pElement = XMLRoot->FirstChildElement("OBJECT");

        do
        {
          if(pElement)
          {
            String objectType = pElement->Attribute("typename");

            /*          if( objectType == "Scene Manager")
                        parseSceneManager( pElement);
                      else if( objectType == "Viewport1")
                        parseViewport( pElement);
                      else if( objectType == "Paging Manager")
                        parsePagingMgr( pElement);
                      else*/
            if(objectType == "Hydrax Object")
            {
              parseHydrax(pElement);
            }
            else if(objectType == "PGInstance Manager Object")
            {
              parsePGInstance(pElement);
            }
            else if(objectType == "Node Object")
            {
              parseSceneNode(pElement);
            }
            else if(objectType == "Entity Object")
            {
              parseEntity(pElement);
            }

            LogManager::getSingleton().logMessage("[OgSceneLoader] object done " + objectType);
          }

          pElement = pElement->NextSiblingElement("OBJECT");
        }
        while(pElement);

        delete XMLDoc;
      };

      virtual ~ogSceneLoader() {}

    protected:

      void parseSceneManager(TiXmlElement *XMLNode)
      {
        return;
      };

      void parseViewport(TiXmlElement *XMLNode)
      {
        return;
      };

      void parsePagingMgr(TiXmlElement *XMLNode)
      {
        return;
      };

      void parseLight(TiXmlElement *XMLNode)
      {
        Light * light = mSceneMgr->createLight(getAttrib(XMLNode, "name"));

        if(getAttrib(XMLNode, "parentnode") == "SceneManager")
        {
          mSceneMgr->getRootSceneNode()->attachObject(light);
        }
        else
        {
          mSceneMgr->getSceneNode(getAttrib(XMLNode, "parentnode"))->attachObject(light);
        }

        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          String objectType = pElement->Attribute("id");

          if(objectType == "attenuation")
          {
            Vector4 v = StringConverter::parseVector4(pElement->Attribute("value"));
            light->setAttenuation(v.x, v.y, v.z, v.w);
          }
          else if(objectType == "castshadows")
          {
            light->setCastShadows(getAttribBool(pElement, "value"));
          }
          else if(objectType == "diffuse")
          {
            light->setDiffuseColour(StringConverter::parseColourValue(pElement->Attribute("value")));
          }
          else if(objectType == "direction")
          {
            light->setDirection(StringConverter::parseVector3(pElement->Attribute("value")));

            if(light->getDirection() != Vector3(0, 0, 1))
            {
              light->setType(Light::LT_DIRECTIONAL);
            }
            else
            {
              light->setType(Light::LT_POINT);
            }
          }
          else if(objectType == "position")
          {
            light->setPosition(StringConverter::parseVector3(pElement->Attribute("value")));
          }
          else if(objectType == "specular")
          {
            light->setSpecularColour(StringConverter::parseColourValue(pElement->Attribute("value")));
          }
          else if(objectType == "power")
          {
            light->setPowerScale(getAttribReal(pElement, "value"));
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);
      };

      void parseTerrainGroup(TiXmlElement *XMLNode)
      {
        int blendmapSize, colourmapSize, lightmapSize, pagemapsSize, pageworldSize,
            densSize, detalDist, pageSize, compositeDist, compositeSize, maxBatchSize,
            maxPixelError, minBatchSize;
        bool colourmapEnabled, locked;
        mTerrainGlobalOptions = OGRE_NEW TerrainGlobalOptions();
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = pElement->Attribute("id");

            if(id == "blendmap::texturesize")
            {
              blendmapSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "colourmap::enabled")
            {
              colourmapEnabled = getAttribBool(pElement, "value");
            }
            else if(id == "colourmap::texturesize")
            {
              if(colourmapEnabled == true)
              {
                colourmapSize = StringConverter::parseInt(getAttrib(pElement, "value"));
              }
            }
            //          else if( id == "layer")
            else if(id == "lightmap::texturesize")
            {
              lightmapSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "locked")
            {
              locked = getAttribBool(pElement, "value");
            }
            else if(id == "pagemapsize")
            {
              pageSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "pagenameprefix")
            {
              pagePrefix = getAttrib(pElement, "value");
            }
            else if(id == "pageworldsize")
            {
              pageworldSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "pg::densitymapsize")
            {
              densSize =  StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "pg::detaildistance")
            {
              detalDist = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "pg::pagesize")
            {
              pageSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "tuning::compositemapdistance")
            {
              compositeDist = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "tuning::compositemaptexturesize")
            {
              compositeSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "tuning::maxbatchsize")
            {
              maxBatchSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "tuning::maxpixelerror")
            {
              maxPixelError = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "tuning::minbatchsize")
            {
              minBatchSize = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        mTerrainGlobalOptions->setMaxPixelError((Ogre::Real)maxPixelError);
        mTerrainGlobalOptions->setCompositeMapDistance((Ogre::Real)compositeDist);
        mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
        mTerrainGlobalOptions->setCompositeMapSize((uint16)compositeSize);
        mTerrainGlobalOptions->setCastsDynamicShadows(true);
        mTerrainGlobalOptions->setLightMapDirection(Vector3(1, 1, 0));
        mTerrainGlobalOptions->setLightMapSize(1024);
        mTerrainGlobalOptions->setDefaultMaterialGenerator(SharedPtr<Ogre::TerrainMaterialGenerator>(OGRE_NEW myTerrainMaterialGenerator()));
        //      mTerrainGlobalOptions->setDefaultMaterialGenerator( SharedPtr<Ogre::TerrainMaterialGenerator>( OGRE_NEW TerrainMaterialGeneratorA()));
        myTerrainMaterialGenerator::SM2Profile *sm2 = static_cast< myTerrainMaterialGenerator::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
        //      TerrainMaterialGeneratorA::SM2Profile *sm2 = static_cast< TerrainMaterialGeneratorA::SM2Profile*>( mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
        sm2->setLayerNormalMappingEnabled(true);
        sm2->setLayerParallaxMappingEnabled(true);
        sm2->setCompositeMapEnabled(false);
        sm2->setGlobalColourMapEnabled(false);
        sm2->setLayerSpecularMappingEnabled(true);
        sm2->setLightmapEnabled(false);
        mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, pageSize, pageworldSize);
        mTerrainGroup->setResourceGroup("Terrain");
        //      mTerrainGroup->setFilenamePrefix( pagePrefix);
        //      mTerrainGroup->setFilenameExtension( "ogt");
        //    mTerrainGroup->setFilenameConvention( pagePrefix, "ogt");
        mTerrainGroup->setOrigin(Vector3::ZERO);
      };

      void parseTerrainPage(TiXmlElement *XMLNode)
      {
        /*
        <PROPERTY id="colourmap::enabled" type="12" value="false"></PROPERTY>
        <PROPERTY id="colourmap::texturesize" type="2" value="1024"></PROPERTY>
        <PROPERTY id="layer" type="2" value="0"></PROPERTY>
        <PROPERTY id="layer0::diffusespecular" type="7" value="volcano_2_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer0::normalheight" type="7" value="volcano_2_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer0::worldsize" type="6" value="120"></PROPERTY>
        <PROPERTY id="layer1::diffusespecular" type="7" value="desert_2_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer1::normalheight" type="7" value="desert_2_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer1::worldsize" type="6" value="120"></PROPERTY>
        <PROPERTY id="layer2::diffusespecular" type="7" value="jungle_0_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer2::normalheight" type="7" value="jungle_0_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer2::worldsize" type="6" value="120"></PROPERTY>
        <PROPERTY id="layer3::diffusespecular" type="7" value="desert_6_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer3::normalheight" type="7" value="desert_6_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer3::worldsize" type="6" value="100"></PROPERTY>
        <PROPERTY id="layer4::diffusespecular" type="7" value="desert_1_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer4::normalheight" type="7" value="desert_1_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer4::worldsize" type="6" value="100"></PROPERTY>
        <PROPERTY id="layer5::diffusespecular" type="7" value="desert_5_diffusespecular.dds"></PROPERTY>
        <PROPERTY id="layer5::normalheight" type="7" value="desert_5_normalheight.dds"></PROPERTY>
        <PROPERTY id="layer5::worldsize" type="6" value="100"></PROPERTY>
        <PROPERTY id="layercount" type="2" value="6"></PROPERTY>
        <PROPERTY id="locked" type="12" value="false"></PROPERTY>
        <PROPERTY id="pagex" type="2" value="1"></PROPERTY>
        <PROPERTY id="pagey" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer0::active" type="12" value="false"></PROPERTY>
        <PROPERTY id="pg::layer0::animate" type="12" value="true"></PROPERTY>
        <PROPERTY id="pg::layer0::density" type="6" value="3"></PROPERTY>
        <PROPERTY id="pg::layer0::fadetech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer0::grasstech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer0::material" type="7" value=""></PROPERTY>
        <PROPERTY id="pg::layer0::maxsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer0::minsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer0::swaydistribution" type="6" value="10"></PROPERTY>
        <PROPERTY id="pg::layer0::swaylength" type="6" value="0.2"></PROPERTY>
        <PROPERTY id="pg::layer0::swayspeed" type="6" value="0.5"></PROPERTY>
        <PROPERTY id="pg::layer1::active" type="12" value="false"></PROPERTY>
        <PROPERTY id="pg::layer1::animate" type="12" value="true"></PROPERTY>
        <PROPERTY id="pg::layer1::density" type="6" value="3"></PROPERTY>
        <PROPERTY id="pg::layer1::fadetech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer1::grasstech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer1::material" type="7" value=""></PROPERTY>
        <PROPERTY id="pg::layer1::maxsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer1::minsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer1::swaydistribution" type="6" value="10"></PROPERTY>
        <PROPERTY id="pg::layer1::swaylength" type="6" value="0.2"></PROPERTY>
        <PROPERTY id="pg::layer1::swayspeed" type="6" value="0.5"></PROPERTY>
        <PROPERTY id="pg::layer2::active" type="12" value="false"></PROPERTY>
        <PROPERTY id="pg::layer2::animate" type="12" value="true"></PROPERTY>
        <PROPERTY id="pg::layer2::density" type="6" value="3"></PROPERTY>
        <PROPERTY id="pg::layer2::fadetech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer2::grasstech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer2::material" type="7" value=""></PROPERTY>
        <PROPERTY id="pg::layer2::maxsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer2::minsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer2::swaydistribution" type="6" value="10"></PROPERTY>
        <PROPERTY id="pg::layer2::swaylength" type="6" value="0.2"></PROPERTY>
        <PROPERTY id="pg::layer2::swayspeed" type="6" value="0.5"></PROPERTY>
        <PROPERTY id="pg::layer3::active" type="12" value="false"></PROPERTY>
        <PROPERTY id="pg::layer3::animate" type="12" value="true"></PROPERTY>
        <PROPERTY id="pg::layer3::density" type="6" value="3"></PROPERTY>
        <PROPERTY id="pg::layer3::fadetech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer3::grasstech" type="2" value="1"></PROPERTY>
        <PROPERTY id="pg::layer3::material" type="7" value=""></PROPERTY>
        <PROPERTY id="pg::layer3::maxsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer3::minsize" type="8" value="1 1"></PROPERTY>
        <PROPERTY id="pg::layer3::swaydistribution" type="6" value="10"></PROPERTY>
        <PROPERTY id="pg::layer3::swaylength" type="6" value="0.2"></PROPERTY>
        <PROPERTY id="pg::layer3::swayspeed" type="6" value="0.5"></PROPERTY>
        <PROPERTY id="position" type="9" value="20000 0 -20000"></PROPERTY>
        //     <PROPERTY id="tempdensitymodified" type="12" value="false"></PROPERTY>
        //     <PROPERTY id="tempmodified" type="12" value="false"></PROPERTY>
        //     <PROPERTY id="tuning::maxbatchsize" type="2" value="129"></PROPERTY>
        //     <PROPERTY id="tuning::minbatchsize" type="2" value="33"></PROPERTY>
        </OBJECT>
        */
        int pageX, pageY, layerCount, gl0Fadetech, gl0Grasstech, gl1Fadetech, gl1Grasstech, gl2Fadetech, gl2Grasstech, gl3Fadetech, gl3Grasstech;
        Vector3 pos;
        String gl0Material, gl1Material, gl2Material, gl3Material;
        bool gl0, gl1, gl2, gl3, gl0Animate, gl1Animate, gl2Animate, gl3Animate;
        Real gl0Maxsize, gl0Minsize, gl0Swaydistribution, gl0Swaylength, gl0Swayspeed, gl0Density;
        Real gl1Maxsize, gl1Minsize, gl1Swaydistribution, gl1Swaylength, gl1Swayspeed, gl1Density;
        Real gl2Maxsize, gl2Minsize, gl2Swaydistribution, gl2Swaylength, gl2Swayspeed, gl2Density;
        Real gl3Maxsize, gl3Minsize, gl3Swaydistribution, gl3Swaylength, gl3Swayspeed, gl3Density;
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "pagex")
            {
              pageX = getAttribInt(pElement, "value");
            }
            else if(id == "pagey")
            {
              pageY = getAttribInt(pElement, "value");
            }
            else if(id == "layercount")
            {
              layerCount = getAttribInt(pElement, "value");
            }
            else if(id == "position")
            {
              pos = parseVector3(pElement, "value");
            }
            else if(id == "pg::layer0::active")
            {
              gl0 = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer0::animate")
            {
              gl0Animate = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer0::density")
            {
              gl0Density = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer0::fadetech")
            {
              gl0Fadetech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer0::grasstech")
            {
              gl0Grasstech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer0::material")
            {
              gl0Material = getAttrib(pElement, "value");
            }
            else if(id == "pg::layer0::maxsize")
            {
              gl0Maxsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer0::minsize")
            {
              gl0Minsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer0::swaydistribution")
            {
              gl0Swaydistribution = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer0::swaylength")
            {
              gl0Swaylength = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer0::swayspeed")
            {
              gl0Swayspeed = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::active")
            {
              gl1 = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer1::animate")
            {
              gl1Animate = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer1::density")
            {
              gl1Density = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::fadetech")
            {
              gl1Fadetech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer1::grasstech")
            {
              gl1Grasstech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer1::material")
            {
              gl1Material = getAttrib(pElement, "value");
            }
            else if(id == "pg::layer1::maxsize")
            {
              gl1Maxsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::minsize")
            {
              gl1Minsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::swaydistribution")
            {
              gl1Swaydistribution = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::swaylength")
            {
              gl1Swaylength = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer1::swayspeed")
            {
              gl1Swayspeed = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::active")
            {
              gl2 = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer2::animate")
            {
              gl2Animate = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer2::density")
            {
              gl2Density = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::fadetech")
            {
              gl2Fadetech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer2::grasstech")
            {
              gl2Grasstech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer2::material")
            {
              gl2Material = getAttrib(pElement, "value");
            }
            else if(id == "pg::layer2::maxsize")
            {
              gl2Maxsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::minsize")
            {
              gl2Minsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::swaydistribution")
            {
              gl2Swaydistribution = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::swaylength")
            {
              gl2Swaylength = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer2::swayspeed")
            {
              gl2Swayspeed = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::active")
            {
              gl3 = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer3::animate")
            {
              gl3Animate = getAttribBool(pElement, "value");
            }
            else if(id == "pg::layer3::density")
            {
              gl3Density = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::fadetech")
            {
              gl3Fadetech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer3::grasstech")
            {
              gl3Grasstech = getAttribInt(pElement, "value");
            }
            else if(id == "pg::layer3::material")
            {
              gl3Material = getAttrib(pElement, "value");
            }
            else if(id == "pg::layer3::maxsize")
            {
              gl3Maxsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::minsize")
            {
              gl3Minsize = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::swaydistribution")
            {
              gl3Swaydistribution = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::swaylength")
            {
              gl3Swaylength = getAttribReal(pElement, "value");
            }
            else if(id == "pg::layer3::swayspeed")
            {
              gl3Swayspeed = getAttribReal(pElement, "value");
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        String PageFileName =  pagePrefix + "_000" + StringConverter::toString(pageX) + "000" + StringConverter::toString(pageY);

        if(ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), PageFileName + ".ogt"))
        {
          mTerrainGroup->defineTerrain(pageX, pageY, PageFileName + ".ogt");
        }

        if(mPGManager == NULL)
        {
          mPGManager = new GamePGManager(mCamera, mSceneMgr, mPxManager, mTerrainGroup);
        }

        if(gl0)
        {
          mPGManager->addGrassLayer(PageFileName + "_density.tga", pos, Vector2(20000, 20000), gl0Density, gl0Maxsize, gl0Minsize, gl0Fadetech, gl0Grasstech, gl0Material , 0);
        }

        if(gl1)
        {
          mPGManager->addGrassLayer(PageFileName + "_density.tga", pos, Vector2(20000, 20000), gl1Density, gl1Maxsize, gl1Minsize, gl1Fadetech, gl1Grasstech, gl1Material, 1);
        }

        if(gl2)
        {
          mPGManager->addGrassLayer(PageFileName + "_density.tga", pos, Vector2(20000, 20000), gl2Density, gl2Maxsize, gl2Minsize, gl2Fadetech, gl2Grasstech, gl2Material, 2);
        }

        if(gl3)
        {
          mPGManager->addGrassLayer(PageFileName + "_density.tga", pos, Vector2(20000, 20000), gl0Density, gl3Maxsize, gl3Minsize, gl3Fadetech, gl3Grasstech, gl3Material, 3);
        }
      };

      void parseSkyX(TiXmlElement *XMLNode)
      {
        /*
        <OBJECT object_id="890743744" name="SkyX" typename="Skyx Object" parentnode="SceneManager">
        <PROPERTY id="layer" type="2" value="0"></PROPERTY>
        <PROPERTY id="locked" type="12" value="false"></PROPERTY>
        <PROPERTY id="options::eastposition" type="8" value="0 1"></PROPERTY>
        <PROPERTY id="options::exposure" type="6" value="2"></PROPERTY>
        <PROPERTY id="options::g" type="6" value="-0.991"></PROPERTY>
        <PROPERTY id="options::height" type="6" value="0.01"></PROPERTY>
        <PROPERTY id="options::innerradius" type="6" value="9.77501"></PROPERTY>
        <PROPERTY id="options::miemultiplier" type="6" value="0.000675"></PROPERTY>
        <PROPERTY id="options::outerradius" type="6" value="10.2963"></PROPERTY>
        <PROPERTY id="options::rayleighmultiplier" type="6" value="0.0022"></PROPERTY>
        <PROPERTY id="options::samplecount" type="2" value="4"></PROPERTY>
        <PROPERTY id="options::sunintensity" type="6" value="30"></PROPERTY>
        <PROPERTY id="options::time" type="9" value="14 7.5 20.5"></PROPERTY>
        <PROPERTY id="options::wavelength" type="9" value="0.57 0.54 0.44"></PROPERTY>
        <PROPERTY id="timemultiplier" type="6" value="0.1"></PROPERTY>
        <PROPERTY id="updatescript" type="7" value=""></PROPERTY>
        <PROPERTY id="vclouds::noisescale" type="6" value="4.2"></PROPERTY>
        <PROPERTY id="vclouds::winddirection" type="6" value="0"></PROPERTY>
        <PROPERTY id="vclouds::windspeed" type="6" value="80"></PROPERTY>
        */
        return;
      };

      void parseHydrax(TiXmlElement *XMLNode)
      {
        Hydrax::Hydrax *hmHydrax;
        Hydrax::Noise::Perlin *hmNoiseModule;
        Hydrax::Module::ProjectedGrid *hmModule;
        Vector3 pos, scale;
        String configfile;
        int complexity;
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "configfile")
            {
              configfile = getAttrib(pElement, "value");
            }
            else if(id == "position")
            {
              pos = parseVector3(pElement, "value");
            }
            else if(id == "pgmodule::complexity")
            {
              complexity = getAttribInt(pElement, "value");
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        // создаём объект гидракса
        hmHydrax = new Hydrax::Hydrax(mSceneMgr, mSceneMgr->getCamera("Camera"), mWindow->getViewport(0));
        /*
              //опции шума
              Hydrax::Noise::Perlin::Options *perlinOptions = new Hydrax::Noise::Perlin::Options(
                4, 0.3, 0.1, 2.2, 1.27, 2, Vector3( 0.5, 50, 150000));
              //и шум
              hmNoiseModule = new Hydrax::Noise::Perlin( *perlinOptions);
        */
        hmNoiseModule = new Hydrax::Noise::Perlin();
        //      Hydrax::Module::ProjectedGrid::Options *pgOptions = new Hydrax::Module::ProjectedGrid::Options(
        //      100, 3, 5, false, false, true, 1);
        // создаём сетку
        hmModule = new Hydrax::Module::ProjectedGrid(hmHydrax,       // Hydrax parent pointer
                                                     hmNoiseModule,                        // Noise module
                                                     Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)), // Base plane
                                                     Hydrax::MaterialManager::NM_VERTEX,             // Normal mode
                                                     Hydrax::Module::ProjectedGrid::Options(complexity));    // Projected grid options
        // Set our module
        hmHydrax->setModule(static_cast<Hydrax::Module::Module*>(hmModule));
        /*
              hmHydrax->getRttManager()->setTextureSize( Hydrax::RttManager::RTT_REFLECTION, 512);
              hmHydrax->getRttManager()->setTextureSize( Hydrax::RttManager::RTT_REFRACTION, 512);
              hmHydrax->getRttManager()->setTextureSize( Hydrax::RttManager::RTT_DEPTH, 512);
              hmHydrax->getRttManager()->setTextureSize( Hydrax::RttManager::RTT_DEPTH_REFLECTION, 512);
              hmHydrax->getRttManager()->setTextureSize( Hydrax::RttManager::RTT_GPU_NORMAL_MAP, 512);

              Hydrax::HydraxComponent ComponentsToLoad[8] = {
                Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,
                Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,
                Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,
                Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE,Hydrax::HydraxComponent::HYDRAX_COMPONENTS_NONE};

                ComponentsToLoad[0] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_SUN;
                ComponentsToLoad[1] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_FOAM;
                ComponentsToLoad[2] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_DEPTH;
                ComponentsToLoad[3] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_SMOOTH;
                ComponentsToLoad[4] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_CAUSTICS;
                ComponentsToLoad[5] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_UNDERWATER;
                ComponentsToLoad[6] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS;
                ComponentsToLoad[7] = Hydrax::HydraxComponent::HYDRAX_COMPONENT_UNDERWATER_GODRAYS;

              hmHydrax->setComponents(static_cast<Hydrax::HydraxComponent>(
                ComponentsToLoad[0] | ComponentsToLoad[1] | ComponentsToLoad[2] | ComponentsToLoad[3] |
                ComponentsToLoad[4] | ComponentsToLoad[5] | ComponentsToLoad[6] | ComponentsToLoad[7]));

              hmHydrax->setSunPosition( Vector3( 2997.92, 7460.89, -16863.8));
              hmHydrax->setSunStrength( 1.75);
              hmHydrax->setSunArea( 746.089);
              hmHydrax->setSunColor( Vector3( 1, 1, 1));

              hmHydrax->setFoamMaxDistance( 7.5e+007);
              hmHydrax->setFoamScale( 0.0075);
              hmHydrax->setFoamStart( 0);
              hmHydrax->setFoamTransparency( 1);


              hmHydrax->setDepthLimit( 0);
              hmHydrax->setSmoothPower( 55);
              hmHydrax->setFullReflectionDistance( 1e+011);

              hmHydrax->setCausticsEnd( 0.95);
              hmHydrax->setCausticsPower( 9);
              hmHydrax->setCausticsScale( 100);

              hmHydrax->getGodRaysManager()->setObjectIntersectionsEnabled( false);
              hmHydrax->getGodRaysManager()->setNumberOfRays( 100);
              hmHydrax->getGodRaysManager()->setRaysSize( 0.03);
              hmHydrax->getGodRaysManager()->setSimulationSpeed( 5);
              hmHydrax->setGodRaysExposure( Vector3( 0.76, 2.46, 2.29));
              hmHydrax->setGodRaysIntensity( 0.015);

              hmHydrax->setGlobalTransparency( 0);

              hmHydrax->setShaderMode( Hydrax::MaterialManager::ShaderMode::SM_HLSL);
              hmHydrax->setWaterColor( Vector3( 0.139765, 0.359464, 0.425373));
        */
        hmHydrax->loadCfg(configfile);
        // создаём воду
        hmHydrax->create();
        //пизиции воды и солнца
        hmHydrax->setPosition(pos);
        hmHydrax->setSunPosition(mSceneMgr->getCamera("Camera")->getDerivedPosition() - mSkyManager->getSkyXSystem()->getAtmosphereManager()->getSunDirection()*mSkyManager->getSkyXSystem()->getMeshManager()->getSkydomeRadius() * 0.1);
        hmHydrax->getMaterialManager()->addDepthTechnique(mTerrainGroup->getTerrain(0, 0)->getMaterial().get()->createTechnique());
        hmHydrax->getMaterialManager()->addDepthTechnique(mTerrainGroup->getTerrain(0, 1)->getMaterial().get()->createTechnique());
        hmHydrax->getMaterialManager()->addDepthTechnique(mTerrainGroup->getTerrain(1, 0)->getMaterial().get()->createTechnique());
        hmHydrax->getMaterialManager()->addDepthTechnique(mTerrainGroup->getTerrain(1, 1)->getMaterial().get()->createTechnique());
        //      hmHydrax->getMesh()->getEntity()->setVisibilityFlags(5); //для дш
        mHydraxMgr = new HydraxManager(mSkyManager, mCamera, hmHydrax, hmModule, hmNoiseModule);
        return;
      };

      void parsePGInstance(TiXmlElement *XMLNode)
      {
        int batchdist, pagesize, impostordist;
        Vector4 bounds;
        bool castshadows;
        String model, fileid;
        fileid = getAttrib(XMLNode, "object_id");
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "batchdistance")
            {
              batchdist = getAttribInt(pElement, "value");
            }
            else if(id == "bounds")
            {
              bounds = parseVector4(pElement, "value");
            }
            else if(id == "castshadows")
            {
              castshadows = getAttribBool(pElement, "value");
            }
            else if(id == "impostordistance")
            {
              impostordist = getAttribInt(pElement, "value");
            }
            else if(id == "model")
            {
              model = getAttrib(pElement, "value");
            }
            else if(id == "pagesize")
            {
              pagesize = getAttribInt(pElement, "value");
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        if(mPGManager == NULL)
        {
          mPGManager = new GamePGManager(mCamera, mSceneMgr, mPxManager, mTerrainGroup);
        }

        mPGManager->addTreeLayer(fileid + ".instance", bounds, batchdist, pagesize, impostordist, model);
        return;
      };

      void parseSceneNode(TiXmlElement *XMLNode)
      {
        // читаем аттрибуты
        String name = getAttrib(XMLNode, "name");
        String parent = getAttrib(XMLNode, "parentnode");
        Quaternion orient;
        Vector3 pos, scale;
        int renderDist;
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "orientation")
            {
              orient = parseQuaternion(pElement, "value");
            }
            else if(id == "position")
            {
              pos = parseVector3(pElement, "value");
            }
            else if(id == "scale")
            {
              scale = parseVector3(pElement, "value");
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        SceneNode *pNode, *pParent;

        if(parent == "SceneManager")
        {
          pParent = mSceneMgr->getRootSceneNode();
        }
        else
        {
          pParent = mSceneMgr->getSceneNode(parent);
        }

        pNode = pParent->createChildSceneNode(name);
        pNode->setPosition(pos);
        pNode->setOrientation(orient);
        pNode->setScale(scale);
      };

      void parseEntity(TiXmlElement *XMLNode)
      {
        // читаем аттрибуты
        String name = getAttrib(XMLNode, "name");
        String parent = getAttrib(XMLNode, "parentnode");
        String meshFile;
        bool castShadows;
        Quaternion orient;
        Vector3 pos, scale;
        int renderDist;
        String materialName;
        bool triangleShape = true; ///////////!!!*
        TiXmlElement *pElement = XMLNode->FirstChildElement("PROPERTY");

        do
        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "meshfile")
            {
              meshFile = getAttrib(pElement, "value");
            }
            else if(id == "castshadows")
            {
              castShadows = StringConverter::parseBool(getAttrib(pElement, "value"));
            }
            else if(id == "orientation")
            {
              orient = parseQuaternion(pElement, "value");
            }
            else if(id == "position")
            {
              pos = parseVector3(pElement, "value");
            }
            else if(id == "renderingdistance")
            {
              renderDist = StringConverter::parseInt(getAttrib(pElement, "value"));
            }
            else if(id == "scale")
            {
              scale = parseVector3(pElement, "value");
            }
            else if(id == "subentity0::material")
            {
              materialName = getAttrib(pElement, "value");
            }
          }

          pElement = pElement->NextSiblingElement("PROPERTY");
        }
        while(pElement);

        pElement = XMLNode->FirstChildElement("CUSTOMPROPERTIES");

        if(pElement)
        {
          pElement = pElement->FirstChildElement("PROPERTY");

          //        do
          //        {
          if(pElement)
          {
            String id = getAttrib(pElement, "id");

            if(id == "TriangleShape")
            {
              triangleShape = getAttribBool(pElement, "value");
            }
          }

          //          pElement = pElement->NextSiblingElement("PROPERTY");
          //        }
          //        while( pElement);
        }

        SceneNode *Node;
        // создаём ентити
        Entity *pEntity = 0;
        SceneNode *pParent = mSceneMgr->getSceneNode(parent);
        MeshManager::getSingleton().load(meshFile, "General");
        pEntity = mSceneMgr->createEntity(name, meshFile);
        pEntity->setCastShadows(castShadows);
        Node = pParent->createChildSceneNode(name);
        Node->attachObject(pEntity);
        Node->setPosition(pos);
        Node->setOrientation(orient);
        Node->setScale(scale);
        pEntity->setMaterialName(materialName);
        NxActor* actor;

        if(triangleShape)
        {
          NxTriangleMeshShapeDesc* trmshd = mPxManager->createFullMeshShapeFromModel(pEntity, scale);
          trmshd->group = GROUP_COLLIDABLE_NON_PUSHABLE;
          NxActorDesc   actorDesc;
          actorDesc.setToDefault();
          actorDesc.globalPose.t = mPxManager->toNx(Node->_getDerivedPosition());
          actorDesc.shapes.push_back(trmshd);
          actorDesc.group = GROUP_COLLIDABLE_NON_PUSHABLE;
          actor = mPxManager->getPxScene()->createActor(actorDesc);
          Quaternion q = Node->_getDerivedOrientation();
          actor->setGlobalOrientationQuat(mPxManager->toNx(q, true));
        }
        else
        {
          mPxManager->createBox(Node->_getDerivedPosition(), pEntity->getBoundingBox().getSize()*scale, true);
        }
      };

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

      Real getAttribReal(TiXmlElement *XMLNode, const String &attrib, Real defaultValue = 0)
      {
        if(XMLNode->Attribute(attrib.c_str()))
        {
          return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
        }
        else
        {
          return defaultValue;
        }
      };

      int getAttribInt(TiXmlElement *XMLNode, const String &attrib, int defaultValue = 0)
      {
        if(XMLNode->Attribute(attrib.c_str()))
        {
          return StringConverter::parseInt(XMLNode->Attribute(attrib.c_str()));
        }
        else
        {
          return defaultValue;
        }
      };

      bool getAttribBool(TiXmlElement *XMLNode, const String &attrib, bool defaultValue = "")
      {
        if(!XMLNode->Attribute(attrib.c_str()))
        {
          return defaultValue;
        }

        if(String(XMLNode->Attribute(attrib.c_str())) == "true")
        {
          return true;
        }

        return false;
      };

      Vector3 parseVector3(TiXmlElement *XMLNode, char* value)
      {
        return StringConverter::parseVector3(getAttrib(XMLNode, value));
      };

      Vector4 parseVector4(TiXmlElement *XMLNode, char* value)
      {
        return StringConverter::parseVector4(getAttrib(XMLNode, value));
      };

      Quaternion parseQuaternion(TiXmlElement *XMLNode, char* value)
      {
        return StringConverter::parseQuaternion(getAttrib(XMLNode, value));
      };
  };
}