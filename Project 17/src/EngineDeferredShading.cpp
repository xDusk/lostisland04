#include "EngineDeferredShading.h"

DeferredShading::DeferredShading(const Ogre::String &_nameMRT, Ogre::Camera* _mCamera,
                                 Ogre::PixelFormat pfRtt0, Ogre::PixelFormat pfRtt1,
                                 Ogre::PixelFormat pfRtt2, Ogre::PixelFormat pfRtt3,
                                 bool _autoUpdate)
  : nameMRT(_nameMRT), mCamera(_mCamera), autoUpdate(_autoUpdate)
{
  bAmbient = false;
  pAmbientQuad = NULL;
  matWVPLast = Ogre::Matrix4::ZERO;
  Ogre::TexturePtr target0 = Ogre::TextureManager::getSingleton().createManual(
                               "rtt0",
                               Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth(),
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight(),
                               32, 1, pfRtt0, Ogre::TU_RENDERTARGET);
  Ogre::TexturePtr target1 = Ogre::TextureManager::getSingleton().createManual(
                               "rtt1",
                               Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth(),
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight(),
                               32, 1, pfRtt1, Ogre::TU_RENDERTARGET);
  Ogre::TexturePtr target2 = Ogre::TextureManager::getSingleton().createManual(
                               "rtt2",
                               Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth(),
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight(),
                               32, 1, pfRtt2, Ogre::TU_RENDERTARGET);
  Ogre::TexturePtr target3 = Ogre::TextureManager::getSingleton().createManual(
                               "rtt3",
                               Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth(),
                               Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight(),
                               32, 1, pfRtt3, Ogre::TU_RENDERTARGET);
  Ogre::RenderTexture* targetOut0 = target0->getBuffer()->getRenderTarget();
  Ogre::RenderTexture* targetOut1 = target1->getBuffer()->getRenderTarget();
  Ogre::RenderTexture* targetOut2 = target2->getBuffer()->getRenderTarget();
  Ogre::RenderTexture* targetOut3 = target3->getBuffer()->getRenderTarget();
  mrt = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget(nameMRT);
  mrt->bindSurface(0, targetOut0);
  mrt->bindSurface(1, targetOut1);
  mrt->bindSurface(2, targetOut2);
  mrt->bindSurface(3, targetOut3);
  vp = mrt->addViewport(_mCamera);
  mrt->setAutoUpdated(autoUpdate);
  vp->setOverlaysEnabled(false);
  vp->setClearEveryFrame(true);
  vp->setSkiesEnabled(true);
  vp->setBackgroundColour(Ogre::ColourValue::White);
  mrt->addListener(this);
}

void DeferredShading::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
{
  Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
  vp->setVisibilityMask(0);
  rendersys->setStencilCheckEnabled(true);

  if(queueGroupId < Ogre::RENDER_QUEUE_MAIN)
  {
    rendersys->setStencilBufferParams(Ogre::CMPF_ALWAYS_PASS, queueGroupId  , 0xFFFFFFFF, Ogre::SOP_KEEP, Ogre::SOP_KEEP, Ogre::SOP_REPLACE, false);
  }

  if(queueGroupId > Ogre::RENDER_QUEUE_MAIN)
  {
    rendersys->setStencilBufferParams(Ogre::CMPF_EQUAL, queueGroupId - Ogre::RENDER_QUEUE_MAIN  , 0xFFFFFFFF, Ogre::SOP_KEEP, Ogre::SOP_KEEP, Ogre::SOP_KEEP, false);
  }

  if(queueGroupId == Ogre::RENDER_QUEUE_OVERLAY)
  {
    rendersys->setStencilBufferParams(Ogre::CMPF_ALWAYS_PASS, queueGroupId  , 0xFFFFFFFF, Ogre::SOP_KEEP, Ogre::SOP_KEEP, Ogre::SOP_REPLACE, false);
  }
}

bool DeferredShading::frameStarted(const Ogre::FrameEvent& evt)
{
  /*  if ( bMotionBlur )
    {
      Ogre::Matrix4 matWVP = mCamera->getProjectionMatrix() * mCamera->getViewMatrix();
      //Ogre::Matrix4 matWVP = mCamera->getViewMatrix() * mCamera->getProjectionMatrix();
      //mat = mat * mSceneNode->_getFullTransform();
      std::vector <Ogre::String>::iterator it;
      for (it = strNamEntMotionBlur.begin(); it != strNamEntMotionBlur.end(); it++)
      {
        Ogre::MaterialPtr mMaterial = Ogre::MaterialManager::getSingleton().getByName(*(it));
        Ogre::GpuProgramParametersSharedPtr paramsVertex = mMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
        paramsVertex->setNamedConstant("matWorldViewProjection", matWVP);
        if (matWVPLast == Ogre::Matrix4::ZERO)
          paramsVertex->setNamedConstant("matWorldViewProjectionLast", matWVP);
        else
          paramsVertex->setNamedConstant("matWorldViewProjectionLast", matWVPLast);
      }
      matWVPLast = matWVP;
    }*/
  vp->setVisibilityMask(10);
  return true;
}

bool DeferredShading::frameEnded(const Ogre::FrameEvent& evt)
{
  Ogre::RenderSystem * rSys = Ogre::Root::getSingleton().getRenderSystem();
  rSys->clearFrameBuffer(Ogre::FBT_DEPTH, Ogre::ColourValue::Black);
  rSys->clearFrameBuffer(Ogre::FBT_STENCIL, Ogre::ColourValue::Black);
  rSys->clearFrameBuffer(Ogre::FBT_COLOUR, Ogre::ColourValue::Black);
  return true;
}

Ogre::ManualObject* DeferredShading::createAmbientQuad(Ogre::SceneManager *mSceneMgr, const Ogre::String& strName,
                                                       const Ogre::String& strMatName, const float width,
                                                       const float height, const Ogre::uint8 uin,
                                                       bool _bVisible)
{
  /*
  VisibilityFlags:
  0 - SkyDome(Draw first),
  1 - Entity for Deferred Shading,
  2..n - Quad = uin
  */
  pAmbientQuad = mSceneMgr->createManualObject(strName);
  pAmbientQuad->setUseIdentityProjection(true);
  pAmbientQuad->setUseIdentityView(true);
  pAmbientQuad->begin(strMatName, Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  pAmbientQuad->position(-width, -height, 0.0), pAmbientQuad->textureCoord(0, 1);
  pAmbientQuad->position(width, -height, 0.0), pAmbientQuad->textureCoord(1, 1);
  pAmbientQuad->position(width,  height, 0.0), pAmbientQuad->textureCoord(1, 0);
  pAmbientQuad->position(-width,  height, 0.0), pAmbientQuad->textureCoord(0, 0);
  pAmbientQuad->index(0), pAmbientQuad->index(1);
  pAmbientQuad->index(2), pAmbientQuad->index(3);
  pAmbientQuad->index(0), pAmbientQuad->end();
  Ogre::AxisAlignedBox aabInf;
  aabInf.setInfinite();
  pAmbientQuad->setBoundingBox(aabInf);
  pAmbientQuad->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
  pAmbientQuad->setVisibilityFlags(uin);
  pAmbientQuad->setVisible(_bVisible);
  bAmbient = _bVisible;
  return pAmbientQuad;
}