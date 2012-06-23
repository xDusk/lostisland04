#pragma once

#include <Ogre.h>
#include "OgreTerrain.h"
#include "SkyManager.h"
#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"
#include "Modules/SimpleGrid/SimpleGrid.h"
#include "SharedData.h"

using namespace Ogre;

enum WaterType
{
  LOW_WATER,
  MIDDLE_WATER,
  HIGHT_WATER,
  MENU_WATER
};

class HydraxManager : public FrameListener
{
  protected:
    Hydrax::Hydrax *hmHydrax;
    Hydrax::Noise::Perlin *hmNoiseModule;
    Hydrax::Module::ProjectedGrid *hmModule;

    SkyManager      *hmSky;
    SceneManager    *hmScene;
    Camera        *hmCam;

    Real timeSinceNextUpdate, secondsToUpdate;
    Vector2 hmSunArea, hmWaterBrightness, hmSeaStrenghtRand, hmSeaAnimSpeedRand, hmCloudCoverRand;
    Vector3 hmWaterColor, hmSunTransparentColour;

    ColourValue hmWaterColorHsl;

  public:
    HydraxManager(SceneManager *scene, Camera *cam, RenderWindow *win, SkyManager *sm, Vector3 position, WaterType waterType, Ogre::Terrain *mTer = 0)
    {
      hmSky  = sm;
      hmScene   = scene;
      hmCam     = cam;
      // создаём объект гидракса
      hmHydrax = new Hydrax::Hydrax(scene, cam, win->getViewport(0));
      //и шума
      hmNoiseModule = new Hydrax::Noise::Perlin(/*Generic one*/);
      // создаём сетку
      hmModule = new Hydrax::Module::ProjectedGrid(hmHydrax,       // Hydrax parent pointer
                                                   hmNoiseModule,                        // Noise module
                                                   Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)), // Base plane
                                                   Hydrax::MaterialManager::NM_VERTEX,             // Normal mode
                                                   Hydrax::Module::ProjectedGrid::Options(2));         // Projected grid options
      // Set our module
      hmHydrax->setModule(static_cast<Hydrax::Module::Module*>(hmModule));

      //грузим параметры из конфиг файла
      //Low = 1; Mid = 2; Hig = 3; Menu = 4
      if(waterType == LOW_WATER)
      {
        hmHydrax->loadCfg("GPUWater.hdx");
      }
      else if(waterType == MIDDLE_WATER)
      {
        hmHydrax->loadCfg("FastWater.hdx");
      }
      else if(waterType == HIGHT_WATER)
      {
        hmHydrax->loadCfg("myHydrax.hdx");
      }
      else if(waterType == MENU_WATER)
      {
        hmHydrax->loadCfg("FFTBeautifuld.hdx");
      }
      else
      {
        hmHydrax->loadCfg("HydraxDemo.hdx");
      }

      // создаём воду
      hmHydrax->create();
      //пизиции воды и солнца
      hmHydrax->setPosition(position);
      hmHydrax->setSunPosition(hmCam->getDerivedPosition() - hmSky->getSkyXSystem()->getAtmosphereManager()->getSunDirection()*hmSky->getSkyXSystem()->getMeshManager()->getSkydomeRadius() * 0.1);
      //              hmHydrax->setSunPosition(hmSky->getSkyXSystem()->getAtmosphereManager()->getSunPosition());

      // Add our hydrax depth technique to island material
      // (Because the terrain mesh is not an Ogre::Entity)
      //добавляем депт технику к материалу терана потому что он не огре ентии.
      //              hmHydrax->getMaterialManager()->addDepthTechnique(
      //                static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("MyTerrain"))
      //                ->createTechnique());
      if(mTer != 0)
      {
        //hmHydrax->getMaterialManager()->addDepthTechnique(  mTer->getMaterial().get()->getBestTechnique());
        hmHydrax->getMaterialManager()->addDepthTechnique(mTer->getMaterial().get()->createTechnique());
        //hmHydrax->getMaterialManager()->addDepthTechnique( mTer->getCompositeMapMaterial().get()->getBestTechnique());
      }

      timeSinceNextUpdate = 0.0;
      //              hmHydrax->getMesh()->getEntity()->setVisibilityFlags(5);
      //              hmHydrax->getMesh()->getEntity()->setRenderQueueGroup(0);
      //hmHydrax->setWaterColor(Vector3( 0.058209 ,0.535822 ,0.679105));
    };

    HydraxManager::HydraxManager(SkyManager *sky, Camera *cam, Hydrax::Hydrax* hydrax, Hydrax::Module::ProjectedGrid* module, Hydrax::Noise::Perlin* noiseModule)
    {
      hmSky = sky;
      hmCam = cam;
      hmHydrax = hydrax;
      hmNoiseModule = noiseModule;
      hmModule = module;
    };

    ~HydraxManager()
    {
      hmHydrax->remove();
      delete hmHydrax;
      //        delete hmModule;
      //        delete hmNoiseModule;
      //        hmSky = nullptr;
      //        hmScene = nullptr;
      //        hmCam = nullptr;
    };

    // Define FrameListener methods
    bool frameStarted(const FrameEvent& evt)
    {
      OgreProfileBegin("Hydrax Update");
      timeSinceNextUpdate -= evt.timeSinceLastFrame;

      if(timeSinceNextUpdate <= 0.0)
      {
        timeSinceNextUpdate = secondsToUpdate;
      }

      updateSkyOnWater();
      updateWeatherOnWater();
      hmHydrax->update(evt.timeSinceLastFrame);
      OgreProfileEnd("Hydrax Update");
      return true;
    };
    bool frameEnded(const FrameEvent& evt)
    {
      return true;
    };

    Hydrax::Hydrax* HydraxManager::getHydraxManager()
    {
      return hmHydrax;
    };

    Hydrax::Noise::Perlin* HydraxManager::getHydraxNoiseModule()
    {
      return hmNoiseModule;
    };

    Hydrax::Module::ProjectedGrid* HydraxManager::getHydraxPGModule()
    {
      return hmModule;
    };


  protected:
    void updateSkyOnWater()
    {
      hmHydrax->setSunPosition(hmCam->getDerivedPosition() - hmSky->getSkyXSystem()->getAtmosphereManager()->getSunDirection() * 80000);
    };
    void updateWeatherOnWater()
    {
      // Update AnimSpeed value
      Hydrax::Noise::Perlin::Options temp_noise_opt = hmNoiseModule->getOptions();
      hmNoiseModule->setOptions(temp_noise_opt);
      // Update Strenght value
      Hydrax::Module::ProjectedGrid::Options temp_pg_opt = hmModule->getOptions();
      hmModule->setOptions(temp_pg_opt);
    };

};
