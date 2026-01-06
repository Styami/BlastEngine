#ifndef APP_HPP
#define APP_HPP

#include "engine.hpp"
#include "inputHandler.hpp"
#include <chrono>

class App {
  public:
    App();
    
    void run();
  
  private:
    InputHandler handler;
    Window window;
    Camera camera;
    Engine engine;
    bool isRunning;
    std::chrono::high_resolution_clock::time_point previousTime;
};

#endif