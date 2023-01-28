#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, Vector4 defaultColour = Vector4(1, 1, 1, 1));
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			void SetDefaultColour(const Vector4& c) {
				defaultColour = c;
			}

			void SetColourToDefault() {
				colour = defaultColour;
			}

			Vector4 GetColour() const {
				return colour;
			}

		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
			Vector4         defaultColour;
		};
	}
}
