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
		std::vector<std::tuple<GLuint,GLuint,GLuint,GLuint,glm::vec3,glm::vec4,glm::vec2>> _ligth_images; //VAO, IBO, Ocluder_Texture, 1DShadowmap, Position, Color, Size
		glm::mat4 _mvp;
		GLuint oclusion_fbo;
		GLuint shadow1D_fbo;
		GLuint quad_vertexbuffer;
		GLuint quad_VertexArrayID;

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

			glGenFramebuffers(1,&oclusion_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER,oclusion_fbo);

			glGenFramebuffers(1,&shadow1D_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);

			glBindFramebuffer(GL_FRAMEBUFFER,0);

			quad_VertexArrayID;
			glGenVertexArrays(1, &quad_VertexArrayID);
			glBindVertexArray(quad_VertexArrayID);
			static const GLfloat g_quad_vertex_buffer_data[] = {
					-1.0f, -1.0f, 0.0f,
					1.0f, -1.0f, 0.0f,
					-1.0f,  1.0f, 0.0f,
					-1.0f,  1.0f, 0.0f,
					1.0f, -1.0f, 0.0f,
					1.0f,  1.0f, 0.0f
			};

			glGenBuffers(1, &quad_vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), &g_quad_vertex_buffer_data, GL_STATIC_DRAW);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			glVertexAttribPointer(
					1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					(3*4),                  // stride
					(void*)0            // array buffer offset
			);
		}


		void window_resize(int width, int height) {
			//glViewport(0, 0, MainClass::width, MainClass::height);

			_mvp = glm::mat4{1};
			_mvp *= glm::ortho(0.f, float(width), 0.f, float(height));

		}

		void render();
		void add_light(unsigned int size = 256, glm::vec2 position = glm::vec2(),glm::vec4 color = glm::vec4(1,1,1,1));

		void add_image(std::string path, float scaling=1.f);

		void render_ocluders(glm::mat4 &mvp);

		std::tuple<GLuint, GLuint> add_image(float width, float hight, bool flipu = true);

		void move_light(int index,glm::vec2 position);

		void framebuffer_check(GLenum result) const;

		auto light_count() const {return _ligth_images.size();}

		auto light_pos(int index){
			auto& size = std::get<6>(_ligth_images.at(index));
			auto& pos = std::get<4>(_ligth_images.at(index));
			return glm::vec2{pos.x + size.x/2, pos.y + size.y/2 };}
	};
}


#endif //INC_2DSHADOWS_RENDER_ENGINE_HPP