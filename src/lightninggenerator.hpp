#pragma once

#include <glm/glm.hpp>

#include "framework/mesh.hpp"

#include <vector>

namespace LightningGenerator {
	using MeshData = std::pair<std::vector<Mesh::VertexPCN>, std::vector<uint32_t>>;

	struct Segment {
		glm::vec3 startpoint;
		glm::vec3 endpoint;
	};

	std::vector<Segment> genBolt(glm::vec3 startpoint, glm::vec3 endpoint, int generations, const glm::vec3& camDir);
	MeshData genMeshData(const std::vector<Segment>& segments, const glm::vec3& camDir);
	MeshData genMeshData(glm::vec3 startpoint, glm::vec3 endpoint, int generations, const glm::vec3& camDir);
};