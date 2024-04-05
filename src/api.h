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
void graphicsBeginBlend(WrenVM* vm);
void graphicsEndBlend(WrenVM* vm);
void graphicsBeginScissor(WrenVM* vm);
void graphicsEndScissor(WrenVM* vm);
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

void mouseDown(WrenVM* vm);
void mousePressed(WrenVM* vm);
void mouseReleased(WrenVM* vm);
void mouseSetPosition(WrenVM* vm);
void mouseSetOffset(WrenVM* vm);
void mouseSetScale(WrenVM* vm);
void mouseGetX(WrenVM* vm);
void mouseGetY(WrenVM* vm);
void mouseGetDx(WrenVM* vm);
void mouseGetDy(WrenVM* vm);
void mouseGetWheel(WrenVM* vm);
void mouseSetCursor(WrenVM* vm);

void keyboardDown(WrenVM* vm);
void keyboardPressed(WrenVM* vm);
void keyboardPressedRepeat(WrenVM* vm);
void keyboardReleased(WrenVM* vm);
void keyboardGetKeyPressed(WrenVM* vm);

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

typedef struct {
    int id;
} Gamepad;

void gamepadAllocate(WrenVM* vm);
void gamepadNew(WrenVM* vm);
void gamepadAvailable(WrenVM* vm);
void gamepadDown(WrenVM* vm);
void gamepadPressed(WrenVM* vm);
void gamepadReleased(WrenVM* vm);
void gamepadAxis(WrenVM* vm);
void gamepadGetId(WrenVM* vm);
void gamepadGetName(WrenVM* vm);
void gamepadGetAxisCount(WrenVM* vm);

void cameraAllocate(WrenVM* vm);
void cameraNew(WrenVM* vm);
void cameraBegin(WrenVM* vm);
void cameraEnd(WrenVM* vm);
void cameraGetX(WrenVM* vm);
void cameraGetY(WrenVM* vm);
void cameraGetOffsetX(WrenVM* vm);
void cameraGetOffsetY(WrenVM* vm);
void cameraGetRotation(WrenVM* vm);
void cameraGetZoom(WrenVM* vm);
void cameraSetX(WrenVM* vm);
void cameraSetY(WrenVM* vm);
void cameraSetOffsetX(WrenVM* vm);
void cameraSetOffsetY(WrenVM* vm);
void cameraSetRotation(WrenVM* vm);
void cameraSetZoom(WrenVM* vm);

void shaderAllocate(WrenVM* vm);
void shaderFinalize(void* data);
void shaderNew(WrenVM* vm);
void shaderNew2(WrenVM* vm);
void shaderBegin(WrenVM* vm);
void shaderEnd(WrenVM* vm);
void shaderSet(WrenVM* vm);

#endif
