//
//  BasePopup.h
//  IndoGame
//
//  Created by longnh on 8/7/14.
//
//
#ifndef BASEPOPUP_H
#define BASEPOPUP_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "Constant.h"
#include "BaseScene.h"
#include "ResourceManager.h"
#include "EventConst.h"
class BaseScene;

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_UI;
USING_NS_STD;

class BasePopup :
	public Layer
{
public:
	BasePopup();
	~BasePopup();
	/**
	* Derived class should override this function to execute it's own job
	* and also call BasePopup::onOpen() to init base members.
	*/
	virtual void onOpen();
	/**
	* Derived class should override this function to execute it's own job
	* and also call BasePopup::onClose() to destroy base members.
	*/
	virtual void onClose();

	virtual void onTouchBackground(Ref* sender, Widget::TouchEventType type);
	virtual void onTouchOverlay(Ref* sender, Widget::TouchEventType type);

	/**
	* Each popup has one special event dispatcher using to register and dispatch
	* it's own event type
	*/
	cocos2d::EventDispatcher* getSpecialEventDispatcher();
    bool onTouchBegan(Touch* touch, Event* event);
private:

protected:
	bool m_bInitialized;
	cocos2d::EventDispatcher* m_pSpecialEventDispatcher;
	Layout* m_pLayout;

	void fadeInContent();
	void fadeOutContent();
    void setLightPosition();
};


#endif // !BASEPOPUP_H
