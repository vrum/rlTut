class Engine {
public :
    enum GameStatus {
        STARTUP,
        IDLE,
        NEW_TURN,
        VICTORY,
        DEFEAT
    } gameStatus;
	TCODList<Actor *> actors;
    Actor *player;
    Map *map;
    CaveGenerator *caveGen;

    Engine();
    ~Engine();
    void update();
    void render();
    int fovRadius;
private :
    bool computeFov;
};

extern Engine engine;