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
#include <iomanip>
#include <utility>

namespace render {

	//struct to manage every relevant resource for the ligths
	struct light{
		GLuint vao;
		GLuint ibo;
		GLuint ocluder_texture;
		GLuint shadowmap_texture;
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 size;

		float selceted;
		float blur_factor;

		bool _owner;

		light() = delete;
		light(light& l) = delete;

		light(light&& l):
				vao(std::move(l.vao)),
				ibo(std::move(l.ibo)),
				ocluder_texture(std::move(l.ocluder_texture)),
				shadowmap_texture(std::move(l.shadowmap_texture)),
				position(std::move(l.position)),
				color(std::move(l.color)),
				size(std::move(l.size)),
				selceted(std::move(l.selceted)),
				blur_factor(std::move(l.blur_factor)),
				_owner(true)
		{
			l._owner = false;
		}
		light& operator=(const light& l) = delete;
		light& operator=(light&& l){
			l._owner = false;
			vao = std::move(l.vao);
			ibo = std::move(l.ibo);
			ocluder_texture = std::move(l.ocluder_texture);
			shadowmap_texture = std::move(l.shadowmap_texture);
			position = std::move(l.position);
			color = std::move(l.color);
			size = std::move(l.size);
			selceted = std::move(l.selceted);
			blur_factor = std::move(l.blur_factor);
			_owner = true;
		}

		light(const GLuint vao, const GLuint ibo, const GLuint ocluder_texture, const GLuint shadowmap_texture,
			  const glm::vec3 &position, const glm::vec4 &color, const glm::vec2 &size) : vao(vao), ibo(ibo),
																						  ocluder_texture(
																								  ocluder_texture),
																						  shadowmap_texture(
																								  shadowmap_texture),
																						  position(position),
																						  color(color),_owner(true), size(size),selceted(0),blur_factor(0) { }

		~light() {
			if (_owner) {
				glDeleteVertexArrays(1,&vao);
				glDeleteBuffers(1,&ibo);
				glDeleteTextures(1,&ocluder_texture);
				glDeleteTextures(1,&shadowmap_texture);
			}
		}

	};
	class render_engine {
	private:
		std::unique_ptr<shader> _basicshader;
		std::unique_ptr<shader> _shadow_mapper_shader;
		std::unique_ptr<shader> _shadow_render_shader;
		std::vector<std::tuple<GLuint,GLuint,texture>> _ocluder_images; //VAO, IBO, Texture
		std::vector<std::tuple<GLuint,GLuint,texture>> _background_images; //VAO, IBO, Texture
		std::vector<light> _ligth_images; //VAO, IBO, Ocluder_Texture, 1DShadowmap, Position, Color, Size
		glm::mat4 _mvp;
		GLuint oclusion_fbo;
		GLuint shadow1D_fbo;
		GLuint quad_vertexbuffer;
		GLuint quad_VertexArrayID;
		GLuint depth_rb;

		float global_blur_factor;

	public:
		render_engine():global_blur_factor(0){
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
			GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

			glGenFramebuffers(1,&shadow1D_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
			glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

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


			glGenRenderbuffers(1, &depth_rb);
			glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 256, 256);
			//-------------------------
			//Attach depth buffer to FBO

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

		void light_color(int index, glm::vec4 color){
			_ligth_images.at(index).color = color;
		}
		glm::vec4 light_color(int index){
			return _ligth_images.at(index).color;
		}
		float light_size(int index){
			return _ligth_images.at(index).size.x;
		}

		void framebuffer_check(GLenum result) const;

		auto light_count() const {return _ligth_images.size();}

		auto light_pos(int index){
			auto& size = _ligth_images.at(index).size;
			auto& pos = _ligth_images.at(index).position;
			return glm::vec2{pos.x + size.x/2, pos.y + size.y/2 };}

		void light_select(int index){
			_ligth_images.at(index).selceted = 1;
		}
		void light_deselect(int index){
			_ligth_images.at(index).selceted = 0;
		}

		void remove_light(int index);

		void add_background(std::string path, float scaling);

		void toogle_blur(){
			global_blur_factor = 1-global_blur_factor;
		}
	};
}


#endif //INC_2DSHADOWS_RENDER_ENGINE_HPP
