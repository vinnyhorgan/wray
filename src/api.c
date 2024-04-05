#include "api.h"

#include <stdio.h>

#include <raylib.h>

#include "lib/map/map.h"

#include "icon.h"

#define VM_ABORT(vm, error)              \
    do {                                 \
        wrenSetSlotString(vm, 0, error); \
        wrenAbortFiber(vm, 0);           \
    } while (false);

#define ASSERT_SLOT_TYPE(vm, slot, type, fieldName)                       \
    if (wrenGetSlotType(vm, slot) != WREN_TYPE_##type) {                  \
        VM_ABORT(vm, "Expected " #fieldName " to be of type " #type "."); \
        return;                                                           \
    }

static map_int_t keys;

void audioInit(WrenVM* vm)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        VM_ABORT(vm, "Failed to initialize audio.");
        return;
    }
}

void audioClose(WrenVM* vm)
{
    CloseAudioDevice();
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

    if (IsAudioDeviceReady())
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

void fontAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Font));
}

void fontFinalize(void* data)
{
    Font* font = (Font*)data;

    if (IsWindowReady())
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

    *font = LoadFontEx(path, size, NULL, 0);
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
    ASSERT_SLOT_TYPE(vm, 5, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 7, FOREIGN, "color");

    const char* text = wrenGetSlotString(vm, 1);
    int x = (int)wrenGetSlotDouble(vm, 2);
    int y = (int)wrenGetSlotDouble(vm, 3);
    float r = (float)wrenGetSlotDouble(vm, 4);
    int ox = (int)wrenGetSlotDouble(vm, 5);
    int oy = (int)wrenGetSlotDouble(vm, 6);
    Color* color = (Color*)wrenGetSlotForeign(vm, 7);

    DrawTextPro(*font, text, (Vector2) { (float)x, (float)y }, (Vector2) { (float)ox, (float)oy }, r, (float)font->baseSize, 0, *color);
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

    map_init(&keys);

    map_set(&keys, "apostrophe", KEY_APOSTROPHE);
    map_set(&keys, "comma", KEY_COMMA);
    map_set(&keys, "minus", KEY_MINUS);
    map_set(&keys, "period", KEY_PERIOD);
    map_set(&keys, "slash", KEY_SLASH);
    map_set(&keys, "zero", KEY_ZERO);
    map_set(&keys, "one", KEY_ONE);
    map_set(&keys, "two", KEY_TWO);
    map_set(&keys, "three", KEY_THREE);
    map_set(&keys, "four", KEY_FOUR);
    map_set(&keys, "five", KEY_FIVE);
    map_set(&keys, "six", KEY_SIX);
    map_set(&keys, "seven", KEY_SEVEN);
    map_set(&keys, "eight", KEY_EIGHT);
    map_set(&keys, "nine", KEY_NINE);
    map_set(&keys, "semicolon", KEY_SEMICOLON);
    map_set(&keys, "equal", KEY_EQUAL);
    map_set(&keys, "a", KEY_A);
    map_set(&keys, "b", KEY_B);
    map_set(&keys, "c", KEY_C);
    map_set(&keys, "d", KEY_D);
    map_set(&keys, "e", KEY_E);
    map_set(&keys, "f", KEY_F);
    map_set(&keys, "g", KEY_G);
    map_set(&keys, "h", KEY_H);
    map_set(&keys, "i", KEY_I);
    map_set(&keys, "j", KEY_J);
    map_set(&keys, "k", KEY_K);
    map_set(&keys, "l", KEY_L);
    map_set(&keys, "m", KEY_M);
    map_set(&keys, "n", KEY_N);
    map_set(&keys, "o", KEY_O);
    map_set(&keys, "p", KEY_P);
    map_set(&keys, "q", KEY_Q);
    map_set(&keys, "r", KEY_R);
    map_set(&keys, "s", KEY_S);
    map_set(&keys, "t", KEY_T);
    map_set(&keys, "u", KEY_U);
    map_set(&keys, "v", KEY_V);
    map_set(&keys, "w", KEY_W);
    map_set(&keys, "x", KEY_X);
    map_set(&keys, "y", KEY_Y);
    map_set(&keys, "z", KEY_Z);
    map_set(&keys, "leftBracket", KEY_LEFT_BRACKET);
    map_set(&keys, "backslash", KEY_BACKSLASH);
    map_set(&keys, "rightBracket", KEY_RIGHT_BRACKET);
    map_set(&keys, "grave", KEY_GRAVE);
    map_set(&keys, "space", KEY_SPACE);
    map_set(&keys, "escape", KEY_ESCAPE);
    map_set(&keys, "enter", KEY_ENTER);
    map_set(&keys, "tab", KEY_TAB);
    map_set(&keys, "backspace", KEY_BACKSPACE);
    map_set(&keys, "insert", KEY_INSERT);
    map_set(&keys, "delete", KEY_DELETE);
    map_set(&keys, "right", KEY_RIGHT);
    map_set(&keys, "left", KEY_LEFT);
    map_set(&keys, "down", KEY_DOWN);
    map_set(&keys, "up", KEY_UP);
    map_set(&keys, "pageUp", KEY_PAGE_UP);
    map_set(&keys, "pageDown", KEY_PAGE_DOWN);
    map_set(&keys, "home", KEY_HOME);
    map_set(&keys, "end", KEY_END);
    map_set(&keys, "capsLock", KEY_CAPS_LOCK);
    map_set(&keys, "scrollLock", KEY_SCROLL_LOCK);
    map_set(&keys, "numLock", KEY_NUM_LOCK);
    map_set(&keys, "printScreen", KEY_PRINT_SCREEN);
    map_set(&keys, "pause", KEY_PAUSE);
    map_set(&keys, "f1", KEY_F1);
    map_set(&keys, "f2", KEY_F2);
    map_set(&keys, "f3", KEY_F3);
    map_set(&keys, "f4", KEY_F4);
    map_set(&keys, "f5", KEY_F5);
    map_set(&keys, "f6", KEY_F6);
    map_set(&keys, "f7", KEY_F7);
    map_set(&keys, "f8", KEY_F8);
    map_set(&keys, "f9", KEY_F9);
    map_set(&keys, "f10", KEY_F10);
    map_set(&keys, "f11", KEY_F11);
    map_set(&keys, "f12", KEY_F12);
    map_set(&keys, "leftShift", KEY_LEFT_SHIFT);
    map_set(&keys, "leftControl", KEY_LEFT_CONTROL);
    map_set(&keys, "leftAlt", KEY_LEFT_ALT);
    map_set(&keys, "leftSuper", KEY_LEFT_SUPER);
    map_set(&keys, "rightShift", KEY_RIGHT_SHIFT);
    map_set(&keys, "rightControl", KEY_RIGHT_CONTROL);
    map_set(&keys, "rightAlt", KEY_RIGHT_ALT);
    map_set(&keys, "rightSuper", KEY_RIGHT_SUPER);
    map_set(&keys, "kbMenu", KEY_KB_MENU);
    map_set(&keys, "kp0", KEY_KP_0);
    map_set(&keys, "kp1", KEY_KP_1);
    map_set(&keys, "kp2", KEY_KP_2);
    map_set(&keys, "kp3", KEY_KP_3);
    map_set(&keys, "kp4", KEY_KP_4);
    map_set(&keys, "kp5", KEY_KP_5);
    map_set(&keys, "kp6", KEY_KP_6);
    map_set(&keys, "kp7", KEY_KP_7);
    map_set(&keys, "kp8", KEY_KP_8);
    map_set(&keys, "kp9", KEY_KP_9);
    map_set(&keys, "kpDecimal", KEY_KP_DECIMAL);
    map_set(&keys, "kpDivide", KEY_KP_DIVIDE);
    map_set(&keys, "kpMultiply", KEY_KP_MULTIPLY);
    map_set(&keys, "kpSubtract", KEY_KP_SUBTRACT);
    map_set(&keys, "kpAdd", KEY_KP_ADD);
    map_set(&keys, "kpEnter", KEY_KP_ENTER);
    map_set(&keys, "kpEqual", KEY_KP_EQUAL);
}

void windowClose(WrenVM* vm)
{
    map_deinit(&keys);

    CloseWindow();
}

void windowToggleFullscreen(WrenVM* vm)
{
    ToggleFullscreen();
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

void windowSetTitle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "title");
    SetWindowTitle(wrenGetSlotString(vm, 1));
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

void windowGetClipboard(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotString(vm, 0, GetClipboardText());
}

void windowSetClipboard(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "text");
    SetClipboardText(wrenGetSlotString(vm, 1));
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

void windowSetTargetFps(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "fps");
    SetTargetFPS((int)wrenGetSlotDouble(vm, 1));
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
        VM_ABORT(vm, "Invalid blend mode. (\"alpha\", \"additive\"...)");
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

    DrawText(text, x, y, size, *color);
}

void graphicsTakeScreenshot(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");
    const char* path = wrenGetSlotString(vm, 1);
    TakeScreenshot(path);
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

void graphicsCircleLine(WrenVM* vm)
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

void graphicsEllipseLine(WrenVM* vm)
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
    ASSERT_SLOT_TYPE(vm, 5, NUM, "ox");
    ASSERT_SLOT_TYPE(vm, 6, NUM, "oy");
    ASSERT_SLOT_TYPE(vm, 7, NUM, "r");
    ASSERT_SLOT_TYPE(vm, 8, FOREIGN, "color");

    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    int width = (int)wrenGetSlotDouble(vm, 3);
    int height = (int)wrenGetSlotDouble(vm, 4);
    int ox = (int)wrenGetSlotDouble(vm, 5);
    int oy = (int)wrenGetSlotDouble(vm, 6);
    float r = (float)wrenGetSlotDouble(vm, 7);
    Color* color = (Color*)wrenGetSlotForeign(vm, 8);

    DrawRectanglePro((Rectangle) { (float)x, (float)y, (float)width, (float)height }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void graphicsRectangleLine(WrenVM* vm)
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

void graphicsTriangleLine(WrenVM* vm)
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

void graphicsPolygonLine(WrenVM* vm)
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

void mouseDown(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");

    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsMouseButtonDown(button));
}

void mousePressed(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");

    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsMouseButtonPressed(button));
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

void mouseGetDx(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetMouseDelta().x);
}

void mouseGetDy(WrenVM* vm)
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
        VM_ABORT(vm, "Invalid cursor. (\"arrow\", \"ibeam\"...)");
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

void keyboardDown(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");

    const char* key = wrenGetSlotString(vm, 1);
    KeyboardKey k = *map_get(&keys, key);

    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key. (\"space\", \"enter\"...)");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyDown(k));
}

void keyboardPressed(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");

    const char* key = wrenGetSlotString(vm, 1);
    KeyboardKey k = *map_get(&keys, key);

    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key. (\"space\", \"enter\"...)");
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
    KeyboardKey k = *map_get(&keys, key);

    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key. (\"space\", \"enter\"...)");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyPressedRepeat(k));
}

void keyboardReleased(WrenVM* vm)
{
    if (!IsWindowReady())
        return;

    ASSERT_SLOT_TYPE(vm, 1, STRING, "key");

    const char* key = wrenGetSlotString(vm, 1);
    KeyboardKey k = *map_get(&keys, key);

    if (k == KEY_NULL) {
        VM_ABORT(vm, "Invalid key. (\"space\", \"enter\"...)");
        return;
    }

    wrenSetSlotBool(vm, 0, IsKeyReleased(k));
}

void keyboardGetKeyPressed(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetKeyPressed());
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

void textureAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Texture));
}

void textureFinalize(void* data)
{
    Texture* texture = (Texture*)data;

    if (IsWindowReady())
        UnloadTexture(*texture);
}

void textureNew(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, STRING, "path");

    const char* path = wrenGetSlotString(vm, 1);

    if (!IsWindowReady()) {
        VM_ABORT(vm, "Cannot load texture before window initialization.");
        return;
    }

    *texture = LoadTexture(path);
    if (!IsTextureReady(*texture)) {
        VM_ABORT(vm, "Failed to load texture.");
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

void textureDrawRect(WrenVM* vm)
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

    if (TextIsEqual(filter, "nearest"))
        SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
    else if (TextIsEqual(filter, "linear"))
        SetTextureFilter(*texture, TEXTURE_FILTER_BILINEAR);
    else
        VM_ABORT(vm, "Invalid texture filter. (\"nearest\" or \"linear\")");
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
        VM_ABORT(vm, "Invalid texture wrap. (\"repeat\" or \"clamp\")");
}

void renderTextureAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(RenderTexture));
}

void renderTextureFinalize(void* data)
{
    RenderTexture* texture = (RenderTexture*)data;

    if (IsWindowReady())
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

void renderTextureDraw(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);

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

    Rectangle source = { 0, 0, (float)texture->texture.width, (float)texture->texture.height };

    if (sx < 0)
        source.width = -source.width;

    if (sy < 0)
        source.height = -source.height;

    source.height = -source.height;

    float absSx = sx < 0 ? -sx : sx;
    float absSy = sy < 0 ? -sy : sy;

    DrawTexturePro(texture->texture, source, (Rectangle) { (float)x, (float)y, (float)texture->texture.width * absSx, (float)texture->texture.height * absSy }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void renderTextureDrawRect(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);

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

    source.height = -source.height;

    float absSx = sx < 0 ? -sx : sx;
    float absSy = sy < 0 ? -sy : sy;

    DrawTexturePro(texture->texture, source, (Rectangle) { (float)dstX, (float)dstY, (float)srcWidth * absSx, (float)srcHeight * absSy }, (Vector2) { (float)ox, (float)oy }, r, *color);
}

void renderTextureGetWidth(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, texture->texture.width);
}

void renderTextureGetHeight(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);
    wrenSetSlotDouble(vm, 0, texture->texture.height);
}

void renderTextureSetFilter(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, STRING, "filter");

    const char* filter = wrenGetSlotString(vm, 1);

    if (TextIsEqual(filter, "nearest"))
        SetTextureFilter(texture->texture, TEXTURE_FILTER_POINT);
    else if (TextIsEqual(filter, "linear"))
        SetTextureFilter(texture->texture, TEXTURE_FILTER_BILINEAR);
    else
        VM_ABORT(vm, "Invalid texture filter. (\"nearest\" or \"linear\")");
}

void renderTextureSetWrap(WrenVM* vm)
{
    RenderTexture* texture = (RenderTexture*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, STRING, "wrap");

    const char* wrap = wrenGetSlotString(vm, 1);

    if (TextIsEqual(wrap, "repeat"))
        SetTextureWrap(texture->texture, TEXTURE_WRAP_REPEAT);
    else if (TextIsEqual(wrap, "clamp"))
        SetTextureWrap(texture->texture, TEXTURE_WRAP_CLAMP);
    else
        VM_ABORT(vm, "Invalid texture wrap. (\"repeat\" or \"clamp\")");
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

void gamepadDown(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");

    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadButtonDown(gamepad->id, button));
}

void gamepadPressed(WrenVM* vm)
{
    Gamepad* gamepad = (Gamepad*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, NUM, "button");

    int button = (int)wrenGetSlotDouble(vm, 1);
    wrenSetSlotBool(vm, 0, IsGamepadButtonPressed(gamepad->id, button));
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

    float z = (float)wrenGetSlotDouble(vm, 1);
    camera->zoom = z;
}

void shaderAllocate(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Shader));
}

void shaderFinalize(void* data)
{
    Shader* shader = (Shader*)data;

    if (IsWindowReady()) {
        UnloadShader(*shader);
    }
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
    } else if (wrenGetSlotType(vm, 2) == WREN_TYPE_FOREIGN) {
        Texture* texture = (Texture*)wrenGetSlotForeign(vm, 2);
        SetShaderValueTexture(*shader, GetShaderLocation(*shader, name), *texture);
    }
}
