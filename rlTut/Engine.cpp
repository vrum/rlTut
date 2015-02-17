#include "libtcod.hpp"
#include "Actor.hpp"
#include "Cellular.hpp"
#include "Map.hpp"
#include "Engine.hpp"

int size_x = 64, size_y = 20;
int fillprob = 40;
generation_params genParams[] = {{5, 5, 5}};

Engine::Engine() {
    TCODConsole::initRoot(size_x, size_y, "rlTut", false);
    map = new Map(size_x, size_y);
	caveGen = new CaveGenerator();
	caveGen->init(size_x, size_y, fillprob, genParams, sizeof(genParams)/sizeof(generation_params));
	caveGen->generateMap();
	caveGen->exportMap(map);
	caveGen->reset();
	int yp = rand() % size_y-1 + 1;
	int xp =1;
	for (; xp < size_x-1; xp++) {
		if (!map->isWall(xp, yp))
			break;
	}
	player = new Actor(xp, yp, '@', TCODColor::white);
	actors.push(player);
    actors.push(new Actor(20, 13, 'D', TCODColor::yellow));
}

Engine::~Engine() {
    actors.clearAndDelete();
    delete map;
	delete caveGen;
}

void Engine::update() {
    TCOD_key_t key;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
    switch(key.vk) {
    case TCODK_UP :
        if ( ! map->isWall(player->x, player->y - 1)) {
            player->y--;
        }
        break;
    case TCODK_DOWN :
        if ( ! map->isWall(player->x, player->y + 1)) {
            player->y++;
        }
        break;
    case TCODK_LEFT :
        if ( ! map->isWall(player->x - 1, player->y)) {
            player->x--;
        }
        break;
    case TCODK_RIGHT :
        if ( ! map->isWall(player->x + 1, player->y)) {
            player->x++;
        }
        break;
    default:
        break;
    }
}

void Engine::render() {
    TCODConsole::root->clear();
    // draw the map
    map->render();
    // draw the actors
    for (Actor **iterator = actors.begin();
            iterator != actors.end(); iterator++) {
        (*iterator)->render();
    }
}