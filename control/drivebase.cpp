#include "drivebase.h"
#include "../util/fixVictor.h"
#include <iostream>
#include <math.h>
#include "../util/util.h"

using namespace std;

unsigned pdb_location(Drivebase::Motor m){
	#define X(NAME,INDEX) if(m==Drivebase::NAME) return INDEX;
	//WILL NEED CORRECT VALUES
	X(FRONTLEFT1,0)
	X(FRONTLEFT2,1)
	X(FRONTRIGHT1,2)
	X(FRONTRIGHT2,13)
	X(BACK1,14)
	X(BACK2,15)
	
	#undef X
	assert(0);
	//assert(m>=0 && m<Drivebase::MOTORS);
}

int encoderconv(Maybe_inline<Encoder_output> encoder){
	if(encoder) return *encoder;
	return 10000;
}

double ticks_to_inches(const int ticks){
	const unsigned int TICKS_PER_REVOLUTION=100;
	const double WHEEL_DIAMETER=7.4;//inches
	const double WHEEL_CIRCUMFERENCE=WHEEL_DIAMETER*PI;//inches
	const double INCHES_PER_TICK=WHEEL_CIRCUMFERENCE/(double)TICKS_PER_REVOLUTION;//0.25 vs 0.251327
	return ticks*INCHES_PER_TICK;
}

#define L_ENCODER_PORTS 0,1
#define R_ENCODER_PORTS 2,3//2016 mounted backwards
#define L_ENCODER_LOC 0
#define R_ENCODER_LOC 1

Robot_inputs Drivebase::Input_reader::operator()(Robot_inputs all,Input in)const{
	for(unsigned i=0;i<MOTORS;i++){
		all.current[pdb_location((Motor)i)]=in.current[i];
	}
	/*auto set=[&](unsigned index,Digital_in value){
		all.digital_io.in[index]=value;
	};
	auto encoder=[&](unsigned a,unsigned b,Encoder_info e){
		set(a,e.first);
		set(b,e.second);
	};
	encoder(L_ENCODER_PORTS,in.left);
	encoder(R_ENCODER_PORTS,in.right);
	all.digital_io.encoder[L_ENCODER_LOC] = in.ticks.first;
	all.digital_io.encoder[R_ENCODER_LOC] = in.ticks.second;*/
	return all;
}

Drivebase::Input Drivebase::Input_reader::operator()(Robot_inputs const& in)const{
	/*auto encoder_info=[&](unsigned a, unsigned b){
		return make_pair(in.digital_io.in[a],in.digital_io.in[b]);
	};*/
	return Drivebase::Input{
		[&](){
			array<double,Drivebase::MOTORS> r;
			for(unsigned i=0;i<Drivebase::MOTORS;i++){
				Drivebase::Motor m=(Drivebase::Motor)i;
				r[i]=in.current[pdb_location(m)];
			}
			return r;
		}()/*,
		encoder_info(L_ENCODER_PORTS),
		encoder_info(R_ENCODER_PORTS),
		{encoderconv(in.digital_io.encoder[L_ENCODER_LOC]),encoderconv(in.digital_io.encoder[R_ENCODER_LOC])}*/
	};
}

float range(const Robot_inputs in){
	float volts=in.analog[2];
	const float voltsperinch=1; 
	float inches=volts*voltsperinch;
	return inches;
}

IMPL_STRUCT(Drivebase::Status::Status,DRIVEBASE_STATUS)
IMPL_STRUCT(Drivebase::Input::Input,DRIVEBASE_INPUT)
IMPL_STRUCT(Drivebase::Output::Output,DRIVEBASE_OUTPUT)

CMP_OPS(Drivebase::Input,DRIVEBASE_INPUT)

CMP_OPS(Drivebase::Status,DRIVEBASE_STATUS)

set<Drivebase::Status> examples(Drivebase::Status*){
	return {Drivebase::Status{
		array<Motor_check::Status,Drivebase::MOTORS>{
			Motor_check::Status::OK_,
			Motor_check::Status::OK_
		}
		,
		false
		/*{0.0,0.0},
		{0.0,0.0}*/
	}};
}

set<Drivebase::Goal> examples(Drivebase::Goal*){
	return {
		Drivebase::Goal{Pt(0,0,0),0},
		Drivebase::Goal{Pt(1,1,1),1}
	};
}

ostream& operator<<(ostream& o,Drivebase::Goal const& a){
	return o<<"Drivebase::Goal("<<a.direction<<" "<<a.field_relative<<")";
}

#define CMP(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

bool operator<(Drivebase::Goal const& a,Drivebase::Goal const& b){
	CMP(direction)
	CMP(field_relative)
	return 0;
}

CMP_OPS(Drivebase::Output,DRIVEBASE_OUTPUT)

set<Drivebase::Output> examples(Drivebase::Output*){
	return {
		Drivebase::Output{0,0,0},
		Drivebase::Output{1,1,1}
	};
}

set<Drivebase::Input> examples(Drivebase::Input*){
	/*auto d=Digital_in::_0;
	auto p=make_pair(d,d);*/
	return {Drivebase::Input{
		{0,0}//,p,p,{0,0}
	}};
}
Drivebase::Estimator::Estimator(){
	stall = false;
	timer.set(.05);
	/*last_ticks = {0,0};
	speeds = {0.0,0.0};*/
}

Drivebase::Status_detail Drivebase::Estimator::get()const{
	array<Motor_check::Status,MOTORS> a;
	for(unsigned i=0;i<a.size();i++){
		a[i]=motor_check[i].get();
	}
	
	return Status{a,stall/*,speeds,last_ticks*/};
}

ostream& operator<<(ostream& o,Drivebase::Output_applicator){
	return o<<"output_applicator";
}

ostream& operator<<(ostream& o,Drivebase const& a){
	return o<<"Drivebase("<<a.estimator.get()<<")";
}

double get_output(Drivebase::Output out,Drivebase::Motor m){
	#define X(NAME,POSITION) if(m==Drivebase::NAME) return out.POSITION;
	X(FRONTLEFT1,a)
	X(FRONTLEFT2,a)
	X(FRONTRIGHT1,b)
	X(FRONTRIGHT2,b)
	X(BACK1,c)
	X(BACK2,c)
	#undef X
	assert(0);
}
double sum(std::array<double, 6ul> a){
	double sum = 0;
	for(unsigned int i=0;i<a.size();i++)
		sum+=a[i];

	return sum;
}
double mean(std::array<double, 6ul> a){
	return sum(a)/a.size();
}
void Drivebase::Estimator::update(Time now,Drivebase::Input in,Drivebase::Output out){\
	/*timer.update(now,true);
	static const double POLL_TIME = .05;//seconds
	if(timer.done()){
		speeds.first = ticks_to_inches((last_ticks.first-in.ticks.first)/POLL_TIME);
		speeds.second = ticks_to_inches((last_ticks.second-in.ticks.second)/POLL_TIME);
		last_ticks = in.ticks;
		timer.set(POLL_TIME);
	}*/
	
	//cout << "Encoder in: " << in << endl;
	for(unsigned i=0;i<MOTORS;i++){
		Drivebase::Motor m=(Drivebase::Motor)i;
		auto current=in.current[i];
		auto set_power_level=get_output(out,m);
		motor_check[i].update(now,current,set_power_level);
		
	}
	stall = mean(in.current) > 5;
}

Robot_outputs Drivebase::Output_applicator::operator()(Robot_outputs robot,Drivebase::Output b)const{
	robot.pwm[0]=pwm_convert(b.a);
	robot.pwm[1]=pwm_convert(b.b);
	robot.pwm[2]=pwm_convert(b.c);

	/*robot.digital_io[0]=Digital_out::encoder(0,1);
	robot.digital_io[1]=Digital_out::encoder(0,0);
	robot.digital_io[2]=Digital_out::encoder(1,1);
	robot.digital_io[3]=Digital_out::encoder(1,0);*/
	return robot;
}

Drivebase::Output Drivebase::Output_applicator::operator()(Robot_outputs robot)const{
	return Drivebase::Output{
		from_pwm(robot.pwm[0]),
		from_pwm(robot.pwm[1]),
		from_pwm(robot.pwm[2])
	};
}

bool operator==(Drivebase::Output_applicator const&,Drivebase::Output_applicator const&){
	return true;
}

bool operator==(Drivebase::Estimator const& a,Drivebase::Estimator const& b){
	for(unsigned i=0; i<Drivebase::MOTORS; i++){
		if(a.motor_check[i]!=b.motor_check[i])return false;
	}
	return true;
}

bool operator!=(Drivebase::Estimator const& a,Drivebase::Estimator const& b){
	return !(a==b);
}

bool operator==(Drivebase const& a,Drivebase const& b){
	return a.estimator==b.estimator && a.output_applicator==b.output_applicator;
}

bool operator!=(Drivebase const& a,Drivebase const& b){
	return !(a==b);
}

double max3(double a,double b,double c){
	return max(max(a,b),c);
}

Drivebase::Output func_inner(double x, double y, double theta){	
	Drivebase::Output r{0,0,0};
	r.a=-double(1)/3* theta- double(1)/3* x -(double(1)/sqrt(3))*y;
	r.b=-double(1)/3* theta- double(1)/3* x +(double(1)/sqrt(3))*y;
	r.c=(-(double(1)/3)* theta) + ((double(2)/3)* x);
	return r;
}

Pt rotate_vector(double x, double y, double theta, double angle){
	double cosA = cos(angle * (3.14159 / 180.0));
	double sinA = sin(angle * (3.14159 / 180.0));
	double xOut = x * cosA - y * sinA;
	double yOut = x * sinA + y * cosA;
	
	return Pt(xOut,yOut,theta);
}

Drivebase::Output maximizeSpeed(Drivebase::Output r){
	const double s=sqrt(3);
	r.a*=s;
	r.b*=s;
	r.c*=s;
	const double m=max3(fabs(r.a),fabs(r.b),fabs(r.c));
	if(m>1){
		r.a/=m;
		r.b/=m;
		r.c/=m;
	}
	return r;
}

Drivebase::Output holonomic_mix(double x,double y,double theta,double orientation,bool fieldRelative){
	//This function exists in order to pull the full power out of the drivetrain.  
	//It makes some of the areas of the x/y/theta space have funny edges/non-smooth areas, but I think this is an acceptable tradeoff.
	//Also rotates the vectors of the drive if the robot is in field relative mode
	Pt p;
	if (fieldRelative){
		p=rotate_vector(x,y,theta,orientation);
	} 
	else {
		p=Pt(x,y,theta);
	}
	static int i = 0;
	if(i==0){
		cerr<<"Joy.x= "<<x<<" "<<"Joy.y= "<<y<<" "<<"Joystick.theta= "<<theta<<"\n";
		cerr<<"P.x= "<<p.x<<" "<<"P.y= "<<p.y<<" "<<"P.theta= "<<p.theta<<"\n";	
	}
	i=(i+1)%500;
	
	return maximizeSpeed(func_inner(p.x,p.y,p.theta));
}

Drivebase::Output holonomic_mix(Pt p){
	return holonomic_mix(p.x,p.y,p.theta,0,false);
}

Drivebase::Output control(Drivebase::Status, Drivebase::Goal goal){
	/*	
	-orientation takes care of 1 variable: float
	-Drivebase::Output has 4 variables: x, y, theta, field_relative
	-Drivebase::Output holonomic_mix needs in order: x, y, theta, orientation, field_relative
	*/
	Pt a = goal.direction;
	return holonomic_mix(a.x, a.y, a.theta, 0/*orientation*/, goal.field_relative);
}

Drivebase::Status status(Drivebase::Status a){ return a; }

bool ready(Drivebase::Status,Drivebase::Goal){ return 1; }

#ifdef DRIVEBASE_TEST
#include "formal.h"
int main(){
	Drivebase d;
	Tester_mode t;
	t.check_outputs_exhaustive=0;
	tester(d,t);
}
#endif
