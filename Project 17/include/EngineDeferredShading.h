#pragma once

#include <Ogre.h>
#include <OgreD3D9Prerequisites.h>

class DeferredShading : public Ogre::RenderTargetListener, public Ogre::RenderQueueListener,
  public Ogre::FrameListener
{
  public:
    DeferredShading(const Ogre::String &_nameMRT, Ogre::Camera* _mCamera,
                    Ogre::PixelFormat pfRtt0, Ogre::PixelFormat pfRtt1, Ogre::PixelFormat pfRtt2,
                    Ogre::PixelFormat pfRtt3, bool _autoUpdate = true);
    ~DeferredShading()
    {
      Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget(nameMRT);
    }

    Ogre::ManualObject* createAmbientQuad(Ogre::SceneManager *mSceneMgr, const Ogre::String& strName,
                                          const Ogre::String& strMatName, const float width,
                                          const float height, const Ogre::uint8 uin,
                                          bool _bVisible = true);
    void setVisibleAmbientQuad(bool b = true);

    void setCoefSH3x9CubeMap(const Ogre::String& _nameCubeMap, Ogre::GpuProgramParametersSharedPtr paramsPixel);
    void addMotionBlur(std::vector <Ogre::Entity*> _mEnts, bool _bMotionBlur = true);
    inline void setMotionBlur(bool b = true)
    {
      bMotionBlur = b;
    }

    inline Ogre::MultiRenderTarget* getMRT()
    {
      return mrt;
    }
    inline Ogre::Viewport* getViewport()
    {
      return vp;
    }
    inline Ogre::String getName()
    {
      return nameMRT;
    }
    inline bool getAutoUpdateMRT()
    {
      return autoUpdate;
    }
    inline bool getVisibleAmbientQuad()
    {
      return bAmbient;
    }

  private:
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { }
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { }

    void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
    void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation) {}

    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

    Ogre::MultiRenderTarget* mrt;
    Ogre::Viewport* vp;
    Ogre::Camera* mCamera;
    Ogre::String nameMRT;
    bool autoUpdate, bAmbient, bMotionBlur;
    Ogre::ManualObject* pAmbientQuad;
    std::vector <Ogre::String> strNamEntMotionBlur;
    Ogre::Matrix4 matWVPLast;
};
