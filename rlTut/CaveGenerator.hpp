class Map;

int amain(int size_x, int size_y, int fillprob, int r1_c, int r2_c, int reps);

typedef struct {
	int r1_cutoff, r2_cutoff;
	int reps;
} generation_params;

class CaveGenerator {
public:
	CaveGenerator(): initialized(false), grid(NULL), grid2(NULL), params_set(NULL) {};
	~CaveGenerator() {
		if (initialized)
		{
			delete grid;
			delete grid2;
		}
		grid = NULL;
		grid2 = NULL;
		params_set = NULL;
	}
	void init(int size_x, int size_y, int fillprob, generation_params *genParams, int generations);
	void init(Map &map, int fillprob, generation_params *genParams, int generations);
	void reset();
	int generateMap();
	void generation(generation_params *params);
	void printfunc(void);
	int exportMap(Map *map);
	int exportMap(const char *filename);
private:
	int **grid;
	int **grid2;

	int fillprob;
	int size_x, size_y;
	generation_params *params_set;
	int generations;
	int randpick(void);
	bool initialized;
};
