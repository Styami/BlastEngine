#include "inputHandler.hpp"
#include "GLFW/glfw3.h"
#include "enum_input.hpp"

InputHandler::InputHandler() : 
    m_cursorPos(0, 0),
    leftClickPressed(false)
{}

void InputHandler::event(const Window& renderer, Camera& camera, const double dt) {
    
    auto [buttonsPressed, newPos] = renderer.getCursorInfo();
    if (buttonsPressed[static_cast<unsigned int>(Input::backward)]) {
        camera.backward(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::forward)]) {
        camera.forward(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::left)]) {
        camera.left(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::right)]) {
        camera.right(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::down)]) {
        camera.downward(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::up)]) {
        camera.upward(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::backward)]) {
        camera.backward(dt);
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::leftClick)] && leftClickPressed) {
        // glm::vec2 newPos = renderer.getCursorPos();
        glm::vec2 rotateVec = newPos - m_cursorPos;
        // if (rotateVec.x != 0 || rotateVec.y != 0)
            // rotateVec = glm::normalize(rotateVec);
        camera.rotate(rotateVec, dt);
        m_cursorPos = newPos;
        //leftClickPressed = false;
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::leftClick)]&& !leftClickPressed) {
        m_cursorPos = newPos;
        leftClickPressed = true;
    }
    if (!buttonsPressed[static_cast<unsigned int>(Input::leftClick)]) {
        leftClickPressed = false;
    }

}