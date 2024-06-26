#ifndef API_H
#define API_H

#include <stdint.h>

#include "lib/map/map.h"
#include "lib/microui/microui.h"
#include "lib/naett/naett.h"
#include "lib/wren/wren.h"

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

#define WRAY_VERSION "1.0.0 dev"

typedef struct vmData {
    bool audioInit;
    bool windowInit;
    bool enetInit;
    mu_Context* uiCtx;
    map_int_t keys;
    WrenHandle* textureClass;
    WrenHandle* peerClass;
} vmData;

void setArgs(int argc, char** argv);
void enetClose();
int uiTextWidth(mu_Font font, const char* text, int len);
int uiTextHeight(mu_Font font);

// Audio

void audioInit(WrenVM* vm);
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

void soundAliasFinalize(void* data);
void soundAliasNew(WrenVM* vm);

// Graphics

void graphicsBegin(WrenVM* vm);
void graphicsEnd(WrenVM* vm);
void graphicsBeginBlend(WrenVM* vm);
void graphicsEndBlend(WrenVM* vm);
void graphicsBeginScissor(WrenVM* vm);
void graphicsEndScissor(WrenVM* vm);
void graphicsScreenshot(WrenVM* vm);
void graphicsMeasure(WrenVM* vm);
void graphicsNoise(WrenVM* vm);
void graphicsClear(WrenVM* vm);
void graphicsPrint(WrenVM* vm);
void graphicsPixel(WrenVM* vm);
void graphicsLine(WrenVM* vm);
void graphicsLineBezier(WrenVM* vm);
void graphicsCircle(WrenVM* vm);
void graphicsCircleLines(WrenVM* vm);
void graphicsEllipse(WrenVM* vm);
void graphicsEllipseLines(WrenVM* vm);
void graphicsRectangle(WrenVM* vm);
void graphicsRectangleLines(WrenVM* vm);
void graphicsTriangle(WrenVM* vm);
void graphicsTriangleLines(WrenVM* vm);
void graphicsPolygon(WrenVM* vm);
void graphicsPolygonLines(WrenVM* vm);
void graphicsSetNoiseSeed(WrenVM* vm);
void graphicsSetLineSpacing(WrenVM* vm);

void uiUpdate(WrenVM* vm);
void uiDraw(WrenVM* vm);
void uiBegin(WrenVM* vm);
void uiEnd(WrenVM* vm);
void uiBeginWindow(WrenVM* vm);
void uiEndWindow(WrenVM* vm);
void uiLabel(WrenVM* vm);
void uiHeader(WrenVM* vm);
void uiButton(WrenVM* vm);
void uiRow(WrenVM* vm);
void uiTextbox(WrenVM* vm);
void uiGetWindowInfo(WrenVM* vm);
void uiSetWindowSize(WrenVM* vm);
void uiOpenPopup(WrenVM* vm);
void uiBeginPopup(WrenVM* vm);
void uiEndPopup(WrenVM* vm);
void uiBeginColumn(WrenVM* vm);
void uiEndColumn(WrenVM* vm);
void uiSlider(WrenVM* vm);
void uiNext(WrenVM* vm);
void uiDrawRect(WrenVM* vm);
void uiDrawText(WrenVM* vm);
void uiBeginTreenode(WrenVM* vm);
void uiEndTreenode(WrenVM* vm);
void uiCheckbox(WrenVM* vm);
void uiText(WrenVM* vm);
void uiBeginPanel(WrenVM* vm);
void uiEndPanel(WrenVM* vm);
void uiFocus(WrenVM* vm);

void colorAllocate(WrenVM* vm);
void colorNew(WrenVM* vm);
void colorNew2(WrenVM* vm);
void colorGetIndex(WrenVM* vm);
void colorSetIndex(WrenVM* vm);

void imageAllocate(WrenVM* vm);
void imageFinalize(void* data);
void imageNew(WrenVM* vm);
void imageNew2(WrenVM* vm);
void imageNew3(WrenVM* vm);
void imageNew4(WrenVM* vm);
void imageNew5(WrenVM* vm);
void imageNew6(WrenVM* vm);
void imageNew7(WrenVM* vm);
void imageNew8(WrenVM* vm);
void imageNew9(WrenVM* vm);
void imageExport(WrenVM* vm);
void imageExportToMemory(WrenVM* vm);
void imageCrop(WrenVM* vm);
void imageResize(WrenVM* vm);
void imageFlipVertical(WrenVM* vm);
void imageFlipHorizontal(WrenVM* vm);
void imageRotate(WrenVM* vm);
void imageGetWidth(WrenVM* vm);
void imageGetHeight(WrenVM* vm);
void imageGetFormat(WrenVM* vm);

void textureAllocate(WrenVM* vm);
void textureFinalize(void* data);
void textureNew(WrenVM* vm);
void textureDraw(WrenVM* vm);
void textureDrawRec(WrenVM* vm);
void textureGetWidth(WrenVM* vm);
void textureGetHeight(WrenVM* vm);
void textureSetFilter(WrenVM* vm);
void textureSetWrap(WrenVM* vm);

void renderTextureAllocate(WrenVM* vm);
void renderTextureFinalize(void* data);
void renderTextureNew(WrenVM* vm);
void renderTextureBegin(WrenVM* vm);
void renderTextureEnd(WrenVM* vm);
void renderTextureGetTexture(WrenVM* vm);

void fontAllocate(WrenVM* vm);
void fontFinalize(void* data);
void fontNew(WrenVM* vm);
void fontNew2(WrenVM* vm);
void fontPrint(WrenVM* vm);
void fontMeasure(WrenVM* vm);
void fontGetSize(WrenVM* vm);

void cameraAllocate(WrenVM* vm);
void cameraNew(WrenVM* vm);
void cameraBegin(WrenVM* vm);
void cameraEnd(WrenVM* vm);
void cameraScreenToWorld(WrenVM* vm);
void cameraWorldToScreen(WrenVM* vm);
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
void shaderNew3(WrenVM* vm);
void shaderNew4(WrenVM* vm);
void shaderBegin(WrenVM* vm);
void shaderEnd(WrenVM* vm);
void shaderSet(WrenVM* vm);

// Input

void keyboardPressed(WrenVM* vm);
void keyboardPressedRepeat(WrenVM* vm);
void keyboardDown(WrenVM* vm);
void keyboardReleased(WrenVM* vm);
void keyboardGetKeyPressed(WrenVM* vm);
void keyboardGetCharPressed(WrenVM* vm);

void mousePressed(WrenVM* vm);
void mouseDown(WrenVM* vm);
void mouseReleased(WrenVM* vm);
void mouseSetPosition(WrenVM* vm);
void mouseSetOffset(WrenVM* vm);
void mouseSetScale(WrenVM* vm);
void mouseGetX(WrenVM* vm);
void mouseGetY(WrenVM* vm);
void mouseGetDeltaX(WrenVM* vm);
void mouseGetDeltaY(WrenVM* vm);
void mouseGetWheel(WrenVM* vm);
void mouseSetCursor(WrenVM* vm);
void mouseGetHidden(WrenVM* vm);
void mouseSetHidden(WrenVM* vm);
void mouseSetEnabled(WrenVM* vm);
void mouseGetOnScreen(WrenVM* vm);

typedef struct {
    int id;
} Gamepad;

void gamepadAllocate(WrenVM* vm);
void gamepadAvailable(WrenVM* vm);
void gamepadNew(WrenVM* vm);
void gamepadPressed(WrenVM* vm);
void gamepadDown(WrenVM* vm);
void gamepadReleased(WrenVM* vm);
void gamepadAxis(WrenVM* vm);
void gamepadGetId(WrenVM* vm);
void gamepadGetName(WrenVM* vm);
void gamepadGetAxisCount(WrenVM* vm);

// System

void windowInit(WrenVM* vm);
void windowToggleFullscreen(WrenVM* vm);
void windowToggleBorderless(WrenVM* vm);
void windowMaximize(WrenVM* vm);
void windowMinimize(WrenVM* vm);
void windowRestore(WrenVM* vm);
void windowSetPosition(WrenVM* vm);
void windowSetMinSize(WrenVM* vm);
void windowSetMaxSize(WrenVM* vm);
void windowSetSize(WrenVM* vm);
void windowFocus(WrenVM* vm);
void windowGetMonitorInfo(WrenVM* vm);
void windowListDropped(WrenVM* vm);
void windowGetClosed(WrenVM* vm);
void windowGetFullscreen(WrenVM* vm);
void windowGetHidden(WrenVM* vm);
void windowGetMinimized(WrenVM* vm);
void windowGetMaximized(WrenVM* vm);
void windowGetFocused(WrenVM* vm);
void windowGetResized(WrenVM* vm);
void windowGetWidth(WrenVM* vm);
void windowGetHeight(WrenVM* vm);
void windowGetMonitorCount(WrenVM* vm);
void windowGetMonitor(WrenVM* vm);
void windowGetX(WrenVM* vm);
void windowGetY(WrenVM* vm);
void windowGetDpi(WrenVM* vm);
void windowGetFileDropped(WrenVM* vm);
void windowSetIcon(WrenVM* vm);
void windowSetTitle(WrenVM* vm);
void windowSetMonitor(WrenVM* vm);
void windowSetOpacity(WrenVM* vm);
void windowSetTargetFps(WrenVM* vm);
void windowGetResizable(WrenVM* vm);
void windowSetResizable(WrenVM* vm);
void windowGetVSync(WrenVM* vm);
void windowSetVSync(WrenVM* vm);
void windowGetDt(WrenVM* vm);
void windowGetTime(WrenVM* vm);
void windowGetFps(WrenVM* vm);

void osReadLine(WrenVM* vm);
void osWait(WrenVM* vm);
void osOpenUrl(WrenVM* vm);
void osGetArgs(WrenVM* vm);
void osGetName(WrenVM* vm);
void osGetWrayVersion(WrenVM* vm);
void osGetClipboard(WrenVM* vm);
void osSetClipboard(WrenVM* vm);

void dataCompress(WrenVM* vm);
void dataDecompress(WrenVM* vm);
void dataEncodeBase64(WrenVM* vm);
void dataDecodeBase64(WrenVM* vm);
void dataEncodeHex(WrenVM* vm);
void dataDecodeHex(WrenVM* vm);
void dataHash(WrenVM* vm);

void directoryExists(WrenVM* vm);
void directoryList(WrenVM* vm);

void fileExists(WrenVM* vm);
void fileSize(WrenVM* vm);
void fileRead(WrenVM* vm);
void fileReadEmbedded(WrenVM* vm);
void fileWrite(WrenVM* vm);

typedef struct {
    uint8_t* data;
    int size;
} Buffer;

void bufferAllocate(WrenVM* vm);
void bufferFinalize(void* data);
void bufferNew(WrenVM* vm);
void bufferNew2(WrenVM* vm);
void bufferGetIndex(WrenVM* vm);
void bufferSetIndex(WrenVM* vm);
void bufferFill(WrenVM* vm);
void bufferReadInt8(WrenVM* vm);
void bufferReadUint8(WrenVM* vm);
void bufferReadInt16(WrenVM* vm);
void bufferReadUint16(WrenVM* vm);
void bufferReadInt32(WrenVM* vm);
void bufferReadUint32(WrenVM* vm);
void bufferReadInt64(WrenVM* vm);
void bufferReadUint64(WrenVM* vm);
void bufferReadFloat(WrenVM* vm);
void bufferReadDouble(WrenVM* vm);
void bufferReadBool(WrenVM* vm);
void bufferReadString(WrenVM* vm);
void bufferWriteInt8(WrenVM* vm);
void bufferWriteUint8(WrenVM* vm);
void bufferWriteInt16(WrenVM* vm);
void bufferWriteUint16(WrenVM* vm);
void bufferWriteInt32(WrenVM* vm);
void bufferWriteUint32(WrenVM* vm);
void bufferWriteInt64(WrenVM* vm);
void bufferWriteUint64(WrenVM* vm);
void bufferWriteFloat(WrenVM* vm);
void bufferWriteDouble(WrenVM* vm);
void bufferWriteBool(WrenVM* vm);
void bufferWriteString(WrenVM* vm);
void bufferGetSize(WrenVM* vm);
void bufferGetToString(WrenVM* vm);
void bufferGetToList(WrenVM* vm);

typedef struct {
    naettReq* req;
    naettRes* res;
} Request;

void requestAllocate(WrenVM* vm);
void requestFinalize(void* data);
void requestNew(WrenVM* vm);
void requestMake(WrenVM* vm);
void requestGetComplete(WrenVM* vm);
void requestGetStatus(WrenVM* vm);
void requestGetBody(WrenVM* vm);

// ENet

void enetInit(WrenVM* vm);
void enetGetVersion(WrenVM* vm);

void hostAllocate(WrenVM* vm);
void hostFinalize(void* data);
void hostNew(WrenVM* vm);
void hostNew2(WrenVM* vm);
void hostConnect(WrenVM* vm);
void hostService(WrenVM* vm);
void hostCheckEvents(WrenVM* vm);
void hostCompressWithRangeCoder(WrenVM* vm);
void hostFlush(WrenVM* vm);
void hostBroadcast(WrenVM* vm);
void hostSetBandwidthLimit(WrenVM* vm);
void hostGetPeer(WrenVM* vm);
void hostGetTotalSent(WrenVM* vm);
void hostGetTotalReceived(WrenVM* vm);
void hostGetServiceTime(WrenVM* vm);
void hostGetPeerCount(WrenVM* vm);
void hostGetAddress(WrenVM* vm);
void hostSetChannelLimit(WrenVM* vm);

void peerDisconnect(WrenVM* vm);
void peerDisconnectNow(WrenVM* vm);
void peerDisconnectLater(WrenVM* vm);
void peerPing(WrenVM* vm);
void peerReset(WrenVM* vm);
void peerSend(WrenVM* vm);
void peerReceive(WrenVM* vm);
void peerConfigThrottle(WrenVM* vm);
void peerSetTimeout(WrenVM* vm);
void peerGetConnectId(WrenVM* vm);
void peerGetIndex(WrenVM* vm);
void peerGetState(WrenVM* vm);
void peerGetRtt(WrenVM* vm);
void peerGetLastRtt(WrenVM* vm);
void peerGetTimeout(WrenVM* vm);
void peerGetToString(WrenVM* vm);
void peerSetPingInterval(WrenVM* vm);
void peerSetRtt(WrenVM* vm);
void peerSetLastRtt(WrenVM* vm);

#endif
