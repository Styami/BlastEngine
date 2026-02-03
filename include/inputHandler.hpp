#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "camera.hpp"
#include "window.hpp"

class InputHandler {
    public:   
        InputHandler();
        void event(const Window& renderer, Camera& cam, double dt);
    private:
        glm::vec2 m_cursorPos;
        bool leftClickPressed; 
};

#endif