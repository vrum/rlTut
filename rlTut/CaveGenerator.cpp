#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CaveGenerator.hpp"
#include "Map.hpp"

#define TILE_FLOOR 0
#define TILE_WALL 1

int CaveGenerator::randpick(void)
{
	if(rand() % 100 < fillprob)
		return TILE_WALL;
	else
		return TILE_FLOOR;
}

void CaveGenerator::init(int size_x, int size_y, int fillprob, generation_params *genParams, int generations)
{
	this->size_x = size_x;
	this->size_y = size_y;
	this->fillprob = fillprob;
	this->params_set = genParams;
	this->generations = generations;
	int xi, yi;

	srand((int)time(NULL));
	grid  = (int**)malloc(sizeof(int*) * size_y);
	grid2 = (int**)malloc(sizeof(int*) * size_y);

	for(yi = 0; yi < size_y; yi++)
	{
		grid [yi] = (int*)malloc(sizeof(int) * size_x);
		grid2[yi] = (int*)malloc(sizeof(int) * size_x);
	}
	for(yi = 1; yi < size_y - 1; yi++)
		for(xi = 1; xi < size_x - 1; xi++)
			grid[yi][xi] = randpick();

	for(yi = 0; yi < size_y; yi++)
		for(xi = 0; xi < size_x; xi++)
			grid2[yi][xi] = TILE_WALL;

	for(yi = 0; yi < size_y; yi++)
		grid[yi][0] = grid[yi][size_x - 1] = TILE_WALL;
	for(xi = 0; xi < size_x; xi++)
		grid[0][xi] = grid[size_y - 1][xi] = TILE_WALL;
	initialized = true;
}
	
void CaveGenerator::reset()
{
	for(int yi = 0; yi < size_y; yi++)
	{
		free(grid[yi]);
		free(grid2[yi]);
	}
	free(grid);
	free(grid2);
	initialized = false;
}

void CaveGenerator::generation(generation_params *params)
{
	int xi, yi, ii, jj;

	for(yi = 1; yi < size_y - 1; yi++)
		for(xi = 1; xi < size_x - 1; xi++)
		{
			int adjcount_r1 = 0,
				adjcount_r2 = 0;

			for(ii = -1; ii <= 1; ii++)
				for(jj = -1; jj <= 1; jj++)
				{
					if(grid[yi + ii][xi + jj] != TILE_FLOOR)
						adjcount_r1++;
				}
				for(ii = yi - 2; ii <= yi + 2; ii++)
					for(jj = xi - 2; jj <= xi + 2; jj++)
					{
						if(abs(ii - yi) == 2 && abs(jj - xi) == 2)
							continue;
						if(ii < 0 || jj < 0 || ii >= size_y || jj >= size_x)
							continue;
						if(grid[ii][jj] != TILE_FLOOR)
							adjcount_r2++;
					}
					if(adjcount_r1 >= params->r1_cutoff || adjcount_r2 <= params->r2_cutoff)
						grid2[yi][xi] = TILE_WALL;
					else
						grid2[yi][xi] = TILE_FLOOR;
		}
		for(yi = 1; yi < size_y - 1; yi++)
			for(xi = 1; xi < size_x - 1; xi++)
				grid[yi][xi] = grid2[yi][xi];
}

void CaveGenerator::printfunc(void)
{
	int ii;

	printf("W[0](p) = rand[0,100) < %i\n", fillprob);

	for(ii = 0; ii < generations; ii++)
	{
		printf("Repeat %i: W'(p) = R[1](p) >= %i",
			params_set[ii].reps, params_set[ii].r1_cutoff);

		if(params_set[ii].r2_cutoff >= 0)
			printf(" || R[2](p) <= %i\n", params_set[ii].r2_cutoff);
		else
			putchar('\n');
	}
}

void CaveGenerator::printmap(void)
{
	int xi, yi;

	for(yi = 0; yi < size_y; yi++)
	{
		for(xi = 0; xi < size_x; xi++)
		{
			switch(grid[yi][xi]) {
			case TILE_WALL:
				putchar('#');
				break;
			case TILE_FLOOR:
				putchar('.');
				break;
			}
		}
		putchar('\n');
	}
}

int CaveGenerator::generateMap()
{
	int ii, jj;

	generation_params *params;

	for(ii = 0; ii < generations; ii++)
	{
		params = &params_set[ii];
		for(jj = 0; jj < params->reps; jj++)
			generation(params);
	}

	printfunc();
	printmap();
	return 0;
}
#if 0
int main(int argc, char **argv)
{
	int ii, jj;

	if(argc < 7) {
		printf("Usage: %s xsize ysize fill (r1 r2 count)+\n", argv[0]);
		return 1;
	}
	size_x     = atoi(argv[1]);
	size_y     = atoi(argv[2]);
	fillprob   = atoi(argv[3]);

	generations = (argc-4)/3;

	params = params_set = (generation_params*)malloc( sizeof(generation_params) * generations );

	for(ii=4; ii+2<argc; ii+=3)
	{
		params->r1_cutoff  = atoi(argv[ii]);
		params->r2_cutoff  = atoi(argv[ii+1]);
		params->reps = atoi(argv[ii+2]);
		params++;
	}

	srand(time(NULL));

	initmap();

	for(ii=0; ii<generations; ii++)
	{
		params = &params_set[ii];
		for(jj=0; jj<params->reps; jj++)
			generation();
	}
	printfunc();
	printmap();
	return 0;
}
#endif //0
int CaveGenerator::exportMap(Map *map)
{
	if (!map)
		return -1;
	if (map->width != this->size_x || map->height != this->size_y)
		return -1;
	for (int x = 0; x < map->width; x++)
		for (int y = 0; y < map->height; y++)
			if (this->grid[y][x] == 1)
				map->setWall(x, y);
	return 0;
}