#ifndef CONTROLER_HPP
#define CONTROLER_HPP

#include "camera.hpp"
#include "engine.hpp"
#include "window.hpp"

class Controller {
    public:
        static const Controller& getInstance();
        Controller(const Controller& c) = delete;
        Controller& operator=(const Controller& c) = delete;
        void mainLoop();
    private:
        Controller();
        vk::SurfaceKHR createSurface(const VkInstance& instance);
        std::tuple<float, float> getFrameBufferSize();
        void waitRenderer();
        bool isWindowResized();
        void resetFrameBuffer();
        static Controller* controller;
        Window window;
        Engine engine;
        Camera camera;
    
};

#endif