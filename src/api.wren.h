// Generated automatically from ..\Documents\projects\wray\src\api.wren. Do not edit.
static const char* apiModuleSource =
"class Audio {\n"
"    foreign static init()\n"
"    foreign static close()\n"
"\n"
"    foreign static volume\n"
"    foreign static volume=(v)\n"
"}\n"
"\n"
"foreign class Sound {\n"
"    foreign construct new(path)\n"
"\n"
"    foreign play()\n"
"    foreign stop()\n"
"    foreign pause()\n"
"    foreign resume()\n"
"\n"
"    foreign playing\n"
"    foreign volume=(v)\n"
"    foreign pitch=(v)\n"
"    foreign pan=(v)\n"
"}\n"
"\n"
"foreign class Font {\n"
"    foreign construct new(path, size)\n"
"\n"
"    foreign print(text, x, y, r, ox, oy, color)\n"
"\n"
"    print(text, x, y, color) {\n"
"        print(text, x, y, 0, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign measure(text)\n"
"\n"
"    foreign size\n"
"}\n"
"\n"
"class Window {\n"
"    foreign static init(width, height, title)\n"
"    foreign static close()\n"
"    foreign static toggleFullscreen()\n"
"    foreign static maximize()\n"
"    foreign static minimize()\n"
"    foreign static restore()\n"
"    foreign static setPosition(x, y)\n"
"    foreign static setMinSize(width, height)\n"
"    foreign static setMaxSize(width, height)\n"
"    foreign static setSize(width, height)\n"
"    foreign static focus()\n"
"\n"
"    foreign static closed\n"
"    foreign static fullscreen\n"
"    foreign static hidden\n"
"    foreign static minimized\n"
"    foreign static maximized\n"
"    foreign static focused\n"
"    foreign static resized\n"
"    foreign static title=(v)\n"
"    foreign static width\n"
"    foreign static height\n"
"    foreign static x\n"
"    foreign static y\n"
"    foreign static dpi\n"
"\n"
"    foreign static resizable\n"
"    foreign static resizable=(v)\n"
"    foreign static vsync\n"
"    foreign static vsync=(v)\n"
"\n"
"    foreign static targetFps=(v)\n"
"    foreign static dt\n"
"    foreign static time\n"
"    foreign static fps\n"
"}\n"
"\n"
"class Graphics {\n"
"    foreign static begin()\n"
"    foreign static end()\n"
"    foreign static beginBlend(mode)\n"
"    foreign static endBlend()\n"
"    foreign static beginScissor(x, y, width, height)\n"
"    foreign static endScissor()\n"
"    foreign static clear(color)\n"
"    foreign static print(text, x, y, size, color)\n"
"    foreign static takeScreenshot(path)\n"
"\n"
"    foreign static pixel(x, y, color)\n"
"    foreign static line(x1, y1, x2, y2, thick, color)\n"
"    foreign static circle(x, y, radius, color)\n"
"    foreign static circleLine(x, y, radius, color)\n"
"    foreign static ellipse(x, y, rx, ry, color)\n"
"    foreign static ellipseLine(x, y, rx, ry, color)\n"
"    foreign static rectangle(x, y, width, height, ox, oy, r, color)\n"
"    foreign static rectangleLine(x, y, width, height, thick, color)\n"
"    foreign static triangle(x1, y1, x2, y2, x3, y3, color)\n"
"    foreign static triangleLine(x1, y1, x2, y2, x3, y3, color)\n"
"    foreign static polygon(x, y, sides, radius, r, color)\n"
"    foreign static polygonLine(x, y, sides, radius, r, thick, color)\n"
"\n"
"    static line(x1, y1, x2, y2, color) {\n"
"        line(x1, y1, x2, y2, 1, color)\n"
"    }\n"
"\n"
"    static rectangle(x, y, width, height, color) {\n"
"        rectangle(x, y, width, height, 0, 0, 0, color)\n"
"    }\n"
"\n"
"    static rectangleLine(x, y, width, height, color) {\n"
"        rectangleLine(x, y, width, height, 1, color)\n"
"    }\n"
"\n"
"    static polygonLine(x, y, sides, radius, r, color) {\n"
"        polygonLine(x, y, sides, radius, r, 1, color)\n"
"    }\n"
"}\n"
"\n"
"foreign class Gamepad {\n"
"    foreign static available(id)\n"
"\n"
"    foreign construct new(id)\n"
"\n"
"    foreign down(button)\n"
"    foreign pressed(button)\n"
"    foreign released(button)\n"
"    foreign axis(axis)\n"
"\n"
"    foreign id\n"
"    foreign name\n"
"    foreign axisCount\n"
"}\n"
"\n"
"class Mouse {\n"
"    foreign static down(button)\n"
"    foreign static pressed(button)\n"
"    foreign static released(button)\n"
"    foreign static setPosition(x, y)\n"
"    foreign static setOffset(x, y)\n"
"    foreign static setScale(x, y)\n"
"\n"
"    foreign static x\n"
"    foreign static y\n"
"    foreign static dx\n"
"    foreign static dy\n"
"    foreign static wheel\n"
"    foreign static cursor=(v)\n"
"    foreign static hidden\n"
"    foreign static hidden=(v)\n"
"    foreign static enabled=(v)\n"
"    foreign static onScreen\n"
"}\n"
"\n"
"class Keyboard {\n"
"    foreign static down(key)\n"
"    foreign static pressed(key)\n"
"    foreign static pressedRepeat(key)\n"
"    foreign static released(key)\n"
"\n"
"    foreign static keyPressed\n"
"}\n"
"\n"
"foreign class Color {\n"
"    foreign construct new(r, g, b, a)\n"
"    foreign construct new(r, g, b)\n"
"\n"
"    foreign [index]\n"
"    foreign [index]=(v)\n"
"\n"
"    r { this[0] }\n"
"    g { this[1] }\n"
"    b { this[2] }\n"
"    a { this[3] }\n"
"\n"
"    r=(v) { this[0] = v }\n"
"    g=(v) { this[0] = v }\n"
"    b=(v) { this[0] = v }\n"
"    a=(v) { this[0] = v }\n"
"\n"
"    toString { \"Color (r: %(r), g: %(g), b: %(b), a: %(a))\" }\n"
"\n"
"    static none { new(0, 0, 0, 0) }\n"
"    static black { new(0, 0, 0) }\n"
"    static darkBlue { new(29, 43, 83) }\n"
"    static darkPurple { new(126, 37, 83) }\n"
"    static darkGreen { new(0, 135, 81) }\n"
"    static brown { new(171, 82, 54) }\n"
"    static darkGray { new(95, 87, 79) }\n"
"    static lightGray { new(194, 195, 199) }\n"
"    static white { new(255, 241, 232) }\n"
"    static red { new(255, 0, 77) }\n"
"    static orange { new(255, 163, 0) }\n"
"    static yellow { new(255, 236, 39) }\n"
"    static green { new(0, 228, 54) }\n"
"    static blue { new(41, 173, 255) }\n"
"    static indigo { new(131, 118, 156) }\n"
"    static pink { new(255, 119, 168) }\n"
"    static peach { new(255, 204, 170) }\n"
"}\n"
"\n"
"foreign class Texture {\n"
"    foreign construct new(path)\n"
"\n"
"    foreign draw(x, y, r, sx, sy, ox, oy, color)\n"
"\n"
"    draw(x, y) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    draw(x, y, color) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)\n"
"\n"
"    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {\n"
"        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {\n"
"        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign width\n"
"    foreign height\n"
"    foreign filter=(v)\n"
"    foreign wrap=(v)\n"
"}\n"
"\n"
"foreign class RenderTexture {\n"
"    foreign construct new(width, height)\n"
"\n"
"    foreign begin()\n"
"    foreign end()\n"
"\n"
"    foreign draw(x, y, r, sx, sy, ox, oy, color)\n"
"\n"
"    draw(x, y) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    draw(x, y, color) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)\n"
"\n"
"    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {\n"
"        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {\n"
"        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign width\n"
"    foreign height\n"
"    foreign filter=(v)\n"
"    foreign wrap=(v)\n"
"}\n"
"\n"
"foreign class Camera {\n"
"    foreign construct new(x, y)\n"
"\n"
"    foreign begin()\n"
"    foreign end()\n"
"\n"
"    foreign x\n"
"    foreign x=(v)\n"
"    foreign y\n"
"    foreign y=(v)\n"
"    foreign ox\n"
"    foreign ox=(v)\n"
"    foreign oy\n"
"    foreign oy=(v)\n"
"    foreign r\n"
"    foreign r=(v)\n"
"    foreign zoom\n"
"    foreign zoom=(v)\n"
"}\n"
"\n"
"foreign class Shader {\n"
"    foreign construct new(vs, fs)\n"
"    foreign construct new(fs)\n"
"\n"
"    foreign begin()\n"
"    foreign end()\n"
"    foreign set(name, value)\n"
"}\n"
"\n"
"class OS {\n"
"    foreign static openUrl(url)\n"
"    foreign static readLine()\n"
"\n"
"    foreign static args\n"
"    foreign static name\n"
"    foreign static clipboard\n"
"    foreign static clipboard=(v)\n"
"}\n"
"\n"
"class Directory {\n"
"    foreign static exists(path)\n"
"    foreign static list(path)\n"
"}\n"
"\n"
"class File {\n"
"    foreign static exists(path)\n"
"    foreign static write(path, data)\n"
"    foreign static read(path)\n"
"    foreign static size(path)\n"
"}\n";
