//
// Created by jerry on 12.02.16.
//

#ifndef INC_2DSHADOWS_MAIN_SCREEN_HPP
#define INC_2DSHADOWS_MAIN_SCREEN_HPP

#include "../Render/render_engine.hpp"
#include <GLFW/glfw3.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/glutil.h>
#include <nanogui/button.h>
#include <nanogui/label.h>
#include <nanogui/slider.h>
#include <Eigen/src/Geometry/AngleAxis.h>
#include <iostream>
#include <nanogui/entypo.h>
#include <memory>
#include <nanogui/colorwheel.h>

namespace ui {
	class main_screen : public nanogui::Screen {
	private:
		nanogui::GLShader mShader;

		std::unique_ptr<render::render_engine> _render_engine;
		static int select_size;
		int selected;
		int hover_selected;
		glm::vec4 _color;
		float size;
		bool move;
		nanogui::Slider *_ligth_size_slider;
		nanogui::ColorWheel *_ligth_color_wheel;
		nanogui::Button *_ligth_delet_button;

		glm::vec4 _color_default;
		float size_default;

	public:
		static int width;
		static int height;

		main_screen(Eigen::Vector2i res, std::string s) :
				nanogui::Screen(res, s),
				selected(-1),
				hover_selected(-1),
				_color(1),
				size(256),
				_color_default(1),
				size_default(256),
				move(false),
				_ligth_size_slider(nullptr),
				_ligth_color_wheel(nullptr),
				_ligth_delet_button(nullptr){
			using namespace nanogui;

			main_screen::width = res[0];
			main_screen::height = res[1];

			auto *window = new Window(this, "Light Control");
			window->setLayout(new GroupLayout());
			window->setPosition(Vector2i(150, 150));
			window->setFixedSize(Vector2i(400, 350));

			//new Label(window, "Push buttons", "sans-bold");

			_ligth_delet_button = new Button(window, "Remove");
			_ligth_delet_button->setCallback([&] { if(selected != -1 ) {
				_render_engine->remove_light(selected);
				_ligth_delet_button->setEnabled(false);
				_ligth_color_wheel->setColor({_color_default.r, _color_default.g, _color_default.b, _color_default.a});
				_ligth_size_slider->setValue(size_default / 1024);
				selected = -1;
			}});

			new Label(window,"Ligth color:");
			_ligth_color_wheel = new ColorWheel(window);
			_ligth_color_wheel->setColor({_color.r, _color.g, _color.b, _color.a});
			_ligth_color_wheel->setCallback([&](const Color &value){
				_color = glm::vec4{value.r(),value.g(),value.b(),1.f};
				if(selected!=-1){
					_render_engine->light_color(selected,_color);
				}
			});

			new Label(window,"Ligth size:");
			_ligth_size_slider = new Slider(window);
			_ligth_size_slider->setValue(size / 1024);
			_ligth_size_slider->setCallback([&](const float& value){
				size = glm::max(value*1024,1.f);
				if(selected!=-1){
					auto pos = _render_engine->light_pos(selected);
					_render_engine->remove_light(selected);
					_render_engine->add_light(unsigned(size), pos,_color);
					selected = _render_engine->light_count()-1;
				}
			});

			performLayout(mNVGContext);


			mShader.init(
					/* An identifying name */
					"a_simple_shader",

					/* Vertex shader */
					"#version 330\n"
							"uniform mat4 modelViewProj;\n"
							"in vec3 position;\n"
							"void main() {\n"
							"    gl_Position = modelViewProj * vec4(position, 1.0);\n"
							"}",

					/* Fragment shader */
					"#version 330\n"
							"out vec4 color;\n"
							"uniform float intensity;\n"
							"void main() {\n"
							"    color = vec4(vec3(intensity), 1.0);\n"
							"}"
			);

			MatrixXu indices(3, 2); /* Draw 2 triangles */
			indices.col(0) << 0, 1, 2;
			indices.col(1) << 2, 3, 0;

			MatrixXf positions(3, 4);
			positions.col(0) << -1, -1, 0;
			positions.col(1) <<  1, -1, 0;
			positions.col(2) <<  1,  1, 0;
			positions.col(3) << -1,  1, 0;

			mShader.bind();
			mShader.uploadIndices(indices);
			mShader.uploadAttrib("position", positions);
			mShader.setUniform("intensity", 0.5f);

			_render_engine = std::make_unique<render::render_engine>();

			_render_engine->window_resize(res[0],res[1]);
			_render_engine->add_image("textures/test.png", 2);

		}
		~main_screen(){
			mShader.free();
		}


		virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) override;

		virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
			if (nanogui::Screen::keyboardEvent(key, scancode, action, modifiers))
				return true;
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				setVisible(false);
				return true;
			}
			return false;
		}

		virtual void draw(NVGcontext *ctx);

		virtual void drawContents();

		void mainLoop(float dt);

		virtual bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button,
									  int modifiers) override;

		int select_light(glm::vec2 pos);
	};
}

#endif //INC_2DSHADOWS_MAIN_SCREEN_HPP
