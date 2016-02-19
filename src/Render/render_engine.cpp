//
// Created by jerry on 15.02.16.
//

#include "render_engine.hpp"
#include "../UI/main_screen.hpp"

namespace render {
	void render_engine::render(){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendFunc(GL_ONE, GL_ONE);
		_shadow_render_shader->use_shader();
		GLint matrix_id = _shadow_render_shader->getUniform("MVP");
		auto color_id =  _shadow_render_shader->getUniform("Color");
		auto res_id =  _shadow_render_shader->getUniform("light_resolution");
		glUniform4f(color_id,1.0f,1.0f,1.0f,1.0f);
		glUniform2f(res_id,256.f*4.f,256.f*4.f);
		for(auto& img : _ligth_images){
			glBindVertexArray(std::get<0>(img));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(img));

			glBindTexture(GL_TEXTURE_2D, std::get<2>(img));
			glm::mat4 mvp = glm::translate(_mvp,std::get<3>(img));
			glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	std::tuple<GLuint,GLuint> render_engine::add_image(int width,int height,bool flipu){
		GLuint VertexArrayID;
		GLuint vertexbuffer;
		GLuint elementbuffer;
		glGenVertexArrays(1, &VertexArrayID);
		//cout << "gogo" << endl;
		glBindVertexArray(VertexArrayID);


		// An array of 3 vectors which represents 3 vertices
		const GLfloat g_vertex_buffer_data[] = {
				0.0f, 0.0f, 0.0f,//Position
				(flipu?1.f:0.f), 0.0f, //UV

				float(width), 0.0f, 0.0f,//Position
				(flipu?0.f:1.f), 0.0f, //UV

				float(width),  float(height), 0.0f,//Position
				(flipu?0.f:1.f), 1.0f, //UV

				0.0f,  float(height), 0.0f,//Position
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

	void render_engine::add_image(std::string path){
		auto img = texture(path);

		auto data = add_image(img.width,img.height);
		auto image_tupel = std::tuple<GLuint,GLuint,texture>(std::get<0>(data),std::get<1>(data),std::move(img));

		_ocluder_images.emplace_back(std::move(image_tupel));
	}

	void render_engine::calc_Light() {
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		GLuint oclusion_fbo;
		GLuint ligthsize = 256*4;
		glm::vec3 pos{200,200,0};

		glGenFramebuffers(1,&oclusion_fbo);
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

		// Poor filtering. Needed !
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Set "oclusion_texture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, oclusion_texture, 0);

		// The depth buffer
		/*GLuint depthrenderbuffer;
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);*/

		glBindFramebuffer(GL_FRAMEBUFFER,oclusion_fbo);
		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		// Always check that our framebuffer is ok

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)	return;
		std::cout<<"go to shadows"<<std::endl;
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

		GLuint shadow1D_fbo;

		glGenFramebuffers(1,&shadow1D_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);

		// The texture we're going to render to
		GLuint shadow1D_texture;
		glGenTextures(1, &shadow1D_texture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, shadow1D_texture);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, ligthsize,1, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set "oclusion_texture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadow1D_texture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
		// Set the list of draw buffers.
		GLenum Draw_Buffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, Draw_Buffers); // "1" is the size of DrawBuffers

		// Always check that our framebuffer is ok
		auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if(result == GL_FRAMEBUFFER_UNDEFINED) std::cout << "GL_FRAMEBUFFER_UNDEFINED"<<std::endl;

		if(result == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"<<std::endl;// is returned if any of the framebuffer attachment points are framebuffer incomplete.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"<<std::endl;//  is returned if the framebuffer does not have at least one image attached to it.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"<<std::endl;//  is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"<<std::endl;//  is returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.

		if(result == GL_FRAMEBUFFER_UNSUPPORTED) std::cout << "GL_FRAMEBUFFER_UNSUPPORTED"<<std::endl;//  is returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"<<std::endl;//  is returned if the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"<<std::endl;//  is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.

		if(result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"<<std::endl;//  is returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.

		if(result != GL_FRAMEBUFFER_COMPLETE)	return;
		std::cout<<"go to shadows 2"<<std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glBindFramebuffer(GL_FRAMEBUFFER,shadow1D_fbo);
		glViewport(0,0,ligthsize,1);
		glClearColor(0.f,0.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		_shadow_mapper_shader->use_shader();
		GLint id = _shadow_mapper_shader->getUniform("light_resolution");
		glUniform2f(id,ligthsize,ligthsize);
		glBindTexture(GL_TEXTURE_2D, oclusion_texture);


		GLuint quad_VertexArrayID;
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

		GLuint quad_vertexbuffer;
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

		//mvp = glm::mat4{1};
		//mvp *= glm::ortho(0.f, float(ligthsize), 0.f, 1.f);

		glDrawArrays(GL_TRIANGLES,0,6);

		//set back to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,ui::main_screen::width,ui::main_screen::height);

		auto pic = add_image(ligthsize,ligthsize,false);

		_ligth_images.emplace_back(std::get<0>(pic),std::get<1>(pic),shadow1D_texture,glm::vec3{pos.x-(ligthsize/2),pos.y-(ligthsize/2),0});
	}
}