//
// Created by jerry on 15.02.16.
//

#include "render_engine.hpp"
#include "../UI/main_screen.hpp"

namespace render {
	void render_engine::render(){

		//First we render everything that should be in the Background
		_basicshader->use_shader();
		GLint matrix_id = _basicshader->getUniform("MVP");
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &_mvp[0][0]);

		for(auto& img : _background_images){
			glBindVertexArray(std::get<0>(img));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(img));

			std::get<2>(img).bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}


		//Render every ligth that is queued
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		_shadow_render_shader->use_shader();
		matrix_id = _shadow_render_shader->getUniform("MVP");
		auto color_id =  _shadow_render_shader->getUniform("Color");
		auto res_id =  _shadow_render_shader->getUniform("light_resolution");
		auto selected_id =  _shadow_render_shader->getUniform("selected");
		auto blur_factor_id =  _shadow_render_shader->getUniform("blur_factor");
		glUniform1f(blur_factor_id,global_blur_factor);


		for(auto& img : _ligth_images){
			glBindVertexArray(img.vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, img.ibo);

			glBindTexture(GL_TEXTURE_2D, img.shadowmap_texture);
			glm::mat4 mvp = glm::translate(_mvp,img.position);
			glUniform4fv(color_id,1,&img.color[0]);
			glUniform1f(selected_id,img.selceted);
			glUniform2fv(res_id,1,&img.size[0]);
			glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto error = glGetError();

		if(error != GL_NO_ERROR){
			//std::cerr << "this shouldn't happend" << std::endl;
		}

		//Render everything that blocks Light.
		_basicshader->use_shader();
		matrix_id = _basicshader->getUniform("MVP");
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &_mvp[0][0]);

		for(auto& img : _ocluder_images){
			glBindVertexArray(std::get<0>(img));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(img));

			std::get<2>(img).bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

	}

	void render_engine::add_background(std::string path, float scaling){
		auto img = texture(path);

		auto data = add_image(img.width*scaling,img.height*scaling);
		auto image_tupel = std::tuple<GLuint,GLuint,texture>(std::get<0>(data),std::get<1>(data),std::move(img));

		_background_images.emplace_back(std::move(image_tupel));
	}

	//render what the light "sees"
	void render_engine::render_ocluders(glm::mat4 &mvp) {
		_basicshader->use_shader();
		GLint matrix_id = _basicshader->getUniform("MVP");
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

		for(auto& img : _ocluder_images){
			glBindVertexArray(std::get<0>(img));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(img));

			std::get<2>(img).bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	std::tuple<GLuint,GLuint> render_engine::add_image(float width, float height, bool flipu){
		GLuint VertexArrayID;
		GLuint vertexbuffer;
		GLuint elementbuffer;
		glGenVertexArrays(1, &VertexArrayID);
		//cout << "gogo" << endl;
		glBindVertexArray(VertexArrayID);


		//A Simple quad for the image.
		const GLfloat g_vertex_buffer_data[] = {
				0.0f, 0.0f, 0.0f,//Position
				(flipu?1.f:0.f), 0.0f, //UV

				width, 0.0f, 0.0f,//Position
				(flipu?0.f:1.f), 0.0f, //UV

				width,  height, 0.0f,//Position
				(flipu?0.f:1.f), 1.0f, //UV

				0.0f,  height, 0.0f,//Position
				(flipu?1.f:0.f), 1.0f, //UV
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

		return std::tuple<GLuint,GLuint>(VertexArrayID, elementbuffer);
	}

	void render_engine::add_image(std::string path, float scaling) {
		auto img = texture(path);

		auto data = add_image(img.width*scaling,img.height*scaling);
		auto image_tupel = std::tuple<GLuint,GLuint,texture>(std::get<0>(data),std::get<1>(data),std::move(img));

		_ocluder_images.emplace_back(std::move(image_tupel));
	}

	void render_engine::move_light(int index,glm::vec2 position){
		//rerender the shadow map for the moved ligth
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		auto& ligth = _ligth_images.at(index);

		auto& oclusion_texture = ligth.ocluder_texture;
		auto& shadow1D_texture = ligth.shadowmap_texture;
		auto& size = ligth.size;
		ligth.position = glm::vec3{position.x-(size.x/2),position.y-(size.y/2),0};//glm::vec3{position.x,position.y,0};

		glBindFramebuffer(GL_FRAMEBUFFER,oclusion_fbo);
		glBindTexture(GL_TEXTURE_2D, oclusion_texture);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, oclusion_texture, 0);

		auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		framebuffer_check(result);

		if(result != GL_FRAMEBUFFER_COMPLETE)	return;

		glViewport(0,0,size.x,size.y);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		auto mvp = glm::mat4{1};
		auto ligthsize_half = (size.x/2.f);
		mvp = glm::ortho(0.f, float(size.x), 0.f, float(size.y));
		mvp *= glm::translate(glm::mat4{1},glm::vec3(-position.x+ligthsize_half,-position.y+ligthsize_half,0));
		render_ocluders(mvp);

		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
		glBindTexture(GL_TEXTURE_2D, shadow1D_texture);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadow1D_texture, 0);

		result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		framebuffer_check(result);

		if(result != GL_FRAMEBUFFER_COMPLETE)	return;

		glViewport(0,0,size.x,1);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		_shadow_mapper_shader->use_shader();
		GLint id = _shadow_mapper_shader->getUniform("light_resolution");
		glUniform2f(id,size.x,size.y);
		glBindTexture(GL_TEXTURE_2D, oclusion_texture);

		glBindVertexArray(quad_VertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,6);

		//set back to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,ui::main_screen::width,ui::main_screen::height);
	}

	void render_engine::remove_light(int index){
		if(index != -1)	_ligth_images.erase(_ligth_images.begin()+index);
	}

	void render_engine::add_light(unsigned int size, glm::vec2 position, glm::vec4 color) {
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		GLuint ligthsize = size;
		glm::vec3 pos{position,0};

		glBindFramebuffer(GL_FRAMEBUFFER,oclusion_fbo);

		// The texture we're going to render to
		GLuint oclusion_texture;
		glGenTextures(1, &oclusion_texture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, oclusion_texture);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, ligthsize, ligthsize, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Set "oclusion_texture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, oclusion_texture, 0);

		// Always check that our framebuffer is ok
		auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		framebuffer_check(result);

		if(result != GL_FRAMEBUFFER_COMPLETE)	return;
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glBindFramebuffer(GL_FRAMEBUFFER,oclusion_fbo);
		glViewport(0,0,ligthsize,ligthsize);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		auto mvp = glm::mat4{1};
		auto ligthsize_half = (ligthsize/2.f);
		mvp = glm::ortho(0.f, float(ligthsize), 0.f, float(ligthsize));
		mvp *= glm::translate(glm::mat4{1},glm::vec3(-pos.x+ligthsize_half,-pos.y+ligthsize_half,0));
		render_ocluders(mvp);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);

		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
		// The texture we're going to render to
		GLuint shadow1D_texture;
		glGenTextures(1, &shadow1D_texture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, shadow1D_texture);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, ligthsize,1, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //set to repeat so we can oversample the circle
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadow1D_texture, 0);

		// Always check that our framebuffer is ok
		result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		framebuffer_check(result);

		if(result != GL_FRAMEBUFFER_COMPLETE)	return;
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
		glViewport(0,0,ligthsize,1);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		_shadow_mapper_shader->use_shader();
		GLint id = _shadow_mapper_shader->getUniform("light_resolution");
		glUniform2f(id,ligthsize,ligthsize);
		glBindTexture(GL_TEXTURE_2D, oclusion_texture);


		glBindVertexArray(quad_VertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,6);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
		//set back to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,ui::main_screen::width,ui::main_screen::height);

		auto pic = add_image(ligthsize,ligthsize,false);

		_ligth_images.emplace_back(std::get<0>(pic),std::get<1>(pic),oclusion_texture,shadow1D_texture,glm::vec3{pos.x-(ligthsize/2.f),pos.y-(ligthsize/2.f),0},color,glm::vec2(size,size));
	}

	void render_engine::framebuffer_check(GLenum result) const {
		if(result == GL_FRAMEBUFFER_UNDEFINED) std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;

		if(result == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" <<
														   std::endl;// is returned if any of the framebuffer attachment points are framebuffer incomplete.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" <<
																   std::endl;//  is returned if the framebuffer does not have at least one image attached to it.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" <<
															std::endl;//  is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" <<
															std::endl;//  is returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.

		if(result == GL_FRAMEBUFFER_UNSUPPORTED) std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" <<
												 std::endl;//  is returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" <<
															std::endl;//  is returned if the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" <<
															std::endl;//  is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" <<
															  std::endl;//  is returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.

	}

}