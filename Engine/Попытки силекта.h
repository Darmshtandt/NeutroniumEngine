#pragma once

if (true || m_Mouse.IsButtonPressed(Nt::Mouse::BUTTON_LEFT, false)) {
	ray.Origin = -m_Camera.GetPosition();

	//Nt::Float3D cameraDirection;
	//cameraDirection.x = -sinf(m_Camera.GetAngle().y * RADf);
	//cameraDirection.y = tanf(m_Camera.GetAngle().x * RADf);
	//cameraDirection.z = -cosf(m_Camera.GetAngle().y * RADf);
	//cameraDirection = { };
	//cameraDirection.x = -sinf(m_Camera.GetAngle().y * RADf) * cosf(-m_Camera.GetAngle().x * RADf);
	//cameraDirection.y = -sinf(m_Camera.GetAngle().y * RADf) * sinf(-m_Camera.GetAngle().x * RADf);
	//cameraDirection.z = -cosf(m_Camera.GetAngle().y * RADf);
	//cameraDirection = { };
	//cameraDirection = m_Camera.GetView() * Nt::Matrix4x4::GetRotateY(45) * Nt::Float3D(1.f, 1.f, -1.f);
	//cameraDirection = cameraDirection.GetNormalize();

	//Nt::Float3D vertical(0.f, 1.f, 0.f);
	//Nt::Float3D horizontal = cameraDirection.GetCross(vertical).GetNormalize();
	//vertical = horizontal.GetCross(cameraDirection).GetNormalize();


	//const Float TanFOV = tanf(m_Settings.Projection_FOV / 2.f * RADf);
	//vertical *= 2.f * m_Settings.Projection_Far * TanFOV;
	//horizontal *= 2.f * m_Settings.Projection_Far * TanFOV;

	//const Nt::Float2D windowSize = m_Window.GetClientRect().RightBottom;
	//horizontal *= windowSize.x / windowSize.y;

	//ray.Direction = ray.Origin - cameraDirection;
	//ray.Direction += vertical / 2.f - horizontal / 2.f;

	//const Nt::Float2D cursorPosition =
	//	//(m_Mouse.GetCursorPosition());
	//	m_Window.ScreenToClient(m_Mouse.GetCursorPosition());

	//ray.Direction += cursorPosition.x * (horizontal / windowSize.x);
	//ray.Direction -= cursorPosition.y * (vertical / windowSize.y);
	//ray.Direction.x = -ray.Direction.x;
	//ray.Direction = ray.Direction.GetNormalize();

	//ray.Direction = cameraDirection;
	//ray.Direction.x += (cursorPosition.x / windowSize.x * 2.f - 1.f) * cosf(m_Camera.GetAngle().y * RADf);
	//ray.Direction.y += (cursorPosition.y / windowSize.y * 2.f - 1.f) * tanf(m_Camera.GetAngle().x * RADf);
	//ray.Direction.z += -(cursorPosition.x / windowSize.x * 2.f - 1.f) * sinf(m_Camera.GetAngle().y * RADf);
	//ray.Direction = ray.Direction.GetNormalize();
	//ray.Direction = cameraDirection;

	///*printf("%f, %f\n", 
	//	cursorPosition.x / windowSize.x * 2.f - 1.f, 
	//	-(cursorPosition.y / windowSize.y * 2.f - 1.f));*/
	////printf("%f, %f\n", cursorPosition.x / windowSize.x * 2.f - 1.f, cursorPosition.y / windowSize.y * 2.f - 1.f);
	////printf("%f, %f\n", ray.Direction.x, ray.Direction.y);
	//printf("%f %f\n", cameraDirection.y, cameraDirection.z);

	//static bool a = true;
	//if (a) {
	//	m_Mouse.SetCursorPosition(windowSize / 2.f - m_Window.ScreenToClient({ 0, 0 }));
	//	a = false;
	//}

	// ================================================

	//const Float YawSin = sinf(m_Camera.GetAngle().y * RADf);
	//const Float YawCos = cosf(m_Camera.GetAngle().y * RADf);
	//const Float PitchSin = sinf(m_Camera.GetAngle().x * RADf);
	//const Float PitchCos = cosf(m_Camera.GetAngle().x * RADf);

	//Nt::Float3D cameraRight(YawCos, 0.f, -YawSin);
	//Nt::Float3D cameraUp(YawSin * PitchSin, PitchCos, YawCos * PitchSin);
	//Nt::Float3D cameraForward(YawSin * PitchCos, -PitchSin, YawCos * PitchCos);

	//const Nt::Float2D windowSize = m_Window.GetClientRect().RightBottom;
	//Nt::Float3D cursorPosition(
	//	m_Window.ScreenToClient(m_Mouse.GetCursorPosition()), 0.f);

	//glReadPixels(cursorPosition.x, cursorPosition.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &cursorPosition.z);

	//Int viewport[4];
	//glGetIntegerv(GL_VIEWPORT, viewport);
	//Double matrixView[16];
	//Double matrixProjection[16];
	//for (uInt i = 0; i < 16; ++i) {
	//	matrixView[i] = m_Window.GetView().Matrix[i];
	//	matrixProjection[i] = m_Window.GetProjection().Matrix[i];
	//}
	//gluUnProject(
	//	cursorPosition.x, cursorPosition.y, cursorPosition.z,
	//	matrixView, matrixProjection, viewport,
	//	(Double*)&ray.Direction.x, (Double*)&ray.Direction.y, (Double*)&ray.Direction.z);

	const Nt::Float2D windowSize = m_Window.GetClientRect().RightBottom;
	const Nt::Float2D cursorPosition(
		m_Window.ScreenToClient(m_Mouse.GetCursorPosition()));

	Nt::Float4D normalizedCoords;
	normalizedCoords.xy = (cursorPosition * 2.f) / windowSize - 1.f;
	normalizedCoords.zw = { -1.f, 1.f };

	normalizedCoords = (m_Window.GetView() * m_Window.GetProjection()).GetInverse() * normalizedCoords;
	normalizedCoords /= -normalizedCoords.w;
	ray.Direction = normalizedCoords.GetNormalize();
	ray.Direction.x = -ray.Direction.x;

	std::cout << normalizedCoords << std::endl;

	if (m_Scence.GetObjectsCount() > 0 && RayCastTest(ray, m_Scence[0]->GetMesh().GetShape()))
		m_Scence.RemoveObject(m_Scence[0]);

}