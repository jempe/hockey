#include "HockeyScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

static const float PI = 3.1416;

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
	_bottomPlayerScore = 0;
	_topPlayerScore = 0;

    //////////////////////////////
    // 1. super init first
    if ( ! CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)))
    {
        return false;
    }

    _screenSize = CCDirector::sharedDirector()->getWinSize();

    // create court items
    _center_circle = CCSprite::create("circle.png");
    _center_circle->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(_center_circle);
    
    CCSprite * center_line = CCSprite::create("line.png");
    center_line->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(center_line);

    CCSprite * bottom_circle = CCSprite::create("circle.png");
    bottom_circle->setPosition(ccp(_screenSize.width * 0.5, 0));
    this->addChild(bottom_circle);

    CCSprite * top_circle = CCSprite::create("circle.png");
    top_circle->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height));
    this->addChild(top_circle);

    CCSprite * logo = CCSprite::create("jempe_logo.png");
    logo->setPosition(ccp(_screenSize.width / 2, (_screenSize.height / 2) + (logo->getContentSize().height / 8)));
    this->addChild(logo);

    //create table borders
    _table_left = CCSprite::create("table_side.png");
    _table_left->setPosition(ccp(_table_left->getContentSize().width * 0.5, _screenSize.height * 0.5));
    this->addChild(_table_left);

    CCSprite * table_right = CCSprite::create("table_side.png");
    table_right->setPosition(ccp(_screenSize.width - table_right->getContentSize().width * 0.5, _screenSize.height * 0.5));
    table_right->setRotation(180);
    this->addChild(table_right);

    _table_bottom_right = CCSprite::create("table_bottom.png");
    _table_bottom_right->setPosition(ccp(_screenSize.width + (_center_circle->getContentSize().width * 0.5), _table_bottom_right->getContentSize().height * 0.5));
    this->addChild(_table_bottom_right);

    CCSprite * table_bottom_left = CCSprite::create("table_bottom.png");
    table_bottom_left->setPosition(ccp(- (_center_circle->getContentSize().width * 0.5), table_bottom_left->getContentSize().height * 0.5));
    table_bottom_left->setScaleX(-1);
    this->addChild(table_bottom_left);

    CCSprite * table_top_right = CCSprite::create("table_bottom.png");
    table_top_right->setPosition(ccp(_screenSize.width + (_center_circle->getContentSize().width * 0.5), _screenSize.height - (table_top_right->getContentSize().height * 0.5)));
    table_top_right->setScaleY(-1);
    this->addChild(table_top_right);

    CCSprite * table_top_left = CCSprite::create("table_bottom.png");
    table_top_left->setPosition(ccp(- (_center_circle->getContentSize().width * 0.5), _screenSize.height - (table_top_left->getContentSize().height * 0.5)));
    table_top_left->setScaleY(-1);
    table_top_left->setScaleX(-1);
    this->addChild(table_top_left);


    // add corners
    CCSprite * corner_top_left = CCSprite::create("corner.png");
    corner_top_left->setPosition(ccp((corner_top_left->getContentSize().width / 2), _screenSize.height - (corner_top_left->getContentSize().height / 2)));
    this->addChild(corner_top_left);

    CCSprite * corner_top_right = CCSprite::create("corner.png");
    corner_top_right->setPosition(ccp(_screenSize.width - (corner_top_right->getContentSize().width / 2), _screenSize.height - (corner_top_right->getContentSize().height / 2)));
    corner_top_right->setRotation(90);
    this->addChild(corner_top_right);

    CCSprite * corner_bottom_right = CCSprite::create("corner.png");
    corner_bottom_right->setPosition(ccp(_screenSize.width - (corner_bottom_right->getContentSize().width / 2), (corner_bottom_right->getContentSize().height / 2)));
    corner_bottom_right->setRotation(180);
    this->addChild(corner_bottom_right);

    CCSprite * corner_bottom_left = CCSprite::create("corner.png");
    corner_bottom_left->setPosition(ccp((corner_bottom_left->getContentSize().width / 2), (corner_bottom_left->getContentSize().height / 2)));
    corner_bottom_left->setRotation(270);
    this->addChild(corner_bottom_left);

    // create score labels
    _top_player_score = CCLabelTTF::create("0", "Clubland", 60);
    _top_player_score->setPosition(ccp(_screenSize.width - 60, (_screenSize.height / 2) + 40));
    _top_player_score->setColor(ccc3(255, 0, 0));
    this->addChild(_top_player_score);

    _bottom_player_score = CCLabelTTF::create("0", "Clubland", 60);
    _bottom_player_score->setPosition(ccp(_screenSize.width - 60, (_screenSize.height / 2) - 40));
    _bottom_player_score->setColor(ccc3(255, 0, 0));
    this->addChild(_bottom_player_score);

    // create players mallets
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

				for(short int j = 0; j < _playersNumber; j++)
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

				for(short int j = 0; j < _playersNumber; j++)
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
								if(nextPos.y < player->get_radius() && nextPos.x > (_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) + _puck->get_radius() && nextPos.x < (_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) - _puck->get_radius())
								{
									nextPos.y = player->get_radius();
								}
								else
								{
									nextPos.y = player->get_radius() + _table_bottom_right->getContentSize().height;
								}
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
								if(nextPos.y > _screenSize.height - player->get_radius() && nextPos.x > (_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) + _puck->get_radius() && nextPos.x < (_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) - _puck->get_radius())
								{
									nextPos.y = _screenSize.height - player->get_radius();
								}
								else
								{
									nextPos.y = _screenSize.height - _table_bottom_right->getContentSize().height - player->get_radius();\
								}
							}
						}

						player->setNextPos(nextPos);
						player->setVector(ccp(nextPos.x - player->getPositionX(), nextPos.y - player->getPositionY()));
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

				for(short int j = 0; j < _playersNumber; j++)
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

/********************************************//**
 *  Method that checks the position of mallets and puck and update the position and vectors
 ***********************************************/

void HockeyScene::update(float dt)
{

	CCPoint puck_position = _puck->getPosition();
	VectorSprite * player;

	/**
	* detects collisions with mallets
	*/
	for(short int j = 0; j < _players->count(); j++)
	{
		player = (VectorSprite *) _players->objectAtIndex(j);

		CCPoint player_position = player->getNextPos();

		puckCollisionVector(player_position, player->get_radius(), player->getVector());
	}

	_puck->setVector(ccpMult(_puck->getVector(), _friction));

	/**
	* detect and handle collisions with the sides of the table
	*/

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

	/**
	* detect and handle collisions with the sides of the goal
	*/

	if((puck_next_position.x < (_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) + _puck->get_radius() || puck_next_position.x > (_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) - _puck->get_radius()) && (puck_next_position.y < 0 || puck_next_position.y > _screenSize.height))
	{
		_puck->setVector(ccp(-current_puck_vector.x, current_puck_vector.y));
	}

	/**
	* detect and handle collisions with bottom and top of the table
	*/

	if(puck_next_position.x < (_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) + _puck->get_radius() - _table_bottom_right->getContentSize().height || puck_next_position.x > (_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) - _puck->get_radius() + _table_bottom_right->getContentSize().height)
	{
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
	}

	/**
	* detect and handle collisions with goal corners
	*/

	// center points of the goal corners
	CCPoint goal_bottom_left = ccp((_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) - _table_bottom_right->getContentSize().height, 0);
	CCPoint goal_bottom_right = ccp((_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) + _table_bottom_right->getContentSize().height, 0);
	CCPoint goal_top_left = ccp((_screenSize.width / 2) - (_center_circle->getContentSize().width / 2) - _table_bottom_right->getContentSize().height, _screenSize.height);
	CCPoint goal_top_right = ccp((_screenSize.width / 2) + (_center_circle->getContentSize().width / 2) + _table_bottom_right->getContentSize().height, _screenSize.height);

	if(puck_next_position.y < _table_bottom_right->getContentSize().height + _puck->get_radius())
	{
		if(puck_next_position.x < _screenSize.width / 2)
		{
			puckCollisionVector(goal_bottom_left, _table_bottom_right->getContentSize().height, CCPoint (0, 0));
		}
		else
		{
			puckCollisionVector(goal_bottom_right, _table_bottom_right->getContentSize().height, CCPoint (0, 0));
		}
	}
	else if(puck_next_position.y > _screenSize.height - _table_bottom_right->getContentSize().height - _puck->get_radius())
	{
		if(puck_next_position.x < _screenSize.width / 2)
		{
			puckCollisionVector(goal_top_left, _table_bottom_right->getContentSize().height, CCPoint (0, 0));
		}
		else
		{
			puckCollisionVector(goal_top_right, _table_bottom_right->getContentSize().height, CCPoint (0, 0));
		}
	}

	/**
	* move puck to next position
	*/
	_puck->setPosition(puck_next_position);

	/**
	* move mallets to next position
	*/

	_bottomPlayer->setPosition(_bottomPlayer->getNextPos());
	_topPlayer->setPosition(_topPlayer->getNextPos());

	/**
	* detect goals
	*/

	if(_puck->getPositionY() < -_puck->get_radius())
	{
		playerScore(2);
	}

	if(_puck->getPositionY() > _screenSize.height + _puck->get_radius())
	{
		playerScore(1);
	}
}

/********************************************//**
 *  Update player score
 *  @param player int of the player
 *  	0 -> bottom player
 *  	1 -> top player
 ***********************************************/

void HockeyScene::playerScore(short int player)
{
	if(player > 1)
	{
		_topPlayerScore++;
	}
	else
	{
		_bottomPlayerScore++;
	}

	/**
	 *  display score
	 */

	char score[10];

	sprintf(score,"%d", _topPlayerScore);
	_top_player_score->setString(score);

	sprintf(score,"%d", _bottomPlayerScore);
	_bottom_player_score->setString(score);

	CCLog("top: %d bottom: %d", _topPlayerScore, _bottomPlayerScore);

	_puck->setVector(ccp(0, 0));

	CCPoint center = ccp(_screenSize.width / 2, _screenSize.height / 2);

	_puck->setNextPos(center);
	_puck->setPosition(center);
}

/********************************************//**
 *  SCENE destructor
 ***********************************************/

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

/********************************************//**
 *  Detects the collision of circular object with the puck and changes the puck vector
 ***********************************************/

void HockeyScene::puckCollisionVector(CCPoint objectCenter, float objectRadius, CCPoint objectVector)
{
	if(pow(objectCenter.x - _puck->getPositionX(),2) + pow(objectCenter.y - _puck->getPositionY(), 2) < pow(objectRadius + _puck->get_radius(), 2))
	{
		CCPoint current_puck_vector = _puck->getVector();

		float puck_vector_force = sqrt(pow(objectVector.x, 2) + pow(objectVector.y, 2) + pow(current_puck_vector.x, 2) + pow(current_puck_vector.y, 2)) * _friction;

		float puck_vector_angle = atan2(_puck->getPositionY() - objectCenter.y, _puck->getPositionX() - objectCenter.x);

		/**
		 *  check if object is a corner and dont let it overlap it
		 */

		if(objectCenter.y == 0 || objectCenter.y == _screenSize.height)
		{
			CCPoint new_puck_position = ccp(objectCenter.x + (sin(puck_vector_angle + PI) * (objectRadius + _puck->get_radius())), objectCenter.y + (cos(puck_vector_angle + PI) * (objectRadius + _puck->get_radius())));

			_puck->setPosition(new_puck_position);
		}

		CCPoint new_puck_vector = ccp(puck_vector_force * cos(puck_vector_angle), puck_vector_force * sin(puck_vector_angle));

		// multiply vector per vectors radio
		_puck->setVector(new_puck_vector);
	}
}
