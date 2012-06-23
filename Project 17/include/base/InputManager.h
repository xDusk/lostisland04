#pragma once

#include <Ogre.h>
#include <OIS.h>

class InputManager : public OIS::KeyListener, OIS::MouseListener
{
  public:
    virtual ~InputManager(void);

    void initialise(Ogre::RenderWindow *renderWindow);
    void capture(void);

    void addKeyListener(OIS::KeyListener *keyListener, const std::string& instanceName);
    void addMouseListener(OIS::MouseListener *mouseListener, const std::string& instanceName);

    void removeKeyListener(const std::string& instanceName);
    void removeMouseListener(const std::string& instanceName);

    void removeKeyListener(OIS::KeyListener *keyListener);
    void removeMouseListener(OIS::MouseListener *mouseListener);

    void removeAllListeners(void);
    void removeAllKeyListeners(void);
    void removeAllMouseListeners(void);
    void removeAllJoystickListeners(void);

    void setWindowExtents(int width, int height);

    OIS::Mouse*    getMouse(void);
    OIS::Keyboard* getKeyboard(void);

    static InputManager* getSingletonPtr(void);
  protected:
    InputManager(void);
    InputManager(const InputManager&) { }
    InputManager & operator = (const InputManager&);

    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);


    OIS::InputManager *mInputSystem;
    OIS::Mouse        *mMouse;
    OIS::Keyboard     *mKeyboard;

    std::map<std::string, OIS::KeyListener*> mKeyListeners;
    std::map<std::string, OIS::MouseListener*> mMouseListeners;

    std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;

    std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;

    static InputManager *mInputManager;
};
