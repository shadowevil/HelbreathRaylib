#include "FontSystem.h"

Vector2 GetAlignedPosition(Font& font, const char* text, rlRectangle rec, float fontSize, float spacing, TextAlign align) {
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

void DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, Color color, FontStyle style /*= FontStyle::Regular*/) {
    Font& font = FontSystem::GetFont(fontIndex, fontSize, style);
    DrawTextEx(font, text, { x, y }, static_cast<float>(fontSize), 1.0f, color);
}

void DrawText(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float spacing, Color color, FontStyle style /*= FontStyle::Regular*/) {
    Font& font = FontSystem::GetFont(fontIndex, fontSize, style);
    DrawTextEx(font, text, { x, y }, static_cast<float>(fontSize), spacing, color);
}

void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    Font& font = FontSystem::GetFont(fontIndex, fontSize, style);
    Vector2 pos = GetAlignedPosition(font, text, rec, static_cast<float>(fontSize), 0.0f, align);
    DrawTextEx(font, text, pos, static_cast<float>(fontSize), 0.0f, color);
}

void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    DrawTextAligned(fontIndex, fontSize, text, { x, y, width, height }, color, align, style);
}

void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, rlRectangle rec, float spacing, Color color, TextAlign align, FontStyle style/* = FontStyle::Regular*/) {
    Font& font = FontSystem::GetFont(fontIndex, fontSize, style);
    Vector2 pos = GetAlignedPosition(font, text, rec, static_cast<float>(fontSize), spacing, align);
    DrawTextEx(font, text, pos, static_cast<float>(fontSize), spacing, color);
}

void DrawTextAligned(uint8_t fontIndex, int fontSize, const char* text, float x, float y, float width, float height, float spacing, Color color, TextAlign align, FontStyle style /*= FontStyle::Regular*/) {
    DrawTextAligned(fontIndex, fontSize, text, { x, y, width, height }, spacing, color, align, style);
}