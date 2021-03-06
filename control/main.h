#ifndef MAIN_H
#define MAIN_H

#include "force_interface.h"
#include "../util/posedge_toggle.h"
#include "../util/perf_tracker.h"
#include "../util/countdown_timer.h"
#include "../util/countup_timer.h"
#include "toplevel.h"
#include "../input/panel.h"
#include "../util/nav.h"
#include "../util/nav2.h"
#include "log.h"
#include "../util/posedge_trigger_debounce.h"
#include "../util/motion_profile.h"
#include "../executive/executive.h"

struct Main{
	#define MODES X(TELEOP)\
		X(DELAY) X(AUTO_NULL) X(AUTO_REACH) X(AUTO_STATIC) \
		X(AUTO_STOP) X(AUTO_STATICTWO) X(AUTO_TEST) \
		X(AUTO_PORTCULLIS) X(AUTO_PORTCULLIS_DONE) \
		X(AUTO_CHEVALPOS) X(AUTO_CHEVALWAIT) X(AUTO_CHEVALDROP) X(AUTO_CHEVALDRIVE) X(AUTO_CHEVALSTOW) \
		X(AUTO_LBLS_CROSS_LB) X(AUTO_LBLS_CROSS_MU) \
		X(AUTO_LBLS_SCORE_SEEK) X(AUTO_LBLS_SCORE_LOCATE) \
		X(AUTO_LBLS_SCORE_CD) X(AUTO_LBLS_SCORE_EJECT) \
		X(AUTO_LBWLS_WALL) X(AUTO_LBWLS_MUP) X(AUTO_LBWLS_ROTATE) X(AUTO_LBWLS_TOWER) \
		X(AUTO_LBWLS_EJECT) X(AUTO_LBWLS_BACK) X(AUTO_LBWLS_C) X(AUTO_LBWLS_BR) \
		X(AUTO_LBWHS_WALL) X(AUTO_LBWHS_MUP) X(AUTO_LBWHS_ROTATE) X(AUTO_LBWHS_TOWER) \
		X(AUTO_LBWHS_EJECT) X(AUTO_LBWHS_BACK) X(AUTO_LBWHS_C) X(AUTO_LBWHS_BR) \
		X(AUTO_LBWHS_PREP) X(AUTO_LBWHS_BP) \
		X(AUTO_BR_STRAIGHTAWAY) X(AUTO_BR_INITIALTURN) X(AUTO_BR_SIDE) X(AUTO_BR_SIDETURN) X(AUTO_BR_ENDTURN)
	enum class Mode{
		#define X(NAME) NAME,
		MODES
		#undef X
	};
	Mode mode;
	Executive mode_;

	Motion_profile motion_profile;
	Countdown_timer in_br_range;
	Nav2 nav2;
	Force_interface force;
	Perf_tracker perf;
	Toplevel toplevel;
	bool topready;
	bool simtest;
	bool set_initial_encoders;
	unsigned int br_step;//starts at lap 0 and increases by one every time it reaches a new node
	std::pair<int,int> initial_encoders;//first is left, second is right
	Robot_inputs in_i;

	Countup_timer since_switch,since_auto_start;

	Posedge_trigger autonomous_start;

	enum Nudges{FORWARD,BACKWARD,CLOCKWISE,COUNTERCLOCKWISE,NUDGES};	
	struct Nudge{
		Posedge_trigger trigger;
		Countdown_timer timer;
	};
	std::array<Nudge,NUDGES> nudges;
	
	#define JOY_COLLECTOR_POS X(STOP) X(LOW) X(LEVEL)
	enum class Joy_collector_pos{
		#define X(name) name,
		JOY_COLLECTOR_POS
		#undef X
	};
	Joy_collector_pos joy_collector_pos;
		
	Posedge_toggle controller_auto;
	#define COLLECTOR_MODES X(CHEVAL) X(NOTHING) X(COLLECT) X(STOW) X(SHOOT_HIGH) X(REFLECT) X(SHOOT_LOW) X(LOW) X(DRAWBRIDGE)
	enum class Collector_mode{
		#define X(name) name,
		COLLECTOR_MODES
		#undef X
	};
	Collector_mode collector_mode;

	Posedge_trigger_debounce cheval_trigger;
	
	Posedge_trigger_debounce set_button;
	bool tilt_learn_mode;

	Countdown_timer shoot_high_timer, shoot_low_timer, cheval_lift_timer, cheval_drive_timer, drawbridge_timer;

	#define CHEVAL_STEPS X(GO_DOWN) X(DRIVE) X(DRIVE_AND_STOW)
	enum class Cheval_steps{
		#define X(NAME) NAME,
		CHEVAL_STEPS
		#undef X
	};
	Cheval_steps cheval_step;
		
	#define SHOOT_STEPS X(SPEED_UP) X(SHOOT)
	enum class Shoot_steps{
		#define X(NAME) NAME,
		SHOOT_STEPS
		#undef X
	};
	Shoot_steps shoot_step;

	Posedge_toggle controller_closed_loop;
	
	Log log;

	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail const&);

	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main const&,Main const&);
bool operator!=(Main const&,Main const&);
std::ostream& operator<<(std::ostream&,Main const&);

#endif
