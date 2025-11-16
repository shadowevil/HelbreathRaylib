#include "FontManager.h"

Vector2 core::GetAlignedPosition(Font& font, const char* text, rlRectangle rec, float fontSize, float spacing, core::TextAlign align) {
    Vector2 measure = MeasureTextEx(font, text, fontSize, spacing);
    float posX = rec.x;
    float posY = rec.y;

    if (HasFlag(align, HorizontalAlign::Center))
        posX += (rec.width - measure.x) / 2.0f;
    else if (HasFlag(align, HorizontalAlign::Right))
        posX += rec.width - measure.x;

    if (HasFlag(align, VerticalAlign::Middle))
        posY += (rec.height - measure.y) / 2.0f;
    else if (HasFlag(align, VerticalAlign::Bottom))
        posY += rec.height - measure.y;

    return { posX, posY };
}

void core::DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, Color color, core::FontStyle style /*= core::FontStyle::Regular*/) {
    Font& font = FontManager::GetFont(fontIndex, fontSize, style);
    DrawTextEx(font, text, { x, y }, static_cast<float>(fontSize), 1.0f, color);
}

void core::DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float spacing, Color color, core::FontStyle style /*= core::FontStyle::Regular*/) {
    Font& font = FontManager::GetFont(fontIndex, fontSize, style);
    DrawTextEx(font, text, { x, y }, static_cast<float>(fontSize), spacing, color);
}

void core::DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, Color color, TextAlign align, core::FontStyle style/* = core::FontStyle::Regular*/) {
    Font& font = FontManager::GetFont(fontIndex, fontSize, style);
    Vector2 pos = core::GetAlignedPosition(font, text, rec, static_cast<float>(fontSize), 0.0f, align);
    DrawTextEx(font, text, pos, static_cast<float>(fontSize), 0.0f, color);
}

void core::DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, Color color, core::TextAlign align, core::FontStyle style/* = core::FontStyle::Regular*/) {
    core::DrawTextAligned(fontIndex, fontSize, text, { x, y, width, height }, color, align, style);
}

void core::DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, float spacing, Color color, core::TextAlign align, core::FontStyle style/* = core::FontStyle::Regular*/) {
    Font& font = FontManager::GetFont(fontIndex, fontSize, style);
    Vector2 pos = core::GetAlignedPosition(font, text, rec, static_cast<float>(fontSize), spacing, align);
    DrawTextEx(font, text, pos, static_cast<float>(fontSize), spacing, color);
}

void core::DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, float spacing, Color color, core::TextAlign align, core::FontStyle style /*= core::FontStyle::Regular*/) {
    core::DrawTextAligned(fontIndex, fontSize, text, { x, y, width, height }, spacing, color, align, style);
}