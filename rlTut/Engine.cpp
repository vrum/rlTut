#include "libtcod.hpp"
#include "Actor.hpp"
#include "CaveGenerator.hpp"
#include "Map.hpp"
#include "Engine.hpp"

int size_x = 64, size_y = 20;
int fillprob = 40;
generation_params genParams[] = {{5, 2, 5}};

Engine::Engine(): fovRadius(5), computeFov(true) {
	TCODConsole::setCustomFont("terminal10x16_ro.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
    TCODConsole::initRoot(size_x+24, size_y+8, "rlTut", false);
    player = new Actor(40, 15, '@', TCODColor::white);
    actors.push(player);
    map = new Map(size_x, size_y);
//	caveGen->init(*map, fillprob, genParams, sizeof(genParams)/sizeof(generation_params));
#if 0
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
#endif

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
    case TCODK_KP8:
        if (!map->isWall(player->x, player->y - 1)) {
            player->y--;
            computeFov=true;
        }
        break;
    case TCODK_DOWN :
    case TCODK_KP2:
        if ( ! map->isWall(player->x, player->y + 1)) {
            player->y++;
            computeFov=true;
        }
        break;
    case TCODK_LEFT :
    case TCODK_KP4:
        if ( ! map->isWall(player->x - 1, player->y)) {
            player->x--;
            computeFov=true;
        }
        break;
    case TCODK_RIGHT :
    case TCODK_KP6:
        if ( ! map->isWall(player->x + 1, player->y)) {
            player->x++;
            computeFov=true;
        }
        break;
    case TCODK_KP1:
        if ( ! map->isWall(player->x - 1, player->y + 1)) {
            player->x--;
            player->y++;
            computeFov=true;
        }
        break;
    case TCODK_KP3:
        if ( ! map->isWall(player->x + 1, player->y + 1)) {
            player->x++;
            player->y++;
            computeFov=true;
        }
        break;
    case TCODK_KP7:
        if ( ! map->isWall(player->x - 1, player->y - 1)) {
            player->x--;
            player->y--;
            computeFov=true;
        }
        break;
    case TCODK_KP9:
        if ( ! map->isWall(player->x + 1, player->y - 1)) {
            player->x++;
            player->y--;
            computeFov=true;
        }
        break;

    default:
        break;
    }
    if ( computeFov ) {
        map->computeFov();
        computeFov=false;
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