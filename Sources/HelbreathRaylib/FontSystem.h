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
    uint8_t fontIndex;
    int fontSize;
    FontStyle style;

    bool operator==(const FontKey& other) const {
        return fontIndex == other.fontIndex && fontSize == other.fontSize && style == other.style;
    }
};

struct FontKeyHash {
    size_t operator()(const FontKey& k) const {
        return (static_cast<size_t>(k.fontIndex) << 40) |
            (static_cast<size_t>(k.fontSize) << 8) |
            static_cast<size_t>(k.style);
    }
};

struct FontPaths {
    std::string regular;
    std::string bold;
    std::string italic;
};

class FontSystem {
private:
    std::unordered_map<FontKey, Font, FontKeyHash> loadedFonts;
    std::unordered_map<uint8_t, FontPaths> fontPaths;

    static FontSystem& Instance() {
        static FontSystem manager;
        return manager;
    }

    Font& GetFontInternal(uint8_t index, int fontSize, FontStyle style) {
        FontKey key = { index, fontSize, style };

        auto it = loadedFonts.find(key);
        if (it != loadedFonts.end()) {
            return it->second;
        }

        auto pathIt = fontPaths.find(index);
        if (pathIt == fontPaths.end()) {
            static Font defaultFont = GetFontDefault();
            return defaultFont;
        }

        std::string path;
        switch (style) {
        case FontStyle::Bold:
            path = !pathIt->second.bold.empty() ? pathIt->second.bold : pathIt->second.regular;
            break;
        case FontStyle::Italic:
            path = !pathIt->second.italic.empty() ? pathIt->second.italic : pathIt->second.regular;
            break;
        default:
            path = pathIt->second.regular;
            break;
        }

        if (path.empty()) {
            static Font defaultFont = GetFontDefault();
            return defaultFont;
        }

        Font font = LoadFontEx(path.c_str(), fontSize * 8, nullptr, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
        loadedFonts[key] = font;

        return loadedFonts[key];
    }

    FontSystem() = default;
    ~FontSystem() {
        for (auto& pair : loadedFonts) {
            UnloadFont(pair.second);
        }
    }

public:
    FontSystem(const FontSystem&) = delete;
    FontSystem& operator=(const FontSystem&) = delete;

    static void RegisterFont(uint8_t index, const char* regularPath, const char* boldPath = nullptr, const char* italicPath = nullptr) {
        if (!regularPath || !FileExists(regularPath)) {
            throw std::runtime_error("Regular font path does not exist: " + std::string(regularPath ? regularPath : "null"));
        }

        if (boldPath && !FileExists(boldPath)) {
            throw std::runtime_error("Bold font path does not exist: " + std::string(boldPath));
        }

        if (italicPath && !FileExists(italicPath)) {
            throw std::runtime_error("Italic font path does not exist: " + std::string(italicPath));
        }

        FontPaths paths;
        paths.regular = regularPath;
        paths.bold = boldPath ? boldPath : "";
        paths.italic = italicPath ? italicPath : "";
        Instance().fontPaths[index] = paths;
    }

    static Font& GetFont(uint8_t index, int fontSize, FontStyle style = FontStyle::Regular) {
        return Instance().GetFontInternal(index, fontSize, style);
    }

    static void UnloadAll() {
        auto& inst = Instance();
        for (auto& pair : inst.loadedFonts) {
            UnloadFont(pair.second);
        }
        inst.loadedFonts.clear();
    }
};

Vector2 GetAlignedPosition(Font& font, const char* text, rlRectangle rec, float fontSize, float spacing, TextAlign align);
void DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, Color color, FontStyle style = FontStyle::Regular);
void DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float spacing, Color color, FontStyle style = FontStyle::Regular);
void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, float spacing, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, float spacing, Color color, TextAlign align, FontStyle style = FontStyle::Regular);
