#include <Ogre.h>
#include <OIS.h>
#include "base/GameManager.h"
#include "base/IntroState.h"
#include "base/PlayState.h"

#include "windows.h"
#include <tchar.h>

int __cdecl _tmain(int, _TCHAR**, _TCHAR**)
{
  GameManager* game = new GameManager();

  try
  {
    game->start(IntroState::getInstance());
  }
  catch(Ogre::Exception& e)
  {
    MessageBoxA(NULL, e.getFullDescription().c_str(), "Achtung! General Failure reading you", MB_OK | MB_ICONERROR | MB_TASKMODAL);
  }

  delete game;
  return 0;
}