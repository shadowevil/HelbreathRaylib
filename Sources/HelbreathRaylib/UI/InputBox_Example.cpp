// InputBox Usage Examples
// This file demonstrates various ways to use the InputBox control

#include "InputBox.h"
#include "../Scenes.h"

// ============================================================================
// Example 1: Basic InputBox Setup
// ============================================================================
void Example1_BasicSetup(std::vector<std::unique_ptr<UI::Control>>& controls) {
    // Create using different constructors

    // Method 1: Default constructor, set bounds later
    auto inputBox1 = std::make_unique<UI::InputBox>();
    inputBox1->SetBounds(100, 50, 200, 30);

    // Method 2: Constructor with x, y, width, height
    auto inputBox2 = std::make_unique<UI::InputBox>(100, 100, 200, 30);

    // Method 3: Using rlx::Rectangle
    rlx::Rectangle<float> bounds{100, 150, 200, 30};
    auto inputBox3 = std::make_unique<UI::InputBox>(bounds);

    // Method 4: Using raylib::Rectangle
    raylib::Rectangle raylibBounds{100, 200, 200, 30};
    auto inputBox4 = std::make_unique<UI::InputBox>(raylibBounds);

    // Add to controls list for automatic update/render
    controls.push_back(std::move(inputBox1));
    controls.push_back(std::move(inputBox2));
    controls.push_back(std::move(inputBox3));
    controls.push_back(std::move(inputBox4));
}

// ============================================================================
// Example 2: Styled InputBox with Configuration
// ============================================================================
void Example2_StyledInputBox(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto inputBox = std::make_unique<UI::InputBox>(300, 100, 250, 35);

    // Create custom configuration
    UI::InputBox::Config config;
    config.background_color = raylib::Color{20, 20, 30, 255};
    config.border_color = raylib::Color{80, 80, 100, 255};
    config.focused_border_color = raylib::Color{100, 150, 255, 255};
    config.text_color = raylib::WHITE;
    config.selection_color = raylib::Color{51, 153, 255, 128};
    config.cursor_color = raylib::SKYBLUE;
    config.font_size = 16;
    config.padding_left = 10.0f;
    config.padding_right = 10.0f;
    config.border_thickness = 2.0f;
    config.cursor_blink_rate = 0.6f;
    config.cursor_width = 2.5f;

    inputBox->SetConfig(config);
    inputBox->SetPlaceholder("Enter your name...");

    controls.push_back(std::move(inputBox));
}

// ============================================================================
// Example 3: Username Input (letters, numbers, underscore only)
// ============================================================================
void Example3_UsernameInput(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto username = std::make_unique<UI::InputBox>(100, 150, 200, 30);

    // Set placeholder
    username->SetPlaceholder("Username");

    // Restrict to alphanumeric and underscore
    username->SetAllowedCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");

    // Set max length
    username->SetMaxLength(16);

    // Add event handlers
    username->OnTextChanged = [](UI::InputBox* box) {
        // Validate username as user types
        std::string text = box->GetText();
        if (text.length() > 0 && text.length() < 3) {
            // Could show warning: "Username must be at least 3 characters"
        }
    };

    username->OnEnterPressed = [](UI::InputBox* box) {
        // Submit form or move to next field
        std::string final_username = box->GetText();
        // Process username...
    };

    controls.push_back(std::move(username));
}

// ============================================================================
// Example 4: Password Input
// ============================================================================
void Example4_PasswordInput(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto password = std::make_unique<UI::InputBox>(100, 200, 200, 30);

    // Enable password mode
    password->SetPasswordMode(true);
    password->SetPlaceholder("Password");
    password->SetMaxLength(32);

    // Custom password character (default is '*')
    UI::InputBox::Config config = password->GetConfig();
    config.password_char = '•';
    password->SetConfig(config);

    password->OnEnterPressed = [](UI::InputBox* box) {
        // Submit password for authentication
        std::string pass = box->GetText();
        // Authenticate...
        box->Clear(); // Clear password after submission
    };

    controls.push_back(std::move(password));
}

// ============================================================================
// Example 5: Numeric Input (numbers only)
// ============================================================================
void Example5_NumericInput(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto numeric = std::make_unique<UI::InputBox>(100, 250, 150, 30);

    // Only allow digits
    numeric->SetAllowedCharacters("0123456789");
    numeric->SetPlaceholder("Enter amount");
    numeric->SetMaxLength(10);

    numeric->OnTextChanged = [](UI::InputBox* box) {
        std::string text = box->GetText();
        if (!text.empty()) {
            // Convert to number and validate
            try {
                int value = std::stoi(text);
                if (value > 9999) {
                    // Could show warning: "Amount too large"
                }
            }
            catch (...) {
                // Handle conversion error
            }
        }
    };

    controls.push_back(std::move(numeric));
}

// ============================================================================
// Example 6: Email Input with Validation
// ============================================================================
void Example6_EmailInput(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto email = std::make_unique<UI::InputBox>(100, 300, 250, 30);

    email->SetPlaceholder("user@example.com");
    email->SetMaxLength(64);

    // Allow typical email characters
    email->SetAllowedCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@._-+");

    email->OnTextChanged = [](UI::InputBox* box) {
        std::string text = box->GetText();
        // Basic email validation
        bool has_at = text.find('@') != std::string::npos;
        bool has_dot = text.find('.') != std::string::npos;
        // Visual feedback could be added here
    };

    controls.push_back(std::move(email));
}

// ============================================================================
// Example 7: Multi-Field Form with Tab Navigation
// ============================================================================
class LoginFormExample {
private:
    std::vector<std::unique_ptr<UI::Control>> controls;
    UI::InputBox* username_input = nullptr;
    UI::InputBox* password_input = nullptr;

public:
    void Initialize() {
        // Username field
        auto username = std::make_unique<UI::InputBox>(200, 150, 300, 35);
        username->SetPlaceholder("Username");
        username->SetMaxLength(20);
        username_input = username.get();

        // Password field
        auto password = std::make_unique<UI::InputBox>(200, 200, 300, 35);
        password->SetPasswordMode(true);
        password->SetPlaceholder("Password");
        password->SetMaxLength(32);
        password_input = password.get();

        // Tab between fields
        username->OnKeyDown = [this](UI::InputBox* box, int key) {
            if (key == raylib::KEY_TAB) {
                password_input->Focus();
            }
        };

        password->OnKeyDown = [this](UI::InputBox* box, int key) {
            if (key == raylib::KEY_TAB &&
                (raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) ||
                 raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT))) {
                username_input->Focus();
            }
        };

        // Submit on Enter in password field
        password->OnEnterPressed = [this](UI::InputBox* box) {
            SubmitLogin();
        };

        controls.push_back(std::move(username));
        controls.push_back(std::move(password));
    }

    void SubmitLogin() {
        std::string user = username_input->GetText();
        std::string pass = password_input->GetText();

        // Validate
        if (user.length() < 3) {
            // Show error: "Username too short"
            return;
        }
        if (pass.length() < 6) {
            // Show error: "Password too short"
            return;
        }

        // Process login...
        password_input->Clear(); // Clear password
    }

    void Update() {
        for (auto& control : controls) {
            control->Update();
        }
    }

    void Render() {
        for (auto& control : controls) {
            control->Render();
        }
    }
};

// ============================================================================
// Example 8: Character Input Events
// ============================================================================
void Example8_CharacterEvents(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto input = std::make_unique<UI::InputBox>(100, 350, 200, 30);

    // Monitor character input
    input->OnCharInput = [](UI::InputBox* box, char c) {
        // Log or process each character typed
        // This fires BEFORE the character filter check
        printf("Character typed: %c\n", c);
    };

    // Monitor key presses
    input->OnKeyDown = [](UI::InputBox* box, int key) {
        if (key == raylib::KEY_ESCAPE) {
            box->Blur();
        }
        else if (key == raylib::KEY_DELETE) {
            // Could add custom delete behavior
        }
    };

    controls.push_back(std::move(input));
}

// ============================================================================
// Example 9: Scene Integration Pattern
// ============================================================================
class LoginScene : public Scene {
public:
    SCENE_TYPE(LoginScene)

    void on_initialize() override {
        // Create username input
        auto username = std::make_unique<UI::InputBox>(300, 250, 300, 40);
        username->SetPlaceholder("Username");
        username->SetMaxLength(16);

        // Style it
        UI::InputBox::Config config;
        config.font_size = 16;
        config.padding_left = 12.0f;
        config.border_thickness = 2.0f;
        username->SetConfig(config);

        // Event: Move to password on Enter
        username->OnEnterPressed = [this](UI::InputBox* box) {
            // Find password input and focus it
            for (auto& control : m_controls) {
                if (auto* inputBox = dynamic_cast<UI::InputBox*>(control.get())) {
                    if (inputBox->IsPasswordMode()) {
                        inputBox->Focus();
                        break;
                    }
                }
            }
        };

        // Create password input
        auto password = std::make_unique<UI::InputBox>(300, 310, 300, 40);
        password->SetPasswordMode(true);
        password->SetPlaceholder("Password");
        password->SetMaxLength(32);
        password->SetConfig(config);

        // Event: Submit on Enter
        password->OnEnterPressed = [this](UI::InputBox* box) {
            // Get both inputs and submit
            std::string user, pass;
            for (auto& control : m_controls) {
                if (auto* inputBox = dynamic_cast<UI::InputBox*>(control.get())) {
                    if (inputBox->IsPasswordMode()) {
                        pass = inputBox->GetText();
                    } else {
                        user = inputBox->GetText();
                    }
                }
            }

            // Process login
            if (!user.empty() && !pass.empty()) {
                // Authenticate...
                sound_player.play_effect(Sound::BUTTON_SOUND);
                scene_manager.set_scene<MainMenuScene>();
            }
        };

        m_controls.push_back(std::move(username));
        m_controls.push_back(std::move(password));

        // Auto-focus username on scene load
        if (auto* inputBox = dynamic_cast<UI::InputBox*>(m_controls[0].get())) {
            inputBox->Focus();
        }
    }

    void on_uninitialize() override {
        // Controls automatically cleaned up
    }

    void on_update() override {
        for (auto& control : m_controls) {
            control->Update();
        }
    }

    void on_render() override {
        // Draw background
        sprites[SPRID_INTERFACE]->draw(0, 0, 0);

        // Draw labels
        FontSystem::draw_text("Username:", 200, 260, 0, 14, FontStyle::Regular, raylib::WHITE);
        FontSystem::draw_text("Password:", 200, 320, 0, 14, FontStyle::Regular, raylib::WHITE);

        // Render controls
        for (auto& control : m_controls) {
            control->Render();
        }
    }

private:
    std::vector<std::unique_ptr<UI::Control>> m_controls;
};

// ============================================================================
// Example 10: Getting Active Input Box
// ============================================================================
void Example10_ActiveInputBox() {
    // From anywhere in your code, you can access the currently focused input box

    if (UI::InputBox* active = UI::InputBox::GetActiveInputBox()) {
        // There is an active input box
        std::string current_text = active->GetText();

        // You could modify it
        active->SetText("New text");

        // Or blur it
        active->Blur();

        // Check if it's a password field
        if (active->IsPasswordMode()) {
            // Handle password input differently
        }
    }
    else {
        // No input box is currently focused
    }
}

// ============================================================================
// Example 11: Scroll Management
// ============================================================================
void Example11_ScrollManagement(std::vector<std::unique_ptr<UI::Control>>& controls) {
    auto input = std::make_unique<UI::InputBox>(100, 400, 150, 30);

    // Set initial text that's longer than the box
    input->SetText("This is a very long text that will scroll");

    // Manual scroll control
    input->ScrollToStart();  // Scroll to beginning
    input->ScrollToEnd();    // Scroll to end
    input->ScrollToCursor(); // Ensure cursor is visible

    // Get max scroll value
    float max_scroll = input->GetMaxScrollOffset();

    controls.push_back(std::move(input));
}
