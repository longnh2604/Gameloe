#include "SSUserInfo.h"
#include "GlobalVariable.h"
#include "ImageLoader.h"
#include "PlayerInfoPopup.h"

SSUserInfo::SSUserInfo()
{
	BaseCom();
    BaseCom::create("SusunUserInfoOtherCom.json");
    Layout* pnlAvatar = static_cast<Layout*>(Helper::seekWidgetByName(m_pLayout, "pnlAvatar"));
    pnlAvatar->addTouchEventListener(CC_CALLBACK_2(SSUserInfo::onPnlAvatarClick, this));
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(UserInfoOther::updateUserInfo), C_UPDATE_INFO_USER, nullptr);
}

void SSUserInfo::onPnlAvatarClick(Ref *pSender, Widget::TouchEventType type)
{
    if (Widget::TouchEventType::ENDED != type) return;
    GlobalVariable::m_bMe = false;
    SFSConnection::getInstance()->getPlayerInfo(m_iSFSId);
}

SSUserInfo::~SSUserInfo()
{
	
}

SSUserInfo* SSUserInfo::create()
{
	SSUserInfo* userInfo = new SSUserInfo();
	userInfo->autorelease();
	return userInfo;
}

void SSUserInfo::fillDataUserInfo(const PlayerData* playerData)
{
    m_dUserId = playerData->m_dId;
    m_iSFSId = playerData->m_iSfsId;
	TextField* txtUserId = static_cast<TextField*>(Helper::seekWidgetByName(m_pLayout, "txtUserId"));
	txtUserId->setString(ccsf("%f", playerData->m_dId));
    txtUserId->setVisible(false);
	TextBMFont* bmfPlayerName = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfPlayerName"));
    if(playerData->m_sName.length() > 12)
    {
        bmfPlayerName->setString(ccsf("%s..", playerData->m_sName.substr(0, 11).c_str()));
    }
    else
    {
        bmfPlayerName->setString(playerData->m_sName.c_str());
    }

	TextBMFont* bmfGold = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfGold"));
	bmfGold->setString(TTutil::addDot(ccsf("%1.0f", playerData->m_dMoney)));
	m_imgBtBg = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgBtnBg"));
    m_imgBtBg->setVisible(false);
    m_bmfState = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfSortState"));
    m_bmfState->setVisible(false);
	ImageView* imgExp = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgExp"));
    imgExp->setVisible(false);
    ImageView* imgVipIcon = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgVipIcon"));
    imgVipIcon->setVisible(false);
    ImageView* imgHost = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgHost"));
    if(playerData->m_bHost)
    {
        imgHost->setVisible(true);
    }
    else
    {
        imgHost->setVisible(false);
    }

    Button* btnKick = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnKick"));
    btnKick->addTouchEventListener(CC_CALLBACK_2(SSUserInfo::onBtnKickClick, this));

	ImageView* imgAvatar = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgAvatar"));
	ImageLoader* imgAva = ImageLoader::create();
	imgAvatar->addChild(imgAva);
	imgAva->setMaxSize(Size(58, 58));
	imgAva->setPosition(Vec2(imgAvatar->getContentSize().width / 2, ((imgAvatar->getContentSize().height) / 2) + 6));
	std::string sImgPath("http://ava.gameloe.com/");
	sImgPath.append(ccsf("%f", playerData->m_dId));
	sImgPath.append(".game.ava");
	imgAva->loadImage(sImgPath.c_str());

    m_pnlCards = static_cast<Layout*>(Helper::seekWidgetByName(m_pLayout, "pnlCards"));

    ImageView* imgDevice = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgDevice"));
    if(playerData->m_iPlatform == 1)
    {
        imgDevice->loadTexture("android.png");
    }
    else if (playerData->m_iPlatform == 2)
    {
        imgDevice->loadTexture("apple.png");
    }
    else
    {
        imgDevice->loadTexture("PC.png");
    }
}

void SSUserInfo::updateMoney(double p_dMoney)
{
	TextBMFont* bmfGold = static_cast<TextBMFont*>(Helper::seekWidgetByName(m_pLayout, "bmfGold"));
	bmfGold->setString(TTutil::addDot(ccsf("%1.0f", p_dMoney)));
}

void SSUserInfo::visibleBGState(bool bVisible)
{
    m_bmfState->setVisible(bVisible);
}

void SSUserInfo::setSortState(bool bSortState)
{
    m_bmfState->setVisible(true);
    m_imgBtBg->setVisible(true);
    m_pnlCards->removeAllChildren();
    spine::SkeletonAnimation* animSwap = nullptr;
	if (bSortState)
	{
        m_bmfState->setString(ResourceManager::getInstance()->getTextByName(kTEXT_FINISH));
        animSwap = spine::SkeletonAnimation::createWithData(ResourceManager::getInstance()->m_aSkeletonRendererCache[kAnimCardSwap]->getSkeleton()->data);
        animSwap->setMix("deck0", "deck1", 1.0f);
        animSwap->addAnimation(0, "deck0", true);
	}
	else
	{
        m_bmfState->setString(ResourceManager::getInstance()->getTextByName(kTEXT_ARRANGING));
        animSwap = spine::SkeletonAnimation::createWithData(ResourceManager::getInstance()->m_aSkeletonRendererCache[kAnimCardSwap]->getSkeleton()->data);
        animSwap->setMix("deck1", "deck1", 1.0f);
        animSwap->addAnimation(0, "deck1", true);
	}
    m_pnlCards->addChild(animSwap);
    animSwap->setScale(0.8f);
    animSwap->setPosition(Vec2(70, -10));
    animSwap->setOpacity(0);
    auto fadein = FadeTo::create(0.3f, 255);
    animSwap->runAction(fadein);
}

void SSUserInfo::visibleSortStateBg(bool bVisible)
{
    m_bmfState->setVisible(bVisible);
    m_imgBtBg->setVisible(bVisible);
    m_pnlCards->removeAllChildren();
}

void SSUserInfo::setBgAlign(bool isAlignRight)
{
    if(isAlignRight)
    {
        m_bmfState->setPosition(Vec2(m_bmfState->getPositionX() - 220, m_bmfState->getPositionY()));
        m_imgBtBg->setPosition(Vec2(m_imgBtBg->getPositionX() - 220, m_imgBtBg->getPositionY()));
        m_pnlCards->setPosition(Vec2(m_pnlCards->getPositionX() - 220, m_imgBtBg->getPositionY()));
    }
}

void SSUserInfo::onBtnKickClick(Ref *pSender, Widget::TouchEventType type)
{
    //CCLOG("onBtnKickClick");
    if (Widget::TouchEventType::ENDED != type) return;
	BaseScene* scene = static_cast<BaseScene*>(Director::getInstance()->getRunningScene());
    ConfirmPopup* cfm = new ConfirmPopup(ResourceManager::getInstance()->getTextByName(kTEXT_MSG_KICK_USER));
    scene->openPopup(cfm);
    cfm->getSpecialEventDispatcher()->addCustomEventListener(ConfirmPopup::ON_CONFIRM, CC_CALLBACK_1(SSUserInfo::confirmKick, this));
}

void SSUserInfo::confirmKick(EventCustom* event)
{
    SFSConnection::getInstance()->kickUser(m_iSFSId, 0);
}

void SSUserInfo::setVisibleKickBtn(bool bValue)
{
    Button* btnKick = static_cast<Button*>(Helper::seekWidgetByName(m_pLayout, "btnKick"));
    btnKick->setVisible(bValue);
}

void SSUserInfo::setVisibleHost(bool bValue)
{
    ImageView* imgHost = static_cast<ImageView*>(Helper::seekWidgetByName(m_pLayout, "imgHost"));
    imgHost->setVisible(bValue);
}

void SSUserInfo::updateUserInfo(Ref* obj)
{
	if (!getParent()) return;
    PlayerData* player = (PlayerData*)obj;
    if(player == nullptr) return;
    if(player->m_dId == m_dUserId)
    {
        fillDataUserInfo(player);
    }
}