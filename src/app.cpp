#include <iostream>
#include <print>
#include "app.hpp"

App::App() {
  try
	{
		engine.initVulkan();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
}

void App::run() {
	engine.run();
	std::println("Program finished.");
}