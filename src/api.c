#include "api.h"

#include <stdio.h>

#include <raylib.h>

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
    wrenSetSlotNewForeign(vm, 0, 0, sizeof(Texture));
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

    SetSoundVolume(*sound, volume);
}

void soundSetPitch(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, NUM, "pitch");

    float pitch = (float)wrenGetSlotDouble(vm, 1);

    SetSoundPitch(*sound, pitch);
}

void soundSetPan(WrenVM* vm)
{
    Sound* sound = (Sound*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, NUM, "pan");

    float pan = (float)wrenGetSlotDouble(vm, 1);

    SetSoundPan(*sound, pan);
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
}

void windowClose(WrenVM* vm)
{
    CloseWindow();
}

void windowGetClosed(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotBool(vm, 0, WindowShouldClose());
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

void windowGetFps(WrenVM* vm)
{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, GetFPS());
}

void windowSetTargetFps(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, NUM, "fps");
    SetTargetFPS((int)wrenGetSlotDouble(vm, 1));
}

void graphicsBegin(WrenVM* vm)
{
    BeginDrawing();
}

void graphicsEnd(WrenVM* vm)
{
    EndDrawing();
}

void graphicsClear(WrenVM* vm)
{
    ASSERT_SLOT_TYPE(vm, 1, FOREIGN, "color");

    Color* color = (Color*)wrenGetSlotForeign(vm, 1);
    ClearBackground(*color);
}

void graphicsRect(WrenVM* vm)
{
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

    DrawRectangle(x, y, width, height, *color);
}

void graphicsText(WrenVM* vm)
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
        VM_ABORT(vm, "Cannot create texture before window initialization.");
        return;
    }

    *texture = LoadTexture(path);
    if (!IsTextureReady(*texture)) {
        VM_ABORT(vm, "Failed to load texture.");
        return;
    }
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

void textureDraw(WrenVM* vm)
{
    Texture* texture = (Texture*)wrenGetSlotForeign(vm, 0);

    ASSERT_SLOT_TYPE(vm, 1, NUM, "x");
    ASSERT_SLOT_TYPE(vm, 2, NUM, "y");
    ASSERT_SLOT_TYPE(vm, 3, FOREIGN, "color");

    int x = (int)wrenGetSlotDouble(vm, 1);
    int y = (int)wrenGetSlotDouble(vm, 2);
    Color* color = (Color*)wrenGetSlotForeign(vm, 3);

    DrawTexture(*texture, x, y, *color);
}
