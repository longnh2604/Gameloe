#include "AppDelegate.h"
#include "LoadingScene.h"
#include "GlobalVariable.h"
#include "SceneManager.h"
USING_NS_CC;

AppDelegate::AppDelegate() 
{
	
}

AppDelegate::~AppDelegate() 
{
}

void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {4, 4, 4, 4, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Indo Game");
        director->setOpenGLView(glview);
    }

    cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
    cocos2d::Size designResolutionSize = Size(960, 640);
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    //director->setContentScaleFactor(MIN(designResolutionSize.height/size.height, designResolutionSize.width/size.width));

    // Set content scale factor


    // turn on display FPS
    director->setDisplayStats(false);
    // set FPS. the default value is 1.0/60 if you don't call this

    director->setAnimationInterval(1.0 / 24);
    // create a scene. it's an autorelease object

    //director->setContentScaleFactor();

    // run
    SceneManager::getInstance()->run(kSceneLoading);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
