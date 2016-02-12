//
// Created by jerry on 12.02.16.
//

#ifndef INC_2DSHADOWS_MAIN_SCREEN_HPP
#define INC_2DSHADOWS_MAIN_SCREEN_HPP

#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/glutil.h>
#include <nanogui/button.h>
#include <nanogui/label.h>
#include <GLFW/glfw3.h>
#include <Eigen/src/Geometry/AngleAxis.h>
#include <iostream>
#include <nanogui/entypo.h>

class main_screen : public nanogui::Screen {
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

	virtual void draw(NVGcontext *ctx) {
		/* Draw the user interface */
		nanogui::Screen::draw(ctx);
	}

	virtual void drawContents() {
		using namespace nanogui;
	}

};


#endif //INC_2DSHADOWS_MAIN_SCREEN_HPP
