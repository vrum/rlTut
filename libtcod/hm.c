#include <stdlib.h>
#include "libtcod.h"
// size of the heightmap
#define HM_WIDTH 160
#define HM_HEIGHT 96
TCOD_random_t rnd=NULL;
TCOD_noise_t noise=NULL;
#include <math.h>
void addHill(TCOD_heightmap_t *hm,int nbHill, float baseRadius, float radiusVar, float height)  {
	int i;
	for (i=0; i<  nbHill; i++ ) {
		float hillMinRadius=baseRadius*(1.0f-radiusVar);
		float hillMaxRadius=baseRadius*(1.0f+radiusVar);
		float radius = TCOD_random_get_float(rnd,hillMinRadius, hillMaxRadius);
		float theta = TCOD_random_get_float(rnd,0.0f, 6.283185f); // between 0 and 2Pi
		float dist = TCOD_random_get_float(rnd,0.0f, (float)MIN(HM_WIDTH,HM_HEIGHT)/2 - radius);
		int xh = (int) (HM_WIDTH/2 + cos(theta) * dist);
		int yh = (int) (HM_HEIGHT/2 + sin(theta) * dist);
		TCOD_heightmap_add_hill(hm,(float)xh,(float)yh,radius,height);
	}
}
// function building the heightmap
void buildMap(TCOD_heightmap_t *hm) {
//	addHill(hm,100,10,0.5,0.5);
	TCOD_heightmap_add_fbm(hm,noise,1,1,0,0,6,0.5,1);
	TCOD_heightmap_add(hm,-0.63);
	TCOD_heightmap_clamp(hm,0.0f,1.0f);
	TCOD_heightmap_normalize(hm,0,1);
	TCOD_heightmap_add_fbm(hm,noise,1,1,0,0,15,0.2,0.4);
	TCOD_heightmap_normalize(hm,0,1);
//	addHill(hm,25,10,0.5,0.27);
}
// test code to print the heightmap
// to compile this file on Linux :
//  gcc hm.c -o hm -I include/ -L . -ltcod
// to compile this file on Windows/mingw32 :
//  gcc hm.c -o hm.exe -I include/ -L lib -ltcod-mingw
const  uint8 TERRAIN_TILE_TYPE_BLANK = ' ';
const  uint8 TERRAIN_TILE_TYPE_FOREST = '*';
const  uint8 TERRAIN_TILE_TYPE_DEEP_WATER = '~';
const  uint8 TERRAIN_TILE_TYPE_SHALLOW_WATER = '~';
const  uint8 TERRAIN_TILE_TYPE_PLAINS = '.';
const  uint8 TERRAIN_TILE_TYPE_COASTLINE = '.';
const  uint8 TERRAIN_TILE_TYPE_MOUNTAIN = '^';
const  uint8 TERRAIN_TILE_TYPE_HILL = '~';


const TCOD_color_t TILE_COLOR_BLACK = {0x00,0x00,0x00};
const TCOD_color_t TILE_COLOR_BLUE = {0x00, 0x00, 0xfc};
const TCOD_color_t TILE_COLOR_BROWN = {0xa4, 0x75, 0x41};
const TCOD_color_t TILE_COLOR_GREEN = {0x00, 0x99, 0x00};
const TCOD_color_t TILE_COLOR_LIGHT_BLUE = {0x11, 0x8c, 0xff};
const TCOD_color_t TILE_COLOR_LIGHT_GRAY = {0xd2, 0xd2, 0xd2};
const TCOD_color_t TILE_COLOR_LIME_GREEN = {0x00, 0xFF, 0x00};
const TCOD_color_t TILE_COLOR_WHITE = {0xFF, 0xFF, 0xFF};
const TCOD_color_t TILE_COLOR_YELLOW = {0xFF, 0xFF, 0x00};

const unsigned int SHALLOW_WATER_THRESHOLD = 40;
const unsigned int COASTLINE_THRESHOLD = 15;
const unsigned int PLAINS_THRESHOLD = 35;
const unsigned int MOUNTAIN_THRESHOLD = 25;
const unsigned int HILL_THRESHOLD = 50;
int main(int argc, char *argv[]) {
	int x,y;
	TCOD_heightmap_t *hm=TCOD_heightmap_new(HM_WIDTH,HM_HEIGHT);
	rnd=TCOD_random_new_from_seed(TCOD_RNG_CMWC, 5U);
	noise=TCOD_noise_new(2,TCOD_NOISE_DEFAULT_HURST,TCOD_NOISE_DEFAULT_LACUNARITY,rnd);
	buildMap(hm);
	TCOD_console_init_root(HM_WIDTH,HM_HEIGHT,"height map test",false, TCOD_RENDERER_SDL);
	for (x=0; x < HM_WIDTH; x ++ ) {
		for (y=0;y < HM_HEIGHT; y++ ) {
			float z = TCOD_heightmap_get_value(hm,x,y);
			uint8 val=(uint8)(z*255);
			uint8 ch =' ';
			uint8 waterLine = 32;
			const TCOD_color_t *fc = NULL, *bc = NULL;
			if (val < waterLine)
			{
				if (val > waterLine - SHALLOW_WATER_THRESHOLD)
				{
					ch = TERRAIN_TILE_TYPE_SHALLOW_WATER;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_LIGHT_BLUE;
				}
				else
				{
					ch = TERRAIN_TILE_TYPE_DEEP_WATER;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_BLUE;
				}
			}
			else
			{
				if (val < waterLine + COASTLINE_THRESHOLD)
				{
					ch = TERRAIN_TILE_TYPE_COASTLINE;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_YELLOW;
				}
				else if (val < waterLine + PLAINS_THRESHOLD)
				{
					ch = TERRAIN_TILE_TYPE_PLAINS;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_LIME_GREEN;
				}
				else if (val > 255 - MOUNTAIN_THRESHOLD)
				{
					ch = TERRAIN_TILE_TYPE_MOUNTAIN;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_LIGHT_GRAY;
				}
				else if (val > 255 - HILL_THRESHOLD)
				{
					ch = TERRAIN_TILE_TYPE_HILL;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_BROWN;
				}
				else
				{
					ch = TERRAIN_TILE_TYPE_FOREST;
					bc = &TILE_COLOR_BLACK;
					fc = &TILE_COLOR_GREEN;
				}
			}
			TCOD_console_set_char_background(NULL,x,y,*bc, TCOD_BKGND_SET);
			TCOD_console_set_char_foreground(NULL,x,y,*fc, TCOD_BKGND_SET);
			TCOD_console_set_char(NULL, x, y, ch);
		}
	}
	TCOD_console_flush();
	TCOD_console_wait_for_keypress(true);
	return 0;
}
