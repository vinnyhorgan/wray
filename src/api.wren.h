// Generated automatically from .\wray\src\api.wren. Do not edit.
static const char* apiModuleSource =
"//------------------------------\n"
"// Audio\n"
"//------------------------------\n"
"\n"
"class Audio {\n"
"    foreign static init()        // Initialize audio device\n"
"\n"
"    foreign static volume        // Get master volume\n"
"    foreign static volume=(v)    // Set master volume\n"
"}\n"
"\n"
"foreign class Sound {\n"
"    foreign construct new(path)    // Load sound from file (WAV, OGG, MP3, FLAC)\n"
"\n"
"    foreign play()                 // Play sound\n"
"    foreign stop()                 // Stop playing sound\n"
"    foreign pause()                // Pause sound\n"
"    foreign resume()               // Resume paused sound\n"
"\n"
"    foreign playing                // Check if sound is playing\n"
"    foreign volume=(v)             // Set sound volume (1.0 = max volume)\n"
"    foreign pitch=(v)              // Set sound pitch (1.0 = normal)\n"
"    foreign pan=(v)                // Set sound pan (0.5 = middle)\n"
"}\n"
"\n"
"foreign class SoundAlias {\n"
"    foreign construct new(sound)    // Load sound alias from sound, it shares the same sound data but does not own it\n"
"\n"
"    foreign play()                  // Play sound\n"
"    foreign stop()                  // Stop playing sound\n"
"    foreign pause()                 // Pause sound\n"
"    foreign resume()                // Resume paused sound\n"
"\n"
"    foreign playing                 // Check if sound is playing\n"
"    foreign volume=(v)              // Set sound volume (1.0 = max volume)\n"
"    foreign pitch=(v)               // Set sound pitch (1.0 = normal)\n"
"    foreign pan=(v)                 // Set sound pan (0.5 = middle)\n"
"}\n"
"\n"
"//------------------------------\n"
"// Graphics\n"
"//------------------------------\n"
"\n"
"class Graphics {\n"
"    foreign static begin()                                               // Begin rendering to screen\n"
"    foreign static end()                                                 // End rendering to screen\n"
"    foreign static beginBlend(mode)                                      // Begin blending mode (\"alpha\", \"additive\", \"multiplied\", \"addColors\", \"subtractColors\", \"alphaPremultiply\")\n"
"    foreign static endBlend()                                            // End blending mode (returns to \"alpha\")\n"
"    foreign static beginScissor(x, y, width, height)                     // Begin scissor mode\n"
"    foreign static endScissor()                                          // End scissor mode\n"
"    foreign static screenshot(path)                                      // Save screenshot to file\n"
"    foreign static measure(text, size)                                   // Measure text width using default font\n"
"    foreign static noise(x, y, frequency, depth)                         // Get perlin noise value\n"
"\n"
"    foreign static clear(color)                                          // Clear the screen\n"
"    foreign static print(text, x, y, size, color)                        // Draw text using default font\n"
"    foreign static pixel(x, y, color)                                    // Draw pixel\n"
"    foreign static line(x1, y1, x2, y2, thick, color)                    // Draw line\n"
"    foreign static lineBezier(x1, y1, x2, y2, thick, color)              // Draw bezier curve\n"
"    foreign static circle(x, y, radius, color)                           // Draw circle\n"
"    foreign static circleLines(x, y, radius, color)                      // Draw circle outline\n"
"    foreign static ellipse(x, y, rx, ry, color)                          // Draw ellipse\n"
"    foreign static ellipseLines(x, y, rx, ry, color)                     // Draw ellipse outline\n"
"    foreign static rectangle(x, y, width, height, r, ox, oy, color)      // Draw rectangle\n"
"    foreign static rectangleLines(x, y, width, height, thick, color)     // Draw rectangle outline\n"
"    foreign static triangle(x1, y1, x2, y2, x3, y3, color)               // Draw triangle\n"
"    foreign static triangleLines(x1, y1, x2, y2, x3, y3, color)          // Draw triangle outline\n"
"    foreign static polygon(x, y, sides, radius, r, color)                // Draw polygon\n"
"    foreign static polygonLines(x, y, sides, radius, r, thick, color)    // Draw polygon outline\n"
"\n"
"    static line(x1, y1, x2, y2, color) {\n"
"        line(x1, y1, x2, y2, 1, color)\n"
"    }\n"
"\n"
"    static lineBezier(x1, y1, x2, y2, color) {\n"
"        lineBezier(x1, y1, x2, y2, 1, color)\n"
"    }\n"
"\n"
"    static rectangle(x, y, width, height, color) {\n"
"        rectangle(x, y, width, height, 0, 0, 0, color)\n"
"    }\n"
"\n"
"    static rectangleLines(x, y, width, height, color) {\n"
"        rectangleLines(x, y, width, height, 1, color)\n"
"    }\n"
"\n"
"    static polygonLines(x, y, sides, radius, r, color) {\n"
"        polygonLines(x, y, sides, radius, r, 1, color)\n"
"    }\n"
"\n"
"    foreign static noiseSeed=(v)                                         // Set noise seed\n"
"    foreign static lineSpacing=(v)                                       // Set vertical line spacing for text\n"
"}\n"
"\n"
"class UI {\n"
"    foreign static update()\n"
"    foreign static draw()\n"
"    foreign static begin()\n"
"    foreign static end()\n"
"    foreign static beginWindow(title, x, y, width, height)\n"
"    foreign static endWindow()\n"
"    foreign static label(text)\n"
"    foreign static header(text, option)\n"
"    foreign static button(text)\n"
"    foreign static row(items, widths, height)\n"
"    foreign static textbox(text)\n"
"    foreign static getWindowInfo()\n"
"    foreign static setWindowSize(width, height)\n"
"    foreign static openPopup(name)\n"
"    foreign static beginPopup(name)\n"
"    foreign static endPopup()\n"
"    foreign static beginColumn()\n"
"    foreign static endColumn()\n"
"    foreign static slider(value, offset, min, max)\n"
"    foreign static next()\n"
"    foreign static drawRect(x, y, width, height, color)\n"
"    foreign static drawText(text, x, y, width, height)\n"
"    foreign static beginTreenode(text)\n"
"    foreign static endTreenode()\n"
"    foreign static checkbox(text, value, offset)\n"
"    foreign static text(text)\n"
"    foreign static beginPanel(name)\n"
"    foreign static endPanel()\n"
"    foreign static focus()\n"
"\n"
"    static header(text) { header(text, \"\") }\n"
"}\n"
"\n"
"foreign class Color {\n"
"    foreign construct new(r, g, b, a)                            // New color from RGBA\n"
"    foreign construct new(r, g, b)                               // New color from RGB\n"
"\n"
"    foreign [index]\n"
"    foreign [index]=(v)\n"
"\n"
"    r { this[0] }                                                // Get R value\n"
"    g { this[1] }                                                // Get G value\n"
"    b { this[2] }                                                // Get B value\n"
"    a { this[3] }                                                // Get A value\n"
"\n"
"    r=(v) { this[0] = v }                                        // Set R value\n"
"    g=(v) { this[1] = v }                                        // Set G value\n"
"    b=(v) { this[2] = v }                                        // Set B value\n"
"    a=(v) { this[3] = v }                                        // Set A value\n"
"\n"
"    toString { \"Color (r: %(r), g: %(g), b: %(b), a: %(a))\" }    // Get string representation\n"
"\n"
"    static none { new(0, 0, 0, 0) }                              // Default color palette\n"
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
"foreign class Image {\n"
"    foreign construct new(pathOrTexture)                                                    // Load image from file (PNG, BMP, JPG) or texture\n"
"    foreign construct new(width, height, color)                                             // New image\n"
"    foreign construct fromMemory(type, data)                                                // Load image from memory, type is: \".png\", \".bmp\", \".jpg\"\n"
"    foreign construct fromScreen()                                                          // New image from screen\n"
"    foreign construct fromImage(image, x, y, width, height)                                 // New image from another image piece\n"
"    foreign construct fromText(text, size, color)                                           // New image from text\n"
"    foreign construct fromGradientLinear(width, height, direction, startColor, endColor)    // New image from linear gradient, direction is in degrees\n"
"    foreign construct fromGradientRadial(width, height, density, innerColor, outerColor)    // New image from radial gradient\n"
"    foreign construct fromGradientSquare(width, height, density, innerColor, outerColor)    // New image from square gradient\n"
"\n"
"    foreign export(path)                                                                    // Save image to file, return true on success\n"
"    foreign exportToMemory(type)                                                            // Save image data to memory, type is: \".png\", \".bmp\", \".jpg\"\n"
"    foreign crop(x, y, width, height)                                                       // Crop image to new size\n"
"    foreign resize(width, height)                                                           // Resize image\n"
"    foreign flipVertical()                                                                  // Flip image vertically\n"
"    foreign flipHorizontal()                                                                // Flip image horizontally\n"
"    foreign rotate(angle)                                                                   // Rotate image by angle in degrees\n"
"\n"
"    foreign width                                                                           // Get image width\n"
"    foreign height                                                                          // Get image height\n"
"    foreign format                                                                          // Get image pixel format\n"
"}\n"
"\n"
"foreign class Texture {\n"
"    foreign construct new(pathOrImage)                                                        // Load texture from file (PNG, BMP, JPG) or image\n"
"\n"
"    foreign draw(x, y, r, sx, sy, ox, oy, color)                                              // Draw texture\n"
"    foreign drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)    // Draw part of texture\n"
"\n"
"    draw(x, y) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    draw(x, y, color) {\n"
"        draw(x, y, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {\n"
"        drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)\n"
"    }\n"
"\n"
"    drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {\n"
"        drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign width                                                                             // Get texture width\n"
"    foreign height                                                                            // Get texture height\n"
"    foreign filter=(v)                                                                        // Set texture filter (\"point\", \"bilinear\")\n"
"    foreign wrap=(v)                                                                          // Set texture wrap (\"repeat\", \"clamp\")\n"
"}\n"
"\n"
"foreign class RenderTexture {\n"
"    foreign construct new(width, height)    // New render texture\n"
"\n"
"    foreign begin()                         // Begin rendering to texture\n"
"    foreign end()                           // End rendering to texture\n"
"\n"
"    foreign texture                         // Get texture\n"
"}\n"
"\n"
"foreign class Font {\n"
"    foreign construct new(path, size)                              // Load font from file (TTF, OTF)\n"
"    foreign construct fromMemory(data, size)                       // Load font from memory\n"
"\n"
"    foreign print(text, x, y, r, scale, spacing, ox, oy, color)    // Draw text\n"
"    foreign measure(text)                                          // Measure text width\n"
"\n"
"    print(text, x, y, color) {\n"
"        print(text, x, y, 0, 1, 0, 0, 0, color)\n"
"    }\n"
"\n"
"    foreign size                                                   // Get font height\n"
"}\n"
"\n"
"foreign class Camera {\n"
"    foreign construct new(x, y)    // New camera\n"
"\n"
"    foreign begin()                // Begin camera mode\n"
"    foreign end()                  // End camera mode\n"
"    foreign screenToWorld(x, y)    // Get world space position from screen space (returns list with x and y)\n"
"    foreign worldToScreen(x, y)    // Get screen space position from world space (returns list with x and y)\n"
"\n"
"    foreign x                      // Get target x\n"
"    foreign y                      // Get target y\n"
"    foreign ox                     // Get offset x\n"
"    foreign oy                     // Get offset y\n"
"    foreign r                      // Get rotation\n"
"    foreign zoom                   // Get zoom\n"
"\n"
"    foreign x=(v)                  // Set target x\n"
"    foreign y=(v)                  // Set target y\n"
"    foreign ox=(v)                 // Set offset x\n"
"    foreign oy=(v)                 // Set offset y\n"
"    foreign r=(v)                  // Set rotation\n"
"    foreign zoom=(v)               // Set zoom\n"
"}\n"
"\n"
"foreign class Shader {\n"
"    foreign construct new(vs, fs)           // Load shader from vertex and fragment files\n"
"    foreign construct new(fs)               // Load shader from fragment file (uses default vertex shader)\n"
"    foreign construct fromMemory(vs, fs)    // Load shader from memory\n"
"    foreign construct fromMemory(fs)        // Load shader from memory (uses default vertex shader)\n"
"\n"
"    foreign begin()                         // Begin shader mode\n"
"    foreign end()                           // End shader mode\n"
"    foreign set(name, value)                // Set uniform value\n"
"}\n"
"\n"
"//------------------------------\n"
"// Input\n"
"//------------------------------\n"
"\n"
"class Keyboard {\n"
"    foreign static pressed(key)          // Check if key is pressed once\n"
"    foreign static pressedRepeat(key)    // Check if key is pressed again\n"
"    foreign static down(key)             // Check if key is being pressed\n"
"    foreign static released(key)         // Check if key is released once\n"
"\n"
"    foreign static keyPressed            // Get latest key pressed (keycode)\n"
"    foreign static charPressed           // Get latest character pressed (unicode)\n"
"}\n"
"\n"
"class Mouse {\n"
"    foreign static pressed(button)      // Check if mouse button is pressed once\n"
"    foreign static down(button)         // Check if mouse button is being pressed\n"
"    foreign static released(button)     // Check if mouse button is released once\n"
"    foreign static setPosition(x, y)    // Set mouse position\n"
"    foreign static setOffset(x, y)      // Set mouse offset\n"
"    foreign static setScale(x, y)       // Set mouse scale\n"
"\n"
"    foreign static x                    // Get mouse position x\n"
"    foreign static y                    // Get mouse position y\n"
"    foreign static dx                   // Get mouse delta x since last frame\n"
"    foreign static dy                   // Get mouse delta y since last frame\n"
"    foreign static wheel                // Get mouse wheel delta\n"
"    foreign static cursor=(v)           // Set mouse cursor (\"default\", \"arrow\", \"ibeam\", \"crosshair\", \"pointingHand\", \"resizeEW\", \"resizeNS\", \"resizeNWSE\", \"resizeNESW\", \"resizeAll\", \"notAllowed\")\n"
"    foreign static hidden               // Check if mouse cursor is not visible\n"
"    foreign static hidden=(v)           // Set mouse cursor visibility\n"
"    foreign static enabled=(v)          // Lock or unlock mouse cursor\n"
"    foreign static onScreen             // Check if mouse is on screen\n"
"}\n"
"\n"
"foreign class Gamepad {\n"
"    foreign static available(id)    // Check if gamepad is available\n"
"\n"
"    foreign construct new(id)       // New gamepad\n"
"\n"
"    foreign pressed(button)         // Check if gamepad button is pressed once\n"
"    foreign down(button)            // Check if gamepad button is being pressed\n"
"    foreign released(button)        // Check if gamepad button is released once\n"
"    foreign axis(axis)              // Get axis value\n"
"\n"
"    foreign id                      // Get gamepad id\n"
"    foreign name                    // Get gamepad name\n"
"    foreign axisCount               // Get gamepad axis count\n"
"}\n"
"\n"
"//------------------------------\n"
"// System\n"
"//------------------------------\n"
"\n"
"class Window {\n"
"    foreign static init(width, height, title)    // Initialize window\n"
"    foreign static toggleFullscreen()            // Toggle fullscreen mode\n"
"    foreign static toggleBorderless()            // Toggle borderless windowed mode\n"
"    foreign static maximize()                    // Maximize window\n"
"    foreign static minimize()                    // Minimize window\n"
"    foreign static restore()                     // Restore window from minimized or maximized state\n"
"    foreign static setPosition(x, y)             // Set window position\n"
"    foreign static setMinSize(width, height)     // Set minimum window dimensions\n"
"    foreign static setMaxSize(width, height)     // Set maximum window dimensions\n"
"    foreign static setSize(width, height)        // Set window dimensions\n"
"    foreign static focus()                       // Set window focus\n"
"    foreign static getMonitorInfo(monitor)       // Get monitor information (returns map)\n"
"    foreign static listDropped()                 // Get list of dropped files (check with fileDropped if any)\n"
"\n"
"    foreign static closed                        // Check if window should be closed\n"
"    foreign static fullscreen                    // Check if window is fullscreen\n"
"    foreign static hidden                        // Check if window is hidden\n"
"    foreign static minimized                     // Check if window is minimized\n"
"    foreign static maximized                     // Check if window is maximized\n"
"    foreign static focused                       // Check if window is focused\n"
"    foreign static resized                       // Check if window is resized last frame\n"
"    foreign static width                         // Get window width\n"
"    foreign static height                        // Get window height\n"
"    foreign static monitorCount                  // Get monitor count\n"
"    foreign static monitor                       // Get current monitor\n"
"    foreign static x                             // Get window position x\n"
"    foreign static y                             // Get window position y\n"
"    foreign static dpi                           // Get window dpi\n"
"    foreign static fileDropped                   // Check if any file has been dropped\n"
"    foreign static icon=(v)                      // Set window icon from image\n"
"    foreign static title=(v)                     // Set window title\n"
"    foreign static monitor=(v)                   // Set window monitor\n"
"    foreign static opacity=(v)                   // Set window opacity (0.0 = transparent, 1.0 = opaque)\n"
"    foreign static targetFps=(v)                 // Set target FPS\n"
"\n"
"    foreign static resizable                     // Check if window is resizable\n"
"    foreign static resizable=(v)                 // Set window resizable\n"
"    foreign static vsync                         // Check if vsync is enabled\n"
"    foreign static vsync=(v)                     // Enable or disable vsync\n"
"\n"
"    foreign static dt                            // Get delta time between frames\n"
"    foreign static time                          // Get time since window has opened\n"
"    foreign static fps                           // Get current FPS\n"
"}\n"
"\n"
"class OS {\n"
"    foreign static readLine()       // Read line from console\n"
"    foreign static wait(seconds)    // Halt program execution for given seconds\n"
"    foreign static openUrl(url)     // Open URL in default browser\n"
"\n"
"    foreign static args             // Get command line arguments list\n"
"    foreign static name             // Get operating system name (\"windows\", \"macos\", \"linux\")\n"
"    foreign static wrayVersion      // Get version of Wray\n"
"    foreign static clipboard        // Get clipboard text\n"
"    foreign static clipboard=(v)    // Set clipboard text\n"
"}\n"
"\n"
"class Data {\n"
"    foreign static compress(data)        // Compress data using deflate algorithm\n"
"    foreign static decompress(data)      // Decompress data using deflate algorithm\n"
"    foreign static encodeBase64(data)    // Encode data using base64\n"
"    foreign static decodeBase64(data)    // Decode data using base64\n"
"    foreign static encodeHex(data)       // Encode data using hex\n"
"    foreign static decodeHex(data)       // Decode data using hex\n"
"    foreign static hash(data)            // Hash data using SHA256\n"
"}\n"
"\n"
"class Directory {\n"
"    foreign static exists(path)    // Check if directory exists\n"
"    foreign static list(path)      // Get list of directory items\n"
"}\n"
"\n"
"class File {\n"
"    foreign static exists(path)          // Check if file exists\n"
"    foreign static size(path)            // Get file size\n"
"    foreign static read(path)            // Read data from file\n"
"    foreign static readEmbedded(path)    // Read data from egg file\n"
"    foreign static write(path, data)     // Write data to file\n"
"}\n"
"\n"
"foreign class Buffer {\n"
"    foreign construct new(size)           // Allocate buffer of given size in bytes\n"
"    foreign construct from(data)          // Create new buffer from: array of bytes, string or buffer\n"
"\n"
"    foreign fill(value)                   // Fill buffer with given value\n"
"\n"
"    foreign readInt8(offset)              // Read int8 from buffer at given offset\n"
"    foreign readUint8(offset)             // Read uint8 from buffer at given offset\n"
"    foreign readInt16(offset)             // Read int16 from buffer at given offset\n"
"    foreign readUint16(offset)            // Read uint16 from buffer at given offset\n"
"    foreign readInt32(offset)             // Read int32 from buffer at given offset\n"
"    foreign readUint32(offset)            // Read uint32 from buffer at given offset\n"
"    foreign readInt64(offset)             // Read int64 from buffer at given offset\n"
"    foreign readUint64(offset)            // Read uint64 from buffer at given offset\n"
"    foreign readFloat(offset)             // Read float from buffer at given offset\n"
"    foreign readDouble(offset)            // Read double from buffer at given offset\n"
"    foreign readBool(offset)              // Read bool from buffer at given offset\n"
"    foreign readString(size, offset)      // Read string of specified size from buffer at given offset\n"
"\n"
"    foreign writeInt8(value, offset)      // Write int8 to buffer at given offset, returns new offset\n"
"    foreign writeUint8(value, offset)     // Write uint8 to buffer at given offset, returns new offset\n"
"    foreign writeInt16(value, offset)     // Write int16 to buffer at given offset, returns new offset\n"
"    foreign writeUint16(value, offset)    // Write uint16 to buffer at given offset, returns new offset\n"
"    foreign writeInt32(value, offset)     // Write int32 to buffer at given offset, returns new offset\n"
"    foreign writeUint32(value, offset)    // Write uint32 to buffer at given offset, returns new offset\n"
"    foreign writeInt64(value, offset)     // Write int64 to buffer at given offset, returns new offset\n"
"    foreign writeUint64(value, offset)    // Write uint64 to buffer at given offset, returns new offset\n"
"    foreign writeFloat(value, offset)     // Write float to buffer at given offset, returns new offset\n"
"    foreign writeDouble(value, offset)    // Write double to buffer at given offset, returns new offset\n"
"    foreign writeBool(value, offset)      // Write bool to buffer at given offset, returns new offset\n"
"    foreign writeString(value, offset)    // Write string to buffer at given offset, returns new offset\n"
"\n"
"    foreign size                          // Get buffer size in bytes\n"
"    foreign toString                      // Convert buffer to string\n"
"    foreign toList                        // Convert buffer to list\n"
"\n"
"    foreign [index]\n"
"    foreign [index]=(v)\n"
"}\n"
"\n"
"foreign class Request {\n"
"    foreign construct new(url)    // New http request\n"
"\n"
"    foreign make()                // Make request (this method is async)\n"
"\n"
"    foreign complete              // Check if request is complete\n"
"    foreign status                // Get request status\n"
"    foreign body                  // Get request body\n"
"}\n"
"\n"
"//------------------------------\n"
"// ENet\n"
"//------------------------------\n"
"\n"
"class ENet {\n"
"    foreign static init()     // Initialize ENet\n"
"\n"
"    foreign static version    // Get ENet version\n"
"}\n"
"\n"
"// This is a port of lua-enet. It tries to be as close as possible while adapting the api to wren.\n"
"// Check out the documentation here: https://leafo.net/lua-enet/\n"
"\n"
"foreign class Host {\n"
"    foreign construct new(address, peerCount, channelCount, inBandwidth, outBandwidth)\n"
"    foreign construct new(address)\n"
"\n"
"    foreign connect(address, channelCount, data)\n"
"    foreign service(timeout)\n"
"    foreign checkEvents()\n"
"    foreign compressWithRangeCoder()\n"
"    foreign flush()\n"
"    foreign broadcast(data, channel, flag)\n"
"    foreign setBandwidthLimit(incoming, outgoing)\n"
"    foreign getPeer(index)\n"
"\n"
"    connect(address) { connect(address, 1, 0) }\n"
"\n"
"    service() { service(0) }\n"
"\n"
"    broadcast(data) {\n"
"        broadcast(data, 0, \"reliable\")\n"
"    }\n"
"\n"
"    foreign totalSent\n"
"    foreign totalReceived\n"
"    foreign serviceTime\n"
"    foreign peerCount\n"
"    foreign address\n"
"    foreign channelLimit=(v)\n"
"}\n"
"\n"
"foreign class Peer {\n"
"    foreign disconnect(data)\n"
"    foreign disconnectNow(data)\n"
"    foreign disconnectLater(data)\n"
"    foreign ping()\n"
"    foreign reset()\n"
"    foreign send(data, channel, flag)\n"
"    foreign receive()\n"
"    foreign configThrottle(interval, acceleration, deceleration)\n"
"    foreign setTimeout(limit, minimum, maximum)\n"
"\n"
"    disconnect() {\n"
"        disconnect(0)\n"
"    }\n"
"\n"
"    disconnectNow() {\n"
"        disconnectNow(0)\n"
"    }\n"
"\n"
"    disconnectLater() {\n"
"        disconnectLater(0)\n"
"    }\n"
"\n"
"    send(data) {\n"
"        send(data, 0, \"reliable\")\n"
"    }\n"
"\n"
"    foreign connectId\n"
"    foreign index\n"
"    foreign state\n"
"    foreign rtt\n"
"    foreign lastRtt\n"
"    foreign timeout\n"
"    foreign toString\n"
"    foreign pingInterval=(v)\n"
"    foreign rtt=(v)\n"
"    foreign lastRtt=(v)\n"
"}\n";
