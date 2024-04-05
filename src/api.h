#ifndef API_H
#define API_H

#include "lib/wren/wren.h"

void audioInit(WrenVM* vm);
void audioClose(WrenVM* vm);
void audioGetVolume(WrenVM* vm);
void audioSetVolume(WrenVM* vm);

void soundAllocate(WrenVM* vm);
void soundFinalize(void* data);
void soundNew(WrenVM* vm);
void soundPlay(WrenVM* vm);
void soundStop(WrenVM* vm);
void soundPause(WrenVM* vm);
void soundResume(WrenVM* vm);
void soundGetPlaying(WrenVM* vm);
void soundSetVolume(WrenVM* vm);
void soundSetPitch(WrenVM* vm);
void soundSetPan(WrenVM* vm);

void fontAllocate(WrenVM* vm);
void fontFinalize(void* data);
void fontNew(WrenVM* vm);
void fontPrint(WrenVM* vm);
void fontMeasure(WrenVM* vm);
void fontGetSize(WrenVM* vm);

void windowInit(WrenVM* vm);
void windowClose(WrenVM* vm);
void windowGetClosed(WrenVM* vm);
void windowGetWidth(WrenVM* vm);
void windowGetHeight(WrenVM* vm);
void windowGetFps(WrenVM* vm);
void windowSetTargetFps(WrenVM* vm);
void windowSetResizable(WrenVM* vm);

void graphicsBegin(WrenVM* vm);
void graphicsEnd(WrenVM* vm);
void graphicsClear(WrenVM* vm);
void graphicsPrint(WrenVM* vm);
void graphicsPixel(WrenVM* vm);
void graphicsLine(WrenVM* vm);
void graphicsCircle(WrenVM* vm);
void graphicsCircleLine(WrenVM* vm);
void graphicsEllipse(WrenVM* vm);
void graphicsEllipseLine(WrenVM* vm);
void graphicsRectangle(WrenVM* vm);
void graphicsRectangleLine(WrenVM* vm);
void graphicsTriangle(WrenVM* vm);
void graphicsTriangleLine(WrenVM* vm);
void graphicsPolygon(WrenVM* vm);
void graphicsPolygonLine(WrenVM* vm);

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
void textureDraw(WrenVM* vm);
void textureDrawRect(WrenVM* vm);
void textureGetWidth(WrenVM* vm);
void textureGetHeight(WrenVM* vm);
void textureSetFilter(WrenVM* vm);
void textureSetWrap(WrenVM* vm);

#endif
