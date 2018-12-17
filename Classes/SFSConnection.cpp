//
//  SFSConnection.cpp
//  IndoGame
//
//  Created by Rainy on 8/1/14.
//
//

#include "SFSConnection.h"
#include "Bitswarm/BitSwarmEvent.h"
#include "Util/EventDispatcher.h"
#include "Requests/LoginRequest.h"
#include "Requests/LogoutRequest.h"
#include "Requests/JoinRoomRequest.h"
#include "Requests/CreateRoomRequest.h"
#include "Requests/RoomSettings.h"
#include "Requests/PublicMessageRequest.h"
#include "Requests/ExtensionRequest.h"
#include "Requests/SetUserVariablesRequest.h"
#include "Entities/Variables/SFSUserVariable.h"
#include "Entities/SFSUser.h"

#include "cocos2d.h"
#include "GlobalVariable.h"
#include "EventConst.h"
#include "PlayerData.h"
#include "ZoneData.h"
#include "SceneManager.h"
#include "LoginHandler.h"
#include "TableData.h"
#include "SFSResponse.h"

USING_NS_CC;

SFSConnection::SFSConnection()
{
	mSmartFox   = boost::shared_ptr<Sfs2X::SmartFox>();
	mGamingRoom = boost::shared_ptr<Room>();
    m_iPingCount = 0;
}

SFSConnection::~SFSConnection()
{

}

//*******************************
// star init smartfox
//*******************************
boost::shared_ptr<Sfs2X::SmartFox> SFSConnection::mSmartFox = NULL;
void SFSConnection::initServer( )
{
    CCLOG("Smartfox: Init Server");
    if(mSmartFox)
    {
        mSmartFox->RemoveAllEventListeners();
        //delete boost::shared_ptr<Sfs2X::SmartFox>( mSmartFox);
        mSmartFox = nullptr;
    }
	// Initialize Smart Fox
	mSmartFox = boost::shared_ptr<Sfs2X::SmartFox>(new Sfs2X::SmartFox(true));
	mSmartFox->ThreadSafeMode(true);
    mSmartFox->Debug(true);

	// Add event listeners
    mSmartFox->AddEventListener(SFSEvent::PING_PONG, boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::onSmartFoxPingPong, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::CONNECTION
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxConnection, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::CONNECTION_LOST
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxConnectionLost, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::LOGIN
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxLogin, (unsigned long long)this)));
    mSmartFox->AddEventListener(SFSEvent::LOGIN_ERROR
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxLoginError, (unsigned long long)this)));
	mSmartFox->AddEventListener(BitSwarmEvent::DISCONNECT
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxDisconnection, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::ROOM_JOIN
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxRoomJoin, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::ROOM_JOIN_ERROR
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxRoomJoinError, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::USER_EXIT_ROOM
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnUserExitRoom, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::EXTENSION_RESPONSE
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnExtensionResponse, (unsigned long long)this)));

	mSmartFox->AddEventListener(SFSEvent::PUBLIC_MESSAGE
                                , boost::shared_ptr<EventListenerDelegate> (new EventListenerDelegate(SFSConnection::OnSmartFoxPublicMessage, (unsigned long long)this)));
    isServerInit = true;
}

/* end init smartfox */
void SFSConnection::updateEvent()
{
	mSmartFox->ProcessEvents();
}

//*******************************
// star send request
//*******************************

//connect server
void SFSConnection::connectToServer(char const* p_sHostIp, int p_iPort )
{
    CCLOG("SFSConnection: connectToServer start");
    //NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_CONNECTING, NULL);
    CCLOG("SFSConnection: connectToServer host %s port %d", p_sHostIp, p_iPort);
	mSmartFox->Connect(p_sHostIp, p_iPort);
    CCLOG("SFSConnection: connectToServer end");
}

void SFSConnection::disconnectToServer()
{
    CCLOG("SFSConnection: disconnectToServer");
	mSmartFox->Disconnect();
}

void SFSConnection::login(std::string p_sID, std::string domain, std::string p_sZoneName)
{
    CCLOG("SFSConnection: login start");
    boost::shared_ptr<ISFSObject> parameters (new SFSObject());
	GlobalVariable::m_sSID = p_sID.c_str();
    parameters->PutUtfString("sidAuthen", GlobalVariable::m_sSID);

    CCLOG("session id: '%s'", GlobalVariable::m_sSID.c_str());
    parameters->PutUtfString("domain", domain.c_str());
    CCLOG("device: '%d'", GlobalVariable::m_sDevice);
	parameters->PutShort("device", GlobalVariable::m_sDevice);
	//send requeste
	boost::shared_ptr<IRequest> request (new LoginRequest("", "", p_sZoneName.c_str(), parameters));
	mSmartFox->Send( request );

    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_LOGGING, NULL);
    CCLOG("SFSConnection: login end");
}

void SFSConnection::getRank()
{
	boost::shared_ptr<IRequest> request (new ExtensionRequest("client_rank", NULL));
	mSmartFox->Send( request );
}

void SFSConnection::getTables()
{
    CCLOG("SFSConnection: send getTables start");
	boost::shared_ptr<IRequest> request (new ExtensionRequest(S_CLIENT_LOBBY, NULL));
	mSmartFox->Send( request );
    CCLOG("SFSConnection: send getTables end");
}

void SFSConnection::invite(int invitedId)
{
    CCLOG("SFSConnection: send invite start %d", invitedId);
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    boost::shared_ptr<ByteArray> btr (new ByteArray());
    btr->WriteInt(invitedId);
    parameters->PutByteArray("d", btr);
	boost::shared_ptr<IRequest> request(new ExtensionRequest(S_COMMAND_INVITE, parameters));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send invite end");
}

void SFSConnection::getWaitingList()
{
    CCLOG("SFSConnection: send getWaitingList start");
	boost::shared_ptr<IRequest> request (new ExtensionRequest(S_COMMAND_GET_LISTWAITING, NULL));
	mSmartFox->Send( request );
    CCLOG("SFSConnection: send getWaitingList end");
}

//ping
void SFSConnection::ping()
{
    CCLOG("SFSConnection: send ping start");
    if(GlobalVariable::m_iCurrentGame == 0)
    {
        boost::shared_ptr<IRequest> request (new ExtensionRequest(SC_CLIENT_PING, NULL));
        mSmartFox->Send( request );
    }
    else if(GlobalVariable::m_iCurrentGame == 1)
    {
        boost::shared_ptr<IRequest> request (new ExtensionRequest(SS_S_PING, NULL));
        mSmartFox->Send( request );
    }
    CCLOG("SFSConnection: send ping end");
}
//auto
void SFSConnection::autoFire()
{
    CCLOG("SFSConnection: autoFire start");
	boost::shared_ptr<IRequest> request (new ExtensionRequest(SC_COMMAND_AUTO, NULL));
	mSmartFox->Send( request );
    CCLOG("SFSConnection: autoFire end");
}
//send a msg
void SFSConnection::sendMsg( const char * strMsg )
{
	// Send chat message
	boost::shared_ptr<ISFSObject> sfsObj(new SFSObject());
	sfsObj->PutUtfString("userName", GlobalVariable::m_pUser->m_sName);
	boost::shared_ptr<IRequest> request(new PublicMessageRequest(strMsg, sfsObj));
	mSmartFox->Send(request);
}

//send request get ranks
void SFSConnection::requestRank()
{
	boost::shared_ptr<IRequest> request(new ExtensionRequest(S_CLIENT_RANK, NULL));
	mSmartFox->Send(request);
}

//send request get bonus
void SFSConnection::getBonus()
{
	boost::shared_ptr<IRequest> request(new ExtensionRequest(C_CLIENT_BONUS_TIME, NULL));
	mSmartFox->Send(request);
}

//send request playnow
void SFSConnection::playNow()
{
    CCLOG("SFSConnection: send playNow start");
	boost::shared_ptr<ISFSObject> sfsObj(new SFSObject());
	//send request
	boost::shared_ptr<IRequest> request(new ExtensionRequest(S_CLIENT_PLAYNOW, sfsObj));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send playNow end");
}

//send request create room
void SFSConnection::createRoom()
{
    CCLOG("SFSConnection: send create room start");
	boost::shared_ptr<ISFSObject> sfsObj(new SFSObject());
	//send request
	boost::shared_ptr<IRequest> request(new ExtensionRequest(C_CLIENT_USER_CREATE_ROOM, sfsObj));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send create room end");
}

//start game
void SFSConnection::startGame()
{
    CCLOG("SFSConnection: send startGame start");
    if(GlobalVariable::m_iCurrentGame == 0)
    {


         boost::shared_ptr<ISFSObject> parameters(new SFSObject());
         boost::shared_ptr<ByteArray> bytesCards (new ByteArray());
         /*
         bytesCards->WriteShort(1);
         bytesCards->WriteShort(5);
         bytesCards->WriteShort(6);
         bytesCards->WriteShort(7);
         bytesCards->WriteShort(8);
         bytesCards->WriteShort(21);
         bytesCards->WriteShort(25);
         bytesCards->WriteShort(29);
         bytesCards->WriteShort(33);
         bytesCards->WriteShort(37);
         bytesCards->WriteShort(41);
         bytesCards->WriteShort(45);
         bytesCards->WriteShort(49);

         parameters->PutByteArray("d", bytesCards);

         boost::shared_ptr<IRequest> request(new ExtensionRequest(SC_CLIENT_START_GAME, parameters));
         mSmartFox->Send(request);
         */
         boost::shared_ptr<IRequest> request(new ExtensionRequest(SC_CLIENT_START_GAME, NULL));
         mSmartFox->Send(request);
    }
    else
    {
        boost::shared_ptr<IRequest> request(new ExtensionRequest(SS_S_GAME_START, NULL));
        mSmartFox->Send(request);
    }
	CCLOG("SFSConnection: send startGame end");
}
//fire cards
void SFSConnection::fireCard(boost::shared_ptr<ByteArray> bytesCards)
{
    CCLOG("SFSConnection: send fireCard start");
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    parameters->PutByteArray("d", bytesCards);
    short cardid;
    bytesCards->ReadShort(cardid);
	boost::shared_ptr<IRequest> request(new ExtensionRequest(SC_CLIENT_DANH_BAI, parameters));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send fireCard end");
}

void SFSConnection::ignoreCard()
{
    CCLOG("SFSConnection: send ignoreCard start");
	boost::shared_ptr<IRequest> request(new ExtensionRequest(SC_CLIENT_BO_LUOT, NULL));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send ignoreCard end");
}

void SFSConnection::joinRoom(std::string p_sRoomName)
{
    CCLOG("SFSConnection: join room start");
    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_ROOM_JOINING, NULL);
    boost::shared_ptr<ISFSObject> parameters (new SFSObject());
    boost::shared_ptr<ByteArray> bytes (new ByteArray());
    bytes->WriteUTF(p_sRoomName);
    parameters->PutByteArray("d", bytes);
	boost::shared_ptr<IRequest> request (new ExtensionRequest(S_CLIENT_JOIN_GAME, parameters));
	mSmartFox->Send( request );
    CCLOG("SFSConnection: join room end");
}

//Susun
void SFSConnection::xepxong(vector<int>* m_vCardsId, bool bStatus)
{
    CCLOG("SFSConnection: send xepxong start");
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    boost::shared_ptr<ByteArray> bytes (new ByteArray());
    bytes->WriteBool(bStatus);
    long type = 1;
    bytes->WriteInt(type);
    for(int i = 0; i < 13; i++)
    {
        bytes->WriteInt(m_vCardsId->at(i));
    }
    parameters->PutByteArray("d", bytes);
	boost::shared_ptr<IRequest> request(new ExtensionRequest(SS_S_XEP_XONG, parameters));
	mSmartFox->Send(request);
	CCLOG("SFSConnection: send xepxong end");
}
//End Susun
void SFSConnection::requestChangeSettingRoom(int stake, int times)
{
    CCLOG("SFSConnection: requestChangeSettingRoom start");
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
	boost::shared_ptr<ByteArray> bytes(new ByteArray());

    if(GlobalVariable::m_iCurrentGame == 0)//banting
    {
        //send request
        bytes->WriteInt(stake);
        bytes->WriteInt(times);
        parameters->PutByteArray("d", bytes);
        boost::shared_ptr<IRequest> request(new ExtensionRequest(S_CLIENT_CHANGE_SETTING_GAME, parameters));
        mSmartFox->Send(request);
    }
    else //susun
    {
        //send request
        bytes->WriteInt(times);
        bytes->WriteInt(stake);
        parameters->PutByteArray("d", bytes);
        boost::shared_ptr<IRequest> request(new ExtensionRequest(SS_S_NEW_CONFIG, parameters));
        mSmartFox->Send(request);
    }

    CCLOG("SFSConnection: requestChangeSettingRoom end");
}


void SFSConnection::requestMauBinh(int typeMB, vector<int> cards)
{
    boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    boost::shared_ptr<ByteArray> b(new ByteArray());
    b->WriteInt(typeMB);
    for (long i = 0, l = cards.size(); i < l; i++)
    {
        b->WriteInt(cards.at(i));
    }
    parameters->PutByteArray("d", b);
    boost::shared_ptr<IRequest> request(new ExtensionRequest(SS_S_MAU_BINH_3S3T, parameters));
    mSmartFox->Send(request);
}


void SFSConnection::kickUser(long iKickedUserId, double dKickedUserId)
{
	boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    boost::shared_ptr<ByteArray> btr (new ByteArray());
    if(GlobalVariable::m_iCurrentGame == 0)
    {
        CCLOG("SFSConnection: send kick start %f", dKickedUserId);
        btr->WriteDouble(dKickedUserId);
        parameters->PutByteArray("d", btr);
        boost::shared_ptr<IRequest> request(new ExtensionRequest(S_COMMAND_KICK_USER, parameters));
        mSmartFox->Send(request);
    }
    else
    {
        CCLOG("SFSConnection: send kick start %ld", iKickedUserId);
        btr->WriteInt(iKickedUserId);
        parameters->PutByteArray("d", btr);
        boost::shared_ptr<IRequest> request(new ExtensionRequest(SS_S_KICK, parameters));
        mSmartFox->Send(request);
    }
	CCLOG("SFSConnection: send kick end");
}

void SFSConnection::getPlayerInfo(long iSFSId)
{
    boost::shared_ptr<ISFSObject> parameters(new SFSObject());
    boost::shared_ptr<ByteArray> btr (new ByteArray());
    btr->WriteInt(iSFSId);
    parameters->PutByteArray("d", btr);
    boost::shared_ptr<IRequest> request(new ExtensionRequest(S_CLIENT_PLAYER_INFO, parameters));
    mSmartFox->Send(request);
}
/* end send server */

//*******************************
// star recieve request
//*******************************
//ping-pong
void SFSConnection::onSmartFoxPingPong(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    //CCLOG("onSFSPingPong start");
    GlobalVariable::m_iPingCount = 0;
    //CCLOG("onSFSPingPong end");
}

void SFSConnection::OnSmartFoxConnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    CCLOG("OnSmartFoxConnection");

	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueSuccess = (*ptrEventParams)["success"];
	boost::shared_ptr<bool> ptrValue = (boost::static_pointer_cast<bool>)(ptrEventParamValueSuccess);

    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_CONNECTION, NULL);
	ZoneData* zoneData = GlobalVariable::m_vGameServerData->at(GlobalVariable::m_iCurrentGame)->m_vZoneArray->at(GlobalVariable::m_iCurrentZone);
	SFSConnection::getInstance()->login(GlobalVariable::m_sSID, GlobalVariable::m_sDomain, zoneData->m_sName);
}

void SFSConnection::OnSmartFoxConnectionLost(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    CCLOG("OnSmartFoxConnectionLost");
    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_CONNECTION_LOST, NULL);
}

void SFSConnection::OnSmartFoxDisconnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    CCLOG("OnSmartFoxDisConnection");
    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_DISCONNECT, NULL);
}

void SFSConnection::OnSmartFoxLogin(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    CCLOG("OnSmartFoxLogin");
    mSmartFox->EnableLagMonitor(true, 3, 6);
    boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueSuccess = (*ptrEventParams)["success"];
    boost::shared_ptr<SFSUser> ptrEventParamUser = (boost::static_pointer_cast<SFSUser>)((*ptrEventParams)["user"]);
    long int iSFSID = ptrEventParamUser->Id();
	boost::shared_ptr<bool> ptrValue = (boost::static_pointer_cast<bool>)(ptrEventParamValueSuccess);
    GlobalVariable::m_pUser->m_iSfsId = iSFSID;
    CCLOG("OnSmartFoxLogin SFSID1 %ld", GlobalVariable::m_pUser->m_iSfsId);
    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_LOGIN_SUCCESSFUL, NULL);
}

void SFSConnection::OnSmartFoxLoginError(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{

	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = (*ptrEventParams)["errorMessage"];
    boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);

	CCLOG("OnSmartFoxLoginError- %s", ptrErrorMessage->c_str());

	CMDDataString* cmd1 = new CMDDataString();
	cmd1->str = ptrErrorMessage->c_str();

	/*rapidjson::Document document;
     document.Parse<0>(ptrErrorMessage->c_str());

     string key = "";
     if (document.HasParseError())
     {
     CCLOG("");
     }
     if (document.HasMember("msg"))
     {
     key = document["msg"].GetString();
     }

     vector<string>* params = new vector<string>();
     if (document.HasMember("ec"))
     {
     params->push_back(document["ec"].GetString());
     }*/

    NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_lOGIN_ERROR, (Ref*)(cmd1));
}

void SFSConnection::OnSmartFoxRoomJoin(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	CCLOG("OnSmartFoxRoomJoin");
	//room joined
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueRoom = (*ptrEventParams)["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);
    boost::shared_ptr<string> name = ptrNotifiedRoom->Name();
    if(strcmp(name->substr(0, 3).c_str(), "cho") == 0)
    {
        GlobalVariable::m_sWaitRoomName = name->c_str();
        GlobalVariable::m_pUser->m_bHost = false;
    }
}

void SFSConnection::OnSmartFoxRoomJoinError(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = (*ptrEventParams)["errorMessage"];
	boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);
	boost::shared_ptr<string> message (new string("Join Room Failure: " +  *ptrErrorMessage + " !"));

    //pop up the warnning dialog
}

void SFSConnection::OnUserExitRoom(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    CCLOG("OnUserExitRoom");
	//get the user parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueUser = (*ptrEventParams)["user"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the room parameter of the event
	boost::shared_ptr<void> ptrEventParamValueRoom = (*ptrEventParams)["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);
}

void SFSConnection::OnSmartFoxPublicMessage(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueSender = (*ptrEventParams)["sender"];
	boost::shared_ptr<User> ptrNotifiedSender = ((boost::static_pointer_cast<User>)(ptrEventParamValueSender));
	boost::shared_ptr<void> ptrEventParamValueMessage = (*ptrEventParams)["message"];
	boost::shared_ptr<string> ptrNotifiedMessage = ((boost::static_pointer_cast<string>)(ptrEventParamValueMessage));

	//do not show the msg that send by self
	if ( ptrNotifiedSender->IsItMe() == true )
		return;
	int id = ptrNotifiedSender->Id();
	std::string msg = std::string(ptrNotifiedMessage->c_str());
	NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_GET_MSR, MsrObject::create(id, msg));
}

void SFSConnection::OnExtensionResponse(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
    boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
    boost::shared_ptr<void> ptrEventParamValueCmd = (*ptrEventParams)["cmd"];
    boost::shared_ptr<string> ptrNotifiedCmd = ((boost::static_pointer_cast<string>)(ptrEventParamValueCmd));

    boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
    boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));
    boost::shared_ptr<ByteArray> btrd = ptrNotifiedISFSObject->GetByteArray("d");
    boost::shared_ptr<ByteArray> btrt = ptrNotifiedISFSObject->GetByteArray("t");
    boost::shared_ptr<ByteArray> btrc = ptrNotifiedISFSObject->GetByteArray("c");
    boost::shared_ptr<ByteArray> btrg = ptrNotifiedISFSObject->GetByteArray("g");
    boost::shared_ptr<ByteArray> btrn = ptrNotifiedISFSObject->GetByteArray("n");

	CCLOG("OnExtensionResponse %s", ptrNotifiedCmd->c_str());
    SFSResponse::parse(GlobalVariable::m_iCurrentGame,  ptrNotifiedCmd->c_str(), btrd, btrt, btrc, btrg, btrn);
}
/* end recieve request */

bool SFSConnection::isConnected()
{
    isServerInit = false;
	return mSmartFox->IsConnected();
}

bool SFSConnection::isServerInit = false;
