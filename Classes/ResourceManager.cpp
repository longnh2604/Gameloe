//
//  ResourceManager.cpp
//  IndoGame
//
//  Created by Rainy on 8/4/14.
//
//

#include "ResourceManager.h"
#include "platform/CCFileUtils.h"
#include "GlobalVariable.h"
#include "EventConst.h"

bool ResourceManager::m_bLoadConfigFinish = false;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::loadTextMessage()
{
    std::string contentStr = FileUtils::getInstance()->getStringFromFile("TextMessage.json");
	m_jTextMessage.Parse<0>(contentStr.c_str());
	//CCLOG("\ndata:\n%s\n", contentStr.c_str());
    /*
	m_requestLang = new HttpRequest();
	m_requestLang->setRequestType(HttpRequest::Type::GET);
	m_requestLang->setResponseCallback(CC_CALLBACK_2(ResourceManager::onResponseLang, this));
	m_requestLang->setUrl(kLanguageURL);
	HttpClient::getInstance()->sendImmediate(m_requestLang);
	m_requestLang->release();
    */
}

void ResourceManager::onResponseLang(HttpClient* sender, HttpResponse* response)
{
    /*
	std::vector<char>* l_v_buffer_data = response->getResponseData();
	std::string l_s_data(l_v_buffer_data->begin(), l_v_buffer_data->end());

	rapidjson::Document document;
	document.Parse<0>(l_s_data.c_str());
    */
	//CCLOG("\ndata:\n%s\n", l_s_data.c_str());
}

void ResourceManager::loadConfig()
{
    HttpRequest* request = new HttpRequest();
    std::string url = kBaseURL;
    url.append(kConfigFile);
    request->setUrl(url.c_str());
    request->setRequestType(HttpRequest::Type::GET);
    std::vector<std::string> headers;
    headers.push_back("Content-Type: text/plain; charset=utf-8");
    request->setHeaders(headers);
    request->setResponseCallback(CC_CALLBACK_2(ResourceManager::onHttpRequestCompleted, this));
    if (true)
    {
        request->setTag("GET immediate config");
        HttpClient::getInstance()->sendImmediate(request);
    }
    else
    {
        request->setTag("GET config");
        HttpClient::getInstance()->send(request);
    }
    request->release();
}

void ResourceManager::checkAccountMode()
{
    HttpRequest* request = new HttpRequest();
    string action = "guestMode";
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    string device_type = "0";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    string device_type = "2";
#endif
	std::string strRequestData =
    "action=" + action +
    "&device_id=" + GlobalVariable::m_sDeviceID +
    "&device_type=" + device_type;

    CCLOG("strRequestData %s", strRequestData.c_str());

	request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setRequestData(strRequestData.c_str(), strlen(strRequestData.c_str()));
	request->setResponseCallback(CC_CALLBACK_2(ResourceManager::onResponseAccountMode, this));
	request->setUrl(kURLGameloeDeviceID);
	HttpClient::getInstance()->sendImmediate(request);
	request->release();
}

void ResourceManager::onResponseAccountMode(HttpClient *sender, HttpResponse *response)
{
    CCLOG("onResponseAccountMode1");
    if (!response)
    {
        return;
    }
        CCLOG("onResponseAccountMode2");
    if(response->getResponseCode() == -1)
    {
        NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_NOT_AVAILBLE, NULL);
        return;
    }
    //dump data
    CCLOG("onResponseAccountMode3");
    std::vector<char>* l_v_buffer_data = response->getResponseData();
    std::string l_s_data(l_v_buffer_data->begin(), l_v_buffer_data->end());
    if(l_s_data.empty())
    {
        NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_NOT_AVAILBLE, NULL);
        return;
    }
    CCLOG("onResponseAccountMode4 %s", l_s_data.c_str());
    rapidjson::Document document;
    document.Parse<0>(l_s_data.c_str());
    std::string mode = document["mode"].GetString();
    CCLOG("onResponseAccountMode5 %s", mode.c_str());
    if(strcmp(mode.c_str(), "personal") == 0)
    {
        GlobalVariable::m_iAccountMode = kAccountModePersonal;//
    }
    else if(strcmp(mode.c_str(), "guest") == 0)
    {
        GlobalVariable::m_iAccountMode = kAccountModeGuest;
    }
    UserDefault::getInstance()->setIntegerForKey("AccountMode", GlobalVariable::m_iAccountMode);
    CCLOG("Account Mode %d", GlobalVariable::m_iAccountMode);
}

void ResourceManager::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    if (!response)
    {
        return;
    }
    
    if(response->getResponseCode() == -1)
    {
        NotificationCenter::getInstance()->postNotification(EVENT_NETWORK_NOT_AVAILBLE, NULL);
        return;
    }
    //dump data

    std::vector<char>* l_v_buffer_data = response->getResponseData();

    int i = 0;
    while(l_v_buffer_data->at(i) != '{')
    {
        i++;
    }
    std::string l_s_data(l_v_buffer_data->begin() + i, l_v_buffer_data->end());
    rapidjson::Document document;
    document.Parse<0>(l_s_data.c_str());
    //CCLOG("\ndata:\n%s\n", l_s_data.c_str());
    std::string newVersion = document["version"].GetString();
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::string inReview   = document["inreview_android"].GetString();
#else
    std::string inReview   = document["inreview"].GetString();
#endif
    GlobalVariable::m_sInreview = inReview;

    if(strcmp(newVersion.c_str(), GlobalVariable::m_sCurrentVersion.c_str()) != 0)
    {
        GlobalVariable::m_bHasNewVersion = true;
    }
    
    for(int i = 0; i < document["data"].Size(); i++)
    {
        GameServerData* gameServerData = new GameServerData();
        gameServerData->m_sId   = document["data"][i]["id"].GetString();
        gameServerData->m_sName = document["data"][i]["name"].GetString();
        for(int j = 0; j < document["data"][i]["zones"].Size(); j++)
        {
            ZoneData* zoneData = new ZoneData();
            zoneData->m_sName     = document["data"][i]["zones"][j]["name"].GetString();
            zoneData->m_sTilte    = document["data"][i]["zones"][j]["title"].GetString();
            zoneData->m_dMinMoney = document["data"][i]["zones"][j]["money"].GetInt();
            zoneData->m_iMinLevel = document["data"][i]["zones"][j]["level"].GetInt();
            zoneData->m_sHostIp   = document["data"][i]["zones"][j]["host"].GetString();
            zoneData->m_iPort     = document["data"][i]["zones"][j]["port"].GetInt();
            gameServerData->m_vZoneArray->push_back(zoneData);
        }

        GlobalVariable::m_vGameServerData->push_back(gameServerData);
    }
    m_bLoadConfigFinish = true;
    CCLOG("Load Config finish");
    
}

void ResourceManager::loadTextData()
{
    std::string contentStr = FileUtils::getInstance()->getStringFromFile("TextData.json");
	m_jTextData.Parse<0>(contentStr.c_str());
	//CCLOG("\ndata:\n%s\n", contentStr.c_str());
}

void ResourceManager::setLanguage(int p_iLanguageId)
{
    GlobalVariable::m_iCurrentLanguage = p_iLanguageId;
    UserDefault::getInstance()->setIntegerForKey("language", p_iLanguageId);
}

std::string ResourceManager::getTextByName(const char* p_sTextName)
{
    int iLanguage = GlobalVariable::m_iCurrentLanguage;
    return m_jTextData["TextData"][iLanguage]["TEXTS"][p_sTextName].GetString();
}

void ResourceManager::loadSoundData()
{
    AudioManager::getInstance()->preloadBackgroundMusic(kSoundMusicTitle);
    AudioManager::getInstance()->preloadEffect(kSoundEffectButtonClick);
    bool muted =  UserDefault::getInstance()->getBoolForKey("muted");
    if(muted)
    {
        AudioManager::getInstance()->muteBackground(true);
        AudioManager::getInstance()->muteEffect(true);
    }
}

void ResourceManager::loadTextureData()
{
    Director::getInstance()->getTextureCache()->addImage("loadingicon.png");
    Director::getInstance()->getTextureCache()->addImage("bar.png");
    Director::getInstance()->getTextureCache()->addImage("newsfeed.png");
    Director::getInstance()->getTextureCache()->addImage("blackline.png");
    Director::getInstance()->getTextureCache()->addImage("Digit-export.png");
    Director::getInstance()->getTextureCache()->addImage("ingame_background.png");
}

void ResourceManager::loadTextureEffect()
{
}

void ResourceManager::cacheScene()
{
    /*
    m_aLayouts[kSceneLobby] = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(kSceneLobbyGameJsonFile));
    m_aLayouts[kSceneLobby]->retain();
     */
	m_loChatPopup = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ChatPopup.json"));
    m_loChatPopup->retain();
}

const string strSpine[kNOAnims] = {"girl", "trang", "thang", "vebet", "moiban", "good", "bad", "sangvanmoi", "Timeline", "card_swap", "exp", "smke", "emo_0", "emo_1", "emo_2", "emo_3", "emo_4", "emo_5", "emo_6", "emo_7"};
spine::SkeletonRenderer* ResourceManager::m_aSkeletonRendererCache[] = {};
spine::SkeletonAnimation* ResourceManager::m_aSkeletonAnimation[] = {};
void ResourceManager::cacheSpine()
{
    for(int i = 0; i < kNOAnims; i++)
    {
        string jsonName = strSpine[i] + ".json";
        string atlastName = strSpine[i] + ".atlas";
        m_aSkeletonRendererCache[i] = spine::SkeletonRenderer::createWithFile(jsonName.c_str(), atlastName.c_str());
        m_aSkeletonRendererCache[i]->retain();
    }
}