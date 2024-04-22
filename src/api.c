#include "api.h"

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
    int ox = (int)wrenGetSlotDouble(vm, 5);
    int oy = (int)wrenGetSlotDouble(vm, 6);
    float r = (float)wrenGetSlotDouble(vm, 7);
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
    *image = LoadImageFromScreen();
}

void imageNew4(WrenVM* vm)
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

void imageNew5(WrenVM* vm)
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

void imageNew6(WrenVM* vm)
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

void imageNew7(WrenVM* vm)
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

    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "x");
    wrenSetSlotDouble(vm, 2, result.x);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "y");
    wrenSetSlotDouble(vm, 2, result.y);
    wrenSetMapValue(vm, 0, 1, 2);
}

void cameraWorldToScreen(WrenVM* vm)
{
    Camera2D* camera = (Camera2D*)wrenGetSlotForeign(vm, 0);
    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);

    Vector2 result = GetWorldToScreen2D((Vector2) { (float)x, (float)y }, *camera);

    wrenSetSlotNewMap(vm, 0);

    wrenSetSlotString(vm, 1, "x");
    wrenSetSlotDouble(vm, 2, result.x);
    wrenSetMapValue(vm, 0, 1, 2);

    wrenSetSlotString(vm, 1, "y");
    wrenSetSlotDouble(vm, 2, result.y);
    wrenSetMapValue(vm, 0, 1, 2);
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

void windowSetTitle(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, STRING, "title");
    const char* title = wrenGetSlotString(vm, 1);
    SetWindowTitle(title);
}

void windowSetIcon(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "icon");
    Texture* icon = (Texture*)wrenGetSlotForeign(vm, 1);
    Image image = LoadImageFromTexture(*icon);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(image);
    UnloadImage(image);
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
