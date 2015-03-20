// rlTut.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "main.hpp"

Engine engine(96, 48);


int _tmain(int argc, _TCHAR *argv[]) {
	//amain(80, 48, 45, 5, 5, 2);

	while (!TCODConsole::isWindowClosed() ) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}

	return 0;
}