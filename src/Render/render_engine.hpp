//
// Created by jerry on 15.02.16.
//

#ifndef INC_2DSHADOWS_RENDER_ENGINE_HPP
#define INC_2DSHADOWS_RENDER_ENGINE_HPP

#include <GL/glew.h>
#include "shader.hpp"
#include "texture.hpp"
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace render {
	class render_engine {
	private:
		std::unique_ptr<shader> _basicshader;
		std::unique_ptr<shader> _shadow_mapper_shader;
		std::unique_ptr<shader> _shadow_render_shader;
		std::vector<std::tuple<GLuint,GLuint,texture>> _ocluder_images; //VAO, IBO, Texture
		std::vector<std::tuple<GLuint,GLuint,GLuint,glm::vec3>> _ligth_images; //VAO, IBO, TextureID, Position
		glm::mat4 _mvp;

	public:
		render_engine(){
			_basicshader = std::make_unique<shader>(shader{"shader/basic.vs", shader_type::Vertex},
													shader{"shader/basic.fs",shader_type::Fragment},
													std::vector<shader_attribute>{{1,"in_pos"},{2,"in_uv"}},
													std::vector<shader_attribute>{{0,"out_color"}},
													std::vector<shader_attribute>{{0,"tex"}});
			_shadow_mapper_shader = std::make_unique<shader>(shader{"shader/passthrough.vs", shader_type::Vertex},
															 shader{"shader/shadow_maper.fs",shader_type::Fragment},
															 std::vector<shader_attribute>{{1,"in_pos"}},
															 std::vector<shader_attribute>{{0,"out_color"}},
															 std::vector<shader_attribute>{{0,"ocluder_texture"}});
			_shadow_render_shader = std::make_unique<shader>(shader{"shader/basic.vs", shader_type::Vertex},
															 shader{"shader/shadow_renderer.fs",shader_type::Fragment},
															 std::vector<shader_attribute>{{1,"in_pos"},{2,"in_uv"}},
															 std::vector<shader_attribute>{{0,"out_color"}},
															 std::vector<shader_attribute>{{0,"shadow_map_texture"}});
		}


		void window_resize(int width, int height) {
			//glViewport(0, 0, MainClass::width, MainClass::height);

			_mvp = glm::mat4{1};
			_mvp *= glm::ortho(0.f, float(width), 0.f, float(height));

		}

		void render();
		void calc_Light();

		void add_image(std::string path);

		void render_ocluders(glm::mat4 &mvp);

		std::tuple<GLuint, GLuint> add_image(int width, int hight);
	};
}


#endif //INC_2DSHADOWS_RENDER_ENGINE_HPP
