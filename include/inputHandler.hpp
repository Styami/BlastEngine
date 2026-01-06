#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "camera.hpp"
#include "enum_input.hpp"

class InputHandler {
    public:   
        InputHandler();
        void event(const Window& renderer, Camera& cam, double dt);
};

#endif