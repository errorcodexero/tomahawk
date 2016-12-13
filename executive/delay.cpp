#include "delay.h"

#include "teleop.h" 
#include "auto_null.h"
#include "auto_forward.h"

using namespace std;

using Mode=Executive;

static const unsigned int AUTO_SELECTOR_PORT_1 = 0, AUTO_SELECTOR_PORT_2 = 1;//there is a three position switch plugged into two of the dios on the roborio used as an auto-selector

Executive auto_mode_convert(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.in.digital_io.in[AUTO_SELECTOR_PORT_1] == Digital_in::_1) return Mode{Auto_forward()};
	else if(info.in.digital_io.in[AUTO_SELECTOR_PORT_2] == Digital_in::_1) return Mode{Delay()};//there isn't another auto mode that we want to use yet
	return Mode{Delay()};
}

Mode Delay::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > 8) return auto_mode_convert(info);
	return Mode{Delay()};
}

Toplevel::Goal Delay::run(Run_info){
	//do nothing
	return {};
}

bool Delay::operator==(Delay const&)const{ return 1; }

#ifdef DELAY_TEST
#include "test.h"
int main(){
	Delay a;
	test_executive(a);
}
#endif 
