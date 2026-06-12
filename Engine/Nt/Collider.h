#pragma once

#include <Nt/Graphics/Objects/Model.h>
#include <Nt/Graphics/Geometry/Ray.h>
#include <Nt/Graphics/Renderer.h>
#include <Nt/Physics/Simplex.h>

namespace Nt {
	struct CollisionPoint final {
		Float3D normal;
		Float depth;
	};
	struct FaceNormal final {
		Float3D normal;
		Float distance;
	};

	class Collider {
	public:
		using PointContainer = std::vector<Float3D>;
		using FaceNormalContainer = std::vector<FaceNormal>;
		using uIntContainer = std::vector<uInt>;

	public:
		Collider();

		void Render(NotNull<Renderer*> pRenderer) const;

		void Show() noexcept;
		void Hide() noexcept;
		void ToggleVisible(const Bool& enabled) noexcept;
		void ToggleVisible() noexcept;

		[[nodiscard]] Float3D FindFurthestPoint(const Float3D& direction) const;
		[[nodiscard]] Float3D Support(const Collider& collider, const Float3D& direction) const;

		[[nodiscard]] Bool Line(Simplex& simplex, Float3D& direction) const;
		[[nodiscard]] Bool Triangle(Simplex& simplex, Float3D& direction) const;
		[[nodiscard]] Bool Polyhedron(Simplex& simplex, Float3D& direction) const;

		[[nodiscard]] Bool NextSimplex(Simplex& simplex, Float3D& direction) const;

		[[nodiscard]] std::pair<Bool, Simplex> GJK(const Collider& collider) const;

		std::pair<FaceNormalContainer, uInt> GetFaceNormals(const PointContainer& points, const uIntContainer& faces) const;

		void AddIfUniqueEdge(std::vector<std::pair<uInt, uInt>>& edges, const std::vector<uInt>& faces, const uInt& a, const uInt& b);

		CollisionPoint EPA(const Simplex& simplex, const Collider& collider);

		Int RayCastTest(const Ray& ray, Float3D* pResultIntersectionPoint = nullptr) const;

		void SetLocalWorld(const Matrix4x4& localWorld) noexcept;
		void SetShape(const Shape& shape);
		void SetPointContainer(const PointContainer& points);

		[[nodiscard]] const PointContainer& GetPointContainer() const;
		[[nodiscard]] const Nt::Model& GetModel() const noexcept;
		[[nodiscard]] Bool IsVisible() const noexcept;

	private:
		Mesh m_Mesh;
		PointContainer m_Points;
		Matrix4x4 m_LocalWorld;
		Model m_Model;
		Bool m_IsVisible = false;

	private:
		Float3D _GetPointRealPosition(const Float3D& point) const noexcept;
	};
}