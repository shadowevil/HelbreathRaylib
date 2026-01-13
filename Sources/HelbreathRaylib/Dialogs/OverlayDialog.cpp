#include "OverlayDialog.h"
#include "../SceneManager.h"
#include "../global_constants.h"

void OverlayDialog::RenderOverlay()
{
	// Draw semi-transparent black overlay over the entire screen
	raylib::DrawRectangle(0, 0, constant::BASE_WIDTH, constant::BASE_HEIGHT,
		raylib::Color{ 0, 0, 0, OVERLAY_ALPHA });
}

void OverlayDialog::CenterDialog(float width, float height)
{
	_dialog_x = (static_cast<float>(constant::BASE_WIDTH) - width) / 2.0f;
	_dialog_y = (static_cast<float>(constant::BASE_HEIGHT) - height) / 2.0f;
}

void OverlayDialog::CloseWithResult(int result)
{
	// Store callback locally before popping overlay (which destroys this dialog)
	auto callback = std::move(_result_callback);

	// Pop the overlay first (this destroys the dialog)
	scene_manager.pop_overlay();

	// Now invoke the callback (after dialog is destroyed)
	if (callback) {
		callback(result);
	}
}

void OverlayDialog::Close()
{
	scene_manager.pop_overlay();
}
