// FontManager.h
#pragma once
#include "raylib_include.h"
#include <unordered_map>
#include <string>
#include <cstdint>

enum class HorizontalAlign : uint8_t {
    Left = 1 << 0,
    Center = 1 << 1,
    Right = 1 << 2
};

enum class VerticalAlign : uint8_t {
    Top = 1 << 3,
    Middle = 1 << 4,
    Bottom = 1 << 5
};

enum class TextAlign : uint8_t {
    None = 0
};

enum class FontStyle : uint8_t {
    Regular = 0,
    Bold = 1,
    Italic = 2
};

inline TextAlign operator|(HorizontalAlign a, VerticalAlign b) {
    return static_cast<TextAlign>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline TextAlign operator|(VerticalAlign a, HorizontalAlign b) {
    return static_cast<TextAlign>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline TextAlign operator|(TextAlign a, HorizontalAlign b) {
    return static_cast<TextAlign>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline TextAlign operator|(TextAlign a, VerticalAlign b) {
    return static_cast<TextAlign>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool HasFlag(TextAlign value, HorizontalAlign flag) {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

inline bool HasFlag(TextAlign value, VerticalAlign flag) {
    return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

struct FontKey {
    uint8_t font_index;
    int font_size;
    FontStyle style;

    bool operator==(const FontKey& other) const {
        return font_index == other.font_index && font_size == other.font_size && style == other.style;
    }
};

struct FontKeyHash {
    size_t operator()(const FontKey& k) const {
        // Use a hash combination that works on both 32-bit and 64-bit platforms
        size_t hash = 0;
        hash ^= std::hash<uint32_t>{}(k.font_index) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}(k.font_size) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(k.style)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct FontPaths {
    std::string regular;
    std::string bold;
    std::string italic;
};

class FontSystem {
private:
    std::unordered_map<FontKey, Font, FontKeyHash> _loaded_fonts;
    std::unordered_map<uint8_t, FontPaths> _font_paths;

    static FontSystem& _instance() {
        static FontSystem Manager;
        return Manager;
    }

    Font& _get_font_internal(uint8_t index, int font_size, FontStyle style) {
        FontKey Key = { index, font_size, style };

        auto It = _loaded_fonts.find(Key);
        if (It != _loaded_fonts.end()) {
            return It->second;
        }

        auto PathIt = _font_paths.find(index);
        if (PathIt == _font_paths.end()) {
            static Font DefaultFont = GetFontDefault();
            return DefaultFont;
        }

        std::string Path;
        switch (style) {
        case FontStyle::Bold:
            Path = !PathIt->second.bold.empty() ? PathIt->second.bold : PathIt->second.regular;
            break;
        case FontStyle::Italic:
            Path = !PathIt->second.italic.empty() ? PathIt->second.italic : PathIt->second.regular;
            break;
        default:
            Path = PathIt->second.regular;
            break;
        }

        if (Path.empty()) {
            static Font DefaultFont = GetFontDefault();
            return DefaultFont;
        }

        Font Font_ = LoadFontEx(Path.c_str(), font_size * 8, nullptr, 0);
        SetTextureFilter(Font_.texture, TEXTURE_FILTER_POINT);
        _loaded_fonts[Key] = Font_;

        return _loaded_fonts[Key];
    }

    FontSystem() = default;
    ~FontSystem() {
        for (auto& Pair : _loaded_fonts) {
            UnloadFont(Pair.second);
        }
    }

public:
    FontSystem(const FontSystem&) = delete;
    FontSystem& operator=(const FontSystem&) = delete;

    static void register_font(uint8_t index, const char* regular_path, const char* bold_path = nullptr, const char* italic_path = nullptr) {
        if (!regular_path || !FileExists(regular_path)) {
            throw std::runtime_error("Regular font path does not exist: " + std::string(regular_path ? regular_path : "null"));
        }

        if (bold_path && !FileExists(bold_path)) {
            throw std::runtime_error("Bold font path does not exist: " + std::string(bold_path));
        }

        if (italic_path && !FileExists(italic_path)) {
            throw std::runtime_error("Italic font path does not exist: " + std::string(italic_path));
        }

        FontPaths Paths;
        Paths.regular = regular_path;
        Paths.bold = bold_path ? bold_path : "";
        Paths.italic = italic_path ? italic_path : "";
        _instance()._font_paths[index] = Paths;
    }

    static Font& get_font(uint8_t index, int font_size, FontStyle style = FontStyle::Regular) {
        return _instance()._get_font_internal(index, font_size, style);
    }

    static void unload_all() {
        auto& Inst = _instance();
        for (auto& Pair : Inst._loaded_fonts) {
            UnloadFont(Pair.second);
        }
        Inst._loaded_fonts.clear();
    }

    static Vector2 get_aligned_position(Font& font, const char* text, raylib::Rectangle rec, float font_size, float spacing, TextAlign align);
    static void draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, Color color, FontStyle style = FontStyle::Regular);
    static void draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, float spacing, Color color, FontStyle style = FontStyle::Regular);
    static void draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
    static void draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
    static void draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, float spacing, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
    static void draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, float spacing, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
};
