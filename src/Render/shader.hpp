//
// Created by jerry on 16.02.16.
//

#ifndef INC_2DSHADOWS_SHADER_HPP
#define INC_2DSHADOWS_SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace render {
	enum class shader_type{
		Programm,
		Vertex,
		Fragment,
		Geometry
	};

	struct shader_attribute{
		unsigned int attribpos;
		std::string name;
	};

	class shader {
	private:
		GLuint shaderid;
		bool loaded;
		std::string shaderPath;
		shader_type type;
	public:
		shader(std::string shaderPath, shader_type type):shaderPath(shaderPath),type(type),shaderid(0),loaded(false){
			load_shader();
		}

		shader(shader vertex, shader fragment
				,const std::vector<shader_attribute>& AttribLocations
				,const std::vector<shader_attribute>& FragDataLocations
				,const std::vector<shader_attribute>& TextureBindings = std::vector<shader_attribute>()):shaderid(0),loaded(false),type(shader_type::Programm){
			load_programm(vertex, fragment, AttribLocations, FragDataLocations,TextureBindings);
		}

		~shader(){
			if(type == shader_type::Programm)
				glDeleteProgram(shaderid);
			else
				glDeleteShader(shaderid);
		}

		void use_shader(){
			glUseProgram(shaderid);
		}

		GLint getUniform(const std::string& name){
			return glGetUniformLocation(shaderid, name.c_str());
		}

	private:
		void load_programm(shader vertex, shader fragment
				,const std::vector<shader_attribute>& AttribLocations
				,const std::vector<shader_attribute>& FragDataLocations
				,const std::vector<shader_attribute>& TextureBindings){
			// Link the program
			//fprintf(stdout, "Linking program\n");
			shaderid = glCreateProgram();
			glAttachShader(shaderid, vertex.shaderid);
			glAttachShader(shaderid, fragment.shaderid);


			for(auto& at : AttribLocations) {
				glBindAttribLocation(shaderid, at.attribpos, at.name.c_str());
			}
			for(auto& at : FragDataLocations) {
				glBindFragDataLocation(shaderid, at.attribpos, at.name.c_str());
			}

			glLinkProgram(shaderid);


			GLint isLinked = 0;
			glGetProgramiv(shaderid, GL_LINK_STATUS, (int *)&isLinked);
			if(isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(shaderid, GL_INFO_LOG_LENGTH, &maxLength);

				//The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(shaderid, maxLength, &maxLength, &infoLog[0]);
				std::cerr << "ProgrammLogLength: " << maxLength<< std::endl;
				std::cerr << "GL_LINK_STATUS: " << isLinked<< std::endl;
				std::cerr << &infoLog[0] << std::endl;

				///return;
			}
			glUseProgram(shaderid);
			for(auto& at : TextureBindings) {
				glUniform1i(glGetUniformLocation(shaderid, at.name.c_str()), at.attribpos);
			}

			//check_shader();
		}

		void load_shader(){
			std::ifstream shader_file;
			shader_file.open(shaderPath);

			std::string shader;

			shader_file.seekg(0, std::ios::end);
			shader.reserve(shader_file.tellg());
			shader_file.seekg(0, std::ios::beg);

			shader.assign((std::istreambuf_iterator<char>(shader_file)),
					   std::istreambuf_iterator<char>());

			switch (type){
				case shader_type::Vertex :
					shaderid = glCreateShader(GL_VERTEX_SHADER);
					break;
				case shader_type::Fragment :
					shaderid = glCreateShader(GL_FRAGMENT_SHADER);
					break;
				case shader_type::Geometry :
					shaderid = glCreateShader(GL_GEOMETRY_SHADER);
					break;
			}



			char const * VertexSourcePointer = shader.c_str();
			glShaderSource(shaderid, 1, &VertexSourcePointer , NULL);
			glCompileShader(shaderid);
			check_shader();
		}

		void check_shader() {
			GLint Result = GL_FALSE;
			int InfoLogLength;
			// Check Shader
			glGetShaderiv(shaderid, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> ShaderErrorMessage(InfoLogLength);
			glGetShaderInfoLog(shaderid, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			if(Result == GL_FALSE) {
				std::cerr << "InfoLogLength: " << InfoLogLength << std::endl;
				std::cerr << "GL_COMPILE_STATUS: " << Result << std::endl;
				std::cerr << &ShaderErrorMessage[0] << std::endl;
			}

			if(Result == GL_TRUE)
				loaded = true;
		}

	};
}


#endif //INC_2DSHADOWS_SHADER_HPP
