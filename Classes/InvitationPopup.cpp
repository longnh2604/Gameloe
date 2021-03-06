//
//  InvitationPopup.cpp
//  IndoGame
//
//  Created by Rainy on 9/30/14.
//
//

#include "InvitationPopup.h"
#include "SFSConnection.h"
std::string InvitationPopup::ON_CONFIRM = "confirm";
std::string InvitationPopup::ON_CANCEL = "cancel";


/*************************************************************/
/* @author longnh												 */
/* Constructor/Destructor functions							 */
/*************************************************************/
InvitationPopup::InvitationPopup()
{
}
InvitationPopup::InvitationPopup(vector<CMDWaitingData*>* vWaitingData)
{
	m_vWaitingDatas = vWaitingData;
}

InvitationPopup::~InvitationPopup()
{
    m_vWaitingDatas->clear();
    CC_SAFE_DELETE(m_vWaitingDatas);
}

InvitationPopup* InvitationPopup::create()
{
	InvitationPopup* popup = new InvitationPopup();
	popup->autorelease();
	return popup;
}
InvitationPopup* InvitationPopup::create(vector<CMDWaitingData*>* vWaitingData)
{
	InvitationPopup* popup = new InvitationPopup(vWaitingData);
	popup->autorelease();
	return popup;
}
/* END CONSTRUCTOR/DESTRUCTOR FUNCTIONS						 */


/*************************************************************/
/* @author longnh												 */
/* Start pragma onOpen/onClose functions					 */
/*************************************************************/
void InvitationPopup::onOpen()
{
	BasePopup::onOpen();
	Size deviceSize = Director::getInstance()->getWinSize();
	m_pLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("InvitationPopup.json"));
	m_pLayout->setAnchorPoint(Vec2(0.5f, 0.5f));
	m_pLayout->setPosition(Vec2(deviceSize.width/2, deviceSize.height/2));
	this->addChild(m_pLayout);

    btnCancel = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnCancel"));
	btnCancel->addTouchEventListener(CC_CALLBACK_2(InvitationPopup::onTouchCancel, this));
	btnConfirm = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnConfirm"));
	btnConfirm->addTouchEventListener(CC_CALLBACK_2(InvitationPopup::onTouchConfirm, this));

    lvInvitations = static_cast<ListView*>(Helper::seekWidgetByName(m_pLayout, "lvInvitations"));
    lvInvitations->setClippingType(LAYOUT_CLIPPING_SCISSOR);
    lvInvitations->setInertiaScrollEnabled(true);
    Layout* itemAnswerModel = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("InvitationItem.json"));
    lvInvitations->setItemModel(itemAnswerModel);
    lvInvitations->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(InvitationPopup::selectedRowEvent, this));
    lvInvitations->removeAllChildren();
    CCLOG("m_vWaitingDatas->size() %lu", m_vWaitingDatas->size());
    for (int i = 0; i < m_vWaitingDatas->size(); i++)
	{
		lvInvitations->pushBackDefaultItem();
	}

	for (int i = 0; i < m_vWaitingDatas->size(); ++i)
	{
        Widget* item = lvInvitations->getItem(i);
        Text* lblUsername = static_cast<Text*>(item->getChildByName("lblUserName"));

        if(m_vWaitingDatas->at(i)->m_sName.length() > 16)
        {
            lblUsername->setString(ccsf("%s..", m_vWaitingDatas->at(i)->m_sName.substr(0, 13).c_str()));
        }
        else
        {
            lblUsername->setString(m_vWaitingDatas->at(i)->m_sName);
        }
        Text* lblMoney = static_cast<Text*>(item->getChildByName("lblMoney"));
        lblMoney->setString(TTutil::addDot(ccsf("%1.0f", m_vWaitingDatas->at(i)->m_dMoney)));
        Text* lblUID = static_cast<Text*>(Helper::seekWidgetByName(item, "lblUID"));
        lblUID->setString(ccsf("%1.0ld", m_vWaitingDatas->at(i)->m_iSfsId));
        lblUID->setVisible(false);
	}
    m_iNOUserWaiting = m_vWaitingDatas->size();
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(SettingLobby::changeLanguage), EVENT_LANGUAGE_CHANGE, nullptr);
    setTextLanguage();
	//fadeInContent();
    setLightPosition();
}

void InvitationPopup::selectedRowEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
        case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
        {
            //AudioManager::getInstance()->playEffect(kSoundEffectButtonClick);
            break;
        }
        case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
        {
			Layout* item = dynamic_cast<Layout*>(lvInvitations->getItem(lvInvitations->getCurSelectedIndex()));
            CheckBox* chkCheck = static_cast<CheckBox*>(item->getChildByName("chkCheck"));
            if(chkCheck->isSelected())
            {
                chkCheck->setSelected(false);
            }
            else
            {
                chkCheck->setSelected(true);
            }
            break;
        }
	}
}


void InvitationPopup::onClose()
{
	BasePopup::onClose();
}
/* END PRAGMA ONOPEN/ONCLOSE FUNCTIONS						 */


/*************************************************************/
/* @author longnh												 */
/* Start pragma event handlers functions					 */
/*************************************************************/
void InvitationPopup::onTouchCancel(Ref* pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED) return;

	getSpecialEventDispatcher()->dispatchCustomEvent(InvitationPopup::ON_CANCEL);

	BaseScene* scene = static_cast<BaseScene*>(Director::getInstance()->getRunningScene());
	scene->closePopup(this);
}

void InvitationPopup::onTouchConfirm(Ref* pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED) return;

	getSpecialEventDispatcher()->dispatchCustomEvent(InvitationPopup::ON_CONFIRM);

	BaseScene* scene = static_cast<BaseScene*>(Director::getInstance()->getRunningScene());

    for (int i = 0; i < m_iNOUserWaiting; ++i)
    {
        Layout* item = dynamic_cast<Layout*>(lvInvitations->getItem(i));
        CheckBox* chkCheck = static_cast<CheckBox*>(item->getChildByName("chkCheck"));
        Text* lblUID = static_cast<Text*>(Helper::seekWidgetByName(item, "lblUID"));
        int sfsId = atoi(lblUID->getString().c_str());
        if(chkCheck->isSelected())
        {
            SFSConnection::getInstance()->invite(sfsId);
        }
    }
    scene->closePopup(this);
}

void InvitationPopup::changeLanguage(Ref* obj)
{
    setTextLanguage();
}

void InvitationPopup::setTextLanguage()
{
    TextBMFont* bmTitle = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmTitle"));
    bmTitle->setString(ResourceManager::getInstance()->getTextByName(kTEXT_INVITATIONS));
    TextBMFont* bmOK = static_cast<TextBMFont*>(btnConfirm->getChildByName("bmOK"));
    bmOK->setString(ResourceManager::getInstance()->getTextByName(kTEXT_OK));
    TextBMFont* bmCancel = static_cast<TextBMFont*>(btnCancel->getChildByName("bmCancel"));
    bmCancel->setString(ResourceManager::getInstance()->getTextByName(kTEXT_CANCEL));
}
