#include "controller.hpp"
#include "engine.hpp"
#include "window.hpp"

Controller::Controller() :
    window(),
    engine()
{
    window.init("Blast Engine");
    engine.initVulkan();
}

const Controller& Controller::getInstance() {
    if (controller == nullptr) {
        controller = new Controller();
    }
    return *controller;
}

void Controller::waitRenderer() {
    window.waitEvents();
}

