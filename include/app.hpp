#ifndef APP_HPP
#define APP_HPP

#include "engine.hpp"

class App {
  public:
    App();
    
    void run();
  
  private:
    Engine engine;
};

#endif