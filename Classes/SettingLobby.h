//
//  SettingLobby.h
//  IndoGame
//
//  Created by longnh on 9/23/14.
//
//

#ifndef __IndoGame__SettingLobby__
#define __IndoGame__SettingLobby__

#include "BasePopup.h"
#include "cocos2d.h"
#include "Constant.h"
#include "GlobalVariable.h"

using namespace cocos2d;
using namespace ui;

class SettingLobby : public BasePopup
{
public:
	SettingLobby();
	~SettingLobby();

	void onOpen();
	void chkBaSelectedEvent(Ref* pSender, CheckBox::EventType type);
	void chkEnSelectedEvent(Ref* pSender, CheckBox::EventType type);
    void onChkInviteOnSelected(Ref* pSender, CheckBox::EventType type);
    void onChkInviteOffSelected(Ref* pSender, CheckBox::EventType type);
	void closePopup(Ref *pSender, Widget::TouchEventType type);
	void onClose();
    void setTextLanguage();
    void changeLanguage(Ref* obj);
	Button* m_pBtnOk;
    Button* m_pBtnCancel;
	CheckBox* m_pChkBa;
	CheckBox* m_pChkEn;
    CheckBox* m_pChkInviteOn;
	CheckBox* m_pChkInviteOff;

    int m_iLanguage;
    int m_bInviteOn;
};

#endif
