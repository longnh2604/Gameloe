//
//  SettingLobby.cpp
//  IndoGame
//
//  Created by longnh on 9/23/14.
//
//

#include "SettingLobby.h"


SettingLobby::SettingLobby()
{
    m_bInviteOn = false;
    m_iLanguage = 0;
}


SettingLobby::~SettingLobby()
{
}

void SettingLobby::onOpen()
{
    BasePopup::onOpen();
    m_pLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(kSettingLobbyPopup));
	// Re-layout popup
	Size layoutSize = m_pLayout->getContentSize();
	Size deviceSize = Director::getInstance()->getWinSize();
	m_pLayout->setAnchorPoint(Vec2(0.5, 0.5));
	m_pLayout->setPosition(Vec2(deviceSize.width / 2, deviceSize.height / 2));
	this->addChild(m_pLayout);

	m_pBtnOk = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnOk"));
	m_pBtnOk->addTouchEventListener(CC_CALLBACK_2(SettingLobby::closePopup, this));
    m_pBtnCancel = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnCancel"));
    m_pBtnCancel->addTouchEventListener(CC_CALLBACK_2(BasePopup::onTouchOverlay, this));
	m_pChkBa = static_cast<CheckBox*>(Helper::seekWidgetByName(m_pLayout, "chk1"));
	m_pChkEn = static_cast<CheckBox*>(Helper::seekWidgetByName(m_pLayout, "chk2"));
	m_pChkBa->addEventListener(CC_CALLBACK_2(SettingLobby::chkBaSelectedEvent, this));
	m_pChkEn->addEventListener(CC_CALLBACK_2(SettingLobby::chkEnSelectedEvent, this));

    m_pChkInviteOn = static_cast<CheckBox*>(Helper::seekWidgetByName(m_pLayout, "chkInviteOn"));
	m_pChkInviteOff = static_cast<CheckBox*>(Helper::seekWidgetByName(m_pLayout, "chkInviteOff"));
	m_pChkInviteOn->addEventListener(CC_CALLBACK_2(SettingLobby::onChkInviteOnSelected, this));
	m_pChkInviteOff->addEventListener(CC_CALLBACK_2(SettingLobby::onChkInviteOffSelected, this));

	m_pLayout->addTouchEventListener(CC_CALLBACK_2(BasePopup::onTouchOverlay, this));
    int language = UserDefault::getInstance()->getIntegerForKey("language");
    GlobalVariable::m_iCurrentLanguage = language;
	switch (language)
	{
	case LANG_ENGLISH:
		m_pChkEn->setSelected(true);
		break;
	case LANG_INDONESIA:
		m_pChkBa->setSelected(true);
		break;
	default:
		break;
	}
    bool invite = UserDefault::getInstance()->getBoolForKey("invite");
    if(invite)
    {
        m_pChkInviteOn->setSelected(true);
    }
    else
    {
        m_pChkInviteOff->setSelected(true);
    }

    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(SettingLobby::changeLanguage), EVENT_LANGUAGE_CHANGE, nullptr);
    setTextLanguage();
    setLightPosition();
}

void SettingLobby::onClose()
{
	BasePopup::onClose();
}

void SettingLobby::closePopup(Ref *pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED)
	{
        GlobalVariable::m_bInviteOn = m_bInviteOn;
        GlobalVariable::m_iCurrentLanguage = m_iLanguage;
		BaseScene* scene = static_cast<BaseScene*>(Director::getInstance()->getRunningScene());
		scene->closeLastPopup();
	}
}

void SettingLobby::chkBaSelectedEvent(Ref* pSender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
        m_iLanguage = LANG_INDONESIA;
        UserDefault::getInstance()->setIntegerForKey("language", LANG_INDONESIA);
        GlobalVariable::m_iCurrentLanguage = m_iLanguage;
		m_pChkEn->setSelected(false);
		NotificationCenter::getInstance()->postNotification(EVENT_LANGUAGE_CHANGE, NULL);
	}
}

void SettingLobby::chkEnSelectedEvent(Ref* pSender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
        UserDefault::getInstance()->setIntegerForKey("language", LANG_ENGLISH);
        m_iLanguage = LANG_ENGLISH;
        GlobalVariable::m_iCurrentLanguage = m_iLanguage;
		m_pChkBa->setSelected(false);
		NotificationCenter::getInstance()->postNotification(EVENT_LANGUAGE_CHANGE, NULL);
	}
}

void SettingLobby::onChkInviteOnSelected(Ref* pSender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		m_pChkInviteOff->setSelected(false);
        UserDefault::getInstance()->setBoolForKey("invite", true);
        m_bInviteOn = true;
	}
}

void SettingLobby::onChkInviteOffSelected(Ref* pSender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		m_pChkInviteOn->setSelected(false);
        UserDefault::getInstance()->setBoolForKey("invite", false);
        m_bInviteOn = false;
	}
}


void SettingLobby::changeLanguage(Ref* obj)
{
    setTextLanguage();
}

void SettingLobby::setTextLanguage()
{
    auto btnOK = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnOk"));
    TextBMFont* bmfOk = static_cast<TextBMFont*>(btnOK->getChildByName("bmOK"));
    bmfOk->setString(ResourceManager::getInstance()->getTextByName(kTEXT_OK));
    auto btnCancel = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnCancel"));
    TextBMFont* bmfCancel = static_cast<TextBMFont*>(btnCancel->getChildByName("bmCancel"));
    bmfCancel->setString(ResourceManager::getInstance()->getTextByName(kTEXT_CANCEL));
    TextBMFont* bmTitle = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmTitle"));
    bmTitle->setString(ResourceManager::getInstance()->getTextByName(kTEXT_SETTING));
    TextBMFont* bmfBahasa = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfBahasa"));
    bmfBahasa->setString(ResourceManager::getInstance()->getTextByName(kTEXT_INDONESIAN));
    TextBMFont* bmfEnglish = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfEnglish"));
    bmfEnglish->setString(ResourceManager::getInstance()->getTextByName(kTEXT_ENGLISH));
    TextBMFont* bmfInviteOn = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfInviteOn"));
    if(bmfInviteOn)bmfInviteOn->setString(ResourceManager::getInstance()->getTextByName(kTEXT_INVITE_ON).c_str());
    TextBMFont* bmfInviteOff = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfInviteOff"));
    if(bmfInviteOff)bmfInviteOff->setString(ResourceManager::getInstance()->getTextByName(kTEXT_INVITE_OFF).c_str());
}
