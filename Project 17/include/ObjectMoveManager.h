//////////////////////////////////////////////////////////////////////////
///////// ����� ���������� ��������� �������          //////////
//////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

#pragma once
class ObjectMoveManager
{
  protected:
    Real mYaw;    // �������� � ����������� ���������� �� ����� (�������� <0 - ������, >0 - �����)
    Real mRoll;   // �������� � ����������� �������� ��� ������ (����� <0 - �� �������, >0 - ������ �������)
    Real mPitch;  // �������� � ����������� ������� (<0 - ������, >0 - �����)
    Real mAlong;  // �������� � ����������� ����� ����� (>0 - �����, <0 - ������)
    Real mAcross; // �������� � ����������� �������� ������� ����� (<0 - �����, >0 - ������)
    Real mVertical; // ������������ �������� � ����������� �������� (<0 - ����, >0 - �����)
    /*������ ��������������� ����� �������
    (����� ��� ������������, ����� ��� ��������� ������ �� 1 � 3-�� ����, ����� ������� �������)
    ��� ��� ����� ������������ ��������� ���� ������� � ������ ������������ � ��������������� ������ � ��������
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

    // ���������� ����� (������������)
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
          //������������� ����� ������������ ��������� ����
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
