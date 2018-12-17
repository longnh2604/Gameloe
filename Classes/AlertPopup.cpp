//
//  AlertPopup.cpp
//  IndoGame
//
//  Created by longnh on 8/7/14.
//
//
#include "AlertPopup.h"
string AlertPopup::OK = "ok";

/*************************************************************/
/* @author longnh												 */
/* Constructor/Destructor functions							 */
/*************************************************************/
AlertPopup::AlertPopup(std::string sMsr)
{
	m_sMsr = sMsr;
}

AlertPopup::AlertPopup()
{
	m_sMsr = "Content alert";
}

AlertPopup::~AlertPopup()
{
}

AlertPopup* AlertPopup::create()
{
	AlertPopup* popup = new AlertPopup();
	popup->autorelease();
	return popup;
}

AlertPopup* AlertPopup::create(std::string sMsr)
{
	AlertPopup* popup = new AlertPopup(sMsr);
	popup->autorelease();
	return popup;
}
/* End Constructor/Destructor functions						  */


/**************************************************************/
/* @author longnh												  */
/* start pragma onOpen/onClose functions					  */
/**************************************************************/
void AlertPopup::onOpen()
{
	BasePopup::onOpen();
	m_pLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("AlertPopup.json"));
	Size layoutSize = m_pLayout->getContentSize();
	Size deviceSize = Director::getInstance()->getWinSize();
	m_pLayout->setAnchorPoint(Vec2(0.5, 0.5));
	m_pLayout->setPosition(Vec2(deviceSize.width / 2, deviceSize.height / 2));
	this->addChild(m_pLayout);

	Button* btnClose = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnClose"));
	btnClose->addTouchEventListener(CC_CALLBACK_2(AlertPopup::onTouchOkBtn, this));

	//TODO: Should show alert content by messenger getting from server
	Text* tfContent = static_cast<Text*>(Helper::seekWidgetByName(m_pLayout, "lblContent"));
	tfContent->setString(m_sMsr);
	tfContent->setTextHorizontalAlignment(TextHAlignment::CENTER);
	tfContent->setEnabled(false);

	ImageView* background = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "background"));
	background->addTouchEventListener(CC_CALLBACK_2(BasePopup::onTouchBackground, this));
	m_pLayout->addTouchEventListener(CC_CALLBACK_2(BasePopup::onTouchOverlay, this));

	fadeInContent();
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(AlertPopup::changeLanguage), EVENT_LANGUAGE_CHANGE, nullptr);
    setTextLanguage();
    setLightPosition();
}

void AlertPopup::onClose()
{
	BasePopup::onClose();
}
/* end pragma onOpen/onClose functions        */

/**********************************************/
/* @author longnh								  */
/* start pragma event handlers functions      */
/**********************************************/
void AlertPopup::onTouchOkBtn(Ref* pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED) return;

	getSpecialEventDispatcher()->dispatchCustomEvent(AlertPopup::OK, nullptr);

	BaseScene* scene = static_cast<BaseScene*>(Director::getInstance()->getRunningScene());
	scene->closePopup(this);
}

void AlertPopup::changeLanguage(Ref* obj)
{
    setTextLanguage();
}

void AlertPopup::setTextLanguage()
{
    Button* btnClose = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnClose"));
    TextBMFont* bmfClose = static_cast<TextBMFont*>(btnClose->getChildByName("bmfClose"));
    bmfClose->setString(ResourceManager::getInstance()->getTextByName(kTEXT_CLOSE));
    TextBMFont* bmfTitle = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfTitle"));
    bmfTitle->setString(ResourceManager::getInstance()->getTextByName(kTEXT_ALERT_TITLE));
}
/* end pragma event handlers functions      */

