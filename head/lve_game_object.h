#pragma once

#include "lve_model.h"
//libs
#include <glm/gtc/matrix_transform.hpp>
//std
#include <memory>
#include <unordered_map>

namespace lve
{
	
	struct TransformComponent
	{
		glm::vec3 translation;// position offset
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, LveGameObject>;

		static LveGameObject createGameObject()
		{
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		const id_t getId() const { return id; }

		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
		std::string path1;
		std::string path2;
			
	private:
		LveGameObject(id_t objId) : id(objId){}
		id_t id;
	};
}