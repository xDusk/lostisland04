// при написании етого класа использовалась стать  Managing Game States with OGRE - изучите полезная статья
#pragma once

#include <Ogre.h>

using namespace Ogre;

class ProgressBar
{
  protected:
    RenderWindow *mWindow;
    Overlay *mLoadBar;
    OverlayElement *mLoadBarElement;
    Real mEndPosition;    //Конечная позиция ползунка
    Real mStep;       //Шаг приращения ползунка
  public:
    ProgressBar::ProgressBar(RenderWindow* win, Real endpos)
    {
      mWindow = win;
      mEndPosition = endpos;
      // Оверлей загрузки
      mLoadBar = OverlayManager::getSingleton().getByName("LoadBar");
      mLoadBar->show();
      // Полоса для приращения
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


