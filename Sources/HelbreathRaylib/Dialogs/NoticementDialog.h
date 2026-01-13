#pragma once
#include "OverlayDialog.h"
#include <string>

// Noticement dialog - displays a message without buttons
// Typically used for server notifications or important announcements
// User must click anywhere to dismiss or wait for auto-close
class NoticementDialog : public OverlayDialog {
public:
	DIALOG_TYPE(NoticementDialog)

	// Constructor with message
	// auto_close_seconds: if > 0, dialog auto-closes after this many seconds (0 = click to dismiss)
	NoticementDialog(const std::string& message, float auto_close_seconds = 0.0f);

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::string _message;
	float _auto_close_seconds;
	float _elapsed_time;

	// Dialog dimensions (from sprite: 252x87)
	static constexpr float DIALOG_WIDTH = 252.0f;
	static constexpr float DIALOG_HEIGHT = 87.0f;

	// Text positioning (centered in dialog)
	static constexpr float TEXT_PADDING = 15.0f;
};
