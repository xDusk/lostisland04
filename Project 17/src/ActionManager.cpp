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
      if(mTimeleft > mDuration * 0.5)   //���������� ������� ��������
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
          return false; //�� ��������� �� ������ ���� �� ���������� ����������
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
          return false; //�� ��������� �� ������ ���� �� ���������� ����������
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
    if(mTimeleft > 0)    //���� ����������
    {
      mTimeleft -= time * mSourceConfig.mSpeed;
    }

    if(!mBuffer.empty())
    {
      Actions act = mBuffer.front();   // �������� �������� �� ������
      std::vector<configAnims>::iterator it = find_if(mConfig.begin(), mConfig.end(), find_byaction(act));

      if(it != mConfig.end())
      {
        if(mSourceConfig.mActionTermination == Normal)
        {
          if(blend(*it) && !mBuffer.empty())
          {
            mBuffer.pop_front();  // ������� �������� �� ������
          }
        }
        else
        {
          if(mTimeleft <= 0)
          {
            mTimeleft = 0;

            if(mSource->getTimePosition() - mSource->getLength() + time >= 0)   //�������� ����� ���������
            {
              if(blend(*it) && !mBuffer.empty())
              {
                mBuffer.pop_front();  // ������� �������� �� ������
              }
            }
          }
          else
          {
            if(mSource->getTimePosition() - mSource->getLength() + time >= 0)   //�������� ����� ���������
            {
              if(blend(*it) && !mBuffer.empty())
              {
                mBuffer.pop_front();  // ������� �������� �� ������
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

    if((mTimeleft > 0) && (mTarget != 0))  //���� ����������
    {
      mSource->setWeight(mTimeleft / mDuration);  //��� ������ �����������
      mTarget->setWeight(1.0 - mTimeleft / mDuration);  //��� ������ �������������

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
  if(mSourceConfig.mActionTermination != Final)   //������� �������� �� �����������
  {
    std::vector<configAnims>::iterator it = find_if(mConfig.begin(), mConfig.end(), find_byaction(action));

    if(it != mConfig.end())
    {
      if((*it).mBreak)  // ������ �������� �������� (�������� ���� ���� ����)
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
