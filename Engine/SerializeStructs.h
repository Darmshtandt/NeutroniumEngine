#pragma once

#include <Nt/Core/NtTypes.h>
#include <Nt/Core/Math/Vector.h>
#include <Nt/Graphics/Ex/Components/Physics.h>
#include <Nt/Graphics/Objects/IObject.h>
#include <Objects/Object.h>
#include <Objects/Primitives/Primitive.h>
#include <Script/Script.h>

namespace SerialStruct {
	struct NtIObject final {
		void Extract(const Nt::IObject* pIObject) {
			IsVisible = pIObject->IsVisible();
			Position = pIObject->GetPosition();
			Origin = pIObject->GetOrigin();
			Angle = pIObject->GetAngle();
			AngleOrigin = pIObject->GetAngleOrigin();
			Size = pIObject->GetSize();
			Color = pIObject->GetColor();
		}

		Nt::Float4D Color;
		Nt::Float3D Position;
		Nt::Float3D Origin;
		Nt::Float3D Angle;
		Nt::Float3D AngleOrigin;
		Nt::Float3D Size;
		Bool IsVisible;
	};

	struct NtRigidBody final {
		void Extract(const NtEx::RigidBody* pBody, const ::Object* pObject) {
			IObj.Extract(pObject);

			Mass = pBody->Body.GetMass();
			Friction = 0.f;
			FrictionStatic = 0.f;
			Enabled = true;
			EnabledCollision = pObject->EnabledCollider();
			EnabledGravitation = pObject->EnabledGravitation();
			IsActive = true;
		}

		NtIObject IObj;
		Float Mass;
		Float Friction;
		Float FrictionStatic;
		Bool IsActive;
		Bool EnabledCollision;
		Bool EnabledGravitation;
		Bool Enabled;
	};

	struct NtVertex final {
		void Extract(const Nt::Vertex& vert) {
			Position = vert.Position;
			Color = vert.Color;
			Normal = vert.Normal;
			TexCoord = vert.TexCoord;
		}

		Nt::Float4D Position;
		Nt::Float4D Color;
		Nt::Float4D Normal;
		Nt::Float3D TexCoord;
	};
	struct Triangle final {
		uInt A, B, C;
	};
	struct NtShape final {
		void Extract(const Nt::Shape& shape) {
			Vertices.clear();
			Triangles.clear();

			for (const Nt::Vertex& vert : shape.Vertices)
				Vertices.emplace_back().Extract(vert);

			for (uInt i = 2; i < shape.Indices.size(); i += 3) {
				Triangles.emplace_back({
					shape.Indices[i - 2],
					shape.Indices[i - 1],
					shape.Indices[i],
					});
			}
		}

		std::list<NtVertex> Vertices;
		std::list<Triangle> Triangles;
	};
	struct NtMesh final {
		void Extract(const Nt::Mesh* pMesh) {
			IsValide = false;
			if (!pMesh)
				return;

			if (!pMesh->GetFilePath().empty()) {
				FilePath = pMesh->GetFilePath();
				return;
			}

			Shape = NtShape();
			Shape.Extract(pMesh->GetShape());
			IsValide = true;
		}

		NtShape Shape;
		std::string FilePath;
		Bool IsValide = false;
	};
	struct NtTexture final {
		void Extact(const Nt::Texture* pTexture) {
			IsValide = false;
			if (!pTexture)
				return;

			FilePath = pTexture->GetFilePath();
			IsValide = true;
		}

		std::string FilePath;
		Bool IsValide = false;
	};

	struct ScriptData final {
		void Extract(const Script::Data& data) {
			FieldName = data.FieldName;
			Name = data.Name;
			Value = data.Value;
			Type = data.Type;
		}

		std::string FieldName;
		std::string Name;
		std::string Value;
		Script::Data::Types Type;
	};
	struct Script final {
		void Extract(const ::Script* pScript) {
			IsValide = false;
			if (!pScript)
				return;

			const auto datas = pScript->GetScriptData();
			for (const ::Script::Data& data : datas)
				DataList.emplace_back().Extract(data);

			IsValide = true;
		}

		std::list<ScriptData> DataList;
		std::string FilePath;
		Bool IsValide = false;
	};

	struct Object final {
		void Extract(const ::Object* pObject) {
			RigidBody.Extract(pObject.GetRigidBody(), pObject);

			Mesh.Extract(pObject->GetMesh().Get());
			Texture.Extact(pObject->GetTexture().Get());
			Script.Extract(pObject->GetScript());

			LayerName = pObject->GetLayerName();
			Name = pObject->GetName();
			IsInvisible = pObject->IsInvisible();
		}

		NtRigidBody RigidBody;
		NtMesh Mesh;
		NtTexture Texture;
		Script Script;
		std::string LayerName;
		std::string Name;
		Bool IsInvisible;
	};

	struct Primitive final {
		void Extract(const ::Primitive* pPrimitive) {
			Obj.Extract(pPrimitive);
			Mesh.Extract(pPrimitive->GetMesh().Get());
			TextureOffset = pPrimitive->GetTextureOffset();
			TextureScale = pPrimitive->GetTextureScale();
		}

		Object Obj;
		NtMesh Mesh;
		Nt::Float2D TextureOffset;
		Nt::Float2D TextureScale;
	};
	struct Entity final {
		void Extract(const ::Entity* pEntity) {
			Obj.Extract(pEntity);
		}

		Object Obj;
	};
}