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
		std::unique_ptr<shader> basicshader;
		std::vector<std::tuple<GLuint,GLuint,texture>> images;
		glm::mat4 mvp;

	public:
		render_engine(){
			basicshader = std::make_unique<shader>(shader{"shader/basic.vs",shader_type::Vertex},
							shader{"shader/basic.fs",shader_type::Fragment},
						   std::vector<shader_attribute>{{1,"in_pos"},{2,"in_uv"}},
						   std::vector<shader_attribute>{{0,"out_color"}},
						   std::vector<shader_attribute>{{0,"tex"}});
		}


		void window_resize(int width, int height) {
			//glViewport(0, 0, MainClass::width, MainClass::height);

			mvp = glm::mat4{1};
			mvp *= glm::ortho(0.f,float(width),0.f,float(height));

		}

		void render(){

			basicshader->use_shader();
			GLint matrix_id = basicshader->getUniform("MVP");
			glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

			for(auto& img : images){
				glBindVertexArray(std::get<0>(img));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(img));

				std::get<2>(img).bind();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}

		void add_image(std::string path){
			auto img = texture(path);

			GLuint VertexArrayID;
			GLuint vertexbuffer;
			GLuint elementbuffer;
			glGenVertexArrays(1, &VertexArrayID);
			//cout << "gogo" << endl;
			glBindVertexArray(VertexArrayID);


			// An array of 3 vectors which represents 3 vertices
			const GLfloat g_vertex_buffer_data[] = {
					0.0f, 0.0f, 0.0f,//Position
					1.0f, 0.0f, //UV

					float(img.width)*2, 0.0f, 0.0f,//Position
					0.0f, 0.0f, //UV

					float(img.width)*2,  float(img.height)*2, 0.0f,//Position
					0.0f, 1.0f, //UV

					0.0f,  float(img.height)*2, 0.0f,//Position
					1.0f, 1.0f, //UV
			};


			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vertexbuffer);

			// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

			// Give our vertices to OpenGL.
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
					1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					(3*4)+8,                  // stride
					(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
					2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
					2,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					(3*4)+8,                  // stride
					(void*)(3*4)            // array buffer offset
			);

			static std::vector<unsigned int> indices;
			indices.clear();
// fill "indices" as needed

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(0);


			// Generate a buffer for the indices
			glGenBuffers(1, &elementbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			images.emplace_back(VertexArrayID,elementbuffer,std::move(img));
		}

	};
}


#endif //INC_2DSHADOWS_RENDER_ENGINE_HPP
