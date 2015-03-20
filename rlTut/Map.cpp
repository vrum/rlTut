#include "main.hpp"
#include <stdio.h>
static const int BSP_NODE_MIN_HSIZE = 12;
static const int BSP_NODE_MIN_VSIZE = 8;

static const int ROOM_MIN_HSIZE = 6;
static const int ROOM_MIN_VSIZE = 4;
static const int ROOM_MAX_HSIZE = 10;
static const int ROOM_MAX_VSIZE = 6;

static const int MAX_ROOM_MONSTERS = 2;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private :
	Map &map; // a map to dig
	int roomNum; // room number
	int lastx, lasty; // center of the last room
public :
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		if ( node->isLeaf() ) {
			int x, y, w, h;
			// dig a room
			TCODRandom *rng = TCODRandom::getInstance();
			w = rng->getInt(ROOM_MIN_HSIZE, ROOM_MAX_HSIZE);
			h = rng->getInt(ROOM_MIN_VSIZE, ROOM_MAX_VSIZE);
			x = rng->getInt(node->x +  1, node->x + node->w - w - 1);
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1);
			node->x = x;
			node->y = y;
			node->w = w;
			node->h = h;
			roomNum++;
		}
		else
		{
			TCODRandom *rng = TCODRandom::getInstance();
			TCODBsp *l = node->getLeft();
			TCODBsp *r = node->getRight();
			int x1, y1, x2, y2;
			do 
			{
				x1 = rng->getInt(l->x, l->x+l->w-1);
				y1 = rng->getInt(l->y, l->y+l->h/2);
				x2 = rng->getInt(r->x, r->x+r->w-1);
				y2 = rng->getInt(r->y, r->y+r->h/2-1);
			}
			while (map.isWall(x1,y1) || map.isWall(x2,y2));
			map.dig2(x1, y1, x2, y2);
			if (node->level < 0) 
			{
				int x11, y11, x22, y22;
				do 
				{
					x11 = rng->getInt(l->x, l->x+l->w-1);
					y11 = rng->getInt(l->y+l->h/2, l->y+l->h-1);
					x22 = rng->getInt(r->x, r->x+r->w-1);
					y22 = rng->getInt(r->y+r->h/2, r->y+r->h-1);
				}
				while (map.isWall(x11,y11) || map.isWall(x22,y22));
				map.dig2(x11, y11, x22, y22);
			}
		}
		return true;
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width * height];
	map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 5, BSP_NODE_MIN_HSIZE, BSP_NODE_MIN_VSIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
	delete [] tiles;
	delete map;
}

void Map::dig(int x1, int y1, int x2, int y2) {
	if ( x2 < x1 ) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if ( y2 < y1 ) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::dig2(int x1, int y1, int x2, int y2) {
	TCODRandom *rng = TCODRandom::getInstance();
	//	int xm = rng->getInt(mx, Mx);
	//	int ym = rng->getInt(my, My);
	int xm = (x1+x2)/2;
	int ym = (y1+y2)/2;
	int horiz = rng->getInt(0,1);
	if (horiz) {
		dig(x1, y1, x1, ym);
		dig(x2, y2, x2, ym);
		dig(x1, ym, x2, ym);
	}
	else
	{
		dig(x1, y1, xm, y1);
		dig(x2, y2, xm, y2);
		dig(xm, y1, xm, y2);
	}
}

void Map::addMonster(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	if ( rng->getInt(0, 100) < 80 ) {
		// create an orc
		Actor *orc = new Actor(x, y, 'o', "orc",
			TCODColor::desaturatedGreen);
		orc->destructible = new MonsterDestructible(10, 0, "dead orc");
		orc->attacker = new Attacker(3);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} else {
		// create a troll
		Actor *troll = new Actor(x, y, 'T', "troll",
			TCODColor::darkerGreen);
		troll->destructible = new MonsterDestructible(16, 1, "troll carcass");
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll);
	}
}

void Map::addItem(int x, int y) {
	TCODRandom *rng=TCODRandom::getInstance();
	int dice = rng->getInt(0,100);
	if ( dice < 70 ) {
		// create a health potion
		Actor *healthPotion=new Actor(x,y,'!',"health potion",
			TCODColor::violet);
		healthPotion->blocks=false;
		healthPotion->pickable=new Pickable(NULL,new HealthEffect(4,NULL));
		engine.actors.push(healthPotion);
	} else if ( dice < 70+10 ) {
		// create a scroll of lightning bolt 
		Actor *scrollOfLightningBolt=new Actor(x,y,'#',"scroll of lightning bolt",
			TCODColor::lightYellow);
		scrollOfLightningBolt->blocks=false;
		scrollOfLightningBolt->pickable=new Pickable(
			new TargetSelector(TargetSelector::CLOSEST_MONSTER,5),
			new HealthEffect(-20,"A lighting bolt strikes the %s with a loud thunder!\n"
				"The damage is %g hit points."));
		engine.actors.push(scrollOfLightningBolt);
	} else if ( dice < 70+10+10 ) {
		// create a scroll of fireball
		Actor *scrollOfFireball=new Actor(x,y,'#',"scroll of fireball",
			TCODColor::lightYellow);
		scrollOfFireball->blocks=false;
		scrollOfFireball->pickable=new Pickable(
			new TargetSelector(TargetSelector::SELECTED_RANGE,3),
			new HealthEffect(-12,"The %s gets burned for %g hit points."));
		engine.actors.push(scrollOfFireball);
	} else {
		// create a scroll of confusion
		Actor *scrollOfConfusion=new Actor(x,y,'#',"scroll of confusion",
			TCODColor::lightYellow);
		scrollOfConfusion->blocks=false;
		scrollOfConfusion->pickable=new Pickable(
			new TargetSelector(TargetSelector::SELECTED_MONSTER,5),
			new AiChangeEffect(new ConfusedMonsterAi(10),
				"The eyes of the %s look vacant,\nas he starts to stumble around!"));
		engine.actors.push(scrollOfConfusion);
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
	dig (x1, y1, x2, y2);
	if ( first ) {
		// put the player in the first room
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	} else {
		TCODRandom *rng = TCODRandom::getInstance();
		// add monsters
		int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if ( canWalk(x, y) ) {
				addMonster(x, y);
			}
			nbMonsters--;
		}
		// add items
		int nbItems=rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
		    int x=rng->getInt(x1,x2);
		    int y=rng->getInt(y1,y2);
    		if ( canWalk(x,y) ) {
				addItem(x,y);
			}
		    nbItems--;
		}
	}
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
	if (isWall(x, y)) {
		// this is a wall
		return false;
	}
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
			Actor *actor = *iterator;
			if ( actor->blocks && actor->x == x && actor->y == y ) {
				// there is a blocking actor here. cannot walk
				return false;
			}
	}
	return true;
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y * width].explored;
}

bool Map::isInFov(int x, int y) const {
	if ( x < 0 || x >= width || y < 0 || y >= height ) {
		return false;
	}
	if ( map->isInFov(x, y) ) {
		tiles[x + y * width].explored = true;
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
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			TCODConsole::root->setChar(x, y, isWall(x, y) ? '#' : '.');
			TCODConsole::root->setCharForeground(x, y, TCODColor::lightGrey);
			TCODConsole::root->setCharBackground( x, y, TCODColor::black);
			TCODConsole::root->setCharForeground( x, y, TCODColor::black);
//			if (isInFov(x, y) ) 
			{
				TCODConsole::root->setCharForeground(x, y, TCODColor::lightGrey);
			} 
/*
else if ( isExplored(x, y) ) {
				TCODConsole::root->setCharForeground(x, y, TCODColor::darkerGrey);
			}
		*/
		}	
	}
}

void Map::print(void)
{
	int xi, yi;

	for (yi = 0; yi < height; yi++)
	{
		for (xi = 0; xi < width; xi++)
		{
			if (isWall(xi,yi)) 
				putchar('#');
			else
				putchar('.');
		}
		putchar('\n');
	}
	putchar('\n');
}

void Map::or(const Map &map1)
{
	int xi, yi;
	for (yi = 0; yi < height; yi++)
	{
		for (xi = 0; xi < width; xi++)
		{
			if (map1.map->isWalkable(xi,yi) || this->map->isWalkable(xi,yi))
				this->map->setProperties(xi, yi, true, true);
			else
				this->map->setProperties(xi, yi, false, false);
		}
	}
}

void Map::and(const Map &map1)
{
	int xi, yi;
	for (yi = 0; yi < height; yi++)
	{
		for (xi = 0; xi < width; xi++)
		{
			if (map1.map->isWalkable(xi,yi) && this->map->isWalkable(xi,yi))
				this->map->setProperties(xi, yi, true, true);
			else
				this->map->setProperties(xi, yi, false, false);
		}
	}
}