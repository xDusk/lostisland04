#pragma once

#include <Ogre.h>
#include "configAnims.h"

using namespace Ogre;


class find_byaction
{
    Actions mActions;
  public:
    find_byaction(Actions action)
    {
      mActions = action;
    }
    bool operator()(const configAnims &ca)
    {
      if(ca.mActions == mActions)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
};

class ActionManager
{
  protected:
    std::vector<configAnims> mConfig; //указатель на настройки действий
    Entity *mEntity;
    AnimationState *mSource;
    AnimationState *mTarget;
    Real mTimeleft, mDuration;
    std::deque<Actions> mBuffer;    //буфер действий для смешивания
    configAnims mSourceConfig;
    configAnims mTargetConfig;

  public:

    ActionManager(Entity *entity, const std::vector<configAnims> &ca)
    {
      mEntity = entity;
      mConfig = ca;
    }
    ~ActionManager()
    {
    }
    void init();
    bool blend(const configAnims &ca);
    void addTime(Real);
    inline Real getProgress()
    {
      return mTimeleft / mDuration;
    }
    inline AnimationState *getSource()
    {
      return mSource;
    }
    inline AnimationState *getTarget()
    {
      return mTarget;
    }
    inline configAnims getSourceConfig()
    {
      return mSourceConfig;
    }

    void DoAction(Actions action);

};
