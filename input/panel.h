#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

struct Panel{
	bool in_use;
	//Buttons:
	//2 position swicthes:
	//3 position switches: 
	//todo: remove this & put in main
	//10 position switches:
	int auto_select;//0-9
	//Dials:
	Panel();
};

bool operator!=(Panel const&,Panel const&);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);
Joystick_data driver_station_input_rand();
Panel rand(Panel*);

#endif
