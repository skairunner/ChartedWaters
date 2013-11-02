#pragma once
#include <libtcod.hpp>
#include <vector>

class GameState
	{
	public:
   // std::vector<GameState*>* stateStack;
		GameState *prev;
		GameState(): pushSomething(false), prev(NULL), popMe(false) {}
  //  GameState() {prev = NULL; popMe = false; pushSomething = false;}
		virtual bool Init(){return true;}
		virtual void Update(){}
		virtual void Render(TCODConsole *root){}
		virtual void End(){}
    virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    bool popMe, pushSomething;
    GameState* nextState;
		//
		virtual void Resize(int new_w,int new_h){}
		virtual void WindowActive(){}
		virtual void WindowInactive(){}
		virtual void KeyUp(const int &key,const int &unicode){}
		virtual void KeyDown(const int &key,const int &unicode){}
		virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
		virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
		virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
	};

/* template

virtual bool Init(){return true;}

		virtual void Update(){}
		virtual void Render(TCODConsole *root){}
		virtual void End(){}
    
		//
		virtual void Resize(int new_w,int new_h){}
		virtual void WindowActive(){}
		virtual void WindowInactive(){}
		virtual void KeyUp(const int &key,const int &unicode){}
		virtual void KeyDown(const int &key,const int &unicode){}
		virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
		virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
		virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
*/
