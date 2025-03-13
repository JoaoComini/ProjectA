#pragma once

namespace Engine
{

	class InputEvent
	{
	public:
		virtual ~InputEvent() = default;

		virtual std::type_index GetType() const = 0;
	};

	enum class KeyCode : uint32_t
	{
		Unknown,

		Space,

		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		Escape,
		Enter,
		Tab,
		Backspace,
		Delete,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		LeftShift,
		LeftControl,
	};
	
	enum class KeyAction
	{
		Unknown,
		Release,
		Press,
		Repeat,
	};

	class KeyInputEvent : public InputEvent
	{
	public:
		KeyInputEvent(KeyCode code, KeyAction action);

		KeyCode GetCode() const;
		KeyAction GetAction() const;

		std::type_index GetType() const override;

	private:
		KeyCode code;
		KeyAction action;
	};

	enum class MouseButton
	{
		Left,
		Right,
		Middle,
		Back,
		Forward,
		Unknown
	};

	enum class MouseButtonAction
	{
		Release,
		Press,
		Unknown
	};

	class MouseButtonInputEvent : public InputEvent
	{
	public:
		MouseButtonInputEvent(MouseButton button, MouseButtonAction action);

		MouseButton GetButton() const;
		MouseButtonAction GetAction() const;

		std::type_index GetType() const override;
	private:
		MouseButton button;
		MouseButtonAction action;
	};

	class MouseMoveInputEvent : public InputEvent
	{
	public:
		MouseMoveInputEvent(float x, float y);

		float GetX() const;
		float GetY() const;

		std::type_index GetType() const override;
	private:
		float x;
		float y;
	};

	class InputEventHandler
	{
	public:
		InputEventHandler(const InputEvent& event) : event(event) {}

		template<typename EventType>
		bool Handle(std::function<void(const EventType&)> func)
		{
			if (event.GetType() != typeid(EventType))
			{
				return false;
			}

			func(static_cast<const EventType&>(event));
			return true;
		}

	private:
		const InputEvent& event;
	};
}