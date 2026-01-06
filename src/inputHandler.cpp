#include "inputHandler.hpp"

InputHandler::InputHandler(){}

void InputHandler::event(const Window& renderer, Camera& camera, const double dt) {
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::forward)) == GLFW_PRESS) 
        camera.forward(dt);
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::backward)) == GLFW_PRESS) 
        camera.backward(dt);
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::right)) == GLFW_PRESS) camera.right(dt);
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::left)) == GLFW_PRESS) camera.left(dt);
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::upside)) == GLFW_PRESS) camera.upward(dt);
    if (glfwGetKey(renderer.getWindow(), static_cast<int>(Input::downside)) == GLFW_PRESS) camera.downward(dt);
    if (glfwGetKey(renderer.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(renderer.getWindow(), true);
}