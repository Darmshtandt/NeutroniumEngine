#pragma once

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
		Collider() = default;

		void Render(Renderer* pRenderer) const {
			glDepthFunc(GL_ALWAYS);
			glLineWidth(10);
			pRenderer->SetDrawingMode(Renderer::DrawingMode::LINE_STRIP);
			m_Model.Render(pRenderer);
			pRenderer->SetDrawingMode(Renderer::DrawingMode::TRIANGLES);
			glLineWidth(1);
			glDepthFunc(GL_LESS);
		}

		[[nodiscard]]
		Float3D FindFurthestPoint(const Float3D& direction) const {
			Float3D maxPoint;
			Float maxDistance = -FLT_MAX;

			Matrix4x4 transformedLocalWorld = m_LocalWorld.Transposition();

			const Float3D left = m_LocalWorld[0];
			const Float3D up = m_LocalWorld[1];
			const Float3D lookAt = m_LocalWorld[2];
			const Float3D eye = m_LocalWorld[3];

			transformedLocalWorld[0].xyz = left / left.LengthSquare();
			transformedLocalWorld[0].w = -eye.Dot(left) / left.LengthSquare();

			transformedLocalWorld[1].xyz = up / up.LengthSquare();
			transformedLocalWorld[1].w = -eye.Dot(up) / up.LengthSquare();

			transformedLocalWorld[2].xyz = lookAt / lookAt.LengthSquare();
			transformedLocalWorld[2].w = -eye.Dot(lookAt) / lookAt.LengthSquare();
			transformedLocalWorld[3][3] = 1.f;

			const Float3D transformedDirection = (transformedLocalWorld * direction).GetNormalize();
			for (Float3D point : m_Points) {
				const Float pointLength = point.Dot(direction);
				if (maxDistance < pointLength) {
					maxDistance = pointLength;
					maxPoint = point;
				}
			}
			return maxPoint + m_LocalWorld[3].xyz;
		}
		[[nodiscard]]
		Float3D Support(const Collider& collider, const Float3D& direction) const {
			return FindFurthestPoint(direction) - collider.FindFurthestPoint(-direction);
		}

		[[nodiscard]]
		Bool Line(Simplex& simplex, Float3D& direction) const {
			const Float3D a = simplex[0];
			const Float3D b = simplex[1];

			const Float3D ab = b - a;
			const Float3D ao = -a;

			if (ab.Dot(ao) > 0) {
				direction = ab.GetCross(ao).GetCross(ab);
			}
			else {
				simplex = { a };
				direction = ao;
			}
			return false;
		}
		[[nodiscard]]
		Bool Triangle(Simplex& simplex, Float3D& direction) const {
			const Float3D a = simplex[0];
			const Float3D b = simplex[1];
			const Float3D c = simplex[2];

			const Float3D ab = b - a;
			const Float3D ac = c - a;
			const Float3D ao = -a;

			const Float3D abc = ab.GetCross(ac);

			if (abc.GetCross(ac).Dot(ao) > 0) {
				if (ac.Dot(ao) > 0) {
					simplex = { a, c };
					direction = ac.GetCross(ao).GetCross(ac);
				}
				else {
					simplex = { a, b };
					return Line(simplex, direction);
				}
			}
			else if (ab.GetCross(abc).Dot(ao) > 0) {
				simplex = { a, b };
				return Line(simplex, direction);
			}
			else {
				if (abc.Dot(ao) > 0) {
					direction = abc;
				}
				else {
					simplex = { a, c, b };
					direction = -abc;
				}
			}
			return false;
		}
		[[nodiscard]]
		Bool Polyhedron(Simplex& simplex, Float3D& direction) const {
			const Float3D a = simplex[0];
			const Float3D b = simplex[1];
			const Float3D c = simplex[2];
			const Float3D d = simplex[3];

			const Float3D ab = b - a;
			const Float3D ac = c - a;
			const Float3D ad = d - a;
			const Float3D ao = -a;

			const Float3D abc = ab.GetCross(ac);
			const Float3D acd = ac.GetCross(ad);
			const Float3D adb = ad.GetCross(ab);

			if (abc.Dot(ao) > 0) {
				simplex = { a, b, c };
				return Triangle(simplex, direction);
			}
			else if (acd.Dot(ao) > 0) {
				simplex = { a, c, d };
				return Triangle(simplex, direction);
			}
			else if (adb.Dot(ao) > 0) {
				simplex = { a, d, b };
				return Triangle(simplex, direction);
			}

			direction = { };
			return true;
		}

		[[nodiscard]]
		Bool NextSimplex(Simplex& simplex, Float3D& direction) const {
			switch (simplex.GetSize()) {
			case 2:
				return Line(simplex, direction);
			case 3:
				return Triangle(simplex, direction);
			case 4:
				return Polyhedron(simplex, direction);
			}
			return true;
		}

		[[nodiscard]]
		std::pair<Bool, Simplex> GJK(const Collider& collider) const {
			Simplex simplex;
			simplex.Add(Support(collider, { 1.f, 0.f, 0.f }));

			Float3D direction = -simplex[0];
			do {
				Float3D point = Support(collider, direction);
				if (point.Dot(direction) <= 0)
					return std::make_pair(false, Simplex());

				simplex.Add(point);
			} 
			while (!NextSimplex(simplex, direction));
			return std::make_pair(true, simplex);
		}

		std::pair<FaceNormalContainer, uInt> GetFaceNormals(const PointContainer& points, const uIntContainer& faces) const {
			FaceNormalContainer normalContainer;
			normalContainer.reserve(faces.size() / 3);

			uInt nearFaceIndex = 0;
			Float minDistance = FLT_MAX;
			for (uInt i = 0; i < faces.size(); i += 3) {
				const Float3D a = points[faces[i + 0]];
				const Float3D b = points[faces[i + 1]];
				const Float3D c = points[faces[i + 2]];

				Float3D normal = (b - a).GetCross(c - a).GetNormalize();
				Float distance = normal.Dot(a);
				if (distance < -0.000001f) {
					normal = -normal;
					distance = -distance;
				}

				normalContainer.emplace_back(FaceNormal(normal, distance));
				if (distance < minDistance) {
					nearFaceIndex = i / 3;
					minDistance = distance;
				}
			}
			return std::make_pair(normalContainer, nearFaceIndex);
		}

		void AddIfUniqueEdge(std::vector<std::pair<uInt, uInt>>& edges, const std::vector<uInt>& faces, const uInt& a, const uInt& b) {
			auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));
			if (reverse != edges.end())
				edges.erase(reverse);
			else
				edges.emplace_back(faces[a], faces[b]);
		}

		CollisionPoint EPA(const Simplex& simplex, const Collider& collider) {
			std::vector<Float3D> polytope;
			for (uInt i = 0; i < simplex.GetSize(); ++i)
				polytope.push_back(simplex[i]);

			std::vector<uInt> faces = {
				0, 1, 2,
				0, 3, 1,
				0, 2, 3,
				1, 3, 2
			};

			std::pair<FaceNormalContainer, uInt> faceNormals = GetFaceNormals(polytope, faces);
			FaceNormalContainer& normalContainer = faceNormals.first;
			uInt& minFace = faceNormals.second;

			Float3D minNormal = normalContainer[minFace].normal;
			Float minDistance = FLT_MAX;

			uInt iters = 0;
			while (minDistance == FLT_MAX && (iters++ < m_Points.size() + collider.m_Points.size())) {
				minNormal = normalContainer[minFace].normal;
				minDistance = normalContainer[minFace].distance;

				Float3D support = Support(collider, minNormal);
				Float supportDistance = minNormal.Dot(support);

				if (std::abs(supportDistance - minDistance) > 0.0001f) {
					minDistance = FLT_MAX;
					std::vector<std::pair<uInt, uInt>> uniqueEdges;

					uInt f = 0;
					for (auto& normal : normalContainer) {
						if (normal.normal.Dot(support) > 0) {
							AddIfUniqueEdge(uniqueEdges, faces, f + 0, f + 1);
							AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
							AddIfUniqueEdge(uniqueEdges, faces, f + 2, f + 0);

							faces.erase(faces.begin() + f);
							faces.erase(faces.begin() + f);
							faces.erase(faces.begin() + f);
						}
						else {
							f += 3;
						}
					}

					std::vector<uInt> newFaces;
					newFaces.reserve(uniqueEdges.size() * 3);
					for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
						newFaces.push_back(edgeIndex1);
						newFaces.push_back(edgeIndex2);
						newFaces.push_back(polytope.size());
					}
					polytope.push_back(support);

					faces.insert(faces.end(), newFaces.begin(), newFaces.end());

					auto newFaceNormals = GetFaceNormals(polytope, faces);

					normalContainer = std::move(newFaceNormals.first);
					minFace = newFaceNormals.second;
				}
			}

			if (std::abs(minDistance - FLT_MAX) < 0.000001f)
				return CollisionPoint { minNormal, 0 };
			return CollisionPoint { minNormal, minDistance + 0.0001f };
		}

		Bool RayIntersectTriangleTest(const Ray& ray, const Float3D face[3]) const {
			const Float3D ab = face[1] - face[0];
			const Float3D ac = face[2] - face[0];

			Float3D P = ray.Direction().GetCross(ac);
			Float determinant = ab.Dot(P);
			if (std::abs(determinant) < 0.000001)
				return false;

			Float inverseDeterminant = 1.0 / determinant;
			Float3D rayStartToA = ray.Start - face[0];

			Float intersectionParam1 = rayStartToA.Dot(P) * inverseDeterminant;
			if (intersectionParam1 < 0 || intersectionParam1 > 1)
				return false;

			const Float3D qvec = rayStartToA.GetCross(ab);

			Float intersectionParam2 = ray.Direction().Dot(qvec) * inverseDeterminant;
			if (intersectionParam2 < 0 || intersectionParam1 + intersectionParam2 > 1)
				return false;
			return true;
		}

		Bool RayCastTest(const Ray& ray) const {
			for (uInt i = 2; i < m_Points.size(); i += 3) {
				const Float3D face[3] = {
					m_Points[i - 2] - m_LocalWorld.Rows[3],
					m_Points[i - 1] - m_LocalWorld.Rows[3],
					m_Points[i - 0] - m_LocalWorld.Rows[3]
				};
				if (RayIntersectTriangleTest(ray, face))
					return true;
			}
			return false;
		}

		void SetLocalWorld(const Matrix4x4& localWorld) noexcept {
			m_LocalWorld = localWorld;
			m_Model.SetPosition(m_LocalWorld[3].xyz);
			m_Model.SetAngle(m_LocalWorld.GetEulerAngles() / RADf);
		}
		void SetShape(const Shape& shape) {
			m_Points.clear();
			if (shape.Indices.size() == 0) {
				for (Index_t index : shape.Indices)
					m_Points.push_back(shape.Vertices[index].Position);
			}
			else {
				for (Vertex vertex : shape.Vertices)
					m_Points.push_back(vertex.Position);
			}
			m_Model.SetMesh(shape);
		}
		void SetPointContainer(const PointContainer& points) {
			m_Points = points;

			Shape shape;
			for (const Float3D& point : m_Points) {
				Vertex vertex = { };
				vertex.Position = point;
				vertex.Color = Colors::White;
				shape.Vertices.push_back(vertex);
			}
			m_Model.SetMesh(shape);
		}

		const PointContainer& GetPointContainer() const {
			return m_Points;
		}

	private:
		PointContainer m_Points;
		Matrix4x4 m_LocalWorld;
		Model m_Model;
	};
}