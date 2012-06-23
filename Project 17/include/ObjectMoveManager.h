//////////////////////////////////////////////////////////////////////////
///////// Класс управления движением объекта          //////////
//////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

#pragma once
class ObjectMoveManager
{
  protected:
    Real mYaw;    // скорость и направление отклонения от курса (рыскание <0 - вправо, >0 - влево)
    Real mRoll;   // скорость и направление вращения или уклона (бочка <0 - по часовой, >0 - против часовой)
    Real mPitch;  // скорость и направление наклона (<0 - вперед, >0 - назад)
    Real mAlong;  // скорость и направление вдоль курса (>0 - назад, <0 - вперед)
    Real mAcross; // скорость и направление движения поперек курса (<0 - влево, >0 - вправо)
    Real mVertical; // вертикальные скорость и направление движения (<0 - вниз, >0 - вверх)
    /*массив вспомогательных точек объекта
    (точка для прицеливания, места для крепления камеры от 1 и 3-го лица, точка взгляда объекта)
    все эти точки относительны основного узла объекта и должны перемещаться и разворачиваться вместе с объектом
    */
    Vector3 mPoints;
    unsigned int mSizePoints;

    SceneNode *mMainNode;
    SceneManager *mSceneMgr;
  public:
    ObjectMoveManager(String name, SceneManager *sceneMgr, Vector3 points = Vector3(0, 0, 0), unsigned int sizePoints = 0): mYaw(0), mRoll(0), mPitch(0), mAlong(0), mAcross(0), mVertical(0)
    {
      mSceneMgr = sceneMgr;
      mMainNode = sceneMgr->getSceneNode(name);
      mPoints = points;
      mSizePoints = sizePoints;
    }
    ~ObjectMoveManager() {}

    // Обновление юнита (передвижение)
    void update(Real elapsedTime)
    {
      if(!(mAcross || mVertical || mAlong) == 0)
      {
        mMainNode->translate(mMainNode->getOrientation()* Vector3(mAcross * elapsedTime, mVertical * elapsedTime, mAlong * elapsedTime));
      }

      if(!(mYaw || mRoll || mPitch) == 0)
      {
        if(!mYaw == 0)
        {
          mMainNode->yaw(Radian(mYaw * elapsedTime));
        }

        if(!mRoll == 0)
        {
          mMainNode->roll(Radian(mRoll * elapsedTime));
        }

        if(!mPitch == 0)
        {
          mMainNode->pitch(Radian(mPitch * elapsedTime));
        }

        if(!mSizePoints == 0)
        {
          //разворачиваем точки относительно основного узла
          Quaternion ary(Radian(mYaw * elapsedTime), Vector3::UNIT_Y);
          Quaternion arz(Radian(mRoll * elapsedTime), Vector3::UNIT_Z);
          Quaternion arx(Radian(mPitch * elapsedTime), Vector3::UNIT_X);

          for(unsigned int i = 0; i < mSizePoints; i++)
          {
            mPoints = ary * arz * arx * mPoints;
          }
        }
      }
    }
    void setYaw(Real yaw)
    {
      mYaw = yaw;
    }
    void setRoll(Real roll)
    {
      mRoll = roll;
    }
    void setPitch(Real pitch)
    {
      mPitch = pitch;
    }
    void setAlong(Real along)
    {
      mAlong = along;
    }
    void setAcross(Real across)
    {
      mAcross = across;
    }
    void setVertical(Real vertical)
    {
      mVertical = vertical;
    }
    Real getYaw()
    {
      return mYaw;
    }
    Real getRoll()
    {
      return mRoll;
    }
    Real getPitch()
    {
      return mPitch;
    }
    Real getAlong()
    {
      return mAlong;
    }
    Real getAcross()
    {
      return mAcross;
    }
    Real getVertical()
    {
      return mVertical;
    }
};
