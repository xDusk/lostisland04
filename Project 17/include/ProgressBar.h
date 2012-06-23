// ��� ��������� ����� ����� �������������� �����  Managing Game States with OGRE - ������� �������� ������
#pragma once

#include <Ogre.h>

using namespace Ogre;

class ProgressBar
{
  protected:
    RenderWindow *mWindow;
    Overlay *mLoadBar;
    OverlayElement *mLoadBarElement;
    Real mEndPosition;    //�������� ������� ��������
    Real mStep;       //��� ���������� ��������
  public:
    ProgressBar::ProgressBar(RenderWindow* win, Real endpos)
    {
      mWindow = win;
      mEndPosition = endpos;
      // ������� ��������
      mLoadBar = OverlayManager::getSingleton().getByName("LoadBar");
      mLoadBar->show();
      // ������ ��� ����������
      mLoadBarElement = OverlayManager::getSingleton().getOverlayElement("ProgressBar");
      mStep = mLoadBarElement->getWidth() / mEndPosition;
      mLoadBarElement->setWidth(0);
    };
    ~ProgressBar()
    {
      mLoadBar->hide();
    }

    void ProgressBar::update(Real dpos)
    {
      mLoadBarElement->setWidth(dpos * mStep);
      mWindow->update();
    };
};


