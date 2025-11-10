#pragma once
#include "raylib_include.h"

namespace core {

	// Event types enumeration
	enum class EventType : uint8_t {
		None = 0,
		WindowClose, WindowResize, WindowMinimize, WindowMaximize, WindowFocus, WindowLostFocus,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Custom
	};

	// Event category flags for filtering
	enum EventCategory : uint8_t {
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

	// Base Event class
	class Event {
	public:
		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual uint8_t GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const {
			return GetCategoryFlags() & category;
		}

		bool handled = false;
	};

	// Event dispatcher
	class EventDispatcher {
	public:
		EventDispatcher(Event& event)
			: m_event(event) {}

		template<typename T, typename F>
		bool Dispatch(const F& func) {
			if (m_event.GetEventType() == T::GetStaticType()) {
				m_event.handled |= func(static_cast<T&>(m_event));
				return true;
			}
			return false;
		}

	private:
		Event& m_event;
	};

	// Macro to implement event type methods
	#define EVENT_CLASS_TYPE(type) \
		static EventType GetStaticType() { return EventType::type; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		virtual const char* GetName() const override { return #type; }

	#define EVENT_CLASS_CATEGORY(category) \
		virtual uint8_t GetCategoryFlags() const override { return category; }

	// Window Events
	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(int32_t width, int32_t height)
			: m_width(width), m_height(height) {}

		inline int32_t GetWidth() const { return m_width; }
		inline int32_t GetHeight() const { return m_height; }

		std::string ToString() const override {
			return std::string("WindowResizeEvent: ") + std::to_string(m_width) + ", " + std::to_string(m_height);
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		int32_t m_width, m_height;
	};

	class WindowMinimizeEvent : public Event {
	public:
		WindowMinimizeEvent() = default;

		EVENT_CLASS_TYPE(WindowMinimize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowMaximizeEvent : public Event {
	public:
		WindowMaximizeEvent() = default;

		EVENT_CLASS_TYPE(WindowMaximize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowFocusEvent : public Event {
	public:
		WindowFocusEvent() = default;

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowLostFocusEvent : public Event {
	public:
		WindowLostFocusEvent() = default;

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	// Key Events
	class KeyEvent : public Event {
	public:
		inline int32_t GetKeyCode() const { return m_keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int32_t keycode)
			: m_keyCode(keycode) {}

		int32_t m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int32_t keycode, bool repeat = false)
			: KeyEvent(keycode), m_repeat(repeat) {}

		inline bool IsRepeat() const { return m_repeat; }

		std::string ToString() const override {
			return std::string("KeyPressedEvent: ") + std::to_string(m_keyCode) + " (repeat = " + (m_repeat ? "true" : "false") + ")";
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		bool m_repeat;
	};

	class KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int32_t keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override {
			return std::string("KeyReleasedEvent: ") + std::to_string(m_keyCode);
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(int32_t keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override {
			return std::string("KeyTypedEvent: ") + std::to_string(m_keyCode);
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};

	// Mouse Events
	class MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y) {}

		inline float GetX() const { return m_mouseX; }
		inline float GetY() const { return m_mouseY; }

		std::string ToString() const override {
			return std::string("MouseMovedEvent: ") + std::to_string(m_mouseX) + ", " + std::to_string(m_mouseY);
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouseX, m_mouseY;
	};

	class MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_xOffset(xOffset), m_yOffset(yOffset) {}

		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }

		std::string ToString() const override {
			return std::string("MouseScrolledEvent: ") + std::to_string(m_xOffset) + ", " + std::to_string(m_yOffset);
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event {
	public:
		inline int32_t GetMouseButton() const { return m_button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(int32_t button)
			: m_button(button) {}

		int32_t m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(int32_t button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override {
			return std::string("MouseButtonPressedEvent: ") + std::to_string(m_button);
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(int32_t button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override {
			return std::string("MouseButtonReleasedEvent: ") + std::to_string(m_button);
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	// Custom Event for user-defined events
	class CustomEvent : public Event {
	public:
		CustomEvent(const std::string& name, void* data = nullptr)
			: m_customName(name), m_data(data) {}

		inline const std::string& GetCustomName() const { return m_customName; }
		inline void* GetData() const { return m_data; }

		std::string ToString() const override {
			return std::string("CustomEvent: ") + m_customName;
		}

		EVENT_CLASS_TYPE(Custom)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		std::string m_customName;
		void* m_data;
	};

	// Event callback function type
	using EventCallbackFn = std::function<void(Event&)>;

} // namespace core
