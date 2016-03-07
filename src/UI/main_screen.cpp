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
			main_screen::width = width;
			main_screen::height = height;
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
		auto result = Widget::mouseButtonEvent(p, button, down, modifiers);

		if(!result) {
			if (down && button == 0) {
				move = true;
				if(selected == -1){
					_color_default = _color;
					size_default = size;
				} else{
					_render_engine->light_deselect(selected);
				}
				selected = select_light({p.x(), height - p.y()});
				if (selected == -1) {
					_render_engine->add_light(unsigned(size), {p.x(), height - p.y()},_color);
					selected = _render_engine->light_count()-1;
					if(hover_selected != -1){
						_render_engine->light_deselect(hover_selected);
					}
				}
				_render_engine->light_select(selected);

				_ligth_delet_button->setEnabled(true);
				auto color = _render_engine->light_color(selected);
				_ligth_color_wheel->setColor({color.r, color.g, color.b, color.a});
				_ligth_size_slider->setValue(_render_engine->light_size(selected) / 1024);

			}else if(down && button == 1){
				if(selected != -1)	_render_engine->light_deselect(selected);
				selected = -1;
				_ligth_delet_button->setEnabled(false);
				_color=_color_default;
				size=size_default;
				_ligth_color_wheel->setColor({_color.r, _color.g, _color.b, _color.a});
				_ligth_size_slider->setValue(size / 1024);
			} else{
				move = false;
			}
		}
		return result;
	}

	int main_screen::select_light(glm::vec2 pos){
		for (int i = 0; i < _render_engine->light_count(); ++i) {
			auto light_pos = _render_engine->light_pos(i);
			auto distance = glm::distance(light_pos, pos);
			if (distance < select_size) {
				return i;
			}
		}
		return -1;
	}


	bool main_screen::mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button,
									   int modifiers) {

		auto result = Widget::mouseMotionEvent(p, rel, button, modifiers);
		if(!result) {
			if (move && selected != -1) {
				_render_engine->move_light(selected, glm::vec2(p.x(), height - p.y()));
				//hover_selected = selected;
			} else {
				for (int i = 0; i < _render_engine->light_count(); ++i) {
					if(selected == i) continue;
					_render_engine->light_deselect(i);
				}

				auto sel = select_light({p.x(), height - p.y()});
				hover_selected = sel;
				if (hover_selected != -1) {
					_render_engine->light_select(hover_selected);
				}
			}
		}

		return result;
	}
}