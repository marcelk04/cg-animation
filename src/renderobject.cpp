#include "renderobject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderObject::RenderObject(Mesh& mesh, const glm::vec3& color)
	: RenderObject(mesh, color, glm::mat4(1.0f)) {
}

RenderObject::RenderObject(Mesh& mesh, const glm::vec3& color, const glm::vec3& position, const float scale) 
 	: RenderObject(mesh, color, glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(scale))) {
}

RenderObject::RenderObject(Mesh& mesh, const glm::vec3& color, const glm::mat4& model) 
	: m_Mesh(mesh), m_Color(color), m_Model(model) {

}

void RenderObject::draw(Program& program) {
	program.bind();
	program.set("uObjectColor", m_Color);
	program.set("uLocalToWorld", m_Model);
	
	m_Mesh.draw();
}