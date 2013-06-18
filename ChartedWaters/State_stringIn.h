#include "GameState.h"
#include <string>

class state_StringIn: public GameState
  {
  public:
    state_StringIn(int w, std::string& input, std::string& displaytext = std::string("Input:"));
    ~state_StringIn();
    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();
    virtual bool Init();
    //virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    //
		virtual void Resize(int new_w,int new_h);
		virtual void WindowActive();
		virtual void WindowInactive();
		virtual void KeyUp(const int &key,const int &unicode);
		virtual void KeyDown(const int &key,const int &unicode);
		virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
		virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
		virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);

    TCODConsole* console;

  private:
    std::string& inputRef;
    std::string buffer;
    std::string text;
    int width;
  };