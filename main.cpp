#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/glutil.h>

#include "src/UI/main_screen.hpp"

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

int main() {
	try {
		nanogui::init();
		GLenum err = glGetError();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (GLEW_ARB_debug_output) {
			glDebugMessageCallbackARB(
					[](GLenum source, GLenum type, GLuint id, GLenum serverity, GLsizei length, const GLchar *message,
					   const void *userParam) {
						std::cout << message << std::endl;
					}, nullptr);
		}

	    auto *screen = new ui::main_screen(Eigen::Vector2i(1024, 768), "2D Shadows");

		screen->drawAll();
		screen->setVisible(true);

		nanogui::mainloop();

		nanogui::shutdown();


	} catch (const std::runtime_error &e) {
		std::string error_msg = std::string("Caught a fatal error: ").append(std::string(e.what()));
#if defined(WIN32)
		MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
		std::cerr << error_msg << endl;
#endif
		return -1;
	}
	return 0;
}