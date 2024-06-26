#include "api.h"

#include <stdio.h>

#include <raylib.h>

#include "font.h"
#include "icon.h"
#include "util.h"

static int argCount;
static char** args;
static Font defaultFont;

void setArgs(int argc, char** argv)
{
    argCount = argc;
    args = argv;
}

int uiTextWidth(mu_Font font, const char* text, int len)
{
    if (len == -1)
        len = TextLength(text);

    Vector2 textSize = { 0 };

    int tempByteCounter = 0;
    int byteCounter = 0;

    float textWidth = 0.0f;
    float tempTextWidth = 0.0f;

    float textHeight = (float)defaultFont.baseSize;

    int letter = 0;
    int index = 0;

    for (int i = 0; i < len;) {
        byteCounter++;

        int next = 0;
        letter = GetCodepointNext(&text[i], &next);
        index = GetGlyphIndex(defaultFont, letter);

        i += next;

        if (defaultFont.glyphs[index].advanceX != 0)
            textWidth += defaultFont.glyphs[index].advanceX;
        else
            textWidth += (defaultFont.recs[index].width + defaultFont.glyphs[index].offsetX);

        if (tempByteCounter < byteCounter)
            tempByteCounter = byteCounter;
    }

    if (tempTextWidth < textWidth)
        tempTextWidth = textWidth;

    textSize.x = tempTextWidth + (float)((tempByteCounter - 1));
    textSize.y = textHeight;

    return textSize.x;
}

int uiTextHeight(mu_Font font)
{
    return defaultFont.baseSize;
}

// Audio

void audioInit(WrenVM* vm)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        VM_ABORT(vm, "Failed to initialize audio.");
        return;
    }

    vmData* data = (vmData*)wrenGetUserData(vm);
    data->audioInit = true;
}

void audioGetVolume(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMasterVolume());
}

void audioSetVolume(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "volume");
    float volume = (float)wrenGetSlotDouble(vm, 1);

    if (volume < 0.0f || volume > 1.0f) {
        VM_ABORT(vm, "Volume must be between 0.0 and 1.0.");
        return;
    }

    SetMasterVolume(volume);
}

void soundAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Sound));
}

void soundFinalize(void* data)
{
    Sound* sound = (Sound*)data;
    UnloadSound(*sound);
}

void soundNew(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);

    if (!IsAudioDeviceReady()) {
        VM_ABORT(vm, "Cannot load sound before audio initialization.");
        return;
    }

    *sound = LoadSound(path);
    if (!IsSoundReady(*sound)) {
        VM_ABORT(vm, "Failed to load sound.");
        return;
    }
}

void soundPlay(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    PlaySound(*sound);
}

void soundStop(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    StopSound(*sound);
}

void soundPause(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    PauseSound(*sound);
}

void soundResume(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ResumeSound(*sound);
}

void soundGetPlaying(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotBool(vm, 0, IsSoundPlaying(*sound));
}

void soundSetVolume(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "volume");
    float volume = (float)wrenGetSlotDouble(vm, 1);

    if (volume < 0.0f || volume > 1.0f) {
        VM_ABORT(vm, "Volume must be between 0.0 and 1.0.");
        return;
    }

    SetSoundVolume(*sound, volume);
}

void soundSetPitch(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "pitch");
    float pitch = (float)wrenGetSlotDouble(vm, 1);

    if (pitch < 0.0f) {
        VM_ABORT(vm, "Pitch must be greater than 0.0.");
        return;
    }

    SetSoundPitch(*sound, pitch);
}

void soundSetPan(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "pan");
    float pan = (float)wrenGetSlotDouble(vm, 1);

    if (pan < 0.0f || pan > 1.0f) {
        VM_ABORT(vm, "Pan must be between 0.0 and 1.0.");
        return;
    }

    SetSoundPan(*sound, pan);
}

void soundAliasFinalize(void* data)
{
    Sound* sound = (Sound*)data;
    UnloadSoundAlias(*sound);
}

void soundAliasNew(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "sound");
    Sound* other = (Sound*)wrenGetSlotForeign(vm, 1);
    *sound = LoadSoundAlias(*other);
}

// Graphics

void graphicsBegin(WrenVM* vm)
{
    BeginDrawing();
}

void graphicsEnd(WrenVM* vm)
{
    EndDrawing();
}

void graphicsBeginBlend(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "mode");
    const char* mode = wrenGetSlotString(vm, 1);

    if (TextIsEqual(mode, "alpha"))
        BeginBlendMode(BLEND_ALPHA);
    else if (TextIsEqual(mode, "additive"))
        BeginBlendMode(BLEND_ADDITIVE);
    else if (TextIsEqual(mode, "multiplied"))
        BeginBlendMode(BLEND_MULTIPLIED);
    else if (TextIsEqual(mode, "addColors"))
        BeginBlendMode(BLEND_ADD_COLORS);
    else if (TextIsEqual(mode, "subtractColors"))
        BeginBlendMode(BLEND_SUBTRACT_COLORS);
    else if (TextIsEqual(mode, "alphaPremultiply"))
        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    else
        VM_ABORT(vm, "Invalid blend mode.");
}

void graphicsEndBlend(WrenVM* vm)
{
    EndBlendMode();
}

void graphicsBeginScissor(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "height");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    BeginScissorMode(x, y, width, height);
}

void graphicsEndScissor(WrenVM* vm)
{
    EndScissorMode();
}

void graphicsScreenshot(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    TakeScreenshot(path);
}

void graphicsMeasure(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "size");
    const char* text = wrenGetSlotString(vm, 1);
    int size = (int)wrenGetSlotDouble(vm, 2);

    if (size < defaultFont.baseSize)
        size = defaultFont.baseSize;

    wrenSetSlotDouble(vm, 0, MeasureTextEx(defaultFont, text, (float)size, size / defaultFont.baseSize).x);
}

void graphicsNoise(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "frequency");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "depth");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float frequency = (float)wrenGetSlotDouble(vm, 3);
    int depth = (int)wrenGetSlotDouble(vm, 4);
    wrenSetSlotDouble(vm, 0, perlin2d(x, y, frequency, depth));
}

void graphicsClear(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "color");
    Color* color = (Color*)wrenGetSlotForeign(vm, 1);
    ClearBackground(*color);
}

void graphicsPrint(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "size");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "color");
    const char* text = wrenGetSlotString(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    int size = (int)wrenGetSlotDouble(vm, 4);
    Color* color = (Color*)wrenGetSlotForeign(vm, 5);

    if (size < defaultFont.baseSize)
        size = defaultFont.baseSize;

    DrawTextEx(defaultFont, text, (Vector2) { (float)x, (float)y }, size, size / defaultFont.baseSize, *color);
}

void graphicsPixel(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    Color* color = (Color*)wrenGetSlotForeign(vm, 3);
    DrawPixel(x, y, *color);
}

void graphicsLine(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x1");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y1");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "x2");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "y2");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "thick");
    ASSERT_SLOT_TYPE(vm, 6, FOREIGN, "color");
    int x1 = (int)wrenGetSlotDouble(vm, 1);
    int y1 = (int)wrenGetSlotDouble(vm, 2);
    int x2 = (int)wrenGetSlotDouble(vm, 3);
    int y2 = (int)wrenGetSlotDouble(vm, 4);
    float thick = (float)wrenGetSlotDouble(vm, 5);
    Color* color = (Color*)wrenGetSlotForeign(vm, 6);
    DrawLineEx((Vector2) { (float)x1, (float)y1 }, (Vector2) { (float)x2, (float)y2 }, thick, *color);
}

void graphicsLineBezier(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x1");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y1");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "x2");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "y2");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "thick");
    ASSERT_SLOT_TYPE(vm, 6, FOREIGN, "color");
    int x1 = (int)wrenGetSlotDouble(vm, 1);
    int y1 = (int)wrenGetSlotDouble(vm, 2);
    int x2 = (int)wrenGetSlotDouble(vm, 3);
    int y2 = (int)wrenGetSlotDouble(vm, 4);
    float thick = (float)wrenGetSlotDouble(vm, 5);
    Color* color = (Color*)wrenGetSlotForeign(vm, 6);
    DrawLineBezier((Vector2) { (float)x1, (float)y1 }, (Vector2) { (float)x2, (float)y2 }, thick, *color);
}

void graphicsCircle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "radius");
    ASSERT_SLOT_TYPE(vm, 4, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float radius = (float)wrenGetSlotDouble(vm, 3);
    Color* color = (Color*)wrenGetSlotForeign(vm, 4);
    DrawCircle(x, y, radius, *color);
}

void graphicsCircleLines(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "radius");
    ASSERT_SLOT_TYPE(vm, 4, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float radius = (float)wrenGetSlotDouble(vm, 3);
    Color* color = (Color*)wrenGetSlotForeign(vm, 4);
    DrawCircleLines(x, y, radius, *color);
}

void graphicsEllipse(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "rx");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "ry");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float rx = (float)wrenGetSlotDouble(vm, 3);
    float ry = (float)wrenGetSlotDouble(vm, 4);
    Color* color = (Color*)wrenGetSlotForeign(vm, 5);
    DrawEllipse(x, y, rx, ry, *color);
}

void graphicsEllipseLines(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "rx");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "ry");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float rx = (float)wrenGetSlotDouble(vm, 3);
    float ry = (float)wrenGetSlotDouble(vm, 4);
    Color* color = (Color*)wrenGetSlotForeign(vm, 5);
    DrawEllipseLines(x, y, rx, ry, *color);
}

void graphicsRectangle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 7, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 8, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    float r = (float)wrenGetSlotDouble(vm, 5);
    int ox = (int)wrenGetSlotDouble(vm, 6);
    int oy = (int)wrenGetSlotDouble(vm, 7);
    Color* color = (Color*)wrenGetSlotForeign(vm, 8);
    DrawRectanglePro((Rectangle) { (float)x, (float)y, (float)width, (float)height }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void graphicsRectangleLines(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "thick");
    ASSERT_SLOT_TYPE(vm, 6, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    float thick = (float)wrenGetSlotDouble(vm, 5);
    Color* color = (Color*)wrenGetSlotForeign(vm, 6);
    DrawRectangleLinesEx((Rectangle) { (float)x, (float)y, (float)width, (float)height }, thick, *color);
}

void graphicsTriangle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x1");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y1");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "x2");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "y2");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "x3");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "y3");
    ASSERT_SLOT_TYPE(vm, 7, FOREIGN, "color");
    int x1 = (int)wrenGetSlotDouble(vm, 1);
    int y1 = (int)wrenGetSlotDouble(vm, 2);
    int x2 = (int)wrenGetSlotDouble(vm, 3);
    int y2 = (int)wrenGetSlotDouble(vm, 4);
    int x3 = (int)wrenGetSlotDouble(vm, 5);
    int y3 = (int)wrenGetSlotDouble(vm, 6);
    Color* color = (Color*)wrenGetSlotForeign(vm, 7);
    DrawTriangle((Vector2) { (float)x1, (float)y1 }, (Vector2) { (float)x2, (float)y2 }, (Vector2) { (float)x3, (float)y3 }, *color);
}

void graphicsTriangleLines(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x1");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y1");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "x2");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "y2");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "x3");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "y3");
    ASSERT_SLOT_TYPE(vm, 7, FOREIGN, "color");
    int x1 = (int)wrenGetSlotDouble(vm, 1);
    int y1 = (int)wrenGetSlotDouble(vm, 2);
    int x2 = (int)wrenGetSlotDouble(vm, 3);
    int y2 = (int)wrenGetSlotDouble(vm, 4);
    int x3 = (int)wrenGetSlotDouble(vm, 5);
    int y3 = (int)wrenGetSlotDouble(vm, 6);
    Color* color = (Color*)wrenGetSlotForeign(vm, 7);
    DrawTriangleLines((Vector2) { (float)x1, (float)y1 }, (Vector2) { (float)x2, (float)y2 }, (Vector2) { (float)x3, (float)y3 }, *color);
}

void graphicsPolygon(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "sides");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "radius");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 6, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int sides = (int)wrenGetSlotDouble(vm, 3);
    float radius = (float)wrenGetSlotDouble(vm, 4);
    float r = (float)wrenGetSlotDouble(vm, 5);
    Color* color = (Color*)wrenGetSlotForeign(vm, 6);
    DrawPoly((Vector2) { (float)x, (float)y }, sides, radius, r, *color);
}

void graphicsPolygonLines(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "sides");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "radius");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "thick");
    ASSERT_SLOT_TYPE(vm, 7, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int sides = (int)wrenGetSlotDouble(vm, 3);
    float radius = (float)wrenGetSlotDouble(vm, 4);
    float r = (float)wrenGetSlotDouble(vm, 5);
    float thick = (float)wrenGetSlotDouble(vm, 6);
    Color* color = (Color*)wrenGetSlotForeign(vm, 7);
    DrawPolyLinesEx((Vector2) { (float)x, (float)y }, sides, radius, r, thick, *color);
}

void graphicsSetNoiseSeed(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "seed");
    int seed = (int)wrenGetSlotDouble(vm, 1);
    setSeed(seed);
}

void graphicsSetLineSpacing(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "spacing");
    int spacing = (int)wrenGetSlotDouble(vm, 1);
    SetTextLineSpacing(spacing);
}

struct ButtonMap {
    MouseButton rl;
    int mu;
};

struct KeyMap {
    KeyboardKey rl;
    int mu;
};

static struct ButtonMap buttonMap[] = {
    { MOUSE_BUTTON_LEFT, MU_MOUSE_LEFT },
    { MOUSE_BUTTON_RIGHT, MU_MOUSE_RIGHT },
    { MOUSE_BUTTON_MIDDLE, MU_MOUSE_MIDDLE },
    { -1, -1 }
};

static struct KeyMap keyMap[] = {
    { KEY_LEFT_SHIFT, MU_KEY_SHIFT },
    { KEY_RIGHT_SHIFT, MU_KEY_SHIFT },
    { KEY_LEFT_CONTROL, MU_KEY_CTRL },
    { KEY_RIGHT_CONTROL, MU_KEY_CTRL },
    { KEY_LEFT_ALT, MU_KEY_ALT },
    { KEY_RIGHT_ALT, MU_KEY_ALT },
    { KEY_ENTER, MU_KEY_RETURN },
    { KEY_KP_ENTER, MU_KEY_RETURN },
    { KEY_BACKSPACE, MU_KEY_BACKSPACE },
    { -1, -1 },
};

void uiUpdate(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);

    int x = GetMouseX();
    int y = GetMouseY();
    mu_input_mousemove(data->uiCtx, x, y);

    Vector2 scroll = GetMouseWheelMoveV();
    mu_input_scroll(data->uiCtx, scroll.x * -30, scroll.y * -30);

    for (int i = 0;; i++) {
        struct ButtonMap button = buttonMap[i];
        if (button.rl == -1)
            break;

        if (IsMouseButtonPressed(button.rl))
            mu_input_mousedown(data->uiCtx, x, y, button.mu);
        else if (IsMouseButtonReleased(button.rl))
            mu_input_mouseup(data->uiCtx, x, y, button.mu);
    }

    for (int i = 0;; i++) {
        struct KeyMap key = keyMap[i];
        if (key.rl == -1)
            break;

        if (IsKeyPressed(key.rl))
            mu_input_keydown(data->uiCtx, key.mu);
        else if (IsKeyReleased(key.rl))
            mu_input_keyup(data->uiCtx, key.mu);
    }

    char buffer[512];
    for (int i = 0; i < 512; i++) {
        char c = GetCharPressed();
        buffer[i] = c;
        if (c == '\0')
            break;
    }
    mu_input_text(data->uiCtx, buffer);
}

void uiDraw(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    BeginScissorMode(0, 0, GetScreenWidth(), GetScreenHeight());

    mu_Command* cmd = NULL;
    while (mu_next_command(data->uiCtx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_TEXT:
            DrawTextEx(defaultFont, cmd->text.str, (Vector2) { (float)cmd->text.pos.x, (float)cmd->text.pos.y }, defaultFont.baseSize, 1, (Color) { cmd->text.color.r, cmd->text.color.g, cmd->text.color.b, cmd->text.color.a });
            break;
        case MU_COMMAND_RECT:
            DrawRectangle(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, (Color) { cmd->rect.color.r, cmd->rect.color.g, cmd->rect.color.b, cmd->rect.color.a });
            break;
        case MU_COMMAND_ICON:
            Color color = (Color) { cmd->icon.color.r, cmd->icon.color.g, cmd->icon.color.b, cmd->icon.color.a };

            switch (cmd->icon.id) {
            case MU_ICON_CLOSE:
                DrawLine(cmd->icon.rect.x + 8, cmd->icon.rect.y + 8, cmd->icon.rect.x + cmd->icon.rect.w - 8, cmd->icon.rect.y + cmd->icon.rect.h - 8, color);
                DrawLine(cmd->icon.rect.x + cmd->icon.rect.w - 8, cmd->icon.rect.y + 8, cmd->icon.rect.x + 8, cmd->icon.rect.y + cmd->icon.rect.h - 8, color);
                break;
            case MU_ICON_CHECK:
                DrawLine(cmd->icon.rect.x + 5, cmd->icon.rect.y + 5, cmd->icon.rect.x + cmd->icon.rect.w - 5, cmd->icon.rect.y + cmd->icon.rect.h - 5, color);
                DrawLine(cmd->icon.rect.x + cmd->icon.rect.w - 5, cmd->icon.rect.y + 5, cmd->icon.rect.x + 5, cmd->icon.rect.y + cmd->icon.rect.h - 5, color);
                break;
            case MU_ICON_COLLAPSED:
                DrawLine(cmd->icon.rect.x + 6, cmd->icon.rect.y + 6, cmd->icon.rect.x + cmd->icon.rect.w / 2, cmd->icon.rect.y + cmd->icon.rect.h / 2, color);
                DrawLine(cmd->icon.rect.x + 6, cmd->icon.rect.y + cmd->icon.rect.h - 6, cmd->icon.rect.x + cmd->icon.rect.w / 2, cmd->icon.rect.y + cmd->icon.rect.h / 2, color);
                break;
            case MU_ICON_EXPANDED:
                DrawLine(cmd->icon.rect.x + 6, cmd->icon.rect.y + 6, cmd->icon.rect.x + cmd->icon.rect.w / 2, cmd->icon.rect.y + cmd->icon.rect.h / 2, color);
                DrawLine(cmd->icon.rect.x + cmd->icon.rect.w - 6, cmd->icon.rect.y + 6, cmd->icon.rect.x + cmd->icon.rect.w / 2, cmd->icon.rect.y + cmd->icon.rect.h / 2, color);
                break;
            }

            break;
        case MU_COMMAND_CLIP:
            EndScissorMode();
            BeginScissorMode(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h);
            break;
        }
    }

    EndScissorMode();
}

void uiBegin(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_begin(data->uiCtx);
}

void uiEnd(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_end(data->uiCtx);
}

void uiBeginWindow(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "title");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "height");
    const char* title = wrenGetSlotString(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    int width = (int)wrenGetSlotDouble(vm, 4);
    int height = (int)wrenGetSlotDouble(vm, 5);
    wrenSetSlotBool(vm, 0, mu_begin_window(data->uiCtx, title, mu_rect(x, y, width, height)));
}

void uiEndWindow(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_end_window(data->uiCtx);
}

void uiLabel(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    const char* text = wrenGetSlotString(vm, 1);
    mu_label(data->uiCtx, text);
}

void uiHeader(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, STRING, "option");
    const char* text = wrenGetSlotString(vm, 1);
    const char* option = wrenGetSlotString(vm, 2);

    int opt;
    if (TextIsEqual(option, "expanded")) {
        opt = MU_OPT_EXPANDED;
    } else {
        opt = 0;
    }

    wrenSetSlotBool(vm, 0, mu_header_ex(data->uiCtx, text, opt));
}

void uiButton(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    const char* text = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, mu_button(data->uiCtx, text));
}

void uiRow(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "items");
    ASSERT_SLOT_TYPE(vm, 2, LIST, "widths");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "height");
    int items = (int)wrenGetSlotDouble(vm, 1);

    int count = wrenGetListCount(vm, 2);
    int* widths = malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        wrenGetListElement(vm, 2, i, 1);
        widths[i] = (int)wrenGetSlotDouble(vm, 1);
    }

    int height = (int)wrenGetSlotDouble(vm, 3);
    mu_layout_row(data->uiCtx, items, widths, height);
    free(widths);
}

void uiTextbox(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "buffer");
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 1);
    wrenSetSlotBool(vm, 0, mu_textbox(data->uiCtx, buffer->data, buffer->size) & MU_RES_SUBMIT);
}

void uiGetWindowInfo(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_Container* win = mu_get_current_container(data->uiCtx);

    wrenEnsureSlots(vm, 3);
    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "x");
    wrenSetSlotDouble(vm, 2, win->rect.x);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "y");
    wrenSetSlotDouble(vm, 2, win->rect.y);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "width");
    wrenSetSlotDouble(vm, 2, win->rect.w);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "height");
    wrenSetSlotDouble(vm, 2, win->rect.h);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "scrollX");
    wrenSetSlotDouble(vm, 2, win->scroll.x);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "scrollY");
    wrenSetSlotDouble(vm, 2, win->scroll.y);
    wrenSetMapValue(vm, 0, 1, 2);
}

void uiSetWindowSize(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    mu_Container* win = mu_get_current_container(data->uiCtx);
    win->rect.w = width;
    win->rect.h = height;
}

void uiOpenPopup(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "name");
    const char* name = wrenGetSlotString(vm, 1);
    mu_open_popup(data->uiCtx, name);
}

void uiBeginPopup(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "name");
    const char* name = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, mu_begin_popup(data->uiCtx, name));
}

void uiEndPopup(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_end_popup(data->uiCtx);
}

void uiBeginColumn(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_layout_begin_column(data->uiCtx);
}

void uiEndColumn(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_layout_end_column(data->uiCtx);
}

void uiSlider(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "buffer");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "min");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "max");
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);
    int min = (int)wrenGetSlotDouble(vm, 3);
    int max = (int)wrenGetSlotDouble(vm, 4);

    if (offset < 0 || offset >= buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    mu_slider(data->uiCtx, (float*)(&buffer->data[offset]), min, max);
}

void uiNext(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewList(vm, 0);

    mu_Rect r = mu_layout_next(data->uiCtx);

    wrenSetSlotDouble(vm, 1, r.x);
    wrenInsertInList(vm, 0, 0, 1);

    wrenSetSlotDouble(vm, 1, r.y);
    wrenInsertInList(vm, 0, 1, 1);

    wrenSetSlotDouble(vm, 1, r.w);
    wrenInsertInList(vm, 0, 2, 1);

    wrenSetSlotDouble(vm, 1, r.h);
    wrenInsertInList(vm, 0, 3, 1);
}

void uiDrawRect(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    Color* color = (Color*)wrenGetSlotForeign(vm, 5);
    mu_draw_rect(data->uiCtx, mu_rect(x, y, width, height), mu_color(color->r, color->g, color->b, color->a));
}

void uiDrawText(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "height");
    const char* text = wrenGetSlotString(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    int width = (int)wrenGetSlotDouble(vm, 4);
    int height = (int)wrenGetSlotDouble(vm, 5);
    mu_draw_control_text(data->uiCtx, text, mu_rect(x, y, width, height), MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
}

void uiBeginTreenode(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    const char* text = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, mu_begin_treenode(data->uiCtx, text));
}

void uiEndTreenode(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_end_treenode(data->uiCtx);
}

void uiCheckbox(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, FOREIGN, "buffer");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "offset");
    const char* text = wrenGetSlotString(vm, 1);
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 2);
    int offset = (int)wrenGetSlotDouble(vm, 3);

    if (offset < 0 || offset >= buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotBool(vm, 0, mu_checkbox(data->uiCtx, text, (int*)(&buffer->data[offset])));
}

void uiText(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_STRING) {
        const char* text = wrenGetSlotString(vm, 1);
        mu_text(data->uiCtx, text);
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN) {
        Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 1);
        mu_text(data->uiCtx, (const char*)buffer->data);
    }
}

void uiBeginPanel(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "name");
    const char* name = wrenGetSlotString(vm, 1);
    mu_begin_panel(data->uiCtx, name);
}

void uiEndPanel(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_end_panel(data->uiCtx);
}

void uiFocus(WrenVM* vm)
{
    vmData* data = (vmData*)wrenGetUserData(vm);
    mu_set_focus(data->uiCtx, data->uiCtx->last_id);
}

void colorAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Color));
}

void colorNew(WrenVM* vm)
{
    Color* color = (Color*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "g");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "b");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "a");
    int r = (int)wrenGetSlotDouble(vm, 1);
    int g = (int)wrenGetSlotDouble(vm, 2);
    int b = (int)wrenGetSlotDouble(vm, 3);
    int a = (int)wrenGetSlotDouble(vm, 4);
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}

void colorNew2(WrenVM* vm)
{
    Color* color = (Color*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "g");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "b");
    int r = (int)wrenGetSlotDouble(vm, 1);
    int g = (int)wrenGetSlotDouble(vm, 2);
    int b = (int)wrenGetSlotDouble(vm, 3);
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = 255;
}

void colorGetIndex(WrenVM* vm)
{
    unsigned char* color = (unsigned char*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "index");
    int index = (int)wrenGetSlotDouble(vm, 1);

    if (index < 0 || index > 3) {
        VM_ABORT(vm, "Invalid color index.");
        return;
    }

    wrenSetSlotDouble(vm, 0, color[index]);
}

void colorSetIndex(WrenVM* vm)
{
    unsigned char* color = (unsigned char*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "index");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "value");
    int index = (int)wrenGetSlotDouble(vm, 1);
    int value = (int)wrenGetSlotDouble(vm, 2);

    if (index < 0 || index > 3) {
        VM_ABORT(vm, "Invalid color index.");
        return;
    }

    color[index] = value;
}

void imageAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Image));
}

void imageFinalize(void* data)
{
    Image* image = (Image*)data;
    UnloadImage(*image);
}

void imageNew(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_STRING) {
        const char* path = wrenGetSlotString(vm, 1);

        *image = LoadImage(path);
        if (!IsImageReady(*image)) {
            VM_ABORT(vm, "Failed to load image.");
            return;
        }
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN) {
        Texture* texture = (Texture*)wrenGetSlotForeign(vm, 1);
        *image = LoadImageFromTexture(*texture);
    } else {
        VM_ABORT(vm, "Invalid image type.");
        return;
    }
}

void imageNew2(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 3, FOREIGN, "color");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    Color* color = (Color*)wrenGetSlotForeign(vm, 3);
    *image = GenImageColor(width, height, *color);
}

void imageNew3(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "type");
    ASSERT_SLOT_TYPE(vm, 2, STRING, "data");
    const char* type = wrenGetSlotString(vm, 1);

    int length;
    const char* data = wrenGetSlotBytes(vm, 2, &length);
    *image = LoadImageFromMemory(type, data, length);
    if (!IsImageReady(*image)) {
        VM_ABORT(vm, "Failed to load image.");
        return;
    }
}

void imageNew4(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    *image = LoadImageFromScreen();
}

void imageNew5(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "image");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "height");
    Image* other = (Image*)wrenGetSlotForeign(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    int width = (int)wrenGetSlotDouble(vm, 4);
    int height = (int)wrenGetSlotDouble(vm, 5);
    *image = ImageFromImage(*other, (Rectangle) { (float)x, (float)y, (float)width, (float)height });
}

void imageNew6(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "size");
    ASSERT_SLOT_TYPE(vm, 3, FOREIGN, "color");
    const char* text = wrenGetSlotString(vm, 1);
    int size = (int)wrenGetSlotDouble(vm, 2);
    Color* color = (Color*)wrenGetSlotForeign(vm, 3);

    if (size < defaultFont.baseSize)
        size = defaultFont.baseSize;

    *image = ImageTextEx(defaultFont, text, size, size / defaultFont.baseSize, *color);
}

void imageNew7(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "direction");
    ASSERT_SLOT_TYPE(vm, 4, FOREIGN, "startColor");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "endColor");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    int direction = (int)wrenGetSlotDouble(vm, 3);
    Color* startColor = (Color*)wrenGetSlotForeign(vm, 4);
    Color* endColor = (Color*)wrenGetSlotForeign(vm, 5);
    *image = GenImageGradientLinear(width, height, direction, *startColor, *endColor);
}

void imageNew8(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "density");
    ASSERT_SLOT_TYPE(vm, 4, FOREIGN, "innerColor");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "outerColor");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    float density = (float)wrenGetSlotDouble(vm, 3);
    Color* innerColor = (Color*)wrenGetSlotForeign(vm, 4);
    Color* outerColor = (Color*)wrenGetSlotForeign(vm, 5);
    *image = GenImageGradientRadial(width, height, density, *innerColor, *outerColor);
}

void imageNew9(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "density");
    ASSERT_SLOT_TYPE(vm, 4, FOREIGN, "innerColor");
    ASSERT_SLOT_TYPE(vm, 5, FOREIGN, "outerColor");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    float density = (float)wrenGetSlotDouble(vm, 3);
    Color* innerColor = (Color*)wrenGetSlotForeign(vm, 4);
    Color* outerColor = (Color*)wrenGetSlotForeign(vm, 5);
    *image = GenImageGradientSquare(width, height, density, *innerColor, *outerColor);
}

void imageExport(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, ExportImage(*image, path));
}

void imageExportToMemory(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "type");
    const char* type = wrenGetSlotString(vm, 1);

    int length;
    unsigned char* data = ExportImageToMemory(*image, type, &length);
    wrenSetSlotBytes(vm, 0, data, length);
}

void imageCrop(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "height");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    ImageCrop(image, (Rectangle) { (float)x, (float)y, (float)width, (float)height });
}

void imageResize(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    ImageResize(image, width, height);
}

void imageFlipVertical(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ImageFlipVertical(image);
}

void imageFlipHorizontal(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ImageFlipHorizontal(image);
}

void imageRotate(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "angle");
    int angle = (int)wrenGetSlotDouble(vm, 1);
    ImageRotate(image, angle);
}

void imageGetWidth(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, image->width);
}

void imageGetHeight(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, image->height);
}

void imageGetFormat(WrenVM* vm)
{
    Image* image = (Image*)wrenGetSlotForeign(vm, 0);

    switch (image->format) {
    case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
        wrenSetSlotString(vm, 0, "grayscale");
        break;
    case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
        wrenSetSlotString(vm, 0, "grayscaleAlpha");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
        wrenSetSlotString(vm, 0, "R5G6B5");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
        wrenSetSlotString(vm, 0, "R8G8B8");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
        wrenSetSlotString(vm, 0, "R5G5B5A1");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
        wrenSetSlotString(vm, 0, "R4G4B4A4");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
        wrenSetSlotString(vm, 0, "R8G8B8A8");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R32:
        wrenSetSlotString(vm, 0, "R32");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
        wrenSetSlotString(vm, 0, "R32G32B32");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
        wrenSetSlotString(vm, 0, "R32G32B32A32");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R16:
        wrenSetSlotString(vm, 0, "R16");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R16G16B16:
        wrenSetSlotString(vm, 0, "R16G16B16");
        break;
    case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
        wrenSetSlotString(vm, 0, "R16G16B16A16");
        break;
    default:
        wrenSetSlotString(vm, 0, "Unknown");
        break;
    }
}

void textureAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Texture));
}

void textureFinalize(void* data)
{
    Texture* texture = (Texture*)data;
    UnloadTexture(*texture);
}

void textureNew(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load texture before window initialization.");
        return;
    }

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_STRING) {
        const char* path = wrenGetSlotString(vm, 1);

        *texture = LoadTexture(path);
        if (!IsTextureReady(*texture)) {
            VM_ABORT(vm, "Failed to load texture.");
            return;
        }
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN) {
        Image* image = (Image*)wrenGetSlotForeign(vm, 1);
        *texture = LoadTextureFromImage(*image);
    } else {
        VM_ABORT(vm, "Invalid texture type.");
        return;
    }
}

void textureDraw(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "sx");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "sy");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 7, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 8, FOREIGN, "color");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    float r = (float)wrenGetSlotDouble(vm, 3);
    float sx = (float)wrenGetSlotDouble(vm, 4);
    float sy = (float)wrenGetSlotDouble(vm, 5);
    int ox = (int)wrenGetSlotDouble(vm, 6);
    int oy = (int)wrenGetSlotDouble(vm, 7);
    Color* color = (Color*)wrenGetSlotForeign(vm, 8);

    Rectangle source = { 0, 0, (float)texture->width, (float)texture->height };

    if (sx < 0)
        source.width = -source.width;

    if (sy < 0)
        source.height = -source.height;

    float absSx = sx < 0 ? -sx : sx;
    float absSy = sy < 0 ? -sy : sy;

    DrawTexturePro(*texture, source, (Rectangle) { (float)x, (float)y, (float)texture->width * absSx, (float)texture->height * absSy }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void textureDrawRec(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "srcX");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "srcY");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "srcWidth");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "srcHeight");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "dstX");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "dstY");
    ASSERT_SLOT_TYPE(vm, 7, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 8, NUM, "sx");
    ASSERT_SLOT_TYPE(vm, 9, NUM, "sy");
    ASSERT_SLOT_TYPE(vm, 10, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 11, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 12, FOREIGN, "color");
    int srcX = (int)wrenGetSlotDouble(vm, 1);
    int srcY = (int)wrenGetSlotDouble(vm, 2);
    int srcWidth = (int)wrenGetSlotDouble(vm, 3);
    int srcHeight = (int)wrenGetSlotDouble(vm, 4);
    int dstX = (int)wrenGetSlotDouble(vm, 5);
    int dstY = (int)wrenGetSlotDouble(vm, 6);
    float r = (float)wrenGetSlotDouble(vm, 7);
    float sx = (float)wrenGetSlotDouble(vm, 8);
    float sy = (float)wrenGetSlotDouble(vm, 9);
    int ox = (int)wrenGetSlotDouble(vm, 10);
    int oy = (int)wrenGetSlotDouble(vm, 11);
    Color* color = (Color*)wrenGetSlotForeign(vm, 12);

    Rectangle source = { (float)srcX, (float)srcY, (float)srcWidth, (float)srcHeight };

    if (sx < 0)
        source.width = -source.width;

    if (sy < 0)
        source.height = -source.height;

    float absSx = sx < 0 ? -sx : sx;
    float absSy = sy < 0 ? -sy : sy;

    DrawTexturePro(*texture, source, (Rectangle) { (float)dstX, (float)dstY, (float)srcWidth * absSx, (float)srcHeight * absSy }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void textureGetWidth(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, texture->width);
}

void textureGetHeight(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, texture->height);
}

void textureSetFilter(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "filter");
    const char* filter = wrenGetSlotString(vm, 1);

    if (TextIsEqual(filter, "point"))
        SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
    else if (TextIsEqual(filter, "bilinear"))
        SetTextureFilter(*texture, TEXTURE_FILTER_BILINEAR);
    else
        VM_ABORT(vm, "Invalid texture filter.");
}

void textureSetWrap(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "wrap");
    const char* wrap = wrenGetSlotString(vm, 1);

    if (TextIsEqual(wrap, "repeat"))
        SetTextureWrap(*texture, TEXTURE_WRAP_REPEAT);
    else if (TextIsEqual(wrap, "clamp"))
        SetTextureWrap(*texture, TEXTURE_WRAP_CLAMP);
    else
        VM_ABORT(vm, "Invalid texture wrap.");
}

void renderTextureAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(RenderTexture));
}

void renderTextureFinalize(void* data)
{
    RenderTexture* texture = (RenderTexture*)data;
    UnloadRenderTexture(*texture);
}

void renderTextureNew(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load texture before window initialization.");
        return;
    }

    *texture = LoadRenderTexture(width, height);
    if (!IsRenderTextureReady(*texture)) {
        VM_ABORT(vm, "Failed to load texture.");
        return;
    }
}

void renderTextureBegin(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);
    BeginTextureMode(*texture);
}

void renderTextureEnd(WrenVM* vm)
{
    EndTextureMode();
}

void renderTextureGetTexture(WrenVM* vm)
{
    wrenEnsureSlots(vm, 2);
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);

    vmData* data = (vmData*)wrenGetUserData(vm);

    wrenSetSlotHandle(vm, 1, data->textureClass);
    Texture* tex = wrenSetSlotNewForeign(vm, 0, 1, sizeof(Texture));
    *tex = texture->texture;
}

void fontAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Font));
}

void fontFinalize(void* data)
{
    Font* font = (Font*)data;
    UnloadFont(*font);
}

void fontNew(WrenVM* vm)
{
    Font* font = (Font*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "size");
    const char* path = wrenGetSlotString(vm, 1);
    int size = (int)wrenGetSlotDouble(vm, 2);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load font before window initialization.");
        return;
    }

    *font = LoadFontEx(path, size, NULL, 250); // insert codepoint count in api
    if (!IsFontReady(*font)) {
        VM_ABORT(vm, "Failed to load font.");
        return;
    }
}

void fontNew2(WrenVM* vm)
{
    Font* font = (Font*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data")
    ASSERT_SLOT_TYPE(vm, 2, NUM, "size");
    int size = (int)wrenGetSlotDouble(vm, 2);

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load font before window initialization.");
        return;
    }

    *font = LoadFontFromMemory(".ttf", data, length, size, NULL, 250);
    if (!IsFontReady(*font)) {
        VM_ABORT(vm, "Failed to load font.");
        return;
    }
}

void fontPrint(WrenVM* vm)
{
    Font* font = (Font*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 3, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 4, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 5, NUM, "scale");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "spacing");
    ASSERT_SLOT_TYPE(vm, 7, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 8, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 9, FOREIGN, "color");
    const char* text = wrenGetSlotString(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    float r = (float)wrenGetSlotDouble(vm, 4);
    float scale = (float)wrenGetSlotDouble(vm, 5);
    float spacing = (float)wrenGetSlotDouble(vm, 6);
    int ox = (int)wrenGetSlotDouble(vm, 7);
    int oy = (int)wrenGetSlotDouble(vm, 8);
    Color* color = (Color*)wrenGetSlotForeign(vm, 9);
    DrawTextPro(*font, text, (Vector2) { (float)x, (float)y }, (Vector2) { (float)ox, (float)oy }, r, (float)font->baseSize * scale, spacing, *color);
}

void fontMeasure(WrenVM* vm)
{
    Font* font = (Font*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    const char* text = wrenGetSlotString(vm, 1);
    wrenSetSlotDouble(vm, 0, MeasureTextEx(*font, text, (float)font->baseSize, 0).x);
}

void fontGetSize(WrenVM* vm)
{
    Font* font = (Font*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, font->baseSize);
}

void cameraAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Camera2D));
}

void cameraNew(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    camera->target = (Vector2) { (float)x, (float)y };
    camera->offset = (Vector2) { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;
}

void cameraBegin(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    BeginMode2D(*camera);
}

void cameraEnd(WrenVM* vm)
{
    EndMode2D();
}

void cameraScreenToWorld(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    Vector2 result = GetScreenToWorld2D((Vector2) { (float)x, (float)y }, *camera);
    wrenSetSlotNewList(vm, 0);
    wrenSetSlotDouble(vm, 1, result.x);
    wrenSetSlotDouble(vm, 2, result.y);
    wrenInsertInList(vm, 0, 0, 1);
    wrenInsertInList(vm, 0, 1, 2);
}

void cameraWorldToScreen(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    Vector2 result = GetWorldToScreen2D((Vector2) { (float)x, (float)y }, *camera);
    wrenSetSlotNewList(vm, 0);
    wrenSetSlotDouble(vm, 1, result.x);
    wrenSetSlotDouble(vm, 2, result.y);
    wrenInsertInList(vm, 0, 0, 1);
    wrenInsertInList(vm, 0, 1, 2);
}

void cameraGetX(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->target.x);
}

void cameraGetY(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->target.y);
}

void cameraGetOffsetX(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->offset.x);
}

void cameraGetOffsetY(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->offset.y);
}

void cameraGetRotation(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->rotation);
}

void cameraGetZoom(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, camera->zoom);
}

void cameraSetX(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    int x = (int)wrenGetSlotDouble(vm, 1);
    camera->target.x = (float)x;
}

void cameraSetY(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "y");
    int y = (int)wrenGetSlotDouble(vm, 1);
    camera->target.y = (float)y;
}

void cameraSetOffsetX(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "ox");
    int ox = (int)wrenGetSlotDouble(vm, 1);
    camera->offset.x = (float)ox;
}

void cameraSetOffsetY(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "oy");
    int oy = (int)wrenGetSlotDouble(vm, 1);
    camera->offset.y = (float)oy;
}

void cameraSetRotation(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "r");
    float r = (float)wrenGetSlotDouble(vm, 1);
    camera->rotation = r;
}

void cameraSetZoom(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "zoom");
    float zoom = (float)wrenGetSlotDouble(vm, 1);
    camera->zoom = zoom;
}

void shaderAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Shader));
}

void shaderFinalize(void* data)
{
    Shader* shader = (Shader*)data;
    UnloadShader(*shader);
}

void shaderNew(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "vs");
    ASSERT_SLOT_TYPE(vm, 2, STRING, "fs");
    const char* vs = wrenGetSlotString(vm, 1);
    const char* fs = wrenGetSlotString(vm, 2);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load shader before window initialization.");
        return;
    }

    *shader = LoadShader(vs, fs);
    if (!IsShaderReady(*shader)) {
        VM_ABORT(vm, "Failed to load shader.");
        return;
    }
}

void shaderNew2(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "fs");
    const char* fs = wrenGetSlotString(vm, 1);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load shader before window initialization.");
        return;
    }

    *shader = LoadShader(NULL, fs);
    if (!IsShaderReady(*shader)) {
        VM_ABORT(vm, "Failed to load shader.");
        return;
    }
}

void shaderNew3(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "vs");
    ASSERT_SLOT_TYPE(vm, 2, STRING, "fs");
    const char* vs = wrenGetSlotString(vm, 1);
    const char* fs = wrenGetSlotString(vm, 2);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load shader before window initialization.");
        return;
    }

    *shader = LoadShaderFromMemory(vs, fs);
    if (!IsShaderReady(*shader)) {
        VM_ABORT(vm, "Failed to load shader.");
        return;
    }
}

void shaderNew4(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "fs");
    const char* fs = wrenGetSlotString(vm, 1);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load shader before window initialization.");
        return;
    }

    *shader = LoadShaderFromMemory(NULL, fs);
    if (!IsShaderReady(*shader)) {
        VM_ABORT(vm, "Failed to load shader.");
        return;
    }
}

void shaderBegin(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    BeginShaderMode(*shader);
}

void shaderEnd(WrenVM* vm)
{
    EndShaderMode();
}

void shaderSet(WrenVM* vm)
{
    Shader* shader = (Shader*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "name");
    const char* name = wrenGetSlotString(vm, 1);

    if (wrenGetSlotType(vm, 2) == WREN_TYPE_NUM) {
        float value = (float)wrenGetSlotDouble(vm, 2);
        SetShaderValue(*shader, GetShaderLocation(*shader, name), &value, SHADER_UNIFORM_FLOAT);
    } else if (wrenGetSlotType(vm, 2) == WREN_TYPE_LIST) {
        wrenEnsureSlots(vm, 4);
        int count = wrenGetListCount(vm, 2);

        if (count == 2) {
            float vec[2];
            wrenGetListElement(vm, 2, 0, 3);
            vec[0] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 1, 3);
            vec[1] = (float)wrenGetSlotDouble(vm, 3);
            SetShaderValue(*shader, GetShaderLocation(*shader, name), vec, SHADER_UNIFORM_VEC2);
        } else if (count == 3) {
            float vec[3];
            wrenGetListElement(vm, 2, 0, 3);
            vec[0] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 1, 3);
            vec[1] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 2, 3);
            vec[2] = (float)wrenGetSlotDouble(vm, 3);
            SetShaderValue(*shader, GetShaderLocation(*shader, name), vec, SHADER_UNIFORM_VEC3);
        } else if (count == 4) {
            float vec[4];
            wrenGetListElement(vm, 2, 0, 3);
            vec[0] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 1, 3);
            vec[1] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 2, 3);
            vec[2] = (float)wrenGetSlotDouble(vm, 3);
            wrenGetListElement(vm, 2, 3, 3);
            vec[3] = (float)wrenGetSlotDouble(vm, 3);
            SetShaderValue(*shader, GetShaderLocation(*shader, name), vec, SHADER_UNIFORM_VEC4);
        } else {
            VM_ABORT(vm, "Invalid argument count.");
            return;
        }
    } else if (wrenGetSlotType(vm, 2) == WREN_TYPE_FOREIGN) {
        Texture* texture = (Texture*)wrenGetSlotForeign(vm, 2);
        SetShaderValueTexture(*shader, GetShaderLocation(*shader, name), *texture);
    } else {
        VM_ABORT(vm, "Invalid argument type.");
        return;
    }
}

// Input

void keyboardPressed(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");
    const char* key = wrenGetSlotString(vm, 1);

    vmData* data = (vmData*)wrenGetUserData(vm);

    KeyboardKey k = *map_get(&data->keys, key);
    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key.");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyPressed(k));
}

void keyboardPressedRepeat(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");
    const char* key = wrenGetSlotString(vm, 1);

    vmData* data = (vmData*)wrenGetUserData(vm);

    KeyboardKey k = *map_get(&data->keys, key);
    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key.");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyPressedRepeat(k));
}

void keyboardDown(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");
    const char* key = wrenGetSlotString(vm, 1);

    vmData* data = (vmData*)wrenGetUserData(vm);

    KeyboardKey k = *map_get(&data->keys, key);
    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key.");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyDown(k));
}

void keyboardReleased(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");
    const char* key = wrenGetSlotString(vm, 1);

    vmData* data = (vmData*)wrenGetUserData(vm);

    KeyboardKey k = *map_get(&data->keys, key);
    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key.");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyReleased(k));
}

void keyboardGetKeyPressed(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetKeyPressed());
}

void keyboardGetCharPressed(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetCharPressed());
}

void mousePressed(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsMouseButtonPressed(button));
}

void mouseDown(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsMouseButtonDown(button));
}

void mouseReleased(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsMouseButtonReleased(button));
}

void mouseSetPosition(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    SetMousePosition(x, y);
}

void mouseSetOffset(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    SetMouseOffset(x, y);
}

void mouseSetScale(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    float x = (float)wrenGetSlotDouble(vm, 1);
    float y = (float)wrenGetSlotDouble(vm, 2);
    SetMouseScale(x, y);
}

void mouseGetX(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseX());
}

void mouseGetY(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseY());
}

void mouseGetDeltaX(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseDelta().x);
}

void mouseGetDeltaY(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseDelta().y);
}

void mouseGetWheel(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseWheelMove());
}

void mouseSetCursor(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "cursor");
    const char* cursor = wrenGetSlotString(vm, 1);

    if (TextIsEqual(cursor, "default"))
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    else if (TextIsEqual(cursor, "arrow"))
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    else if (TextIsEqual(cursor, "ibeam"))
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    else if (TextIsEqual(cursor, "crosshair"))
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    else if (TextIsEqual(cursor, "pointingHand"))
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    else if (TextIsEqual(cursor, "resizeEW"))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
    else if (TextIsEqual(cursor, "resizeNS"))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
    else if (TextIsEqual(cursor, "resizeNWSE"))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
    else if (TextIsEqual(cursor, "resizeNESW"))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
    else if (TextIsEqual(cursor, "resizeAll"))
        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
    else if (TextIsEqual(cursor, "notAllowed"))
        SetMouseCursor(MOUSE_CURSOR_NOT_ALLOWED);
    else
        VM_ABORT(vm, "Invalid cursor.");
}

void mouseGetHidden(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsCursorHidden());
}

void mouseSetHidden(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, BOOL, "hidden");
    bool hidden = wrenGetSlotBool(vm, 1);

    if (hidden)
        HideCursor();
    else
        ShowCursor();
}

void mouseSetEnabled(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, BOOL, "enabled");
    bool enabled = wrenGetSlotBool(vm, 1);

    if (enabled)
        EnableCursor();
    else
        DisableCursor();
}

void mouseGetOnScreen(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsCursorOnScreen());
}

void gamepadAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Gamepad));
}

void gamepadNew(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "id");
    int id = (int)wrenGetSlotDouble(vm, 1);

    if (!IsGamepadAvailable(id)) {
        VM_ABORT(vm, "Gamepad not available.");
        return;
    }

    gamepad->id = id;
}

void gamepadAvailable(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "id");
    int id = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadAvailable(id));
}

void gamepadPressed(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadButtonPressed(gamepad->id, button));
}

void gamepadDown(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadButtonDown(gamepad->id, button));
}

void gamepadReleased(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");
    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadButtonReleased(gamepad->id, button));
}

void gamepadAxis(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "axis");
    int axis = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotDouble(vm, 0, GetGamepadAxisMovement(gamepad->id, axis));
}

void gamepadGetId(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, gamepad->id);
}

void gamepadGetName(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotString(vm, 0, GetGamepadName(gamepad->id));
}

void gamepadGetAxisCount(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, GetGamepadAxisCount(gamepad->id));
}

// System

void windowInit(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    ASSERT_SLOT_TYPE(vm, 3, STRING, "title");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    const char* title = wrenGetSlotString(vm, 3);

    InitWindow(width, height, title);
    if (!IsWindowReady()) {
        VM_ABORT(vm, "Failed to initialize window.");
        return;
    }

    Image icon = {
        .data = ICON_DATA,
        .width = ICON_WIDTH,
        .height = ICON_HEIGHT,
        .format = ICON_FORMAT,
        .mipmaps = 1
    };

    SetWindowIcon(icon);
    SetExitKey(KEY_NULL);

    Image font = {
        .data = FONT_DATA,
        .width = FONT_WIDTH,
        .height = FONT_HEIGHT,
        .format = FONT_FORMAT,
        .mipmaps = 1
    };

    defaultFont = LoadFontFromImage(font, MAGENTA, 32);

    vmData* data = (vmData*)wrenGetUserData(vm);
    data->windowInit = true;
}

void windowToggleFullscreen(WrenVM* vm)
{
    ToggleFullscreen();
}

void windowToggleBorderless(WrenVM* vm)
{
    ToggleBorderlessWindowed();
}

void windowMaximize(WrenVM* vm)
{
    MaximizeWindow();
}

void windowMinimize(WrenVM* vm)
{
    MinimizeWindow();
}

void windowRestore(WrenVM* vm)
{
    RestoreWindow();
}

void windowSetPosition(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    SetWindowPosition(x, y);
}

void windowSetMinSize(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    SetWindowMinSize(width, height);
}

void windowSetMaxSize(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    SetWindowMaxSize(width, height);
}

void windowSetSize(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "width");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "height");
    int width = (int)wrenGetSlotDouble(vm, 1);
    int height = (int)wrenGetSlotDouble(vm, 2);
    SetWindowSize(width, height);
}

void windowFocus(WrenVM* vm)
{
    SetWindowFocused();
}

void windowGetMonitorInfo(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "monitor");
    int monitor = (int)wrenGetSlotDouble(vm, 1);

    wrenEnsureSlots(vm, 3);
    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "name");
    wrenSetSlotString(vm, 2, GetMonitorName(monitor));
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "x");
    wrenSetSlotDouble(vm, 2, GetMonitorPosition(monitor).x);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "y");
    wrenSetSlotDouble(vm, 2, GetMonitorPosition(monitor).y);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "width");
    wrenSetSlotDouble(vm, 2, GetMonitorWidth(monitor));
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "height");
    wrenSetSlotDouble(vm, 2, GetMonitorHeight(monitor));
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "physicalWidth");
    wrenSetSlotDouble(vm, 2, GetMonitorPhysicalWidth(monitor));
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "physicalHeight");
    wrenSetSlotDouble(vm, 2, GetMonitorPhysicalHeight(monitor));
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "refreshRate");
    wrenSetSlotDouble(vm, 2, GetMonitorRefreshRate(monitor));
    wrenSetMapValue(vm, 0, 1, 2);
}

void windowListDropped(WrenVM* vm)
{
    FilePathList list = LoadDroppedFiles();
    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewList(vm, 0);

    for (int i = 0; i < (int)list.count; i++) {
        wrenSetSlotString(vm, 1, list.paths[i]);
        wrenInsertInList(vm, 0, i, 1);
    }

    UnloadDroppedFiles(list);
}

void windowGetClosed(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, WindowShouldClose());
}

void windowGetFullscreen(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowFullscreen());
}

void windowGetHidden(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowHidden());
}

void windowGetMinimized(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowMinimized());
}

void windowGetMaximized(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowMaximized());
}

void windowGetFocused(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowFocused());
}

void windowGetResized(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowResized());
}

void windowGetWidth(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetScreenWidth());
}

void windowGetHeight(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetScreenHeight());
}

void windowGetMonitorCount(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMonitorCount());
}

void windowGetMonitor(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetCurrentMonitor());
}

void windowGetX(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetWindowPosition().x);
}

void windowGetY(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetWindowPosition().y);
}

void windowGetDpi(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetWindowScaleDPI().x);
}

void windowGetFileDropped(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsFileDropped());
}

void windowSetIcon(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "icon");
    Image* icon = (Image*)wrenGetSlotForeign(vm, 1);
    ImageFormat(icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(*icon);
}

void windowSetTitle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "title");
    const char* title = wrenGetSlotString(vm, 1);
    SetWindowTitle(title);
}

void windowSetMonitor(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "monitor");
    int monitor = (int)wrenGetSlotDouble(vm, 1);
    SetWindowMonitor(monitor);
}

void windowSetOpacity(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "opacity");
    float opacity = (float)wrenGetSlotDouble(vm, 1);

    if (opacity < 0.0f || opacity > 1.0f) {
        VM_ABORT(vm, "Opacity must be between 0.0 and 1.0.");
        return;
    }

    SetWindowOpacity(opacity);
}

void windowSetTargetFps(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "fps");
    int fps = (int)wrenGetSlotDouble(vm, 1);
    SetTargetFPS(fps);
}

void windowGetResizable(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowState(FLAG_WINDOW_RESIZABLE));
}

void windowSetResizable(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, BOOL, "resizable");
    bool resizable = wrenGetSlotBool(vm, 1);

    if (resizable) {
        SetWindowState(FLAG_WINDOW_RESIZABLE);
    } else {
        ClearWindowState(FLAG_WINDOW_RESIZABLE);
    }
}

void windowGetVSync(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, IsWindowState(FLAG_VSYNC_HINT));
}

void windowSetVSync(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, BOOL, "vsync");
    bool vsync = wrenGetSlotBool(vm, 1);

    if (vsync) {
        SetWindowState(FLAG_VSYNC_HINT);
    } else {
        ClearWindowState(FLAG_VSYNC_HINT);
    }
}

void windowGetDt(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetFrameTime());
}

void windowGetTime(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetTime());
}

void windowGetFps(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetFPS());
}

void osReadLine(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);

    char* result = readLine();
    if (result == NULL) {
        VM_ABORT(vm, "Failed to read line.");
        return;
    }

    wrenSetSlotString(vm, 0, result);
}

void osWait(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "seconds");
    double seconds = wrenGetSlotDouble(vm, 1);
    WaitTime(seconds);
}

void osOpenUrl(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "url");
    const char* url = wrenGetSlotString(vm, 1);
    OpenURL(url);
}

void osGetArgs(WrenVM* vm)
{
    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewList(vm, 0);

    for (int i = 0; i < argCount; i++) {
        wrenSetSlotString(vm, 1, args[i]);
        wrenInsertInList(vm, 0, i, 1);
    }
}

void osGetName(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);

#ifdef _WIN32
    wrenSetSlotString(vm, 0, "windows");
#else
    wrenSetSlotString(vm, 0, "linux");
#endif
}

void osGetWrayVersion(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotString(vm, 0, WRAY_VERSION);
}

void osGetClipboard(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotString(vm, 0, GetClipboardText());
}

void osSetClipboard(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    const char* text = wrenGetSlotString(vm, 1);
    SetClipboardText(text);
}

void dataCompress(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    int compressedLength;
    unsigned char* compressed = CompressData(data, length, &compressedLength);

    wrenSetSlotBytes(vm, 0, compressed, compressedLength);
    free(compressed);
}

void dataDecompress(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    int decompressedLength;
    unsigned char* decompressed = DecompressData(data, length, &decompressedLength);

    wrenSetSlotBytes(vm, 0, decompressed, decompressedLength);
    free(decompressed);
}

void dataEncodeBase64(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    int encodedLength;
    char* encoded = EncodeDataBase64(data, length, &encodedLength);

    wrenSetSlotBytes(vm, 0, encoded, encodedLength);
    free(encoded);
}

void dataDecodeBase64(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    int decodedLength;
    unsigned char* decoded = DecodeDataBase64(data, &decodedLength);

    wrenSetSlotBytes(vm, 0, decoded, decodedLength);
    free(decoded);
}

void dataEncodeHex(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    size_t encodedLength;
    char* encoded = bytesToHex(data, length, &encodedLength);

    wrenSetSlotBytes(vm, 0, encoded, encodedLength);
    free(encoded);
}

void dataDecodeHex(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    size_t decodedLength;
    unsigned char* decoded = hexToBytes(data, length, &decodedLength);

    wrenSetSlotBytes(vm, 0, decoded, decodedLength);
    free(decoded);
}

void dataHash(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "data");

    int length;
    const char* data = wrenGetSlotBytes(vm, 1, &length);

    BYTE buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, length);
    sha256_final(&ctx, buf);

    wrenSetSlotBytes(vm, 0, buf, SHA256_BLOCK_SIZE);
}

void directoryExists(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, DirectoryExists(path));
}

void directoryList(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    FilePathList list = LoadDirectoryFiles(path);

    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewList(vm, 0);

    for (int i = 0; i < (int)list.count; i++) {
        wrenSetSlotString(vm, 1, list.paths[i]);
        wrenInsertInList(vm, 0, i, 1);
    }

    UnloadDirectoryFiles(list);
}

void fileExists(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    wrenSetSlotBool(vm, 0, FileExists(path));
}

void fileSize(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    wrenSetSlotDouble(vm, 0, GetFileLength(path));
}

void fileRead(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);

    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        VM_ABORT(vm, "Failed to open file.");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* data = (char*)malloc(size);
    if (data == NULL) {
        fclose(file);
        VM_ABORT(vm, "Failed to allocate memory.");
        return;
    }

    fread(data, 1, size, file);
    fclose(file);

    wrenSetSlotBytes(vm, 0, data, size);
    free(data);
}

void fileReadEmbedded(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);

    int length;
    unsigned char* file = LoadFileData(path, &length);
    wrenSetSlotBytes(vm, 0, file, length);
    UnloadFileData(file);
}

void fileWrite(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    ASSERT_SLOT_TYPE(vm, 2, STRING, "data");
    const char* path = wrenGetSlotString(vm, 1);

    int length;
    const char* data = wrenGetSlotBytes(vm, 2, &length);
    SaveFileData(path, (void*)data, length);
}

void bufferAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Buffer));
}

void bufferFinalize(void* data)
{
    Buffer* buffer = (Buffer*)data;
    free(buffer->data);
}

void bufferNew(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "size");
    int size = (int)wrenGetSlotDouble(vm, 1);

    buffer->data = calloc(size, sizeof(uint8_t));
    if (buffer->data == NULL) {
        VM_ABORT(vm, "Failed to allocate buffer.");
        return;
    }

    buffer->size = size;
}

void bufferNew2(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);

    if (wrenGetSlotType(vm, 1) == WREN_TYPE_LIST) {
        int size = wrenGetListCount(vm, 1);
        buffer->data = calloc(size, sizeof(uint8_t));
        if (buffer->data == NULL) {
            VM_ABORT(vm, "Failed to allocate buffer.");
            return;
        }

        wrenEnsureSlots(vm, 3);
        for (int i = 0; i < size; i++) {
            wrenGetListElement(vm, 1, i, 2);

            if (wrenGetSlotType(vm, 2) != WREN_TYPE_NUM) {
                VM_ABORT(vm, "Invalid buffer data.");
                return;
            }

            buffer->data[i] = (uint8_t)wrenGetSlotDouble(vm, 2);
        }

        buffer->size = size;
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_STRING) {
        int length;
        const char* data = wrenGetSlotBytes(vm, 1, &length);

        buffer->data = calloc(length, sizeof(uint8_t));
        if (buffer->data == NULL) {
            VM_ABORT(vm, "Failed to allocate buffer.");
            return;
        }

        memcpy(buffer->data, data, length);
        buffer->size = length;
    } else if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN) {
        Buffer* other = (Buffer*)wrenGetSlotForeign(vm, 1);

        buffer->data = calloc(other->size, sizeof(uint8_t));
        if (buffer->data == NULL) {
            VM_ABORT(vm, "Failed to allocate buffer.");
            return;
        }

        memcpy(buffer->data, other->data, other->size);
        buffer->size = other->size;
    } else {
        VM_ABORT(vm, "Invalid buffer data.");
        return;
    }
}

void bufferGetIndex(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "index");
    int index = (int)wrenGetSlotDouble(vm, 1);

    if (index < 0 || index >= buffer->size) {
        VM_ABORT(vm, "Invalid buffer index.");
        return;
    }

    wrenSetSlotDouble(vm, 0, buffer->data[index]);
}

void bufferSetIndex(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "index");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "value");
    int index = (int)wrenGetSlotDouble(vm, 1);
    uint8_t value = (uint8_t)wrenGetSlotDouble(vm, 2);

    if (index < 0 || index >= buffer->size) {
        VM_ABORT(vm, "Invalid buffer index.");
        return;
    }

    buffer->data[index] = value;
}

void bufferFill(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    uint8_t value = (uint8_t)wrenGetSlotDouble(vm, 1);
    memset(buffer->data, value, buffer->size);
}

void bufferReadInt8(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(int8_t*)(&buffer->data[offset]));
}

void bufferReadUint8(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(uint8_t*)(&buffer->data[offset]));
}

void bufferReadInt16(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 2 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(int16_t*)(&buffer->data[offset]));
}

void bufferReadUint16(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 2 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(uint16_t*)(&buffer->data[offset]));
}

void bufferReadInt32(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(int32_t*)(&buffer->data[offset]));
}

void bufferReadUint32(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(uint32_t*)(&buffer->data[offset]));
}

void bufferReadInt64(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(int64_t*)(&buffer->data[offset]));
}

void bufferReadUint64(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(uint64_t*)(&buffer->data[offset]));
}

void bufferReadFloat(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(float*)(&buffer->data[offset]));
}

void bufferReadDouble(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotDouble(vm, 0, *(double*)(&buffer->data[offset]));
}

void bufferReadBool(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "offset");
    int offset = (int)wrenGetSlotDouble(vm, 1);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    wrenSetSlotBool(vm, 0, *(bool*)(&buffer->data[offset]));
}

void bufferReadString(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "size");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    int size = (int)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + size > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    char* str = malloc(size + 1);
    memcpy(str, &buffer->data[offset], size);
    str[size] = '\0';
    wrenSetSlotString(vm, 0, str);
    free(str);
}

void bufferWriteInt8(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    int8_t value = (int8_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(int8_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 1);
}

void bufferWriteUint8(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    uint8_t value = (uint8_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(uint8_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 1);
}

void bufferWriteInt16(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    int16_t value = (int16_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 2 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(int16_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 2);
}

void bufferWriteUint16(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    uint16_t value = (uint16_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 2 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(uint16_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 2);
}

void bufferWriteInt32(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    int32_t value = (int32_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(int32_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 4);
}

void bufferWriteUint32(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    uint32_t value = (uint32_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(uint32_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 4);
}

void bufferWriteInt64(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    int64_t value = (int64_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(int64_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 8);
}

void bufferWriteUint64(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    uint64_t value = (uint64_t)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(uint64_t*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 8);
}

void bufferWriteFloat(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    float value = (float)wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 4 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(float*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 4);
}

void bufferWriteDouble(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    double value = wrenGetSlotDouble(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 8 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(double*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 8);
}

void bufferWriteBool(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, BOOL, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    bool value = wrenGetSlotBool(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    if (offset < 0 || offset + 1 > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    *(bool*)(&buffer->data[offset]) = value;
    wrenSetSlotDouble(vm, 0, offset + 1);
}

void bufferWriteString(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "value");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "offset");
    const char* value = wrenGetSlotString(vm, 1);
    int offset = (int)wrenGetSlotDouble(vm, 2);

    int size = TextLength(value);

    if (offset < 0 || offset + size > buffer->size) {
        VM_ABORT(vm, "Invalid buffer offset.");
        return;
    }

    memcpy(&buffer->data[offset], value, size);
    wrenSetSlotDouble(vm, 0, offset + size);
}

void bufferGetSize(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, buffer->size);
}

void bufferGetToString(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotBytes(vm, 0, buffer->data, buffer->size);
}

void bufferGetToList(WrenVM* vm)
{
    Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);

    wrenEnsureSlots(vm, 2);
    wrenSetSlotNewList(vm, 0);

    for (int i = 0; i < buffer->size; i++) {
        wrenSetSlotDouble(vm, 1, buffer->data[i]);
        wrenInsertInList(vm, 0, i, 1);
    }
}

void requestAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Request));
}

void requestFinalize(void* data)
{
    Request* request = (Request*)data;
    naettClose(request->res);
    naettFree(request->req);
}

void requestNew(WrenVM* vm)
{
    Request* request = (Request*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, STRING, "url");
    const char* url = wrenGetSlotString(vm, 1);
    naettInit(NULL);
    request->req = naettRequest(url, naettMethod("GET"), naettHeader("accept", "*/*"));
}

void requestMake(WrenVM* vm)
{
    Request* request = (Request*)wrenGetSlotForeign(vm, 0);
    request->res = naettMake(request->req);
}

void requestGetComplete(WrenVM* vm)
{
    Request* request = (Request*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotBool(vm, 0, naettComplete(request->res));
}

void requestGetStatus(WrenVM* vm)
{
    Request* request = (Request*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, naettGetStatus(request->res));
}

void requestGetBody(WrenVM* vm)
{
    Request* request = (Request*)wrenGetSlotForeign(vm, 0);
    int size;
    const char* data = naettGetBody(request->res, &size);
    wrenSetSlotBytes(vm, 0, data, size);
}
