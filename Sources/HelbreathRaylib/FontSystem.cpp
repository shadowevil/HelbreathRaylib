#include "FontSystem.h"

Vector2 FontSystem::get_aligned_position(Font& font, const char* text, raylib::Rectangle rec, float font_size, float spacing, TextAlign align) {
    Vector2 Measure = MeasureTextEx(font, text, font_size, spacing);
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

void FontSystem::draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, Color color, FontStyle style /*= FontStyle::Regular*/) {
    Font& Font_ = FontSystem::get_font(font_index, font_size, style);
    DrawTextEx(Font_, text, { x, y }, static_cast<float>(font_size), 1.0f, color);
}

void FontSystem::draw_text(uint8_t font_index, int font_size, const char* text, float x, float y, float spacing, Color color, FontStyle style /*= FontStyle::Regular*/) {
    Font& Font_ = FontSystem::get_font(font_index, font_size, style);
    DrawTextEx(Font_, text, { x, y }, static_cast<float>(font_size), spacing, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    Font& Font_ = FontSystem::get_font(font_index, font_size, style);
    Vector2 Pos = get_aligned_position(Font_, text, rec, static_cast<float>(font_size), 0.0f, align);
    DrawTextEx(Font_, text, Pos, static_cast<float>(font_size), 0.0f, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    draw_text_aligned(font_index, font_size, text, { x, y, width, height }, color, align, style);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, raylib::Rectangle rec, float spacing, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    Font& Font_ = FontSystem::get_font(font_index, font_size, style);
    Vector2 Pos = get_aligned_position(Font_, text, rec, static_cast<float>(font_size), spacing, align);
    DrawTextEx(Font_, text, Pos, static_cast<float>(font_size), spacing, color);
}

void FontSystem::draw_text_aligned(uint8_t font_index, int font_size, const char* text, float x, float y, float width, float height, float spacing, Color color, TextAlign align, FontStyle style /*= FontStyle::Regular*/) {
    draw_text_aligned(font_index, font_size, text, { x, y, width, height }, spacing, color, align, style);
}