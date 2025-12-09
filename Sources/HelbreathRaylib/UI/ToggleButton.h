#pragma once
#include "Button.h"

namespace UI {

	// Toggle button control (checkbox/radio button style)
	class ToggleButton : public Button {
	public:
		ToggleButton(CSpriteCollection& sprites);
		virtual ~ToggleButton() = default;

		// Toggle state
		void SetToggled(bool toggled);
		bool IsToggled() const { return _is_toggled; }

		// Toggle on click
		void Toggle();

		// Event callback for toggle state changes
		std::function<void(ToggleButton*, bool)> OnToggleChanged;

		// Override Update to handle toggle logic
		void Update() override;

	protected:
		bool _is_toggled = false;

		// Override to show hover state when toggled
		int32_t GetCurrentFrame() const override;
		raylib::Color GetCurrentTextColor() const override;
	};

} // namespace UI
