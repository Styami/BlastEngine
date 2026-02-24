#include "inputHandler.hpp"
#include "GLFW/glfw3.h"
#include "enum_input.hpp"

InputHandler::InputHandler() : 
    m_cursorPos(0, 0),
    leftClickPressed(false)
{}

void InputHandler::event(const Window& renderer, Camera& camera, const double dt) {
    
    auto [buttonsPressed, newPos] = renderer.getInputInfo();
    if (buttonsPressed[static_cast<unsigned int>(Input::sprint)] && !camera.isSprinting()) {
        camera.setSprint(true);
        camera.accelerate(2);
    } else if (!buttonsPressed[static_cast<unsigned int>(Input::sprint)] && camera.isSprinting()) {
        camera.setSprint(false);
        camera.accelerate(0.5);
    }
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
        glm::vec2 rotateVec = newPos - m_cursorPos;
        camera.rotate(rotateVec, dt);
        m_cursorPos = newPos;
    }
    if (buttonsPressed[static_cast<unsigned int>(Input::leftClick)] && !leftClickPressed) {
        m_cursorPos = newPos;
        leftClickPressed = true;
    }
    if (!buttonsPressed[static_cast<unsigned int>(Input::leftClick)]) {
        leftClickPressed = false;
    }

}