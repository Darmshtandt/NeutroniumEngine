#pragma once

#include <ObjectEvents.h>
#include <Nt/Core/EventBus.h>
#include <Tools/PropertyRegistrar.h>
#include <Nt/Graphics/Layouts.h>


class PropertyTransform : public PropertyComponent {
private:
	enum Texts {
		TEXT_POSITION,
		TEXT_X, TEXT_Y, TEXT_Z,

		TEXT_SIZE,
		TEXT_WIDTH, TEXT_HEIGHT, TEXT_LENGTH,

		TEXT_ANGLE,
		TEXT_ROLL, TEXT_PITCH, TEXT_YAW,
		TEXT_COUNT
	};
	enum TextEdits {
		TEXTEDIT_X, TEXTEDIT_Y, TEXTEDIT_Z,
		TEXTEDIT_WIDTH, TEXTEDIT_HEIGHT, TEXTEDIT_LENGTH,
		TEXTEDIT_ROLL, TEXTEDIT_PITCH, TEXTEDIT_YAW,
		TEXTEDIT_COUNT
	};

public:
	PropertyTransform(Selector* pSelector, Scene* pScene) :
		PropertyComponent(pSelector, TEXT_COUNT, TEXTEDIT_COUNT, 0),
		m_ContentLayout({ 2, 12 })
	{
		(void)pScene;
	}

	void Initialize(const Settings& settings) override {
		SetLanguage(settings.Language);

		constexpr Nt::FloatRect headerPadding(5.f, 5.f, 0.f, 0.f);
		constexpr Nt::FloatRect nonHeaderPadding(15.f, 0.f, 0.f, 0.f);

		Nt::IntRect windowRect = m_ClientRect;
		windowRect.Right = settings.PropertyWindowRect.Right - GetSystemMetrics(SM_CXDLGFRAME) * 2;
		windowRect.Bottom = Nt::TextEdit::DefaultSize.y + Int(headerPadding.Top + headerPadding.Bottom) / 3;
		windowRect.Bottom *= TEXT_COUNT;

		_Create(settings, settings.Language["Window.Property.Transform"], windowRect);
		RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		AddStyles(STYLE_BORDER);

		m_ContentLayout.SetParent(*this);
		m_ContentLayout.RemoveStyles(STYLE_OVERLAPPEDWINDOW);
		m_ContentLayout.Create(m_ClientRect, "Transform-layout");
		m_ContentLayout.SetPadding({ 10, 5, 10, 10 }, Nt::UnitType::UNIT_PIXEL);
		m_ContentLayout.Show();

		uInt textEditID = 0;
		for (uInt i = 0; i < TEXT_COUNT; ++i) {
			m_Texts[i].DisableDefaultRectSize();

			const Bool isHeader = (i % 4 == 0);
			if (isHeader) {
				m_ContentLayout[0][i].SetPadding(headerPadding, Nt::UnitType::UNIT_PIXEL);
			}
			else {
				Nt::TextEdit* textEditPtr = &m_TextEdits[textEditID];

				m_ContentLayout[0][i].SetPadding(nonHeaderPadding, Nt::UnitType::UNIT_PIXEL);
				m_ContentLayout.Insert(Nt::uInt2D(1, i), textEditPtr);

				textEditPtr->SetID(textEditID);
				textEditPtr->Create("0.0", true);
				textEditPtr->Show();

				++textEditID;
			}

			m_ContentLayout.Insert({ 0, i }, &m_Texts[i]);
		}
	}

	void Update() override {
		static Bool prevActiveState = false;

		const Bool isSelectedOnlyOne = (m_SelectorPtr->GetObjectCount() == 1);

		if ((!prevActiveState) && isSelectedOnlyOne) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.EnableWindow();

			prevActiveState = true;
		}
		else if (prevActiveState && (!isSelectedOnlyOne)) {
			for (Nt::TextEdit& textEdit : m_TextEdits)
				textEdit.DisableWindow();

			prevActiveState = false;
		}

		if (isSelectedOnlyOne)
			_UpdateUI(m_SelectorPtr->GetObjectPtr(0).lock().get(), false);
	}

	void SetTheme(const Style& style) override {
		SetBackgroundColor(style["Property.BackgroundColor"]);
		SetBorderColor(style["Property.BorderColor"]);

		m_ContentLayout.SetBackgroundColor(style["Property.BackgroundColor"]);

		for (uInt i = 0; i < m_Texts.size(); ++i) {
			const Bool isHeader = (i % 4 == 0);
			if (isHeader) {
				m_Texts[i].SetColor(style["Property.HeaderTexts.Color"]);
				m_Texts[i].SetWeight(style["Property.HeaderTexts.Weight"]);
			}
			else {
				m_Texts[i].SetColor(style["Property.Texts.Color"]);
				m_Texts[i].SetWeight(style["Property.Texts.Weight"]);
			}
		}

		for (uInt i = 0; i < m_TextEdits.size(); ++i) {
			m_TextEdits[i].SetBackgroundColor(style["Property.TextEdits.BackgroundColor"]);
			m_TextEdits[i].SetTextColor(style["Property.TextEdits.Text.Color"]);
			m_TextEdits[i].SetTextWeight(style["Property.TextEdits.Text.Weight"]);
		}
	}
	void SetLanguage(const Language& language) override {
		SetName(language["Window.Property.Transform"]);

		const std::string texts[Texts::TEXT_COUNT] = {
			language["Window.Property.Transform.Position"],
			language["Window.Property.Transform.Position.X"],
			language["Window.Property.Transform.Position.Y"],
			language["Window.Property.Transform.Position.Z"],
			language["Window.Property.Transform.Size"],
			language["Window.Property.Transform.Size.Width"],
			language["Window.Property.Transform.Size.Height"],
			language["Window.Property.Transform.Size.Length"],
			language["Window.Property.Transform.Angle"],
			language["Window.Property.Transform.Angle.Roll"],
			language["Window.Property.Transform.Angle.Pitch"],
			language["Window.Property.Transform.Angle.Yaw"]
		};

		for (uInt i = 0; i < Texts::TEXT_COUNT; ++i)
			m_Texts[i].SetText(texts[i]);
	}

	void SetEventBus(const std::weak_ptr<Nt::EventBus>& pBus) override {
		m_pEventBus = pBus;
		if (m_pEventBus.expired())
			return;

		auto sharedBus = m_pEventBus.lock();
		sharedBus->Subscribe<UpdateObjectTransformEvent>([this] (const UpdateObjectTransformEvent& e) {
			if (e.pEmmiter == this)
				return;

			const uInt textEditID = static_cast<uInt>(e.Type) * 3;
			for (uInt i = 0; i < 3; ++i)
				m_TextEdits[textEditID + i].SetText(e.Value[i]);
			});
	}

private:
	Nt::GridLayout m_ContentLayout;
	std::weak_ptr<Nt::EventBus> m_pEventBus;
	inline static PropertyRegistrar<PropertyTransform> m_Registrar { "Transform" };

private:
	void _AddSelection(Object* pObject) override {
		_UpdateUI(pObject, true);
		PropertyComponent::_AddSelection(pObject);
	}

	void _TextEditsNotification_Update(const uInt& id, const HWND& handle) override {
		assert(!m_pEventBus.expired());
		if (handle == nullptr || m_SelectorPtr->GetObjectCount() != 1)
			return;

		auto sharedBus = m_pEventBus.lock();

		try {
			const auto object = m_SelectorPtr->GetObjectPtr(0).lock();

			const Bool isPosition = (id / 3 == 0);
			const Bool isSize = (id / 3 == 1);
			const Bool isAngle = (id / 3 == 2);

			const uInt scalarID = (id % 3);

			const Float value = m_TextEdits[id].GetText();

			if (isPosition) {
				Nt::Float3D position = object->GetPosition();
				if (std::abs(position[scalarID] - value) < FLT_EPSILON)
					return;

				position[scalarID] = value;
				object->SetPosition(position);

				sharedBus->Emmit<UpdateObjectTransformEvent>({
					position, TransforType::POSITION, this });
			}
			else if (isSize) {
				Nt::Float3D size = object->GetSize();
				if (std::abs(size[scalarID] - value) < FLT_EPSILON)
					return;

				size[scalarID] = value;
				object->SetSize(size);

				sharedBus->Emmit<UpdateObjectTransformEvent>({
					size, TransforType::SIZE, this });
			}
			else if (isAngle) {
				Nt::Float3D angle = object->GetAngle();
				if (std::abs(angle[scalarID] - value * RADf) < FLT_EPSILON)
					return;

				angle[scalarID] = value * RADf;
				object->SetAngle(angle);

				sharedBus->Emmit<UpdateObjectTransformEvent>({
					angle, TransforType::ROTATION, this });
			}

			object->StaticUpdate();
		}
		catch (const Nt::Error& error) {
			Nt::MessageWindow(error.what(), "Warning").ShowWarning();
		}
		catch (...)
		{
		}
	}

	void _UpdateUI(Object* pObject, Bool fForce) {
		if (!pObject->IsDirty() && !fForce)
			return;

		for (uInt i = 0; i < 3; ++i)
			m_TextEdits[i].SetText(pObject->GetPosition()[i]);
		for (uInt i = 3; i < 6; ++i)
			m_TextEdits[i].SetText(pObject->GetSize()[i - 3]);
		for (uInt i = 6; i < 9; ++i)
			m_TextEdits[i].SetText(pObject->GetAngle()[i - 6] / RADf);
	}
};