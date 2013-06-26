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
	_gamePaused = false;
	_playersNumber = 2;
	_friction = 0.98;

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
    _table_left = CCSprite::create("table_side.png");
    _table_left->setPosition(ccp(_table_left->getContentSize().width * 0.5, _screenSize.height * 0.5));
    this->addChild(_table_left);

    CCSprite * table_right = CCSprite::create("table_side.png");
    table_right->setPosition(ccp(_screenSize.width - table_right->getContentSize().width * 0.5, _screenSize.height * 0.5));
    table_right->setRotation(180);
    this->addChild(table_right);

    _table_bottom_right = CCSprite::create("table_bottom.png");
    _table_bottom_right->setPosition(ccp(_screenSize.width + (center_circle->getContentSize().width * 0.5), _table_bottom_right->getContentSize().height * 0.5));
    this->addChild(_table_bottom_right);

    CCSprite * table_bottom_left = CCSprite::create("table_bottom.png");
    table_bottom_left->setPosition(ccp(- (center_circle->getContentSize().width * 0.5), table_bottom_left->getContentSize().height * 0.5));
    table_bottom_left->setScaleX(-1);
    this->addChild(table_bottom_left);

    CCSprite * table_top_right = CCSprite::create("table_bottom.png");
    table_top_right->setPosition(ccp(_screenSize.width + (center_circle->getContentSize().width * 0.5), _screenSize.height - (table_top_right->getContentSize().height * 0.5)));
    table_top_right->setScaleY(-1);
    this->addChild(table_top_right);

    CCSprite * table_top_left = CCSprite::create("table_bottom.png");
    table_top_left->setPosition(ccp(- (center_circle->getContentSize().width * 0.5), _screenSize.height - (table_top_left->getContentSize().height * 0.5)));
    table_top_left->setScaleY(-1);
    table_top_left->setScaleX(-1);
    this->addChild(table_top_left);


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
    _puck->setPosition(ccp(_screenSize.width / 2, _screenSize.height / 2));
    this->addChild(_puck);

    // listen for touches
    this->setTouchEnabled(true);
    this->schedule(schedule_selector(HockeyScene::update));

    return true;
}

void HockeyScene::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator i;
	CCTouch* touch;
	CCPoint tap;
	VectorSprite * player;

	if( ! _gamePaused)
	{
		for(i = touches->begin(); i != touches->end(); i++)
		{
			touch = (CCTouch*) (*i);

			if(touch)
			{
				tap = touch->getLocation();

				for(int j = 0; j < _playersNumber; j++)
				{
					player = (VectorSprite *) _players->objectAtIndex(j);

					if(player->boundingBox().containsPoint(tap))
					{
						player->setTouch(touch);
						CCLog("player: %d", j);
					}
				}
			}
		}
	}
}

void HockeyScene::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
	CCSetIterator i;
	CCTouch* touch;
	CCPoint tap;
	VectorSprite * player;

	if( ! _gamePaused)
	{
		for(i = touches->begin(); i != touches->end(); i++)
		{
			touch = (CCTouch*) (*i);

			if(touch)
			{
				tap = touch->getLocation();

				for(int j = 0; j < _playersNumber; j++)
				{
					player = (VectorSprite *) _players->objectAtIndex(j);

					if(player->getTouch() != NULL && player->getTouch() == touch)
					{
						CCPoint nextPos = tap;

						if(nextPos.x < _table_left->getContentSize().width + player->get_radius())
						{
							nextPos.x = _table_left->getContentSize().width + player->get_radius();
						}

						if(nextPos.x > _screenSize.width - _table_left->getContentSize().width - player->get_radius())
						{
							nextPos.x = _screenSize.width - _table_left->getContentSize().width - player->get_radius();
						}

						if(j == 0)
						{
							if(nextPos.y > (_screenSize.height / 2) - player->get_radius())
							{
								nextPos.y = (_screenSize.height / 2) - player->get_radius();
							}

							if(nextPos.y < (player->get_radius() + _table_bottom_right->getContentSize().height))
							{
								nextPos.y = player->get_radius() + _table_bottom_right->getContentSize().height;
							}
						}

						if(j == 1)
						{
							if(nextPos.y < (_screenSize.height / 2) + player->get_radius())
							{
								nextPos.y = (_screenSize.height / 2) + player->get_radius();
							}

							if(nextPos.y > _screenSize.height - _table_bottom_right->getContentSize().height - player->get_radius())
							{
								nextPos.y = _screenSize.height - _table_bottom_right->getContentSize().height - player->get_radius();
							}
						}

						player->setNextPos(nextPos);
						player->setVector(ccp(tap.x - player->getPositionX(), tap.y - player->getPositionY()));
					}
				}
			}
		}
	}
}

void HockeyScene::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	CCSetIterator i;
	CCTouch* touch;
	CCPoint tap;
	VectorSprite * player;

	if( ! _gamePaused)
	{
		for(i = touches->begin(); i != touches->end(); i++)
		{
			touch = (CCTouch*) (*i);

			if(touch)
			{
				tap = touch->getLocation();

				for(int j = 0; j < _playersNumber; j++)
				{
					player = (VectorSprite *) _players->objectAtIndex(j);

					if(player->getTouch() != NULL && player->getTouch() == touch)
					{
						player->setTouch(NULL);
						player->setVector(ccp(0, 0));
					}
				}
			}
		}
	}
}

void HockeyScene::update(float dt)
{
	// detect collision
	CCPoint puck_position = _puck->getPosition();
	VectorSprite * player;

	float puck_vector_length = ccpLength(_puck->getVector());

	for(int j = 0; j < _players->count(); j++)
	{
		player = (VectorSprite *) _players->objectAtIndex(j);

		CCPoint player_position = player->getNextPos();

		if(pow(player_position.x - puck_position.x,2) + pow(player_position.y - puck_position.y, 2) < pow(player->get_radius() + _puck->get_radius(), 2))
		{
			CCLog("collision player: %d", j);

			// get the angle of center of puck and center of player
			CCPoint angle_vector = ccp(_puck->getPositionX() - player_position.x, _puck->getPositionY() - player_position.y);

			// multiply vector per vectors radio
			_puck->setVector(ccpMult(angle_vector, (ccpLength(player->getVector()) + puck_vector_length) / 1.5/ ccpLength(angle_vector)));
		}
	}

	_puck->setVector(ccpMult(_puck->getVector(), _friction));

	CCPoint puck_next_position = ccpAdd(_puck->getPosition(), _puck->getVector());

	CCPoint current_puck_vector = _puck->getVector();

	if(puck_next_position.x < _table_left->getContentSize().width + _puck->get_radius())
	{
		puck_next_position.x = _table_left->getContentSize().width + _puck->get_radius();

		if(current_puck_vector.x < 0)
		{
			_puck->setVector(ccp(- current_puck_vector.x, current_puck_vector.y));
		}
	}
	else if(puck_next_position.x > _screenSize.width - _table_left->getContentSize().width - _puck->get_radius())
	{
		puck_next_position.x = _screenSize.width - _table_left->getContentSize().width - _puck->get_radius();

		if(current_puck_vector.x > 0)
		{
			_puck->setVector(ccp(- current_puck_vector.x, current_puck_vector.y));
		}
	}

	if(puck_next_position.y < _table_bottom_right->getContentSize().height + _puck->get_radius())
	{
		puck_next_position.y = _table_bottom_right->getContentSize().height + _puck->get_radius();

		if(current_puck_vector.y < 0)
		{
			_puck->setVector(ccp(current_puck_vector.x, -current_puck_vector.y));
		}
	}
	else if(puck_next_position.y > _screenSize.height -_table_bottom_right->getContentSize().height - _puck->get_radius())
	{
		puck_next_position.y = _screenSize.height -_table_bottom_right->getContentSize().height - _puck->get_radius();

		if(current_puck_vector.y > 0)
		{
			_puck->setVector(ccp(current_puck_vector.x, -current_puck_vector.y));
		}
	}

	_puck->setPosition(puck_next_position);

	_bottomPlayer->setPosition(_bottomPlayer->getNextPos());
	_topPlayer->setPosition(_topPlayer->getNextPos());
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
