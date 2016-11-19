#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void setViewPointCenter(Point position);

    // Gerer les mouvements
    void setPlayerPosition(Point position);

    bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);

	// Gerer les collisions
	Point tileCoordForPosition(Point position);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    TMXTiledMap *_tileMap;
    TMXLayer *_background;

    TMXLayer *_meta;

    Sprite *_player;

    std::string collision;

};

#endif // __HELLOWORLD_SCENE_H__
