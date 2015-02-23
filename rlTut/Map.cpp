#include "libtcod.hpp"
#include "Actor.hpp"
#include "CaveGenerator.hpp"
#include "Engine.hpp"
#include "Map.hpp"
#include <stdio.h>

int aaa = 0;
class BspListener : public ITCODBspCallback {
private :
    Map &map; // a map to dig
    int roomNum; // room number
    int lastx,lasty; // center of the last room
public :
    BspListener(Map &map) : map(map), roomNum(0) {}
    bool visitNode(TCODBsp *node, void *userData) {
        if ( node->isLeaf() ) {
            int x,y,w,h;
            // dig a room
            TCODRandom *rng=TCODRandom::getInstance();
            w=rng->getInt(ROOM_MIN_HSIZE, node->w-2);
            h=rng->getInt(ROOM_MIN_VSIZE, node->h-2);
            x=rng->getInt(node->x+1, node->x+node->w-w-1);
            y=rng->getInt(node->y+1, node->y+node->h-h-1);
            printf("x:%d y:%d w:%d h:%d\n", x, y, w, h);
            map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1);
            if ( roomNum != 0 ) {
                // dig a corridor from last room
                map.dig(lastx, lasty, x+w/2, lasty);
                map.dig(x+w/2, lasty, x+w/2, y+h/2);
            }
            lastx=x+w/2;
            lasty=y+h/2;
            roomNum++;
        }
        return true;
    }
};

Map::Map(int width, int height) : width(width), height(height) {
    tiles = new Tile[width * height];
    /**/
    map=new TCODMap(width,height);
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 10, BSP_NODE_MIN_HSIZE, BSP_NODE_MIN_VSIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener,NULL);
    /**/
}

Map::~Map() {
    delete [] tiles;
    delete map;
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x,y);
}

bool Map::isExplored(int x, int y) const {
    return tiles[x+y*width].explored;
}

bool Map::isInFov(int x, int y) const {
    if ( map->isInFov(x,y) ) {
        tiles[x+y*width].explored=true;
        return true;
    }
    return false;
}

void Map::computeFov() {
    map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

void Map::render() const {
    static const TCODColor darkBg(0, 0, 0);
    static const TCODColor greyFg(160, 160, 160);
    static const TCODColor darkWall(0,0,100);
    static const TCODColor darkGround(50,50,150);
    static const TCODColor lightWall(130,110,50);
    static const TCODColor lightGround(200,180,50);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            TCODConsole::root->setChar(x, y, isWall(x, y) ? '#' : '.');
            TCODConsole::root->setCharForeground(x, y, TCODColor::lightGrey);
            TCODConsole::root->setCharBackground( x, y, TCODColor::black);
			TCODConsole::root->setCharForeground( x, y, TCODColor::black);
            if (isInFov(x,y) ) {
                TCODConsole::root->setCharForeground(x, y, TCODColor::lightGrey);
            } else if ( isExplored(x, y) ) {
                TCODConsole::root->setCharForeground(x, y, TCODColor::darkerGrey);
            }
        }
    }
}

void Map::dig(int x1, int y1, int x2, int y2) {
    if ( x2 < x1 ) {
        int tmp=x2;
        x2=x1;
        x1=tmp;
    }
    if ( y2 < y1 ) {
        int tmp=y2;
        y2=y1;
        y1=tmp;
    }
    for (int tilex=x1; tilex <= x2; tilex++) {
        for (int tiley=y1; tiley <= y2; tiley++) {
            map->setProperties(tilex,tiley,true,true);
        }
    }
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
    dig (x1,y1,x2,y2);
    if ( first ) {
        // put the player in the first room
        engine.player->x=(x1+x2)/2;
        engine.player->y=(y1+y2)/2;
    } else {
        TCODRandom *rng=TCODRandom::getInstance();
        if ( rng->getInt(0,3)==0 ) {
            engine.actors.push(new Actor((x1+x2)/2,(y1+y2)/2,'@',"orc",
                                         TCODColor::yellow));
        }
    }
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x,y)) {
        // this is a wall
        return false;
    }
    for (Actor **iterator=engine.actors.begin();
        iterator!=engine.actors.end();iterator++) {
        Actor *actor=*iterator;
        if ( actor->x == x && actor->y == y ) {
            // there is an actor there. cannot walk
            return false;
        }
    }
    return true;
}