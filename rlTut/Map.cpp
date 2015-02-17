#include "libtcod.hpp"
#include "Map.hpp"

Map::Map(int width, int height) : width(width), height(height) {
    tiles = new Tile[width * height];
}

Map::~Map() {
    delete [] tiles;
}

bool Map::isWall(int x, int y) const {
    return !tiles[x + y * width].canWalk;
}

void Map::setWall(int x, int y) {
    tiles[x + y * width].canWalk = false;
}

void Map::render() const {
    static const TCODColor darkBg(0, 0, 0);
    static const TCODColor greyFg(160, 160, 160);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
			TCODConsole::root->setChar(x, y, isWall(x, y) ? '#' : '.');
            TCODConsole::root->setCharBackground( x, y, darkBg);
            TCODConsole::root->setCharForeground( x, y, greyFg);
        }
    }
}
