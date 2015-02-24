#include "main.hpp"

int size_x = 64, size_y = 20;
int fillprob = 40;
generation_params genParams[] = {{5, 2, 5}};

Engine::Engine(int screenWidth, int screenHeight): gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth),screenHeight(screenHeight) {
	TCODConsole::setCustomFont("terminal10x16_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
    TCODConsole::initRoot(screenWidth, screenHeight, "rlTut", false);
    player = new Actor(40,25,'@',"player",TCODColor::white);
    player->destructible=new PlayerDestructible(30,2,"your cadaver");
    player->attacker=new Attacker(5);
    player->ai = new PlayerAi();
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
    if ( gameStatus == STARTUP ) map->computeFov();
    gameStatus=IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	player->update();
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
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor != player && map->isInFov(actor->x,actor->y) ) {
	        actor->render();
	    }
	}
	player->render();
	// show the player's stats
	TCODConsole::root->print(1,screenHeight-2, "HP : %d/%d",
		(int)player->destructible->hp,(int)player->destructible->maxHp);
}

void Engine::sendToBack(Actor *actor) {
    actors.remove(actor);
    actors.insertBefore(actor,0);
}