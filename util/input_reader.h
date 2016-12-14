#ifndef INPUT_READER_H
#define INPUT_READER_H

#include "interface.h"
typedef bool Input;

struct Input_reader {
	Input operator()(Robot_inputs const&)const;
	Robot_inputs operator()(Robot_inputs,Input)const;

};



























#endif
