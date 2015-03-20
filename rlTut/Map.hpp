
struct Tile {
	bool explored;
	Tile() : explored(false) {}
};

class Map {
public :
	int width, height;

	Map(int width, int height);
	~Map();
	bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	bool canWalk(int x, int y) const;
	void computeFov();
	void render() const;
	void print(void);
	void or(const Map &map);
	void and(const Map &map);
protected :
	Tile *tiles;
	TCODMap *map;
	friend class BspListener;
	friend class CaveGenerator;
	void dig(int x1, int y1, int x2, int y2);
	void dig2(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2);
	void addMonster(int x, int y);
};
