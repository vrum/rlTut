#include "libtcod.hpp"
#include "Actor.hpp"
#include "CaveGenerator.hpp"
#include "Map.hpp"
#include "Engine.hpp"

int size_x = 64, size_y = 20;
int fillprob = 40;
generation_params genParams[] = {{5, 2, 5}};

Engine::Engine(): gameStatus(STARTUP), fovRadius(10) {
	TCODConsole::setCustomFont("terminal10x16_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
    TCODConsole::initRoot(size_x+24, size_y+8, "rlTut", false);
    player = new Actor(40,25,'@',"player",TCODColor::white);
    actors.push(player);
    caveGen = new CaveGenerator();
    map = new Map(size_x, size_y);
#if 0
	caveGen->init(*map, fillprob, genParams, sizeof(genParams)/sizeof(generation_params));
    caveGen->generateMap();
    caveGen->exportMap(map);
//    caveGen->init(size_x, size_y, fillprob, genParams, sizeof(genParams)/sizeof(generation_params));
    caveGen->reset();
    int yp = rand() % size_y-1 + 1;
    int xp =1;
    for (; xp < size_x-1; xp++) {
        if (!map->isWall(xp, yp))
            break;
    }
#endif

}

Engine::~Engine() {
    actors.clearAndDelete();
    delete map;
    delete caveGen;
}

void Engine::update() {
    TCOD_key_t key;
    if ( gameStatus == STARTUP ) map->computeFov();
    gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
	int dx=0,dy=0;
    switch(key.vk) {
    case TCODK_UP :
    case TCODK_KP8: dy = -1; break;
    case TCODK_DOWN:
	case TCODK_KP2: dy = 1; break;
	case TCODK_LEFT:
	case TCODK_KP4: dx = -1; break;
    case TCODK_KP6:
	case TCODK_RIGHT: dx = 1; break;
    case TCODK_KP1: dx = -1; dy = 1; break;
    case TCODK_KP3: dx = 1; dy = 1; break;
    case TCODK_KP7: dx = -1; dy = -1;break;
    case TCODK_KP9: dx = 1; dy = -1;break;
	default: break;
	}
	if ( dx != 0 || dy != 0 ) {
		gameStatus=NEW_TURN;
		if ( player->moveOrAttack(player->x+dx,player->y+dy) ) {
			map->computeFov();
		}
	}
	if ( gameStatus == NEW_TURN ) {
		for (Actor **iterator=actors.begin(); iterator != actors.end();
			iterator++) {
				Actor *actor=*iterator;
				if ( actor != player ) {
					actor->update();
				}
		}
	}
}

void Engine::render() {
    TCODConsole::root->clear();
    // draw the map
    map->render();
    // draw the actors
    for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
        Actor *actor=*iterator;
        if (map->isInFov(actor->x,actor->y) ) {
            actor->render();
        }
    }
}