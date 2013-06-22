#include "HockeyScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HockeyScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HockeyScene *layer = HockeyScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HockeyScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( ! CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)))
    {
        return false;
    }

    _screenSize = CCDirector::sharedDirector()->getWinSize();

    // create court items
    CCSprite * center_circle = CCSprite::create("circle.png");
    center_circle->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(center_circle);
    
    CCSprite * center_line = CCSprite::create("line.png");
    center_line->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(center_line);

    CCSprite * bottom_circle = CCSprite::create("circle.png");
    bottom_circle->setPosition(ccp(_screenSize.width * 0.5, 0));
    this->addChild(bottom_circle);

    CCSprite * top_circle = CCSprite::create("circle.png");
    top_circle->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height));
    this->addChild(top_circle);


    //create table borders
    CCSprite * table_left = CCSprite::create("table_side.png");
    table_left->setPosition(ccp(table_left->getContentSize().width * 0.5, _screenSize.height * 0.5));
    this->addChild(table_left);

    CCSprite * table_right = CCSprite::create("table_side.png");
    table_right->setPosition(ccp(_screenSize.width - table_right->getContentSize().width * 0.5, _screenSize.height * 0.5));
    table_right->setRotation(180);
    this->addChild(table_right);


    // created players mallets
    _topPlayer = VectorSprite::vectorSpriteWithFile("mallet.png");
    _topPlayer->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.75));
    this->addChild(_topPlayer);

    _bottomPlayer = VectorSprite::vectorSpriteWithFile("mallet.png");
    _bottomPlayer->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.25));
    this->addChild(_bottomPlayer);

    _players = CCArray::create(_bottomPlayer, _topPlayer, NULL);
    _players->retain();


    // create puck
    _puck = VectorSprite::vectorSpriteWithFile("puck.png");
    _puck->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(_puck);

    // listen for touches
    this->setTouchEnabled(true);
    this->schedule(schedule_selector(HockeyScene::update));

    return true;
}

void HockeyScene::ccTouchesBegan(CCset * touches, CCEvent * event)
{

}

void HockeyScene::update(float dt)
{

}

HockeyScene::~HockeyScene()
{
	CC_SAFE_RELEASE(_players);
}

void HockeyScene::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
