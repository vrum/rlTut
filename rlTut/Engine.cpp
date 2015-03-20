#include "main.hpp"

int size_x = 80, size_y = 25;
int fillprob = 15;
//generation_params genParams[] = {{5, 2, 5}};
generation_params genParams[] = {{5, 2, 4}};

Engine::Engine(int screenWidth, int screenHeight): gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::setCustomFont("..\\Data\\fonts\\terminal10x16_gs_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
	TCODConsole::initRoot(screenWidth, screenHeight, "rlTut", false);
	player = new Actor(40, 25, '@', "player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
    player->container = new Container(26);
	actors.push(player);
	caveGen = new CaveGenerator();
	map = new Map(size_x, size_y);
	map->print();
    gui = new Gui();
    gui->message(TCODColor::red, 
    	"Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");

#if 0
   caveGen->init(size_x, size_y, fillprob, genParams, sizeof(genParams)/sizeof(generation_params));
//	caveGen->init(*map, fillprob, genParams, sizeof(genParams) / sizeof(generation_params));
	caveGen->generateMap();
	Map *map1 = new Map(size_x, size_y);
	caveGen->exportMap(map1);
	map1->print();
	map->or(*map1);
	map->print();
 	caveGen->reset();
	int yp = rand() % size_y - 1 + 1;
	int xp = 1;
	for (; xp < size_x - 1; xp++) {
		if (!map->isWall(xp, yp))
			break;
	}
	delete map1;
#endif

}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete caveGen;
	delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
	gameStatus = IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    if ( lastKey.vk == TCODK_ENTER && lastKey.lalt ) {
    	TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
    }
	player->update();
	if ( gameStatus == NEW_TURN ) {
		for (Actor **iterator = actors.begin(); iterator != actors.end();
				iterator++) {
			Actor *actor = *iterator;
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
	for (Actor **iterator = actors.begin();
			iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if ( actor != player && map->isInFov(actor->x, actor->y) ) {
			actor->render();
		}
	}
	player->render();
	// show the player's stats
	gui->render();
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor->x == x && actor->y ==y && actor->destructible
			&& ! actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest=NULL;
	float bestDistance=1E6f;
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor != player && actor->destructible 
			&& !actor->destructible->isDead() ) {
			float distance=actor->getDistance(x,y);
			if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
				bestDistance=distance;
				closest=actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int *x, int *y, float maxRange) {
	while ( !TCODConsole::isWindowClosed() ) {
		render();
		// highlight the possible range
		for (int cx=0; cx < map->width; cx++) {
			for (int cy=0; cy < map->height; cy++) {
				if ( map->isInFov(cx,cy)
					&& ( maxRange == 0 || player->getDistance(cx,cy) <= maxRange) ) {
					TCODColor col=TCODConsole::root->getCharBackground(cx,cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx,cy,col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
		if ( map->isInFov(mouse.cx,mouse.cy)
			&& ( maxRange == 0 || player->getDistance(mouse.cx,mouse.cy) <= maxRange )) {
			TCODConsole::root->setCharBackground(mouse.cx,mouse.cy,TCODColor::white);
			if ( mouse.lbutton_pressed ) {
				*x=mouse.cx;
				*y=mouse.cy;
				return true;
			}
		} 
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}
