#pragma once
#include "../Scene.h"
#include "../UI/Control.h"
#include "../UI/Button.h"
#include "../UI/Label.h"
#include <vector>
#include <memory>
#include <functional>

// Macro to define dialog type - similar to SCENE_TYPE but for dialogs inheriting from OverlayDialog
#define DIALOG_TYPE(ClassName) \
    static constexpr SceneTypeId scene_type_id = #ClassName; \
    SceneTypeId get_type_id() const override { return scene_type_id; }

// Base class for all overlay dialogs
// Provides semi-transparent background overlay and centered dialog positioning
class OverlayDialog : public Scene {
public:
	OverlayDialog() : Scene() {}
	virtual ~OverlayDialog() = default;

	// Result callback type - called when dialog closes
	using ResultCallback = std::function<void(int result)>;

	// Set callback to be invoked when dialog closes
	void SetResultCallback(ResultCallback callback) { _result_callback = std::move(callback); }

protected:
	// Render the semi-transparent background overlay
	void RenderOverlay();

	// Calculate centered position for a dialog of given size
	void CenterDialog(float width, float height);

	// Get the dialog position (after centering)
	float GetDialogX() const { return _dialog_x; }
	float GetDialogY() const { return _dialog_y; }

	// Close the dialog and invoke callback with result
	void CloseWithResult(int result);

	// Close the dialog without callback
	void Close();

	// UI controls for the dialog
	std::vector<std::unique_ptr<UI::Control>> m_controls;

	// Overlay color (semi-transparent black)
	static constexpr unsigned char OVERLAY_ALPHA = 150;

private:
	float _dialog_x = 0.0f;
	float _dialog_y = 0.0f;
	ResultCallback _result_callback;
};
