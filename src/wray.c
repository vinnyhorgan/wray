#include <raylib.h>

#include "icon.h"

int main()
{
    InitWindow(640, 480, "wray");
    SetTargetFPS(60);

    Image icon = {
        .data = ICON_DATA,
        .width = ICON_WIDTH,
        .height = ICON_HEIGHT,
        .format = ICON_FORMAT,
        .mipmaps = 1
    };

    SetWindowIcon(icon);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
