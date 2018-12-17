//
//  CapsaSusunScene.h
//  IndoGame
//
//  Created by Rainy on 8/1/14.
//
//

#ifndef __IndoGame__CapsaSusunScene__
#define __IndoGame__CapsaSusunScene__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"

#include "BaseScene.h"
#include "CardHelper.h"
#include "UserInfo.h"
#include "UserInfoOther.h"
#include "ChatToast.h"
#include "SceneManager.h"
#include "Constant.h"
#include "CMDSusunData.h"
#include "SSCard.h"
#include "SSUserInfo.h"
#include "ChatToast.h"

USING_NS_CC;
USING_NS_UI;
USING_NS_STD;
USING_NS_CC_EXT;

class CapsaSusunScene :
	public BaseScene
{
public:
	CapsaSusunScene();
	~CapsaSusunScene();

	SSCMDGameData* m_startGameData;

	virtual void onEnter();
	virtual void onExit();
	virtual void update(float dt);
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void changeLanguage(Ref* obj);
    vector<SSUserInfo*>* m_vUserInfoOthers;
    void placePlayers();
    void addNotification();
    void onGameStarted(Ref* obj);
    void onUserData(Ref* obj);
private:
	bool m_bJoinGame;
	static const int SORT_RANK;
	static const int SORT_SUIT;
    static const int SORT_HAND;
	void relayout();
	void verifyHand();

	void swapCard(SSCard* cardA, SSCard* cardB);
	void setUsingCards(vector<int> vCardId);

	bool onCardTouchBegan(Touch* touch, Event* event);
	void onCardTouchMoved(Touch* touch, Event* event);
	void onCardTouchEnded(Touch* touch, Event* event);
	void onTouchBtnStart(Ref* sender, Widget::TouchEventType type);
	void onTouchBtnFinish(Ref* sender, Widget::TouchEventType type);
	void onTouchBtnChange(Ref* sender, Widget::TouchEventType type);
	void onTouchBtnSort(Ref* sender, Widget::TouchEventType type);
	void onTouchBtnSwap(Ref* sender, Widget::TouchEventType type);
	void onNetworkSendPublishMsr(Ref* obj);
	void onNetworkGetPublishMsr(Ref* obj);
    void onWaitingList(Ref* obj);
    void onBtnInviteClick(Ref *pSender, Widget::TouchEventType type);
    void onTouchBtnBonus(Ref *pSender, Widget::TouchEventType type);

	void dealCards(int cards[13]);
	void onFinishDealingCards();
	void addCardsTouchHandler();
	void onPrgTimer(float dt);
	void onEndPrgTimer();
	void updateCardPos();
	void showHandWithDelay(float delayTime, int cardList[13], int iPlayerIndex, int iHandIndex);
	void showHand(int cardList[13], int iPlayerIndex, int iHandIndex);
	void showSpecialHandResult(std::string sResult, std::string sFont, int aCardId[13], int iPlayerIndex, float showingTime);
	void showHandResultWithDelay(float time, int iType, int iPlayerIndex, int iHandIndex);
	void showHandResult(int iType, int iPlayerIndex, int iHandIndex);
	void showChiResultWithDelay(float time, int iType, int iPlayerIndex, int iHandIndex);
	void showChiResult(int iType, int iPlayerIndex, int iHandIndex);
	void onOpenCard(SSCard* SSCard, int id);
    void onUserDataEnd(Ref* obj);
    void onUserDataEndView(Ref* obj);
    void onUpdateEndGame(Ref* obj);
	void showMoney(int value, int iPlayerIndex);
	void showMoneyWithDelay(float delayTime, int value, int iPlayerIndex);
	void onDeleteNode (Node* node);
	void onAddNode (Node* parent, Node* child);
	void onVisibleNode (Node* node);
	void onShowEndGameResult();
    void onFullTime(Ref* obj);
    void setTurnId(long turnId);
    void showInvite();
    void onSortFinish(Ref* obj);
    void showSettingRoom(Ref* obj);
    void onSetMinMaxStake(Ref* obj);
    void updateNewSetting(Ref* obj);
    void onGetMinMaxStake(Ref* obj);
    void onRestart(Ref* obj);
    void showItemsRoom();
    void showHostUser();
    void showKickUser(bool bShow);
    void showMessage(bool bVisible, const char* messageid = "");
    void reconnect();
    void spectator();
    void setRotationCard(SSCard* card, int index);
    
	long m_iTime;
	int m_iSortState;
	bool m_bMovingCard;
	bool m_bTouchBeganCard;
	bool m_bHasEndGameResult;
	vector<double> m_vMoneyResult;
	Vec2 m_diffPos;
	Vec2 m_startPos;

	Layout* m_pnlTimer;
    Layout* pnlInvite;
	ImageView* m_imgLight;
	ImageView* m_imgTimeBg;
	TextBMFont* m_bmfTimer;
	LoadingBar* m_prgTImer;
	
	SSCard* m_selectingCard;
	SSCard* m_targetCard;
	Sprite* m_deck;
	Button* m_btnFinish;
	Button* m_btnChange;
	Button* m_btnStart;
	Button* m_btnSort;
	Button* m_btnSwap;
    Button* m_btnBonus;
	Layout* m_pnlPlayerAreas[4];
    Layout* m_pnlUserInfo;
    Layout* m_pnlSetting;
    Button* m_btnHand;
    Button* m_btnHandActivate;
    Button* m_btnDeck;
    Button* m_btnDeckActivate;

    TextBMFont* m_titleBtnHand;
    TextBMFont* m_titleBtnHandActivate;
    TextBMFont* m_titleBtnDeck;
    TextBMFont* m_titleBtnDeckActivate;
    TextBMFont* m_bmfSortDone;

    ImageView* m_gamebarContainer;
    TextBMFont* m_bmRoomNo;

    TextBMFont* m_bmfTime;
    TextBMFont* m_bmfBet;
    TextBMFont* m_bmfCountDown;
    Layout*     m_pnlCountDown;
	vector<SSCard*> m_vCard;
	vector<vector<Vec2>> m_v_vCardPos;
	vector<vector<Vec2>> m_v_vCardAnimPos;
	vector<Vec2> m_vBottomCardPos;
	vector<vector<SSCard*>> m_v_vCard;
	vector<Vec2> m_vChatToastPosition;
    bool m_bFinishSended = false;

    float m_fCountTimeLeft;
    std::string m_sCountDown;
    int m_iLastCount;
    int m_iCurrentCount;
    void onCountDownTimer(float dt);

    CMDMinMaxStake* m_pDataMinMaxStake;

    void onCmdSpectator(Ref* obj);
	void disableSort();
	void enableSort();
	bool m_bCanTouchBtnSort;
	bool m_bCansort;
	bool m_bCanKick;
	TextBMFont* m_tfMauBinh;
};

#endif /* defined(__IndoGame__CapsaSusunScene__) */
