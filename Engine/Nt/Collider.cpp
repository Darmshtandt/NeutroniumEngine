#include <GL/GLEW.h>
#include <GL/GL.h>

#include <Nt/Collider.h>

#include <cassert>

Nt::Collider::Collider() :
	m_Model(&m_Mesh)
{
}

void Nt::Collider::Render(NotNull<Renderer*> pRenderer) const {
	if (!m_IsVisible)
		return;

	glDepthFunc(GL_ALWAYS);
	glLineWidth(10);
	pRenderer->SetDrawingMode(Renderer::DrawingMode::LINE_STRIP);
	m_Model.Render(pRenderer);
	pRenderer->SetDrawingMode(Renderer::DrawingMode::TRIANGLES);
	glLineWidth(1);
	glDepthFunc(GL_LESS);
}

void Nt::Collider::Show() noexcept {
	if (!m_IsVisible)
		m_IsVisible = true;
}

void Nt::Collider::Hide() noexcept {
	if (m_IsVisible)
		m_IsVisible = false;
}

void Nt::Collider::ToggleVisible(const Bool& enabled) noexcept {
	if (m_IsVisible != enabled)
		m_IsVisible = enabled;
}

void Nt::Collider::ToggleVisible() noexcept {
	m_IsVisible = !m_IsVisible;
}

Nt::Float3D Nt::Collider::FindFurthestPoint(const Float3D& direction) const {
	Float3D maxPoint;
	Float maxDistance = -FLT_MAX;

	const Float3D worldDirection = (m_LocalWorld * Float4D(direction, 1.f)).GetNormalize().xyz;
	for (const Float3D& point : m_Points) {
		const Float pointLength = point.Dot(worldDirection);
		if (maxDistance < pointLength) {
			maxDistance = pointLength;
			maxPoint = point;
		}
	}
	return (m_LocalWorld * Float4D(maxPoint, 1.f)).xyz + m_LocalWorld[3].xyz;
}

Nt::Float3D Nt::Collider::Support(const Collider& collider, const Float3D& direction) const {
	return FindFurthestPoint(direction) - collider.FindFurthestPoint(-direction);
}

Bool Nt::Collider::Line(Simplex& simplex, Float3D& direction) const {
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

Bool Nt::Collider::Triangle(Simplex& simplex, Float3D& direction) const {
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

Bool Nt::Collider::Polyhedron(Simplex& simplex, Float3D& direction) const {
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

Bool Nt::Collider::NextSimplex(Simplex& simplex, Float3D& direction) const {
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

std::pair<Bool, Nt::Simplex> Nt::Collider::GJK(const Collider& collider) const {
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

std::pair<Nt::Collider::FaceNormalContainer, uInt> Nt::Collider::GetFaceNormals(const PointContainer& points, const uIntContainer& faces) const {
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

void Nt::Collider::AddIfUniqueEdge(std::vector<std::pair<uInt, uInt>>& edges, const std::vector<uInt>& faces, const uInt& a, const uInt& b) {
	auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));
	if (reverse != edges.end())
		edges.erase(reverse);
	else
		edges.emplace_back(faces[a], faces[b]);
}

Nt::CollisionPoint Nt::Collider::EPA(const Simplex& simplex, const Collider& collider) {
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

			uInt pointID = 0;
			for (auto& normal : normalContainer) {
				if (normal.normal.Dot(support) > 0) {
					AddIfUniqueEdge(uniqueEdges, faces, pointID + 0, pointID + 1);
					AddIfUniqueEdge(uniqueEdges, faces, pointID + 1, pointID + 2);
					AddIfUniqueEdge(uniqueEdges, faces, pointID + 2, pointID + 0);

					faces.erase(faces.begin() + pointID, faces.begin() + (pointID + 3));
				}
				else {
					pointID += 3;
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

Int Nt::Collider::RayCastTest(const Ray& ray, Float3D* pResultIntersectionPoint /*= nullptr*/) const {
	Float3D intersectionPoint;
	for (uInt i = 2; i < m_Points.size(); i += 3) {
		const Float3D face[3] = {
			_GetPointRealPosition(m_Points[i - 2]),
			_GetPointRealPosition(m_Points[i - 1]),
			_GetPointRealPosition(m_Points[i - 0]),
		};

		if (ray.IntersectTriangleTest(face, intersectionPoint)) {
			if (pResultIntersectionPoint != nullptr)
				(*pResultIntersectionPoint) = intersectionPoint;
			return (i / 3);
		}
	}
	return -1;
}

void Nt::Collider::SetLocalWorld(const Matrix4x4& localWorld) noexcept {
	m_LocalWorld = localWorld;
	m_Model.SetPosition(m_LocalWorld[3].xyz);
	m_Model.SetAngle(m_LocalWorld.GetEulerAngles());
}

void Nt::Collider::SetShape(const Shape& shape) {
	m_Points.clear();
	if (shape.Indices.empty()) {
		m_Points.reserve(shape.Vertices.size());
		for (const Vertex& vertex : shape.Vertices)
			m_Points.emplace_back(vertex.Position.xyz);
	}
	else {
		m_Points.reserve(shape.Indices.size());
		for (Index_t index : shape.Indices)
			m_Points.emplace_back(shape.Vertices[index].Position.xyz);
	}

	m_Mesh.SetShape(shape);
}

void Nt::Collider::SetPointContainer(const PointContainer& points) {
	m_Points = points;

	Shape shape;
	for (const Float3D& point : m_Points) {
		Vertex vertex = { };
		vertex.Position.xyz = point;
		vertex.Color = Colors::White;

		shape.Vertices.push_back(vertex);
	}

	m_Mesh.SetShape(shape);
}

const Nt::Collider::PointContainer& Nt::Collider::GetPointContainer() const {
	return m_Points;
}

const Nt::Model& Nt::Collider::GetModel() const noexcept {
	return m_Model;
}

Bool Nt::Collider::IsVisible() const noexcept {
	return m_IsVisible;
}

Nt::Float3D Nt::Collider::_GetPointRealPosition(const Float3D& point) const noexcept {
	return (m_LocalWorld * Float4D(point, 1.f)).xyz;
}