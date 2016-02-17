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
#include <Eigen/src/Geometry/AngleAxis.h>
#include <iostream>
#include <nanogui/entypo.h>
#include <memory>

namespace ui {
	class main_screen : public nanogui::Screen {
	private:
		static int width;
		static int height;

		nanogui::GLShader mShader;

		std::unique_ptr<render::render_engine> _render_engine;
	public:
		main_screen(Eigen::Vector2i res, std::string s) : nanogui::Screen(res, s) {
			using namespace nanogui;

			auto *window = new Window(this, "Button demo");
			window->setLayout(new GroupLayout());
			window->setPosition(Vector2i(150, 150));
			window->setFixedSize(Vector2i(400, 350));

			new Label(window, "Push buttons", "sans-bold");

			Button *b = new Button(window, "Plain button");
			b->setCallback([] { std::cout << "pushed!" << std::endl; });
			b = new Button(window, "Styled", ENTYPO_ICON_ROCKET);
			b->setBackgroundColor(Color(0, 0, 255, 25));
			b->setCallback([] { std::cout << "pushed!" << std::endl; });

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
			_render_engine->add_image("textures/cat4.png");
		}
		~main_screen(){
			mShader.free();
		}

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
	};
}

#endif //INC_2DSHADOWS_MAIN_SCREEN_HPP
