#include "input_reader.h"

using namespace std;




Robot_inputs Input_reader::operator()(Robot_inputs all,Input in)const{
	assert(0);
}

Input Input_reader::operator()(Robot_inputs const& in)const{
	if(in.digital_io == ) return 1;	
	else return 0;
	assert(0);
}

#ifdef INPUT_READER_TEST
int main(){

}

#endif
