#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 1. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add map
    std::string file = "medfan.tmx";
    auto str = String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(file.c_str()).c_str());
    _tileMap = TMXTiledMap::createWithXML(str->getCString(),"");
    _background = _tileMap->layerNamed("background");
    addChild(_tileMap, 0);

    /////////////////////////////
    // 4. add player
    TMXObjectGroup *objectGroup = _tileMap->getObjectGroup("objects");
    CCASSERT(NULL != objectGroup, "'objects' objectGroup not find !");

    if(objectGroup == NULL){
    	return false;
    }

    ValueMap spawnPoint = objectGroup->getObject("SpawnPoint");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint not find !");

    cocos2d::log("----> Nom : %s", spawnPoint["name"].asString().c_str());

    int id1sP = spawnPoint["id"].asInt();
    int x = spawnPoint["x"].asInt();
    int y = spawnPoint["y"].asInt();
    cocos2d::log("----> spawnPoint[] id(%i)->(%i,%i)",id1sP,x,y);

    int id2sP = spawnPoint.at("id").asInt();
    int x2 = spawnPoint.at("x").asInt();
    int y2 = spawnPoint.at("y").asInt();
    cocos2d::log("----> spawnPoint.at() id(%i)->(%i,%i)",id2sP,x2,y2);

    //* auto& objects = objectGroup->getObjects();
    //* for (auto &obj : objects) {
    //*    auto &properties = obj.asValueMap();
    //*    cocos2d::log("Type = %f",properties["x"].asFloat());
    //*}

    _player = new Sprite();
    _player->initWithFile("Player.png");
    _player->setPosition(Vec2(x,y));

    this->addChild(_player);
    this->setViewPointCenter(_player->getPosition());

    /////////////////////
    // 5. Touch
    this->setTouchEnabled(true);

    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);

    /////////////////////
    // 6. collisions
    _meta = _tileMap->layerNamed("meta");
    _meta->setVisible(false);

    return true;
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event){

	auto actionTo1 = RotateTo::create(0, 0, 180);
	auto actionTo2 = RotateTo::create(0, 0, 0);

	Point touchLocation = touch->getLocationInView();
	touchLocation = Director::sharedDirector()->convertToGL(touchLocation);
	touchLocation = this->convertToNodeSpace(touchLocation);

	Point playerPos = _player->getPosition();
	Point diff = touchLocation - playerPos;

	if (abs(diff.x) > abs(diff.y)) {
		if (diff.x > 0) {
			playerPos.x += _tileMap->getTileSize().width;
			_player->runAction(actionTo2);
		}else{
			playerPos.x -= _tileMap->getTileSize().width;
			_player->runAction(actionTo1);
		}
	}else{
		if (diff.y > 0) {
			playerPos.y += _tileMap->getTileSize().height;
		}else{
			playerPos.y -= _tileMap->getTileSize().height;
		}
 	}

	if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getMapSize().width) &&
	 playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getMapSize().height) &&
	 playerPos.y >= 0 && playerPos.x >= 0)
	{
		this->setPlayerPosition(playerPos);
 	}

	this->setViewPointCenter(_player->getPosition());

}
void HelloWorld::setPlayerPosition(Point position)
{

    cocos2d::log("setPlayerPosition(position.y=%f", position.y);
    Point tileCoord = this->tileCoordForPosition(position);

    //----------------------------------------
    int tileGid = _meta->tileGIDAt(tileCoord);

    if (tileGid) {
    	cocos2d::log("after if(tileGid)");
        ValueMap properties = _tileMap->getPropertiesForGID(tileGid).asValueMap();
        cocos2d::log("properties.size=%i", properties.size());
        if (properties.size()>0){
        	const bool collision = properties.at("Collidable").asBool();
        	if (collision == true){
        		cocos2d::log("collision=TRUE");
        	}else{
        		cocos2d::log("collision=FALSE");
        	}
        }
        /* ------------------- WHEN COLLISION WILL BE RESOLVED, LAST THING TO DO
        if (properties) {
            collision = properties->valueForKey("Collidable");
            if (collision && (collision->compare("True") == 0)) {
                return;
            }
        }
        ----------------- */
    }

	_player->setPosition(position);
}

Point HelloWorld::tileCoordForPosition(Point position)
{
	int x = position.x / _tileMap->getTileSize().width;
	int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height)- position.y) / _tileMap->getTileSize().height;
	return Vec2(x,y);
}

void HelloWorld::setViewPointCenter(Point position){

	Size winSize = Director::getInstance()->getWinSize();

	int x = MAX(position.x, winSize.width/2);
	int y = MAX(position.y, winSize.height/2);

	x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width)-winSize.width/2);
	y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height)-winSize.height/2);

	Point actualPosition = Vec2(x,y);

	Point centerOfView = Vec2(winSize.width/2, winSize.height/2);
	//** Point viewPoint = ccpSub(centerOfView, actualPosition);
	Point viewPoint = centerOfView - actualPosition;
	this->setPosition(viewPoint);

}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}
