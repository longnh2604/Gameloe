//
//  AppController.m
//  Gameloe
//
//  Created by Rainy on 3/25/15.
//
//

#import "AppController.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "SFSConnection.h"
#import "LoginHandler.h"
#import "ASIFormDataRequest.h"
#import "ASIHTTPRequest.h"
#import "platform/ios/CCEAGLView-ios.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import "IAPHelper.h"
#import <StoreKit/StoreKit.h>
#import "RageIAPHelper.h"
#import "ACTReporter.h"
@implementation AppController

#pragma mark -
#pragma mark Application lifecycle
@class GTMOAuth2Authentication;
// cocos2d application instance
static AppDelegate s_sharedApplication;
static AppController* me;
static NSString* uDeviceID;

#define kURLGameloeLoginFacebook @"http://ids.gameloe.com/facebook/login?m=1"
#define kURLGameloeLoginGoogle   @"http://ids.gameloe.com/google/login?m=1"
#define kURLGameloeLoginDeviceID @"http://ids.gameloe.com/api/client"
#define kURLGameloeLogout        @"http://ids.gameloe.com/users/logout"
#define kGoogleClientID          @"496374930911-be2kehkr1qbi3mf19b9usmqth17pieg7.apps.googleusercontent.com"
#define kGoogleServerID          @"496374930911-quqepni67ve4c43e1p9a3onjnpnc36ik.apps.googleusercontent.com"

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // Override point for customization after application launch.
    cocos2d::Application *app = cocos2d::Application::getInstance();
    app->initGLContextAttrs();
    cocos2d::GLViewImpl::convertAttrs();
    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];

    // Init the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                         pixelFormat: (NSString*)cocos2d::GLViewImpl::_pixelFormat
                                         depthFormat: cocos2d::GLViewImpl::_depthFormat
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: NO
                                     numberOfSamples: 0 ];

    // Use RootViewController manage CCEAGLView
    _viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    //_viewController.wantsFullScreenLayout = YES;
    _viewController.view = eaglView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }

    [window makeKeyAndVisible];
    NSUUID   *strUniqueIdentifier = [[UIDevice currentDevice] identifierForVendor];
    NSString *strName             = [[UIDevice currentDevice] name];
    uDeviceID                     = [strUniqueIdentifier UUIDString];
    NSLog(@"device id: %@", uDeviceID);
    NSString* deviceId = [uDeviceID stringByReplacingOccurrencesOfString:@"-" withString:@""];
    GlobalVariable::m_sDeviceID  = std::string([deviceId UTF8String]);
    [uDeviceID retain];
    
    [[UIApplication sharedApplication] setStatusBarHidden:true];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    me = self;

    // You previously set kClientId in the "Initialize the Google+ client" step
    [GPPSignIn sharedInstance].clientID = kGoogleClientID;

    // Your server's OAuth 2.0 client ID
    [GPPSignIn sharedInstance].homeServerClientID = kGoogleServerID;
    // Uncomment one of these two statements for the scope you chose in the previous step
    [GPPSignIn sharedInstance].scopes = @[kGTLAuthScopePlusLogin];

    //Google plus login
    [GPPSignIn sharedInstance].shouldFetchGooglePlusUser = YES;
    [GPPSignIn sharedInstance].shouldFetchGoogleUserEmail = YES;  // Uncomment to get the user's email
    [GPPSignIn sharedInstance].useClientIDForURLScheme = NO;
    // Optional: declare signIn.actions, see "app activities"
    [GPPSignIn sharedInstance].delegate = me;

    [GPPDeepLink setDelegate:self];
    [GPPDeepLink readDeepLinkAfterInstall];
    //offline notification
    [self deleteAlarm];
    [self setAlarm];
    [self setTracking];
    
    //end offline notification
    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);
    cocos2d::Director::getInstance()->setDisplayStats(false);
    cocos2d::Application::getInstance()->run();
    return YES;
}

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation {

    // Call FBAppCall's handleOpenURL:sourceApplication to handle Facebook app responses
    BOOL wasHandledFB = [FBAppCall handleOpenURL:url sourceApplication:sourceApplication];

    // You can add your app-specific url handling code here if needed
    BOOL wasHandledGP = [GPPURLHandler handleURL:url sourceApplication:sourceApplication annotation:annotation];
    return wasHandledFB && wasHandledGP;
}

//request response
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    // A response has been received, this is where we initialize the instance var you created
    // so that we can append data to it in the didReceiveData method
    // Furthermore, this method is called each time there is a redirect so reinitializing it
    // also serves to clear it
    //_responseData = [[NSMutableData alloc] init];

    NSHTTPCookie *cookie;
    NSString* sessionID;
    NSHTTPCookieStorage *cookieJar = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    for (cookie in [cookieJar cookies])
    {
        NSLog(@"Login session id1 %@", cookie.name);
        if ([cookie.name isEqualToString:@"SESSa32639e00374d719e31425e62cf17ed3"])
        {
            sessionID = [NSString stringWithFormat:@"%@",[cookie value]];
        }
    }
    NSLog(@"Login session id2 %@", sessionID);
    GlobalVariable::m_sSID  = std::string([sessionID UTF8String]);
    ZoneData* zoneData = GlobalVariable::m_vGameServerData->at(GlobalVariable::m_iCurrentGame)->m_vZoneArray->at(GlobalVariable::m_iCurrentZone);
    SFSConnection::getInstance()->connectToServer(zoneData->m_sHostIp.c_str(), zoneData->m_iPort);
}
//==================================================================================================
//Util
//==================================================================================================
- (void)setTracking
{
    [ACTAutomatedUsageTracker enableAutomatedUsageReportingWithConversionID:@"950670153"];
    [ACTConversionReporter reportWithConversionID:@"950670153" label:@"W-FuCNOGllwQyaaoxQM" value:@"1.00" isRepeatable:NO];
}

- (void)setAlarm
{
    NSArray *megs = @[
                    @"Satu jagoan baru saja menang besar di www.gameloe.com. Masuk untuk bermain game segera.",
                    @"Keberuntungan kalian sudah datang, ayo mainkan game dan dapat banyak emas.",
                    @"Sudah 3 hari kami tidak bertemu kalian. Kami rindu kalian sekali. Ayo mainkan segara.",
                    @"Banyak jagoan sedang menunggu kalian. Ayo mainkan",
                    @"Memiliki 2 wanita cantik sedang bermain capsa susun dan masih kurang satu pemain lagi untuk bermain permainannya. Kalian bergabung sekarang.",
                    @"Kalian lelah, stres ataupun ingin hiburan. Ayo akses website www.gameloe.com segera."
                    ];
    int index = rand() % 6;
    UILocalNotification* localNotification = [[UILocalNotification alloc] init];
    localNotification.fireDate = [NSDate dateWithTimeIntervalSinceNow:259200];
    localNotification.alertBody = megs[index];
    localNotification.timeZone = [NSTimeZone defaultTimeZone];
    //localNotification.applicationIconBadgeNumber += 1;
    localNotification.accessibilityLabel = @"localNotification";
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotification];
}

- (void)deleteAlarm
{
    NSArray *arrayOfLocalNotifications = [[UIApplication sharedApplication] scheduledLocalNotifications] ;
    for (UILocalNotification *localNotification in arrayOfLocalNotifications)
    {
        if ([localNotification.accessibilityLabel  isEqualToString:@"localNotification"])
        {
            NSLog(@"the notification this is canceld is %@", localNotification.accessibilityLabel);
            [[UIApplication sharedApplication] cancelLocalNotification:localNotification];
            //localNotification.applicationIconBadgeNumber = 0;
        }
    }
}

void makeCallSupport(std::string number)
{
    NSLog(@"makeCallSupport");
    NSString* phone = [NSString stringWithFormat:@"%s", number.c_str()];
    NSString* phoneNumber = [@"telprompt://" stringByAppendingString:phone];
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:phoneNumber]];
}

void makeOpenFanpage(std::string url)
{
    NSLog(@"makeOpenFanpage");
    NSString* str    = [NSString stringWithFormat:@"%s", url.c_str()];
    NSURL *fanPageURL = [NSURL URLWithString:@"fb://profile/723652577701779"];

    if (![[UIApplication sharedApplication] openURL: fanPageURL]) {
        //fanPageURL failed to open.  Open the website in Safari instead
        NSURL *webURL = [NSURL URLWithString:str];
        [[UIApplication sharedApplication] openURL: webURL];
    }
}

//==================================================================================================
//In App Purchase
//==================================================================================================

void sendIAP(string sku)
{
    NSLog(@"sku: %s", sku.c_str());
    NSString* strSKU = [NSString stringWithFormat:@"%s", sku.c_str()];
    NSSet * productIdentifiers = [NSSet setWithObjects:
                                  strSKU,
                                  nil];
    [[RageIAPHelper sharedInstance] initWithProductIdentifiers:productIdentifiers];
    [[RageIAPHelper sharedInstance] requestProductsWithCompletionHandler:^(BOOL success, NSArray *products)
     {
         if (success)
         {
             SKProduct *product = nil;
             NSLog(@"success...");
             for(int i = 0; i <  products.count; i++)
             {
                 product = products[i];
                 if([strSKU isEqualToString:product.productIdentifier])
                 {
                     NSLog(@"Buying %@...", product.productIdentifier);
                     [[RageIAPHelper sharedInstance  ] buyProduct:product];
                 }
             }
         }
     }];
}

//==================================================================================================
//start login
//==================================================================================================
void loginByAccountBase(string username, string password)
{
    ASIFormDataRequest* requestLogout = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLogout]];
    [requestLogout setCompletionBlock:^{
        ASIFormDataRequest* requestLogin = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLoginDeviceID]];
        NSString* email = [NSString stringWithFormat:@"%s", username.c_str()];
        NSString* pw = [NSString stringWithFormat:@"%s", password.c_str()];
        //0: iPhone; 1: iPad; 2: Android; 3: Tablet
        NSLog(@"loginAccountBase username: %@ password: %@", email, pw);
        [requestLogin setPostValue:@"login" forKey:@"action"];
        [requestLogin setPostValue:email forKey:@"email"];
        [requestLogin setPostValue:pw forKey:@"password"];
        [requestLogin setCompletionBlock:^{
            NSData* data = [requestLogin responseData];
            NSString *returnString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            NSLog(@"login: %@", returnString);
            NSDictionary *jsonDataDict = [NSJSONSerialization JSONObjectWithData:data options: NSJSONReadingMutableContainers error: nil];
            NSString  *sessionID;
            int uid = 0;
            uid = [[jsonDataDict objectForKey:@"uid"] integerValue];
            if (uid != 0)
            {
                sessionID = [jsonDataDict objectForKey:@"sid"];
                string sid = std::string([sessionID UTF8String]);
                LoginHandler::getInstance()->getAccountSuccessed(sid);
            }
            else
            {
                NSString* message = [jsonDataDict objectForKey:@"message"];
                string msg = std::string([message UTF8String]);
                LoginHandler::getInstance()->getAccountFail(msg);
            }
        }];
        [requestLogin setFailedBlock:^{
            NSError *error    = [requestLogin error];
            NSString* message = [NSString stringWithFormat:@"%@", error];
            string msg        = std::string([message UTF8String]);
            LoginHandler::getInstance()->getAccountFail(msg);
            SFSConnection::getInstance()->disconnectToServer();
            SceneManager::getInstance()->switchScene(kSceneLogin);
        }];
        dispatch_async(dispatch_get_main_queue(), ^{
            [requestLogin startAsynchronous];
        });
    }];
    [requestLogout setFailedBlock:^{
        LoginHandler::getInstance()->getAccountFail("loginAccountBase requestLogout fail");
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [requestLogout startAsynchronous];
    });
}

void loginByDeviceID()
{
    NSLog(@"loginByDeviceID");
    if (uDeviceID != NULL)
    {
        ASIFormDataRequest* requestLogout = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLogout]];
        [requestLogout setCompletionBlock:^{
            ASIFormDataRequest* requestLogin = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLoginDeviceID]];
            NSString* deviceId = [uDeviceID stringByReplacingOccurrencesOfString:@"-" withString:@""];
            //0: iPhone; 1: iPad; 2: Android; 3: Tablet
            [requestLogin setPostValue:@"guestMode" forKey:@"action"];
            [requestLogin setPostValue:deviceId forKey:@"device_id"];
            [requestLogin setPostValue:@"0" forKey:@"device_type"];
            [requestLogin setCompletionBlock:^{
                NSData* data = [requestLogin responseData];
                NSString *returnString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                NSLog(@"login: %@", returnString);
                NSDictionary *jsonDataDict = [NSJSONSerialization JSONObjectWithData:data options: NSJSONReadingMutableContainers error: nil];
                NSString  *sessionID;
                int uid = 0;
                uid = [[jsonDataDict objectForKey:@"uid"] integerValue];
                if (uid != 0)
                {
                    sessionID = [jsonDataDict objectForKey:@"sid"];
                    string sid = std::string([sessionID UTF8String]);
                    LoginHandler::getInstance()->getAccountSuccessed(sid);
                }
                else
                {
                    NSString* message = [jsonDataDict objectForKey:@"message"];
                    string msg = std::string([message UTF8String]);
                    LoginHandler::getInstance()->getAccountFail(msg);
                }
            }];
            [requestLogin setFailedBlock:^{
                NSError *error    = [requestLogin error];
                NSString* message = [NSString stringWithFormat:@"%@", error];
                string msg        = std::string([message UTF8String]);
                LoginHandler::getInstance()->getAccountFail(msg);
                SFSConnection::getInstance()->disconnectToServer();
                SceneManager::getInstance()->switchScene(kSceneLogin);
            }];
            dispatch_async(dispatch_get_main_queue(), ^{
                [requestLogin startAsynchronous];
            });
        }];
        [requestLogout setFailedBlock:^{
            LoginHandler::getInstance()->getAccountFail("loginByDeviceID requestLogout fail");
        }];
        dispatch_async(dispatch_get_main_queue(), ^{
            [requestLogout startAsynchronous];
        });
    }
    else
    {
        LoginHandler::getInstance()->getAccountFail("loginByDeviceID login Error");
    }
}

//login facebook
void loginByFacebook()
{
    NSLog(@"loginByFacebook");
    if (FBSession.activeSession.isOpen)
    {
        [me sendFacebookLogin];
    }
    else
    {
        [FBSession openActiveSessionWithReadPermissions:@[@"public_profile", @"user_friends", @"email"]
                                           allowLoginUI:YES
                                      completionHandler:
         ^(FBSession *session, FBSessionState state, NSError *error)
         {
             if (!error)
             {
                 // If the session was opened successfully
                 if (state == FBSessionStateOpen)
                 {
                     // Your code here
                     [me sendFacebookLogin];

                 }
                 else
                 {
                     // There was an error, handle it
                     if ([FBErrorUtility shouldNotifyUserForError:error] == YES)
                     {
                         NSString* message = [FBErrorUtility userMessageForError:error];
                         string msg        = std::string([message UTF8String]);
                         LoginHandler::getInstance()->getAccountFail(msg);

                     }
                     else
                     {
                         // If the user cancelled login
                         if ([FBErrorUtility errorCategoryForError:error] == FBErrorCategoryUserCancelled)
                         {
                             NSString* message = @"Login cancelled";
                             string msg        = std::string([message UTF8String]);
                             LoginHandler::getInstance()->getAccountFail(msg);
                         }
                         else
                         {
                             NSDictionary *errorInformation = [[[error.userInfo objectForKey:@"com.facebook.sdk:ParsedJSONResponseKey"]
                                                                objectForKey:@"body"]
                                                               objectForKey:@"error"];
                             NSString* message = [NSString stringWithFormat:@"Please retry. \n If the problem persists contact us and mention this error code: %@", [errorInformation objectForKey:@"message"]];
                             string msg        = std::string([message UTF8String]);
                             LoginHandler::getInstance()->getAccountFail(msg);
                         }
                     }
                 }
             }
             else
             {
                 if ([FBErrorUtility shouldNotifyUserForError:error] == YES)
                 {
                     NSString* message = [FBErrorUtility userMessageForError:error];
                     string msg        = std::string([message UTF8String]);
                     LoginHandler::getInstance()->getAccountFail(msg);

                 }
                 else
                 {
                     // If the user cancelled login
                     if ([FBErrorUtility errorCategoryForError:error] == FBErrorCategoryUserCancelled)
                     {
                         NSString* message = @"Login Canceled";
                         string msg        = std::string([message UTF8String]);
                         LoginHandler::getInstance()->getAccountFail(msg);

                     }
                     else
                     {
                         NSDictionary *errorInformation = [[[error.userInfo objectForKey:@"com.facebook.sdk:ParsedJSONResponseKey"]
                                                            objectForKey:@"body"]
                                                           objectForKey:@"error"];
                         NSString* message = [NSString stringWithFormat:@"Please retry. \n If the problem persists contact us and mention this error code: %@",
                                              [errorInformation objectForKey:@"message"]];
                         string msg        = std::string([message UTF8String]);
                         LoginHandler::getInstance()->getAccountFail(msg);
                     }
                 }
             }
         }];
    }
}

-(void)sendFacebookLogin
{
    if (FBSession.activeSession.isOpen)
    {
        NSString *fbAccessToken = [[[FBSession activeSession] accessTokenData] accessToken];
        NSLog(@"fbAccessToken: %@", fbAccessToken);
        [[FBRequest requestForMe] startWithCompletionHandler:^(FBRequestConnection *connection, NSDictionary<FBGraphUser> *user, NSError *error)
         {
             if (!error)
             {
                 NSString* id = [user objectForKey:@"id"];
                 GlobalVariable::m_sUserFacebookID = std::string([id UTF8String]);

                 ASIFormDataRequest* requestLogout = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLogout]];
                 [requestLogout setCompletionBlock:^{
                     ASIFormDataRequest* requestLogin = [ASIFormDataRequest requestWithURL:[NSURL URLWithString:kURLGameloeLoginFacebook]];
                     [requestLogin setPostValue:fbAccessToken forKey:@"access_token"];
                     [requestLogin setCompletionBlock:^{
                         NSData* data = [requestLogin responseData];
                         NSString *returnString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                         NSLog(@"login: %@", returnString);
                         NSDictionary *jsonDataDict = [NSJSONSerialization JSONObjectWithData:data options: NSJSONReadingMutableContainers error: nil];
                         NSString  *sessionID;
                         int status = [[jsonDataDict objectForKey:@"status"] integerValue];
                         if (status == 1)
                         {
                             sessionID = [jsonDataDict objectForKey:@"sid"];
                             string sid = std::string([sessionID UTF8String]);
                             LoginHandler::getInstance()->getAccountSuccessed(sid);
                         }
                         if (status == 0)
                         {
                             NSString* message = [jsonDataDict objectForKey:@"message"];
                             string msg = std::string([message UTF8String]);
                             LoginHandler::getInstance()->getAccountFail(msg);
                         }
                     }];
                     [requestLogin setFailedBlock:^{
                         NSError *error    = [requestLogin error];
                         NSString* message = [NSString stringWithFormat:@"%@", error];
                         string msg        = std::string([message UTF8String]);
                         LoginHandler::getInstance()->getAccountFail(msg);
                         logoutByFacebook();
                         SFSConnection::getInstance()->disconnectToServer();
                         SceneManager::getInstance()->switchScene(kSceneLogin);
                     }];
                     dispatch_async(dispatch_get_main_queue(), ^{
                         [requestLogin startAsynchronous];
                     });
                 }];
                 [requestLogout setFailedBlock:^{
                     LoginHandler::getInstance()->getAccountFail("Facebook requestLogout fail");
                 }];
                 dispatch_async(dispatch_get_main_queue(), ^{
                     [requestLogout startAsynchronous];
                 });
             }
             else
             {
                 LoginHandler::getInstance()->getAccountFail("Facebook login Error");
             }
         }];
    }
}

void logoutByFacebook()
{
    if (FBSession.activeSession.isOpen)
    {
        [[FBSession activeSession] closeAndClearTokenInformation];
    }
}

void loginByGooglePlus()
{
    [[GPPSignIn sharedInstance] authenticate];
}

void logoutByGooglePlus()
{
    [[GPPSignIn sharedInstance] disconnect];
}

-(void)sendGooglePlusLogin
{
    NSString* code = [GPPSignIn sharedInstance].homeServerAuthorizationCode;
    NSURL *url = [[NSURL alloc] initWithString:kURLGameloeLoginGoogle];
    NSMutableURLRequest *req = [[NSMutableURLRequest alloc] initWithURL:url
                                                            cachePolicy:NSURLRequestReloadIgnoringCacheData
                                                        timeoutInterval:30] ;
    [req setHTTPShouldHandleCookies:YES];
    [req setHTTPMethod:@"POST"];
    NSMutableData *body = [NSMutableData data];
    NSString *info = [NSString stringWithFormat:@"code=%@", code];
    [body appendData:[[NSString stringWithString:info] dataUsingEncoding:NSUTF8StringEncoding]];
    [req setHTTPBody:body];
    NSData *data = [NSURLConnection sendSynchronousRequest:req returningResponse:nil error:nil];
    NSString *returnString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"returnString  %@", returnString);
    NSDictionary *jsonDataDict = [NSJSONSerialization JSONObjectWithData:data options: NSJSONReadingMutableContainers error: nil];
    NSString  *sessionID;
    int status = [[jsonDataDict objectForKey:@"status"] integerValue];
    if (status == 1)
    {
        sessionID = [jsonDataDict objectForKey:@"sid"];
        string sid = std::string([sessionID UTF8String]);
        LoginHandler::getInstance()->getAccountSuccessed(sid);
        [[GPPSignIn sharedInstance] disconnect];
    }
    if (status == 0)
    {
        NSString* message = [jsonDataDict objectForKey:@"message"];
        string msg = std::string([message UTF8String]);
        LoginHandler::getInstance()->getAccountFail(msg);
    }

}
/*
 -(void)sendGooglePlusLogin
 {
 NSString* code = [GPPSignIn sharedInstance].homeServerAuthorizationCode;
 NSURL *url = [[NSURL alloc] initWithString:@"http://ids.gameloe.com/google/login?m=1"];
 //NSURL *url = [[NSURL alloc] initWithString:@"http://thapthanh.com/user/login"];
 NSMutableURLRequest *req = [[NSMutableURLRequest alloc] initWithURL:url
 cachePolicy:NSURLRequestReloadIgnoringCacheData
 timeoutInterval:30] ;
 [req setHTTPShouldHandleCookies:YES];
 [req setHTTPMethod:@"POST"];
 NSMutableData *body = [NSMutableData data];
 NSString *info = [NSString stringWithFormat:@"code=%@", code];
 [body appendData:[[NSString stringWithString:info] dataUsingEncoding:NSUTF8StringEncoding]];
 [req setHTTPBody:body];
 NSData *data = [NSURLConnection sendSynchronousRequest:req returningResponse:nil error:nil];
 NSString *returnString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
 NSLog(@"returnString  %@", returnString);}
 */

- (void) finishedWithAuth:(GTMOAuth2Authentication *)auth error:(NSError *)error
{
    if (error) {
        // Do some error handling here.
    } else {
        NSString *serverCode = [GPPSignIn sharedInstance].homeServerAuthorizationCode;
        NSLog(@"finishedWithAuth code %@", serverCode);
        [self sendGooglePlusLogin];
    }
}

- (void)didReceiveDeepLink:(GPPDeepLink *)deepLink {
    // An example to handle the deep link data.
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Deep-link Data"
                          message:[deepLink deepLinkID]
                          delegate:nil
                          cancelButtonTitle:@"OK"
                          otherButtonTitles:nil];
    [alert show];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    // Append the new data to the instance variable you declared
    //NSMutableData* _responseData = [[NSMutableData alloc] init];
    //[_responseData appendData:data];
    //NSString *strData = [[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    // Print out new string
    //NSLog(@"%@",strData);


}

void saveScreenShot(std::string imgSSPath)
{
    NSString *documentsDirectory = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSString* fileName = [NSString stringWithUTF8String:imgSSPath.c_str()];
    NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];
    UIImage* imgSS = [UIImage imageWithContentsOfFile:filePath];
    ALAuthorizationStatus status = [ALAssetsLibrary authorizationStatus];

    if (status == ALAuthorizationStatusNotDetermined) {
        ALAssetsLibrary *assetsLibrary = [[ALAssetsLibrary alloc] init];
        [assetsLibrary enumerateGroupsWithTypes:ALAssetsGroupAll usingBlock:^(ALAssetsGroup *group, BOOL *stop) {
            if (*stop) {
                // INSERT CODE TO PERFORM WHEN USER TAPS OK eg. :
                NSLog(@"Clicked ok");
                UIImageWriteToSavedPhotosAlbum(imgSS, nil, nil, nil);
                return;
            }
            *stop = TRUE;
        } failureBlock:^(NSError *error) {
            // INSERT CODE TO PERFORM WHEN USER TAPS DONT ALLOW, eg. :
            NSLog(@"User dont allow");
            //show alert for asking the user to give permission
            UIAlertView *alert = [[UIAlertView alloc]
                                  initWithTitle: @"Info"
                                  message: @"You need allow game access Photos by:\n Settings > Privacy > Photos"
                                  delegate: nil
                                  cancelButtonTitle:@"Close"
                                  otherButtonTitles:nil];
            [alert show];
            [alert release];
        }];
    }
    else
    {
        UIImageWriteToSavedPhotosAlbum(imgSS, nil, nil, nil);
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->pause(); */
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->resume(); */
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
    [window release];
    [super dealloc];
}

@end
