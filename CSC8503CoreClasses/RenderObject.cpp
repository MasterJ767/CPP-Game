#include "RenderObject.h"
#include "MeshGeometry.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, Vector4 defaultColour) {
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->defaultColour = defaultColour;
	this->colour	= defaultColour;
}

RenderObject::~RenderObject() {

}