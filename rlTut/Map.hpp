static const int BSP_NODE_MIN_HSIZE = 8;
static const int BSP_NODE_MIN_VSIZE = 8;

static const int ROOM_MIN_HSIZE = 6;
static const int ROOM_MIN_VSIZE = 6;

struct Tile {
    bool explored;
    Tile() : explored(false) {}
};

class BspListener;

class Map {
public :
    int width, height;

    Map(int width, int height);
    ~Map();
    bool isWall(int x, int y) const;
    void render() const;
    bool isInFov(int x, int y) const;
    bool isExplored(int x, int y) const;
    void computeFov();
	bool canWalk(int x, int y) const;
protected :
    Tile *tiles;
    TCODMap *map;
    friend class BspListener;
    friend class CaveGenerator;
    void dig(int x1, int y1, int x2, int y2);
    void createRoom(bool first, int x1, int y1, int x2, int y2);
};
