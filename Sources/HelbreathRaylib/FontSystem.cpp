#include "FontSystem.h"

raylib::Vector2 FontSystem::get_aligned_position(raylib::Font& font, const char* text, raylib::Rectangle rec, float font_size, float spacing, TextAlign align) {
    raylib::Vector2 Measure = raylib::MeasureTextEx(font, text, font_size, spacing);
    float PosX = rec.x;
    float PosY = rec.y;

    if (HasFlag(align, HorizontalAlign::Center))
        PosX += (rec.width - Measure.x) / 2.0f;
    else if (HasFlag(align, HorizontalAlign::Right))
        PosX += rec.width - Measure.x;

    if (HasFlag(align, VerticalAlign::Middle))
        PosY += (rec.height - Measure.y) / 2.0f;
    else if (HasFlag(align, VerticalAlign::Bottom))
        PosY += rec.height - Measure.y;

    return { PosX, PosY };
}

void FontSystem::draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, raylib::Color color, FontStyle style, const TextEffects* effects) {
    // Extract only Bold/Italic for font loading
    FontStyle font_variant = static_cast<FontStyle>(static_cast<uint8_t>(style) & (static_cast<uint8_t>(FontStyle::Bold) | static_cast<uint8_t>(FontStyle::Italic)));
    raylib::Font& font = FontSystem::get_font(font_index, font_size, font_variant);

    // Draw shadow if requested
    if (HasFontStyle(style, FontStyle::Shadow)) {
        raylib::DrawTextEx(font, text, { x - 1, y + 1 }, (float)font_size, 0.0f, raylib::BLACK);
    }

    // Draw main text
    raylib::DrawTextEx(font, text, { x, y }, (float)font_size, 0.0f, color);
}

void FontSystem::draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, float spacing, raylib::Color color, FontStyle style, const TextEffects* effects) {
    // Extract only Bold/Italic for font loading
    FontStyle font_variant = static_cast<FontStyle>(static_cast<uint8_t>(style) & (static_cast<uint8_t>(FontStyle::Bold) | static_cast<uint8_t>(FontStyle::Italic)));
    raylib::Font& font = FontSystem::get_font(font_index, font_size, font_variant);

    // Draw shadow if requested
    if (HasFontStyle(style, FontStyle::Shadow)) {
        raylib::DrawTextEx(font, text, { x - 1, y + 1 }, (float)font_size, 0.0f, raylib::BLACK);
    }

    // Draw main text
    raylib::DrawTextEx(font, text, { x, y }, (float)font_size, spacing, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, raylib::Color color, TextAlign align, FontStyle style, const TextEffects* effects) {
    // Extract only Bold/Italic for font loading
    FontStyle font_variant = static_cast<FontStyle>(static_cast<uint8_t>(style) & (static_cast<uint8_t>(FontStyle::Bold) | static_cast<uint8_t>(FontStyle::Italic)));
    raylib::Font& font = FontSystem::get_font(font_index, font_size, font_variant);
    raylib::Vector2 pos = get_aligned_position(font, text, rec, static_cast<float>(font_size), 0.0f, align);

    // Draw shadow if requested
    if (HasFontStyle(style, FontStyle::Shadow)) {
        raylib::DrawTextEx(font, text, { pos.x - 1, pos.y + 1 }, (float)font_size, 0.0f, raylib::BLACK);
    }

    // Draw main text
    raylib::DrawTextEx(font, text, pos, (float)font_size, 0.0f, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, raylib::Color color, TextAlign align, FontStyle style, const TextEffects* effects) {
    draw_text_aligned(font_index, font_size, text, { x, y, width, height }, color, align, style, effects);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, float spacing, raylib::Color color, TextAlign align, FontStyle style, const TextEffects* effects) {
    // Extract only Bold/Italic for font loading
    FontStyle font_variant = static_cast<FontStyle>(static_cast<uint8_t>(style) & (static_cast<uint8_t>(FontStyle::Bold) | static_cast<uint8_t>(FontStyle::Italic)));
    raylib::Font& font = FontSystem::get_font(font_index, font_size, font_variant);
    raylib::Vector2 pos = get_aligned_position(font, text, rec, static_cast<float>(font_size), spacing, align);

    // Draw shadow if requested
    if (HasFontStyle(style, FontStyle::Shadow)) {
        raylib::DrawTextEx(font, text, { pos.x - 1, pos.y + 1 }, (float)font_size, spacing, raylib::BLACK);
    }

    // Draw main text
    raylib::DrawTextEx(font, text, pos, (float)font_size, spacing, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, float spacing, raylib::Color color, TextAlign align, FontStyle style, const TextEffects* effects) {
    draw_text_aligned(font_index, font_size, text, { x, y, width, height }, spacing, color, align, style, effects);
}
