//
//  SSCard.cpp
//  IndoGame
//
//  Created by Rainy on 8/20/14.
//
//

#include "SSCard.h"

const int SSCard::RANK_NONE	    = 0;
const int SSCard::RANK_ACE      = 14;
const int SSCard::RANK_2        = 2;
const int SSCard::RANK_3        = 3;
const int SSCard::RANK_4        = 4;
const int SSCard::RANK_5        = 5;
const int SSCard::RANK_6        = 6;
const int SSCard::RANK_7        = 7;
const int SSCard::RANK_8        = 8;
const int SSCard::RANK_9        = 9;
const int SSCard::RANK_10       = 10;
const int SSCard::RANK_JACK     = 11;
const int SSCard::RANK_QUEEN    = 12;
const int SSCard::RANK_KING     = 13;
	  
const int SSCard::SUIT_DIAMOND  = 4;
const int SSCard::SUIT_CLUB     = 3;
const int SSCard::SUIT_HEART    = 2;
const int SSCard::SUIT_SPADE    = 1;

const int SSCard::SIZE_MEDIUM   = 1;
const int SSCard::SIZE_SMALL    = 2;

// const std::string kResourceString[] = {
//     "bich_2.png", "co_2.png", "nhep_2.png", "ro_2.png", 
//     "bich_3.png","co_3.png","nhep_3.png", "ro_3.png",  
//     "bich_4.png", "co_4.png", "nhep_4.png","ro_4.png", 
//     "bich_5.png","co_5.png", "nhep_5.png", "ro_5.png",  
//     "bich_6.png", "co_6.png", "nhep_6.png", "ro_6.png", 
//     "ro_7.png" ,"nhep_7.png", "co_7.png", "bich_7.png", 
//     "ro_8.png", "nhep_8.png", "co_8.png", "bich_8.png", 
//     "ro_9.png", "nhep_9.png", "co_9.png", "bich_9.png", 
//     "ro_10.png", "nhep_10.png", "co_10.png", "bich_10.png", 
//     "ro_J.png", "nhep_J.png", "co_J.png", "bich_J.png", 
//     "ro_Q.png", "nhep_Q.png",  "co_Q.png", "bich_Q.png", 
//     "ro_K.png", "nhep_K.png", "co_K.png", "bich_K.png", 
//     "ro_A.png", "nhep_A.png", "co_A.png", "bich_A.png"
// };
const std::string kResourceString[] = {
    "poker_4.png",  "poker_7.png",  "poker_5.png",  "poker_6.png",
    "poker_8.png",  "poker_11.png", "poker_9.png", "poker_10.png",
    "poker_12.png", "poker_15.png", "poker_13.png", "poker_14.png",
    "poker_16.png", "poker_19.png", "poker_17.png", "poker_18.png",
    "poker_20.png", "poker_23.png", "poker_21.png", "poker_22.png",
    "poker_24.png" ,"poker_27.png", "poker_25.png", "poker_26.png",
    "poker_28.png", "poker_31.png", "poker_29.png", "poker_30.png",
    "poker_32.png", "poker_35.png", "poker_33.png", "poker_34.png",
    "poker_36.png", "poker_39.png", "poker_37.png", "poker_38.png",
    "poker_40.png", "poker_43.png", "poker_41.png", "poker_42.png",
    "poker_44.png", "poker_47.png", "poker_45.png", "poker_46.png",
    "poker_48.png", "poker_51.png", "poker_49.png", "poker_50.png",
    "poker_0.png",  "poker_3.png",  "poker_1.png",  "poker_2.png",
};

const std::string kSmallResourceString[] = {
    "small_poker_4.png",  "small_poker_7.png",  "small_poker_5.png",  "small_poker_6.png",
    "small_poker_8.png",  "small_poker_11.png", "small_poker_9.png",  "small_poker_10.png",
    "small_poker_12.png", "small_poker_15.png", "small_poker_13.png", "small_poker_14.png",
    "small_poker_16.png", "small_poker_19.png", "small_poker_17.png", "small_poker_18.png",
    "small_poker_20.png", "small_poker_23.png", "small_poker_21.png", "small_poker_22.png",
    "small_poker_24.png" ,"small_poker_27.png", "small_poker_25.png", "small_poker_26.png",
    "small_poker_28.png", "small_poker_31.png", "small_poker_29.png", "small_poker_30.png",
    "small_poker_32.png", "small_poker_35.png", "small_poker_33.png", "small_poker_34.png",
    "small_poker_36.png", "small_poker_39.png", "small_poker_37.png", "small_poker_38.png",
    "small_poker_40.png", "small_poker_43.png", "small_poker_41.png", "small_poker_42.png",
    "small_poker_44.png", "small_poker_47.png", "small_poker_45.png", "small_poker_46.png",
    "small_poker_48.png", "small_poker_51.png", "small_poker_49.png", "small_poker_50.png",
    "small_poker_0.png",  "small_poker_3.png",  "small_poker_1.png",  "small_poker_2.png",

};

SSCard::SSCard(void)
{
    m_iState = kCardStateGrabbed;
    m_iId    = 0;
    m_iTouchPaddingLeft = 0;
    m_iTouchPaddingTop  = 0;
	m_iSize = SIZE_MEDIUM;
	m_posOrigin = Vec2(0.0f, 0.0f);
}

SSCard::~SSCard(void)
{
	_eventDispatcher->resumeEventListenersForTarget (this);
}

SSCard* SSCard::create()
{
    SSCard* card = new SSCard();
    if (card)
    {
        card->autorelease();
        return card;
    }
    CC_SAFE_DELETE (card);
    return nullptr;
}

SSCard* SSCard::create(int cardId)
{
    SSCard* card = new SSCard();
    if (card)
    {
        card->init (cardId);
        card->autorelease();
        return card;
    }
    CC_SAFE_DELETE (card);
    return nullptr;
}

void SSCard::init(int p_iResourceId)
{
    m_iId = p_iResourceId;
	//m_iRank = CardHelper::getRank(m_iId);
	//m_iSuit = CardHelper::getSuit(m_iId);
	Texture2D* texture = nullptr;
	if (p_iResourceId == -1) texture = Director::getInstance()->getTextureCache()->addImage("poker_52.png");
	else texture = Director::getInstance()->getTextureCache()->addImage(kResourceString[p_iResourceId]);
	initWithTexture(texture);
}
cocos2d::Rect SSCard::getRect()
{
    auto s = getTexture()->getContentSize();
    return Rect(-s.width / 2, -s.height / 2, s.width, s.height);
}

SSCard* SSCard::createWithTexture(Texture2D* aTexture)
{
    SSCard* pCard = new SSCard();
    aTexture->setAntiAliasTexParameters();
    pCard->initWithTexture(aTexture);
    pCard->autorelease();

    return pCard;
}

bool SSCard::initWithTexture(Texture2D* aTexture)
{
    if( Sprite::initWithTexture(aTexture) )
    {
        m_iState = kCardStateUngrabbed;
    }

    return true;
}

void SSCard::onEnter()
{
    Sprite::onEnter();

    // Register Touch Event
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(SSCard::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SSCard::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SSCard::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void SSCard::onExit()
{
    Sprite::onExit();
}

bool SSCard::containsTouchLocation(Touch* touch)
{
    return getRect().containsPoint(convertTouchToNodeSpaceAR(touch));
}

bool SSCard::onTouchBegan(Touch* touch, Event* event)
{
    //if (m_iState != kCardStateUngrabbed) return false;
    if ( !containsTouchLocation(touch) ) return false;

    Vec2 touchPoint = touch->getLocation();
    m_iTouchPaddingLeft = touchPoint.x - getPosition().x;
    m_iTouchPaddingTop  = touchPoint.y - getPosition().y;
    if(m_iState == kCardStateSelected)
    {
		setState(kCardStateUngrabbed);
    }
	else if (m_iState == kCardStateUngrabbed)
    {
		setState(kCardStateSelected);
		NotificationCenter::getInstance()->postNotification("select SSCard", (Ref*)this);
    }
	
    return true;
}

void SSCard::onTouchMoved(Touch* touch, Event* event)
{
    return;
    auto touchPoint = touch->getLocation();
    setPosition( Vec2(touchPoint.x - m_iTouchPaddingLeft, touchPoint.y - m_iTouchPaddingTop));
}

SSCard* SSCard::clone() const
{
    SSCard* ret = SSCard::createWithTexture(_texture);
    ret->m_iState = m_iState;
    ret->m_iId    = m_iId;
    ret->setPosition(getPosition());
    ret->setAnchorPoint(getAnchorPoint());
    return ret;
}

void SSCard::setState(CardState iState)
{
	m_iState = iState;
	if (m_iState == kCardStateSelected)
	{
		if (ceil(m_posOrigin.x) != 0 && ceil(m_posOrigin.y) != 0) setPosition(Vec2(m_posOrigin.x, m_posOrigin.y + 20.0f));
		else setPosition(Vec2(getPosition().x, getPosition().y + 20));
	}
	else if (m_iState == kCardStateUngrabbed)
	{
		if (ceil(m_posOrigin.x) != 0 && ceil(m_posOrigin.y) != 0) setPosition(Vec2(m_posOrigin.x, m_posOrigin.y));
		else setPosition(Vec2(getPosition().x, getPosition().y));
	}
}

void SSCard::onTouchEnded(Touch* touch, Event* event)
{
    //m_iState = kCardStateUngrabbed;
} 

int SSCard::getRank()
{
	return CardSusunHelper::getRank(m_iId);
}

int SSCard::getSuit()
{
	return CardSusunHelper::getSuit(m_iId);
}

void SSCard::setId(const int id)
{
    m_iId = id;
	if (SIZE_MEDIUM == m_iSize) {
		if (m_iId == -1) setTexture( Director::getInstance()->getTextureCache()->addImage("poker_52.png"));
		else setTexture( Director::getInstance()->getTextureCache()->addImage(kResourceString[m_iId]));
	}
	else if (SIZE_SMALL == m_iSize) {
		if (m_iId == -1) setTexture(Director::getInstance()->getTextureCache()->addImage("small_poker_52.png"));
		else setTexture(Director::getInstance()->getTextureCache()->addImage(kSmallResourceString[m_iId]));
	}
}

void SSCard::setCardSize(int size)
{
	if (m_iSize == size) return;
	m_iSize = size;
	if (SIZE_MEDIUM == m_iSize) {
		if (m_iId == -1) setTexture(Director::getInstance()->getTextureCache()->addImage("poker_52.png"));
		else setTexture(Director::getInstance()->getTextureCache()->addImage(kResourceString[m_iId]));
	}
	else if (SIZE_SMALL == m_iSize) {
		if (m_iId == -1) setTexture(Director::getInstance()->getTextureCache()->addImage("small_poker_52.png"));
		else setTexture(Director::getInstance()->getTextureCache()->addImage(kSmallResourceString[m_iId]));
	}
}