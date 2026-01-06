#ifndef ENUM_INPUT_HPP
#define ENUM_INPUT_HPP
#include "window.hpp"
#include <GLFW/glfw3.h>

enum class Input {
	right = GLFW_KEY_D,
	left = GLFW_KEY_A,
	upside = GLFW_KEY_SPACE,
	downside = GLFW_KEY_LEFT_CONTROL,
	forward = GLFW_KEY_W,
	backward = GLFW_KEY_S,
    
};

#endif