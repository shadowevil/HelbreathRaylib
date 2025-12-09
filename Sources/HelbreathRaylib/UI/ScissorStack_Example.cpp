// ScissorStack Usage Examples
// Demonstrates proper nested scissor region management using OpenGL via rlgl

#include "ScissorStack.h"
#include "InputBox.h"

namespace UI {

// ============================================================================
// Example 1: Basic Push/Pop Pattern
// ============================================================================
void Example1_BasicPushPop() {
    // Render some UI element with clipping
    ScissorStack::Push(100, 100, 200, 150);

    // Draw content that will be clipped to this region
    raylib::DrawRectangle(50, 50, 300, 300, raylib::RED);  // Will be clipped

    ScissorStack::Pop();
    // Scissor is now disabled (if stack was empty before)
}

// ============================================================================
// Example 2: RAII Pattern (Recommended)
// ============================================================================
void Example2_RAIIPattern() {
    // Using RAII helper - automatic cleanup on scope exit
    {
        ScissorStack::ScopedScissor scissor(100, 100, 200, 150);

        // Draw clipped content
        raylib::DrawRectangle(50, 50, 300, 300, raylib::RED);

    } // Automatically pops when scissor goes out of scope

    // Scissor is automatically restored here
}

// ============================================================================
// Example 3: Nested Scissor Regions
// ============================================================================
void Example3_NestedScissors() {
    // Outer container (400x300 region)
    ScissorStack::ScopedScissor outer(100, 100, 400, 300);

    // Draw outer container background
    raylib::DrawRectangle(100, 100, 400, 300, raylib::LIGHTGRAY);

    {
        // Inner panel (300x200 region) - will be intersected with outer
        ScissorStack::ScopedScissor inner(150, 150, 300, 200);

        // This will only be visible in the intersection of both regions
        raylib::DrawRectangle(0, 0, 800, 600, raylib::BLUE);
    }

    // Back to outer scissor region
    raylib::DrawText("Outside inner panel", 120, 120, 20, raylib::BLACK);
}

// ============================================================================
// Example 4: Scrollable Container with Input Boxes
// ============================================================================
class ScrollablePanel {
private:
    raylib::Rectangle bounds;
    float scroll_offset = 0.0f;
    std::vector<std::unique_ptr<UI::InputBox>> input_boxes;

public:
    ScrollablePanel(float x, float y, float width, float height)
        : bounds{x, y, width, height} {

        // Create multiple input boxes in a vertical list
        for (int i = 0; i < 10; ++i) {
            auto input = std::make_unique<UI::InputBox>(
                x + 10,
                y + 10 + i * 40,
                width - 20,
                30
            );
            input->SetPlaceholder("Field " + std::to_string(i + 1));
            input_boxes.push_back(std::move(input));
        }
    }

    void Update() {
        // Handle scroll wheel
        scroll_offset -= raylib::GetMouseWheelMove() * 20.0f;
        scroll_offset = std::max(0.0f, scroll_offset);

        // Update input boxes (they handle their own scissor internally)
        for (auto& input : input_boxes) {
            input->Update();
        }
    }

    void Render() {
        // Draw panel background
        raylib::DrawRectangleRec(bounds, raylib::Color{30, 30, 40, 255});
        raylib::DrawRectangleLinesEx(bounds, 2.0f, raylib::Color{100, 100, 120, 255});

        // Apply scissor for the entire panel
        ScissorStack::ScopedScissor panel_scissor(bounds);

        // Offset content by scroll amount
        for (auto& input : input_boxes) {
            auto input_bounds = input->GetBounds();

            // Temporarily offset Y position for scrolling
            float original_y = input_bounds.y;
            input->SetPosition(input_bounds.x, original_y - scroll_offset);

            // Render the input box (it will apply its own nested scissor)
            input->Render();

            // Restore original position
            input->SetPosition(input_bounds.x, original_y);
        }

        // Panel scissor is automatically popped when ScopedScissor goes out of scope
    }
};

// ============================================================================
// Example 5: Multiple Panels Side by Side
// ============================================================================
void Example5_MultiplePanels() {
    // Left panel
    {
        ScissorStack::ScopedScissor left_panel(50, 50, 300, 500);
        raylib::DrawRectangle(50, 50, 300, 500, raylib::Color{40, 40, 50, 255});

        // Content in left panel
        raylib::DrawText("Left Panel", 60, 60, 20, raylib::WHITE);

        // Nested input box in left panel
        {
            ScissorStack::ScopedScissor input_area(60, 100, 280, 30);
            // Input box rendering would go here
        }
    }

    // Right panel - independent scissor region
    {
        ScissorStack::ScopedScissor right_panel(400, 50, 300, 500);
        raylib::DrawRectangle(400, 50, 300, 500, raylib::Color{50, 40, 40, 255});

        // Content in right panel
        raylib::DrawText("Right Panel", 410, 60, 20, raylib::WHITE);
    }

    // No scissor active here - both panels have been popped
}

// ============================================================================
// Example 6: Debugging Scissor Stack
// ============================================================================
void Example6_DebugScissorStack() {
    // Check stack depth for debugging
    size_t depth_before = ScissorStack::GetDepth();
    printf("Scissor stack depth: %zu\n", depth_before);

    {
        ScissorStack::ScopedScissor outer(100, 100, 400, 400);
        printf("After outer push: %zu\n", ScissorStack::GetDepth());

        {
            ScissorStack::ScopedScissor inner(150, 150, 200, 200);
            printf("After inner push: %zu\n", ScissorStack::GetDepth());

            // Get current scissor region
            auto current = ScissorStack::GetCurrent();
            printf("Current scissor: x=%d, y=%d, w=%d, h=%d\n",
                current.x, current.y, current.width, current.height);
        }

        printf("After inner pop: %zu\n", ScissorStack::GetDepth());
    }

    printf("After outer pop: %zu\n", ScissorStack::GetDepth());
}

// ============================================================================
// Example 7: Error Recovery
// ============================================================================
void Example7_ErrorRecovery() {
    // If something goes wrong and scissor stack gets out of sync,
    // you can clear it completely

    try {
        ScissorStack::Push(100, 100, 200, 200);
        // ... some code that might throw ...
        throw std::runtime_error("Something went wrong!");
        ScissorStack::Pop(); // This won't execute!
    }
    catch (...) {
        // Clear the entire scissor stack to recover
        ScissorStack::Clear();
        // Now we're back to a clean state
    }

    // Better approach: Always use RAII
    try {
        ScissorStack::ScopedScissor scissor(100, 100, 200, 200);
        // ... code that might throw ...
        throw std::runtime_error("Something went wrong!");
    }
    catch (...) {
        // Scissor is automatically popped by destructor!
        // No cleanup needed
    }
}

// ============================================================================
// Example 8: Tab Container with Nested Input Boxes
// ============================================================================
class TabContainer {
private:
    struct Tab {
        std::string name;
        std::vector<std::unique_ptr<UI::InputBox>> inputs;
    };

    std::vector<Tab> tabs;
    int active_tab = 0;
    raylib::Rectangle bounds;

public:
    TabContainer(float x, float y, float width, float height)
        : bounds{x, y, width, height} {

        // Create tabs
        Tab profile_tab;
        profile_tab.name = "Profile";
        profile_tab.inputs.push_back(
            std::make_unique<UI::InputBox>(x + 20, y + 70, width - 40, 30)
        );
        profile_tab.inputs[0]->SetPlaceholder("Username");

        tabs.push_back(std::move(profile_tab));
    }

    void Render() {
        // Container background
        raylib::DrawRectangleRec(bounds, raylib::Color{25, 25, 35, 255});

        // Apply scissor for the entire tab container
        ScissorStack::ScopedScissor container_scissor(bounds);

        // Draw tab headers (outside content area, but inside container)
        float tab_x = bounds.x;
        for (size_t i = 0; i < tabs.size(); ++i) {
            raylib::Color tab_color = (i == active_tab)
                ? raylib::Color{40, 40, 50, 255}
                : raylib::Color{30, 30, 40, 255};

            raylib::DrawRectangle(
                static_cast<int>(tab_x),
                static_cast<int>(bounds.y),
                100, 30, tab_color
            );

            raylib::DrawText(tabs[i].name.c_str(),
                static_cast<int>(tab_x + 10),
                static_cast<int>(bounds.y + 8),
                14, raylib::WHITE
            );

            tab_x += 110;
        }

        // Content area scissor (nested within container scissor)
        {
            raylib::Rectangle content_bounds{
                bounds.x,
                bounds.y + 40,
                bounds.width,
                bounds.height - 40
            };

            ScissorStack::ScopedScissor content_scissor(content_bounds);

            // Render active tab's content
            if (active_tab < static_cast<int>(tabs.size())) {
                for (auto& input : tabs[active_tab].inputs) {
                    // Input boxes apply their own scissor internally
                    // This will be nested within the content scissor
                    input->Render();
                }
            }
        }
        // Content scissor popped here

    } // Container scissor popped here

    void Update() {
        if (active_tab < static_cast<int>(tabs.size())) {
            for (auto& input : tabs[active_tab].inputs) {
                input->Update();
            }
        }
    }
};

// ============================================================================
// Example 9: Integration Pattern - Scene with Multiple Clipping Regions
// ============================================================================
void Example9_SceneIntegration() {
    // Typical scene rendering with multiple clipped regions

    // Background (no clipping)
    raylib::DrawRectangle(0, 0, 800, 600, raylib::DARKGRAY);

    // Left sidebar with scrollable content
    {
        ScissorStack::ScopedScissor sidebar(0, 0, 200, 600);
        raylib::DrawRectangle(0, 0, 200, 600, raylib::Color{30, 30, 40, 255});

        // Scrollable content would go here
        for (int i = 0; i < 20; ++i) {
            raylib::DrawText(
                ("Item " + std::to_string(i)).c_str(),
                10, 10 + i * 30, 16, raylib::WHITE
            );
        }
    }

    // Main content area with input form
    {
        ScissorStack::ScopedScissor main_area(210, 10, 580, 580);
        raylib::DrawRectangle(210, 10, 580, 580, raylib::Color{25, 25, 35, 255});

        // Input boxes here will have their own nested scissor regions
        // Example: username and password fields
    }

    // Bottom status bar (no clipping)
    raylib::DrawRectangle(0, 560, 800, 40, raylib::Color{20, 20, 30, 255});
    raylib::DrawText("Status: Ready", 10, 570, 16, raylib::WHITE);
}

// ============================================================================
// Performance Note
// ============================================================================
/*
 * The ScissorStack uses OpenGL's scissor test via rlgl, which is very efficient.
 * Each push/pop operation:
 * - Calculates intersection of scissor rectangles (CPU)
 * - Calls rlScissor() to update OpenGL state (GPU)
 *
 * Performance characteristics:
 * - Intersection calculation: O(1)
 * - Stack operations: O(1)
 * - OpenGL state change: Very fast (hardware-accelerated)
 *
 * Best practices:
 * 1. Always use ScopedScissor for automatic cleanup
 * 2. Minimize scissor stack depth (typically 2-3 levels is fine)
 * 3. Don't push/pop inside tight loops - pre-calculate regions
 * 4. Use GetDepth() to debug unexpected scissor states
 */

} // namespace UI
