#ifndef OPENGL_CG_MESH_H
#define OPENGL_CG_MESH_H

#include "Includes.h"
#include "Group.h"

class Mesh {
public:
	vector<Group*> groups;
	vector<glm::vec3*> vertex;
	vector<glm::vec3*> normals;
	vector<glm::vec2*> mappings;
};

#endif 