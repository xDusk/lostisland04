#include "ActionManager.h"

void ActionManager::init()
{
  AnimationStateSet *set = mEntity->getAllAnimationStates();
  AnimationStateIterator it = set->getAnimationStateIterator();

  while(it.hasMoreElements())
  {
    AnimationState *anim = it.getNext();
    anim->setEnabled(false);
    anim->setWeight(0);
    anim->setTimePosition(0);
  }

  std::vector<configAnims>::iterator iter = find_if(mConfig.begin(), mConfig.end(), find_byaction(IDLE));

  if(iter != mConfig.end())
  {
    mSource = mEntity->getAnimationState((*iter).mName);
    mSource->setEnabled(true);
    mSource->setWeight(1);
    mSourceConfig = (*iter);
    mDuration = mSourceConfig.mDuration;
  }
  else
  {
    mSource = 0;
    mDuration = 1;
  }

  mTimeleft = 0;
  mTarget = 0;
}
bool ActionManager::blend(const configAnims &ca)
{
  if(mSource != 0)
  {
    if(mTarget == 0)
    {
      mTarget = mEntity->getAnimationState(ca.mName);
      mTargetConfig = ca;
      mTarget->setEnabled(true);
      mTarget->setTimePosition(0);
      mTimeleft = mDuration = mSourceConfig.mDuration;
      mTarget->setWeight(1.0 - mTimeleft / mDuration);
      return true;
    }
    else
    {
      if(mTimeleft > mDuration * 0.5)   //смешивание недавно началось
      {
        if(mTargetConfig.mActionTermination == Limit)
        {
          mSource->setEnabled(false);
          mSource->setWeight(0);
          mTarget->setEnabled(false);
          mTarget->setWeight(0);
          mSource = mTarget;
          mSourceConfig = mTargetConfig;
          mSource->setEnabled(true);
          mSource->setWeight(1);
          mSource->setTimePosition(0);
          mTarget = 0;
          mTimeleft = mDuration = mSourceConfig.mDuration;
          return false; //не считываем из буфера пока не закончится смешивание
        }

        mTarget->setEnabled(false);
        mTarget->setWeight(0);
        mTarget = mEntity->getAnimationState(ca.mName);
        mTargetConfig = ca;
        mTarget->setEnabled(true);
        mTarget->setTimePosition(0);
        mTarget->setWeight(1.0 - mTimeleft / mDuration);
        return true;
      }
      else
      {
        if(mTargetConfig.mActionTermination == Limit)
        {
          mSource->setEnabled(false);
          mSource->setWeight(0);
          mTarget->setEnabled(false);
          mTarget->setWeight(0);
          mSource = mTarget;
          mSourceConfig = mTargetConfig;
          mSource->setEnabled(true);
          mSource->setWeight(1);
          mSource->setTimePosition(0);
          mTarget = 0;
          mTimeleft = mDuration = mSourceConfig.mDuration;
          return false; //не считываем из буфера пока не закончится смешивание
        }

        mSource->setEnabled(false);
        mSource->setWeight(0);
        mTarget->setEnabled(false);
        mTarget->setWeight(0);
        mSource = mTarget;
        mSourceConfig = mTargetConfig;
        mSource->setEnabled(true);
        mSource->setWeight(1);
        mTarget = mEntity->getAnimationState(ca.mName);
        mTargetConfig = ca;
        mTarget->setEnabled(true);
        mTarget->setTimePosition(0);
        mTimeleft = mDuration = mSourceConfig.mDuration;
        mTarget->setWeight(1.0 - mTimeleft / mDuration);
        return true;
      }
    }
  }

  return false;
}
void ActionManager::addTime(Real time)
{
  if(mSource != 0)
  {
    if(mTimeleft > 0)    //идет смешивание
    {
      mTimeleft -= time * mSourceConfig.mSpeed;
    }

    if(!mBuffer.empty())
    {
      Actions act = mBuffer.front();   // получаем действие из буфера
      std::vector<configAnims>::iterator it = find_if(mConfig.begin(), mConfig.end(), find_byaction(act));

      if(it != mConfig.end())
      {
        if(mSourceConfig.mActionTermination == Normal)
        {
          if(blend(*it) && !mBuffer.empty())
          {
            mBuffer.pop_front();  // удаляем действие из буфера
          }
        }
        else
        {
          if(mTimeleft <= 0)
          {
            mTimeleft = 0;

            if(mSource->getTimePosition() - mSource->getLength() + time >= 0)   //анимация почти завершена
            {
              if(blend(*it) && !mBuffer.empty())
              {
                mBuffer.pop_front();  // удаляем действие из буфера
              }
            }
          }
          else
          {
            if(mSource->getTimePosition() - mSource->getLength() + time >= 0)   //анимация почти завершена
            {
              if(blend(*it) && !mBuffer.empty())
              {
                mBuffer.pop_front();  // удаляем действие из буфера
              }
            }
          }
        }
      }
    }
    else
    {
      if((mTimeleft <= 0) && (mTarget != 0))
      {
        mSource->setEnabled(false);
        mSource->setWeight(0);
        mTarget->setEnabled(false);
        mTarget->setWeight(0);
        mSource = mTarget;
        mSourceConfig = mTargetConfig;
        mTimeleft = mDuration = mSourceConfig.mDuration;
        mSource->setEnabled(true);
        mSource->setWeight(1);
        mTarget = 0;
      }
    }

    if((mTimeleft > 0) && (mTarget != 0))  //идет смешивание
    {
      mSource->setWeight(mTimeleft / mDuration);  //вес плавно уменьшается
      mTarget->setWeight(1.0 - mTimeleft / mDuration);  //вес плавно увеличивается

      if(mTargetConfig.mTransition == BlendWhileAnimating)
      {
        mTarget->addTime(time * mTargetConfig.mSpeed);
      }
    }

    if((mSource->getTimePosition() - mSource->getLength() + time < 0) || (mSourceConfig.mActionTermination == Normal) || (mBuffer.empty()))
    {
      mSource->addTime(time * mSourceConfig.mSpeed);
      mSource->setLoop(mSourceConfig.mLooped);
    }
  }
}

void ActionManager::DoAction(Actions action)
{
  if(mSourceConfig.mActionTermination != Final)   //текущая анимация не завершающая
  {
    std::vector<configAnims>::iterator it = find_if(mConfig.begin(), mConfig.end(), find_byaction(action));

    if(it != mConfig.end())
    {
      if((*it).mBreak)  // срочно прервать действие (например если перс убит)
      {
        mBuffer.clear();
        blend(*it);
      }
      else
      {
        if(mBuffer.size() > 1)
        {
          mBuffer.clear();
        }

        mBuffer.push_back(action);
      }
    }
  }
}
