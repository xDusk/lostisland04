#pragma once

#include <Ogre.h>

using namespace Ogre;

class RTTManager : public FrameListener
{
  protected:
    Ogre::RenderTexture *renderTexture;
    Ogre::TexturePtr  texture;

  public:
    RTTManager(Camera *cam, RenderWindow *win) : renderTexture(NULL)
    {
      texture = Ogre::TextureManager::getSingleton().createManual("RtTexture",
                                                                  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, win->getWidth(), win->getHeight(), 0, PF_R8G8B8A8,
                                                                  TU_RENDERTARGET);
      renderTexture = texture->getBuffer()->getRenderTarget();
      renderTexture->addViewport(cam);
      renderTexture->getViewport(0)->setClearEveryFrame(true);
      renderTexture->getViewport(0)->setBackgroundColour(ColourValue::Black);
      renderTexture->getViewport(0)->setOverlaysEnabled(false);
      // Either this way
      renderTexture->setAutoUpdated(true);
      // or this way
      //    renderTexture->update();
      //    renderTexture->writeContentsToFile("rtt.png");
    };
    ~RTTManager()
    {
      //    renderTexture->~RenderTexture();
      texture->freeInternalResources();
    };

    void saveTexture()
    {
      renderTexture->writeContentsToFile("rtt.png");
    }

    void setTexture(TexturePtr texPtr)
    {
      texture = texPtr;
    }

    // Define FrameListener methods
    bool frameStarted(const FrameEvent& evt)
    {
      //    renderTexture->update();
      renderTexture = texture->getBuffer()->getRenderTarget();
      //    saveTexture("RTT.PNG");
      return true;
    };
    bool frameEnded(const FrameEvent& evt)
    {
      return true;
    };
};