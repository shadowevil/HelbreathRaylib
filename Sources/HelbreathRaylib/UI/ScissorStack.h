#pragma once
#include "raylib_include.h"
#include "../global_constants.h"
#include <vector>
#include <algorithm>

namespace UI {

	// Scissor stack manager for nested scissor regions
	// Since raylib only supports one scissor mode at a time, we need to manage
	// a stack and calculate intersections manually using OpenGL directly
	class ScissorStack {
	public:
		struct ScissorRect {
			int x, y, width, height;

			bool IsValid() const {
				return width > 0 && height > 0;
			}

			// Calculate intersection of two scissor rectangles
			ScissorRect Intersect(const ScissorRect& other) const {
				int x1 = std::max(x, other.x);
				int y1 = std::max(y, other.y);
				int x2 = std::min(x + width, other.x + other.width);
				int y2 = std::min(y + height, other.y + other.height);

				return ScissorRect{
					x1,
					y1,
					std::max(0, x2 - x1),
					std::max(0, y2 - y1)
				};
			}
		};

		// Push a new scissor region onto the stack
		static void Push(int x, int y, int width, int height) {
			ScissorRect rect{ x, y, width, height };

			// If there's already a scissor region, intersect with it
			if (!_stack.empty()) {
				rect = rect.Intersect(_stack.back());
			}

			_stack.push_back(rect);
			ApplyCurrentScissor();
		}

		static void Push(const raylib::Rectangle& bounds) {
			Push(
				static_cast<int>(bounds.x),
				static_cast<int>(bounds.y),
				static_cast<int>(bounds.width),
				static_cast<int>(bounds.height)
			);
		}

		// Pop the current scissor region from the stack
		static void Pop() {
			if (_stack.empty()) {
				// Error: Attempting to pop from empty scissor stack
				return;
			}

			_stack.pop_back();

			// Restore previous scissor region or disable if stack is empty
			// ApplyCurrentScissor handles the batch flush and state changes
			ApplyCurrentScissor();
		}

		// Get the current scissor rectangle (top of stack)
		static ScissorRect GetCurrent() {
			if (_stack.empty()) {
				// Return screen-sized rect if no scissor active
				return ScissorRect{
					0, 0,
					raylib::GetScreenWidth(),
					raylib::GetScreenHeight()
				};
			}
			return _stack.back();
		}

		// Check if scissor stack is empty
		static bool IsEmpty() {
			return _stack.empty();
		}

		// Get stack depth (for debugging)
		static size_t GetDepth() {
			return _stack.size();
		}

		// Clear the entire scissor stack (for error recovery)
		static void Clear() {
			_stack.clear();
			raylib::rlDisableScissorTest();
		}

		// RAII helper for automatic push/pop
		class ScopedScissor {
		public:
			ScopedScissor(int x, int y, int width, int height) {
				ScissorStack::Push(x, y, width, height);
			}

			ScopedScissor(const raylib::Rectangle& bounds)
				: ScopedScissor(
					static_cast<int>(bounds.x),
					static_cast<int>(bounds.y),
					static_cast<int>(bounds.width),
					static_cast<int>(bounds.height)
				) {}

			~ScopedScissor() {
				ScissorStack::Pop();
			}

			// Disable copy and move
			ScopedScissor(const ScopedScissor&) = delete;
			ScopedScissor& operator=(const ScopedScissor&) = delete;
			ScopedScissor(ScopedScissor&&) = delete;
			ScopedScissor& operator=(ScopedScissor&&) = delete;
		};

	private:
		static std::vector<ScissorRect> _stack;

		// Apply the current scissor region
		static void ApplyCurrentScissor() {
			// CRITICAL: Flush the render batch before changing scissor state
			// Raylib batches drawing operations, and we must flush before GPU state changes
			raylib::rlDrawRenderBatchActive();

			if (_stack.empty()) {
				// No scissor region - disable scissor test
				if (_scissor_active) {
					raylib::EndScissorMode();
					_scissor_active = false;
				}
				return;
			}

			const ScissorRect& rect = _stack.back();

			if (!rect.IsValid()) {
				// Invalid rect - disable scissor test
				if (_scissor_active) {
					raylib::EndScissorMode();
					_scissor_active = false;
				}
				return;
			}

			// When rendering to a texture with Camera2D zoom, BeginScissorMode doesn't
			// properly account for the zoom factor. We need to manually transform coordinates.
			// Note: The game uses BeginMode2D with zoom = UPSCALE_FACTOR (8)
			const float zoom = static_cast<float>(constant::UPSCALE_FACTOR);

			// Transform scissor coordinates by zoom factor
			int scaled_x = static_cast<int>(rect.x * zoom);
			int scaled_y = static_cast<int>(rect.y * zoom);
			int scaled_w = static_cast<int>(rect.width * zoom);
			int scaled_h = static_cast<int>(rect.height * zoom);

			// End previous scissor mode if active
			if (_scissor_active) {
				raylib::EndScissorMode();
			}

			// Apply scissor with scaled coordinates
			raylib::BeginScissorMode(scaled_x, scaled_y, scaled_w, scaled_h);
			_scissor_active = true;
		}

		static bool _scissor_active;
	};

	// Static member initialization (put this in a .cpp file)
	inline std::vector<ScissorStack::ScissorRect> ScissorStack::_stack;
	inline bool ScissorStack::_scissor_active = false;

} // namespace UI
