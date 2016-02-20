//
// Created by jerry on 17.02.16.
//

#ifndef INC_2DSHADOWS_TEXTURE_HPP
#define INC_2DSHADOWS_TEXTURE_HPP

#include <GL/glew.h>
#include <lodepng.h>
#include <vector>
#include <iostream>

namespace render {
	class texture{
	private:
	public:
		//std::vector<unsigned char> data;
		GLuint texture_id;
		unsigned width, height;

		texture(std::string path){
			std::vector<unsigned char> png;
			std::vector<unsigned char> image; //the raw pixels
			std::vector<unsigned char> bufimage;
			std::vector<unsigned char> data;

			//load and decode
			lodepng::load_file(png, path);
			unsigned error = lodepng::decode(bufimage, width, height, png);


			for(auto i = 0; i < width*height;++i){ // converting into opengl frendly pixel layout
				auto a = bufimage.back();
				bufimage.pop_back();
				auto b = bufimage.back();
				bufimage.pop_back();
				auto g = bufimage.back();
				bufimage.pop_back();
				auto r = bufimage.back();
				bufimage.pop_back();

				data.push_back(r);
				data.push_back(g);
				data.push_back(b);
				data.push_back(a);
			}

			//if there's an error, display it
			if (error) std::cerr << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);

			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		void bind(GLuint sampler = 0){
			glActiveTexture(GL_TEXTURE0 + sampler);
			glBindTexture(GL_TEXTURE_2D, texture_id);
		}
	};
}

#endif //INC_2DSHADOWS_TEXTURE_HPP
