//
//  SettingCom.cpp
//  IndoGame
//
//  Created by longnh on 8/14/14.
//
//

#ifndef IndoGame_SettingCom_h
#define IndoGame_SettingCom_h

#include <iostream>
#include "GlobalVariable.h"
#include "ImageLoader.h"
#include "BaseCom.h"
#include "BaseScene.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "Constant.h"
#include "AudioManager.h"
#include "ChatPopup.h"
#include "SettingRoomPopup.h"
#include "Constant.h"
#include "SettingLobby.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_UI;

class SettingCom : public BaseCom
{
public:
	SettingCom();
	~SettingCom();

	void onBtnSettingsClick(Ref *pSender, Widget::TouchEventType type);
	void onBtnRankingClick(Ref *pSender, Widget::TouchEventType type);
	void onBtnMoneyClick(Ref *pSender, Widget::TouchEventType type);
	void addEventlistener();
	void hideSettings(Node* node);
	void onMuteClick(Ref *pSender, Widget::TouchEventType type);
	void onUnmuteClick(Ref *pSender, Widget::TouchEventType type);
	void onCaptureClick(Ref *pSender, Widget::TouchEventType type);
	void onSettingInClick(Ref *pSender, Widget::TouchEventType type);
	void hideSlideButtons();
	void onBtnToggleMuteClick(Ref* pSender, Widget::TouchEventType type);
    void onBtnChatClick(Ref *pSender, Widget::TouchEventType type);
    void onBtnHistory(Ref *pSender, Widget::TouchEventType type);
    void onTouchBtnFanPage(Ref *pSender, Widget::TouchEventType type);
    void onTouchBtnPhone(Ref *pSender, Widget::TouchEventType type);
    
    void setVisibleItems(bool p_bMoney = false, bool P_bRank = false, bool p_bChat = false, bool p_bHistory = false, bool p_bLogout = false, bool p_bUnipin = false, bool p_bFanPage = false, bool p_bPhone = false, bool p_bMimoPay = false);
    void setVisiblePanelIAP(bool bVisible = false);
    void onBtnLogout(Ref *pSender, Widget::TouchEventType type);
    void onBtnIAPClick(Ref *pSender, Widget::TouchEventType type);
    void onBtnUnipinClick(Ref *pSender, Widget::TouchEventType type);
    
    bool onWebViewShouldStartLoading(experimental::ui::WebView *sender, const std::string &url);
    void onWebViewDidFinishLoading(experimental::ui::WebView *sender, const std::string &url);
    void onWebViewDidFailLoading(experimental::ui::WebView *sender, const std::string &url);

	Button* m_p_bBtnSettings;
	Button* m_p_bBtnRanking;
    Button* m_p_bBtnLogout;
	Button* m_p_bBtnMoney;
    Button* m_p_bBtnIAP;
    Button* m_p_bBtnFanPage;
    Button* m_p_bBtnPhone;
    Text*   m_p_lblPhone;
    Button* m_p_bBtnMimoPay;
    Button* m_p_bBtnUnipin;
	Button* m_p_bBtnCapture;
	Button* m_p_bBtnMute;
	Button* m_p_bBtnUnmute;
	Button* m_p_bBtnSettingsIn;
    Button* m_p_bBtnHistory;
    Button* m_p_bBtnChat;
	Layout* m_p_lSettingBtnsDropDown;
    Layout* m_pnlIAP;
	ImageView* m_p_iImageSlide;


	bool m_bIsSettingsVisible = false;
	float m_fBtnCapture_X;
	float m_fBtnCapture_Y;
	float m_fBtnMute_X;
	float m_fBtnMute_Y;
	float m_fBtnSettingsIn_X;
	float m_fBtnSettingsIn_Y;
    float m_fBtnHistory_X;
	float m_fBtnHistory_Y;
	float m_fDurationTrans;
    Vec2  m_posSetting;
    Vec2  m_posBtnCapture;
    Vec2  m_posBtnMute;
    Vec2  m_posBtnSettingIn;
    Vec2  m_posBtnUnmute;
    Vec2  m_posBtnHistory;
};

#endif /* defined(IndoGame_SettingCom_h) */
