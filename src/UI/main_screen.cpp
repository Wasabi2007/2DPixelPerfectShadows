//
// Created by jerry on 12.02.16.
//

#include <chrono>
#include "main_screen.hpp"

namespace ui {
	int main_screen::width = 1024;
	int main_screen::height = 768;
	int main_screen::select_size = 126;

	void main_screen::mainLoop(float dt) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		//glClearColor(0.4f, 0.6f, 0.9f, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto window = glfwGetCurrentContext();
		int width = 0;
		int height = 0;

		glfwGetFramebufferSize(window, &width, &height);

		if (main_screen::width != width || main_screen::height != height) {
			_render_engine->window_resize(width,height);
		}

		_render_engine->render();

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


	void main_screen::drawContents() {
		using namespace nanogui;

		mShader.bind();

		Matrix4f _mvp;
		_mvp.setIdentity();
		_mvp.topLeftCorner<3,3>() = Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(),  Vector3f::UnitZ())) * 0.25f;

		_mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

		mShader.setUniform("modelViewProj", _mvp);

		/* Draw 2 triangles starting at index 0 */
		mShader.drawIndexed(GL_TRIANGLES, 0, 2);
	}

	bool main_screen::mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) {
		static std::random_device r;
		static std::default_random_engine e1(r());
		static std::uniform_real_distribution<float> uniform_dist(0, 1);

		if(down) {
			for(int i = 0; i < _render_engine->light_count(); ++i) {
				auto light_pos = _render_engine->light_pos(i);
				auto distance = glm::distance(light_pos,{p.x(), height - p.y()});
				if(distance<select_size){
					selected = i;
				}
			}
			if(selected == -1){
				_render_engine->add_light(256,{p.x(), height - p.y()},{uniform_dist(e1),uniform_dist(e1),uniform_dist(e1),1.f});
			}
		} else{
			selected = -1;
		}

		return Widget::mouseButtonEvent(p, button, down, modifiers);
	}


	bool main_screen::mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button,
									   int modifiers) {

		if(selected != -1) {
			_render_engine->move_light(selected, glm::vec2(p.x(), height - p.y()));
		}

		return Widget::mouseMotionEvent(p, rel, button, modifiers);
	}
}