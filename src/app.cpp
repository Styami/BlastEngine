#include <chrono>
#include <iostream>
#include <print>
#include "app.hpp"

App::App() :
	handler(),
	window(),
	camera(1920.f/1080, glm::radians(45.f)),
	engine(window, camera),
	isRunning(true),
	previousTime()
{
  try
	{
		window.init("Blast Engine");
		engine.setRenderer(window);
		engine.initVulkan();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
}

void App::run() {
	previousTime = std::chrono::high_resolution_clock::now();
	while (isRunning) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime);
		previousTime = currentTime;
		isRunning = window.loop();
		handler.event(window, camera, deltaTime.count());	
		engine.drawFrame(deltaTime.count());
	}
	engine.cleanUp();
	window.clean();
	std::println("Program finished.");
}