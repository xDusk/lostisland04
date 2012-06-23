#pragma once

#include <OgreParticle.h>

namespace Ogre
{
  /// custom visual data for shader renderer
  class ParticleCustomParam : public ParticleVisualData
  {
    public:
      ParticleCustomParam() : paramValue(0, 0, 0, 0) {}
      virtual ~ParticleCustomParam() {}

      Vector4   paramValue;
  };
}
