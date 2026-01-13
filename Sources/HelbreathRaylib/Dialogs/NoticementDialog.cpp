#include "NoticementDialog.h"
#include "../SpriteIDs.h"
#include "../FontIDs.h"
#include "../FontSystem.h"
#include "../global_constants.h"

NoticementDialog::NoticementDialog(const std::string& message, float auto_close_seconds)
	: _message(message), _auto_close_seconds(auto_close_seconds), _elapsed_time(0.0f)
{
}

void NoticementDialog::on_initialize()
{
	// Center the dialog on screen
	CenterDialog(DIALOG_WIDTH, DIALOG_HEIGHT);
}

void NoticementDialog::on_uninitialize()
{
	// Don't clear m_controls here - they will be cleaned up when the dialog
	// is destroyed. This is consistent with other dialogs even though this
	// dialog doesn't use buttons.
}

void NoticementDialog::on_update()
{
	// Check for auto-close
	if (_auto_close_seconds > 0.0f) {
		_elapsed_time += raylib::GetFrameTime();
		if (_elapsed_time >= _auto_close_seconds) {
			Close();
			return;
		}
	}

	// Check for click to dismiss
	if (raylib::IsMouseButtonPressed(raylib::MOUSE_BUTTON_LEFT)) {
		Close();
	}
}

void NoticementDialog::on_render()
{
	// Render semi-transparent overlay
	RenderOverlay();

	// Get dialog position
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Draw the dialog background sprite
	sprites[SPRID_DIALOG_MISC]->draw(
		static_cast<int>(dialog_x),
		static_cast<int>(dialog_y),
		SPR_DIALOG_MISC_INFO::NOTICEMENT
	);

	// Calculate text area
	float text_x = dialog_x + TEXT_PADDING;
	float text_y = dialog_y + TEXT_PADDING;
	float text_width = DIALOG_WIDTH - (TEXT_PADDING * 2);
	float text_height = DIALOG_HEIGHT - (TEXT_PADDING * 2);

	// Draw message text (centered, Fancy font, size 16)
	if (!_message.empty()) {
		FontSystem::draw_text_aligned(
			FontFamily::Fancy, 16,
			_message.c_str(),
			text_x, text_y,
			text_width, text_height,
			raylib::WHITE,
			HorizontalAlign::Center | VerticalAlign::Middle,
			FontStyle::Shadow
		);
	}
}
