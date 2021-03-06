#pragma once
/************************************************************************/
/* BoundingBox
/* ------
/* Box that specifies where the edges of an object are.
/************************************************************************/
#include "Mesh.h"
#include "../Scene/Objects.h"
#include "../Scene/SceneObject.h"
#include "../Scene/MeshOverlay.h"

#include <glm\glm.hpp>


class BoundingBox : public SceneObject
{

private:
	glm::mat2x3 edges;

public:

	BoundingBox(glm::vec3 pos, glm::vec3 greater, glm::vec3 lesser);
	BoundingBox(MeshOverlay& mesh);//, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(ModelObject& obj, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(SceneObject& obj, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(Fluid& fluid);

	~BoundingBox();
	
	const glm::mat2x3& getEdges();
	const bool inBox(glm::vec3 vec);
	const bool intersect(BoundingBox* box);
	void draw();
};

inline const glm::mat2x3& BoundingBox::getEdges() { return edges;}

inline const bool BoundingBox::inBox(glm::vec3 vec)
{
	if(	vec.x >= edges[0].x && vec.x <= edges[1].x &&
		vec.y >= edges[0].y && vec.y <= edges[1].y &&
		vec.z >= edges[0].z && vec.z <= edges[1].z)
	{
		return true;
	}
	return false;
}
