#pragma once

#include "SkyX.h"

using namespace Ogre;
using namespace std;

class SkyManager
{
  protected:
    SkyX::SkyX              *smSkyXSystem;
    SkyX::AtmosphereManager::Options  smSkyXOptions;
    bool                smVolumeClouds;
    SceneManager            *smSceneMgr;
    Camera                *smCamera;
    Light               *smLight;
    float               smLightIntens;

    SceneNode *dnode;

  public:
    SkyManager(SceneManager *scene, Camera *camera)
    {
      smSceneMgr  = scene;
      smCamera  = camera;
      smLight   = scene->getLight("Light1");
      smSkyXSystem = new SkyX::SkyX(smSceneMgr, smCamera);
      // No smooth fading
      smSkyXSystem->getMeshManager()->setSkydomeFadingParameters(false);
      smSkyXSystem->setLightingMode(smSkyXSystem->LM_HDR);
      smSkyXSystem->create();
      smLightIntens = 0;
      ///debug
      //  Entity *dbox = smSceneMgr->createEntity("box", "Crate.mesh");
      //  dnode = smSceneMgr->getRootSceneNode()->createChildSceneNode("dnode");
      //  dnode->attachObject( dbox);
      //  dnode->scale( 50, 50, 50);
      //debug end
      smVolumeClouds = false;
    };
    ~SkyManager()
    {
      smSkyXSystem->~SkyX();
    };

    void SimpleCloudsCreate()
    {
      smSkyXSystem->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
    }

    void VolumeCloudsCreate()
    {
      smSkyXSystem->getVCloudsManager()->create();
      smVolumeClouds = true;
      //    smSkyXSystem->getVCloudsManager()->setHeight( Vector2( 1000,2000)); //требуетс€ врем€ дл€ применени€ см vclouds->getDataManager()->setUpdateTime(10);
    };

    void weather()
    {
      float coverage = Ogre::Math::UnitRandom();
      SkyX::VClouds::VClouds *vclouds = smSkyXSystem->getVCloudsManager()->getVClouds();
      vclouds->setWheater(coverage, coverage);
      vclouds->getDataManager()->setUpdateTime(10);
    };

    void saveState()
    {
      //врем€ и погоду
    };
    void loadState();

    //x врем€; y восход; z полдень
    void setCurrentTime(Vector3 hms)
    {
      smSkyXOptions = smSkyXSystem->getAtmosphereManager()->getOptions();
      smSkyXOptions.Time = hms;
      smSkyXSystem->getAtmosphereManager()->setOptions(smSkyXOptions);
    };

    //x-час y-минуты
    Vector2 getCurrentTime()
    {
      Vector2 time;
      time.x = smSkyXSystem->getAtmosphereManager()->getOptions().Time.x;
      int h = time.x;
      time.y = (int)((time.x - h) * 60); //чЄ это за хрень
      time.x = (int)(time.x);
      return time;
    };

    int getLightIntense()
    {
      return smLightIntens;
    };

    Vector3 getLightDirection()
    {
      return smSkyXSystem->getAtmosphereManager()->getSunDirection();
    }

    Vector3 getLightPosition()
    {
      return smLight->getPosition();
    };

    Vector3 getSunPosition()
    {
      return smSkyXSystem->getAtmosphereManager()->getSunPosition();
    }

    Light* getLight()
    {
      return smLight;
    };

    void initSkyXManager()
    {
      //    smSkyXSystem->setLightingMode( ); //переключатель лдр и хдр
      smSkyXOptions = smSkyXSystem->getAtmosphereManager()->getOptions();
      // Time
      smSkyXSystem->setTimeMultiplier(0.1); //0.001f  - минута = около 15 сек// 0.01f около 1 сек
      // Rayleigh multiplier
      smSkyXOptions.RayleighMultiplier = 0.0022f;
      // Mie multiplier
      smSkyXOptions.MieMultiplier = 0.000675f;
      // Exposure
      smSkyXOptions.Exposure = 2.0f;
      // Inner radius
      smSkyXOptions.InnerRadius = 9.77f;
      // Outer radius
      smSkyXOptions.OuterRadius = 10.3f;
      // Number of samples
      smSkyXOptions.NumberOfSamples = 4;
      // Height position
      smSkyXOptions.HeightPosition = 0.01f;
      smSkyXSystem->getAtmosphereManager()->setOptions(smSkyXOptions);

      if(smVolumeClouds)
      {
        // скорость ветра
        smSkyXSystem->getVCloudsManager()->setWindSpeed(8000.0f);
        // направление ветра
        smSkyXSystem->getVCloudsManager()->getVClouds()->setWindDirection(Ogre::Radian(Degree(90)));
        // "грубость" облаков
        smSkyXSystem->getVCloudsManager()->getVClouds()->setNoiseScale(4.0f);
        smSkyXSystem->getVCloudsManager()->setHeight(Vector2(4000, 5000));
        // плотность облаков //мен€етс€ не сразу=)
        float coverage = 0.05; //nearly no clouds
        SkyX::VClouds::VClouds *vclouds = smSkyXSystem->getVCloudsManager()->getVClouds();
        vclouds->getDataManager()->setUpdateTime(1);
        vclouds->setWheater(coverage, coverage);
        //      time.sleep(2);//#while the 2 seconds, the clouds are changing their density
        vclouds->getDataManager()->setUpdateTime(10);
      }

      //очередь рендеринга (дл€ ƒЎ)
      smSkyXSystem->getMeshManager()->getEntity()->setVisibilityFlags(5);
      smSkyXSystem->getMeshManager()->getEntity()->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);
      //    smLight->setPosition( smSkyXSystem->getAtmosphereManager()->getSunPosition());
      //smLight->setPosition( playerpos - lightDir*smSkyXSystem->getMeshManager()->getSkydomeRadius()*0.01);
      //    smLight->setDirection( smSkyXSystem->getAtmosphereManager()->getSunDirection());
    };

  public:
    void update(const Ogre::Real &timeSinceLastFrame, Vector3 playerpos = Vector3(20000, 0, 20000))
    {
      smSkyXSystem->update(timeSinceLastFrame);
      Ogre::Vector3 lightDir = smSkyXSystem->getAtmosphereManager()->getSunDirection();

      //    lightDir.x /= lightDir.normalise();
      //    lightDir.y /= lightDir.normalise();
      //    lightDir.z /= lightDir.normalise();
      //    lightDir = Vector3( 0, -1, 0);
      if(lightDir.y > 0)
      {
        smLight->setDiffuseColour(ColourValue(0, 0, 0));
      }
      else
      {
        smLight->setDiffuseColour(ColourValue(1, 1, 1));
      }

      //    smLight->setPosition( smSkyXSystem->getAtmosphereManager()->getSunPosition());
      //    smLight->setPosition( -lightDir*smSkyXSystem->getMeshManager()->getSkydomeRadius()*0.01);
      //свет вращаетс€ вокруг игрока
      //    smLight->setPosition( playerpos - lightDir*smSkyXSystem->getMeshManager()->getSkydomeRadius()*0.1);
      //(позици€ солнца - позици€ цели) нормализовать и всЄ это в сет дирекшн
      //    smLight->setDirection( lightDir);
      //    smLight->setDiffuseColour( Ogre::ColourValue::White*Ogre::Math::Clamp<Ogre::Real>( Ogre::Math::Pow( -lightDir.y, 0.25)*2, 0.2, 1));
      ///debug
      // dnode->setPosition( smLight->getPosition());
      //    dnode->setDirection( smLight->getDirection());
      //debug end
      //поправка света амбиента наночь.
      //    float li = getCurrentTime().x*( Ogre::Math::HALF_PI)/12;//в 12 будет макс €ркость в 0 минимальна€
      ////    float li = ( getCurrentTime().x*getCurrentTime().y)*( Ogre::Math::HALF_PI)/720;//в 12 будет макс €ркость в 0 минимальна€
      //    li = Ogre::Math::Sin( li);
      //    if( li<0.1) li=0.1;
      //    if( li>0.8) li=0.8;
      ////    smLight->setDiffuseColour( 1*li, 1*li, 1*li);
      //    smSceneMgr->setAmbientLight( ColourValue(li, li, li));
      //иф прошол час вызывать метод рандома погоды.
    };

    inline void setLightingModeHDR()
    {
      smSkyXSystem->setLightingMode(smSkyXSystem->LM_HDR);
    };

    inline void setLightingModeLDR()
    {
      smSkyXSystem->setLightingMode(smSkyXSystem->LM_LDR);
    };

    SkyX::SkyX* getSkyXSystem()
    {
      return smSkyXSystem;
    };
};
