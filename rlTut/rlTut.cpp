// rlTut.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "libtcod.hpp"
#include "Actor.hpp"
#include "Cellular.hpp"
#include "Map.hpp"
#include "Engine.hpp"

Engine engine;


int _tmain(int argc, _TCHAR* argv[]) {
	//amain(80, 48, 45, 5, 5, 2);

	while (!TCODConsole::isWindowClosed() ) {
        engine.update();
        engine.render();
        TCODConsole::flush();
    }

    return 0;
}