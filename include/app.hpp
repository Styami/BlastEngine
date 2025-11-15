#ifndef BLASTENGINE_HPP
#define BLASTENGINE_HPP

#include "engine.hpp"

class App {
  public:
    App();
    
    void run();
  
  private:
    Engine engine;
};

#endif