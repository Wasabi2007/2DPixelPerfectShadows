//
// Created by jerry on 17.02.16.
//

#ifndef INC_2DSHADOWS_MODEL_HPP
#define INC_2DSHADOWS_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>

namespace render {

	struct model{
		std::vector<float> data;

		void add(glm::vec3 pos, glm::vec2 uv){
			data.emplace_back(pos.x);
			data.emplace_back(pos.y);
			data.emplace_back(pos.z);
			data.emplace_back(uv.x);
			data.emplace_back(uv.y);
		}
	};
}
#endif //INC_2DSHADOWS_MODEL_HPP
