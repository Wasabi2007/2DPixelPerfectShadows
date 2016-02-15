//
// Created by jerry on 12.02.16.
//

#include <chrono>
#include "main_screen.hpp"

namespace UI {
	bool main_screen::mvpreload = false;
	int main_screen::width = 1024;
	int main_screen::height = 768;

	void main_screen::mainLoop(float dt) {
		glClearColor(0.4f, 0.6f, 0.9f, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto window = glfwGetCurrentContext();
		int width = 0;
		int height = 0;

		glfwGetFramebufferSize(window, &width, &height);

		if (main_screen::mvpreload || main_screen::width != width || main_screen::height != height) {
			main_screen::width = width;
			main_screen::height = height;
			main_screen::mvpreload = false;
			WindowSize();
		}
	}

	void main_screen::draw(NVGcontext *ctx) {
		/* Draw the user interface */
		/* Animate the scrollbar */
		//mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));
		static auto time = std::chrono::high_resolution_clock::now();
		static float dt = 0.f;

		auto t2 = std::chrono::high_resolution_clock::now();
		dt = float(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - time).count()) / 1000;
		mainLoop(dt);
		//        running = glWindow.renderFrame();
		//TwDraw();
		time = t2;
		//ctx->
		//mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

		/* Draw the user interface */
		nanogui::Screen::draw(ctx);
	}

	void main_screen::WindowSize() {

	}

	void main_screen::drawContents() {
		using namespace nanogui;
	}
}