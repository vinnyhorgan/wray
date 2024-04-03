#ifndef API_H
#define API_H

#include "lib/wren/wren.h"

void windowInit(WrenVM* vm);
void windowClose(WrenVM* vm);
void windowGetClosed(WrenVM* vm);
void windowGetWidth(WrenVM* vm);
void windowGetHeight(WrenVM* vm);
void windowGetFps(WrenVM* vm);
void windowSetTargetFps(WrenVM* vm);

void graphicsBegin(WrenVM* vm);
void graphicsEnd(WrenVM* vm);
void graphicsClear(WrenVM* vm);
void graphicsRect(WrenVM* vm);
void graphicsText(WrenVM* vm);

void mousePressed(WrenVM* vm);
void mouseDown(WrenVM* vm);
void mouseGetX(WrenVM* vm);
void mouseGetY(WrenVM* vm);

void colorAllocate(WrenVM* vm);
void colorNew(WrenVM* vm);
void colorNew2(WrenVM* vm);
void colorGetIndex(WrenVM* vm);
void colorSetIndex(WrenVM* vm);

void textureAllocate(WrenVM* vm);
void textureFinalize(void* data);
void textureNew(WrenVM* vm);
void textureGetWidth(WrenVM* vm);
void textureGetHeight(WrenVM* vm);
void textureDraw(WrenVM* vm);

#endif
