//------------------------------
// Audio
//------------------------------

class Audio {
    foreign static init()        // Initialize audio device

    foreign static volume        // Get master volume
    foreign static volume=(v)    // Set master volume
}

foreign class Sound {
    foreign construct new(path)    // Load sound from file (WAV, OGG, MP3, FLAC)

    foreign play()                 // Play sound
    foreign stop()                 // Stop playing sound
    foreign pause()                // Pause sound
    foreign resume()               // Resume paused sound

    foreign playing                // Check if sound is playing
    foreign volume=(v)             // Set sound volume (1.0 = max volume)
    foreign pitch=(v)              // Set sound pitch (1.0 = normal)
    foreign pan=(v)                // Set sound pan (0.5 = middle)
}

foreign class SoundAlias {
    foreign construct new(sound)    // Load sound alias from sound, it shares the same sound data but does not own it

    foreign play()                  // Play sound
    foreign stop()                  // Stop playing sound
    foreign pause()                 // Pause sound
    foreign resume()                // Resume paused sound

    foreign playing                 // Check if sound is playing
    foreign volume=(v)              // Set sound volume (1.0 = max volume)
    foreign pitch=(v)               // Set sound pitch (1.0 = normal)
    foreign pan=(v)                 // Set sound pan (0.5 = middle)
}

//------------------------------
// Graphics
//------------------------------

class Graphics {
    foreign static begin()                                               // Begin rendering to screen
    foreign static end()                                                 // End rendering to screen
    foreign static beginBlend(mode)                                      // Begin blending mode ("alpha", "additive", "multiplied", "addColors", "subtractColors", "alphaPremultiply")
    foreign static endBlend()                                            // End blending mode (returns to "alpha")
    foreign static beginScissor(x, y, width, height)                     // Begin scissor mode
    foreign static endScissor()                                          // End scissor mode
    foreign static screenshot(path)                                      // Save screenshot to file
    foreign static measure(text, size)                                   // Measure text width using default font
    foreign static noise(x, y, frequency, depth)                         // Get perlin noise value

    foreign static clear(color)                                          // Clear the screen
    foreign static print(text, x, y, size, color)                        // Draw text using default font
    foreign static pixel(x, y, color)                                    // Draw pixel
    foreign static line(x1, y1, x2, y2, thick, color)                    // Draw line
    foreign static lineBezier(x1, y1, x2, y2, thick, color)              // Draw bezier curve
    foreign static circle(x, y, radius, color)                           // Draw circle
    foreign static circleLines(x, y, radius, color)                      // Draw circle outline
    foreign static ellipse(x, y, rx, ry, color)                          // Draw ellipse
    foreign static ellipseLines(x, y, rx, ry, color)                     // Draw ellipse outline
    foreign static rectangle(x, y, width, height, r, ox, oy, color)      // Draw rectangle
    foreign static rectangleLines(x, y, width, height, thick, color)     // Draw rectangle outline
    foreign static triangle(x1, y1, x2, y2, x3, y3, color)               // Draw triangle
    foreign static triangleLines(x1, y1, x2, y2, x3, y3, color)          // Draw triangle outline
    foreign static polygon(x, y, sides, radius, r, color)                // Draw polygon
    foreign static polygonLines(x, y, sides, radius, r, thick, color)    // Draw polygon outline

    static line(x1, y1, x2, y2, color) {
        line(x1, y1, x2, y2, 1, color)
    }

    static lineBezier(x1, y1, x2, y2, color) {
        lineBezier(x1, y1, x2, y2, 1, color)
    }

    static rectangle(x, y, width, height, color) {
        rectangle(x, y, width, height, 0, 0, 0, color)
    }

    static rectangleLines(x, y, width, height, color) {
        rectangleLines(x, y, width, height, 1, color)
    }

    static polygonLines(x, y, sides, radius, r, color) {
        polygonLines(x, y, sides, radius, r, 1, color)
    }

    foreign static noiseSeed=(v)                                         // Set noise seed
    foreign static lineSpacing=(v)                                       // Set vertical line spacing for text
}

class UI {
    foreign static update()
    foreign static draw()
    foreign static begin()
    foreign static end()
    foreign static beginWindow(title, x, y, width, height)
    foreign static endWindow()
    foreign static label(text)
    foreign static header(text, option)
    foreign static button(text)
    foreign static row(items, widths, height)
    foreign static textbox(text)
    foreign static getWindowInfo()
    foreign static setWindowSize(width, height)
    foreign static openPopup(name)
    foreign static beginPopup(name)
    foreign static endPopup()
    foreign static beginColumn()
    foreign static endColumn()
    foreign static slider(value, offset, min, max)
    foreign static next()
    foreign static drawRect(x, y, width, height, color)
    foreign static drawText(text, x, y, width, height)
    foreign static beginTreenode(text)
    foreign static endTreenode()
    foreign static checkbox(text, value, offset)
    foreign static text(text)
    foreign static beginPanel(name)
    foreign static endPanel()
    foreign static focus()

    static header(text) { header(text, "") }
}

foreign class Color {
    foreign construct new(r, g, b, a)                            // New color from RGBA
    foreign construct new(r, g, b)                               // New color from RGB

    foreign [index]
    foreign [index]=(v)

    r { this[0] }                                                // Get R value
    g { this[1] }                                                // Get G value
    b { this[2] }                                                // Get B value
    a { this[3] }                                                // Get A value

    r=(v) { this[0] = v }                                        // Set R value
    g=(v) { this[1] = v }                                        // Set G value
    b=(v) { this[2] = v }                                        // Set B value
    a=(v) { this[3] = v }                                        // Set A value

    toString { "Color (r: %(r), g: %(g), b: %(b), a: %(a))" }    // Get string representation

    static none { new(0, 0, 0, 0) }                              // Default color palette
    static black { new(0, 0, 0) }
    static darkBlue { new(29, 43, 83) }
    static darkPurple { new(126, 37, 83) }
    static darkGreen { new(0, 135, 81) }
    static brown { new(171, 82, 54) }
    static darkGray { new(95, 87, 79) }
    static lightGray { new(194, 195, 199) }
    static white { new(255, 241, 232) }
    static red { new(255, 0, 77) }
    static orange { new(255, 163, 0) }
    static yellow { new(255, 236, 39) }
    static green { new(0, 228, 54) }
    static blue { new(41, 173, 255) }
    static indigo { new(131, 118, 156) }
    static pink { new(255, 119, 168) }
    static peach { new(255, 204, 170) }
}

foreign class Image {
    foreign construct new(pathOrTexture)                                                    // Load image from file (PNG, BMP, JPG) or texture
    foreign construct new(width, height, color)                                             // New image
    foreign construct fromMemory(type, data)                                                // Load image from memory, type is: ".png", ".bmp", ".jpg"
    foreign construct fromScreen()                                                          // New image from screen
    foreign construct fromImage(image, x, y, width, height)                                 // New image from another image piece
    foreign construct fromText(text, size, color)                                           // New image from text
    foreign construct fromGradientLinear(width, height, direction, startColor, endColor)    // New image from linear gradient, direction is in degrees
    foreign construct fromGradientRadial(width, height, density, innerColor, outerColor)    // New image from radial gradient
    foreign construct fromGradientSquare(width, height, density, innerColor, outerColor)    // New image from square gradient

    foreign export(path)                                                                    // Save image to file, return true on success
    foreign exportToMemory(type)                                                            // Save image data to memory, type is: ".png", ".bmp", ".jpg"
    foreign crop(x, y, width, height)                                                       // Crop image to new size
    foreign resize(width, height)                                                           // Resize image
    foreign flipVertical()                                                                  // Flip image vertically
    foreign flipHorizontal()                                                                // Flip image horizontally
    foreign rotate(angle)                                                                   // Rotate image by angle in degrees

    foreign width                                                                           // Get image width
    foreign height                                                                          // Get image height
    foreign format                                                                          // Get image pixel format
}

foreign class Texture {
    foreign construct new(pathOrImage)                                                        // Load texture from file (PNG, BMP, JPG) or image

    foreign draw(x, y, r, sx, sy, ox, oy, color)                                              // Draw texture
    foreign drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)    // Draw part of texture

    draw(x, y) {
        draw(x, y, 0, 1, 1, 0, 0, Color.white)
    }

    draw(x, y, color) {
        draw(x, y, 0, 1, 1, 0, 0, color)
    }

    drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {
        drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)
    }

    drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {
        drawRec(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)
    }

    foreign width                                                                             // Get texture width
    foreign height                                                                            // Get texture height
    foreign filter=(v)                                                                        // Set texture filter ("point", "bilinear")
    foreign wrap=(v)                                                                          // Set texture wrap ("repeat", "clamp")
}

foreign class RenderTexture {
    foreign construct new(width, height)    // New render texture

    foreign begin()                         // Begin rendering to texture
    foreign end()                           // End rendering to texture

    foreign texture                         // Get texture
}

foreign class Font {
    foreign construct new(path, size)                              // Load font from file (TTF, OTF)
    foreign construct fromMemory(data, size)                       // Load font from memory

    foreign print(text, x, y, r, scale, spacing, ox, oy, color)    // Draw text
    foreign measure(text)                                          // Measure text width

    print(text, x, y, color) {
        print(text, x, y, 0, 1, 0, 0, 0, color)
    }

    foreign size                                                   // Get font height
}

foreign class Camera {
    foreign construct new(x, y)    // New camera

    foreign begin()                // Begin camera mode
    foreign end()                  // End camera mode
    foreign screenToWorld(x, y)    // Get world space position from screen space (returns list with x and y)
    foreign worldToScreen(x, y)    // Get screen space position from world space (returns list with x and y)

    foreign x                      // Get target x
    foreign y                      // Get target y
    foreign ox                     // Get offset x
    foreign oy                     // Get offset y
    foreign r                      // Get rotation
    foreign zoom                   // Get zoom

    foreign x=(v)                  // Set target x
    foreign y=(v)                  // Set target y
    foreign ox=(v)                 // Set offset x
    foreign oy=(v)                 // Set offset y
    foreign r=(v)                  // Set rotation
    foreign zoom=(v)               // Set zoom
}

foreign class Shader {
    foreign construct new(vs, fs)           // Load shader from vertex and fragment files
    foreign construct new(fs)               // Load shader from fragment file (uses default vertex shader)
    foreign construct fromMemory(vs, fs)    // Load shader from memory
    foreign construct fromMemory(fs)        // Load shader from memory (uses default vertex shader)

    foreign begin()                         // Begin shader mode
    foreign end()                           // End shader mode
    foreign set(name, value)                // Set uniform value
}

//------------------------------
// Input
//------------------------------

class Keyboard {
    foreign static pressed(key)          // Check if key is pressed once
    foreign static pressedRepeat(key)    // Check if key is pressed again
    foreign static down(key)             // Check if key is being pressed
    foreign static released(key)         // Check if key is released once

    foreign static keyPressed            // Get latest key pressed (keycode)
    foreign static charPressed           // Get latest character pressed (unicode)
}

class Mouse {
    foreign static pressed(button)      // Check if mouse button is pressed once
    foreign static down(button)         // Check if mouse button is being pressed
    foreign static released(button)     // Check if mouse button is released once
    foreign static setPosition(x, y)    // Set mouse position
    foreign static setOffset(x, y)      // Set mouse offset
    foreign static setScale(x, y)       // Set mouse scale

    foreign static x                    // Get mouse position x
    foreign static y                    // Get mouse position y
    foreign static dx                   // Get mouse delta x since last frame
    foreign static dy                   // Get mouse delta y since last frame
    foreign static wheel                // Get mouse wheel delta
    foreign static cursor=(v)           // Set mouse cursor ("default", "arrow", "ibeam", "crosshair", "pointingHand", "resizeEW", "resizeNS", "resizeNWSE", "resizeNESW", "resizeAll", "notAllowed")
    foreign static hidden               // Check if mouse cursor is not visible
    foreign static hidden=(v)           // Set mouse cursor visibility
    foreign static enabled=(v)          // Lock or unlock mouse cursor
    foreign static onScreen             // Check if mouse is on screen
}

foreign class Gamepad {
    foreign static available(id)    // Check if gamepad is available

    foreign construct new(id)       // New gamepad

    foreign pressed(button)         // Check if gamepad button is pressed once
    foreign down(button)            // Check if gamepad button is being pressed
    foreign released(button)        // Check if gamepad button is released once
    foreign axis(axis)              // Get axis value

    foreign id                      // Get gamepad id
    foreign name                    // Get gamepad name
    foreign axisCount               // Get gamepad axis count
}

//------------------------------
// System
//------------------------------

class Window {
    foreign static init(width, height, title)    // Initialize window
    foreign static toggleFullscreen()            // Toggle fullscreen mode
    foreign static toggleBorderless()            // Toggle borderless windowed mode
    foreign static maximize()                    // Maximize window
    foreign static minimize()                    // Minimize window
    foreign static restore()                     // Restore window from minimized or maximized state
    foreign static setPosition(x, y)             // Set window position
    foreign static setMinSize(width, height)     // Set minimum window dimensions
    foreign static setMaxSize(width, height)     // Set maximum window dimensions
    foreign static setSize(width, height)        // Set window dimensions
    foreign static focus()                       // Set window focus
    foreign static getMonitorInfo(monitor)       // Get monitor information (returns map)
    foreign static listDropped()                 // Get list of dropped files (check with fileDropped if any)

    foreign static closed                        // Check if window should be closed
    foreign static fullscreen                    // Check if window is fullscreen
    foreign static hidden                        // Check if window is hidden
    foreign static minimized                     // Check if window is minimized
    foreign static maximized                     // Check if window is maximized
    foreign static focused                       // Check if window is focused
    foreign static resized                       // Check if window is resized last frame
    foreign static width                         // Get window width
    foreign static height                        // Get window height
    foreign static monitorCount                  // Get monitor count
    foreign static monitor                       // Get current monitor
    foreign static x                             // Get window position x
    foreign static y                             // Get window position y
    foreign static dpi                           // Get window dpi
    foreign static fileDropped                   // Check if any file has been dropped
    foreign static icon=(v)                      // Set window icon from image
    foreign static title=(v)                     // Set window title
    foreign static monitor=(v)                   // Set window monitor
    foreign static opacity=(v)                   // Set window opacity (0.0 = transparent, 1.0 = opaque)
    foreign static targetFps=(v)                 // Set target FPS

    foreign static resizable                     // Check if window is resizable
    foreign static resizable=(v)                 // Set window resizable
    foreign static vsync                         // Check if vsync is enabled
    foreign static vsync=(v)                     // Enable or disable vsync

    foreign static dt                            // Get delta time between frames
    foreign static time                          // Get time since window has opened
    foreign static fps                           // Get current FPS
}

class OS {
    foreign static readLine()       // Read line from console
    foreign static wait(seconds)    // Halt program execution for given seconds
    foreign static openUrl(url)     // Open URL in default browser

    foreign static args             // Get command line arguments list
    foreign static name             // Get operating system name ("windows", "macos", "linux")
    foreign static wrayVersion      // Get version of Wray
    foreign static clipboard        // Get clipboard text
    foreign static clipboard=(v)    // Set clipboard text
}

class Data {
    foreign static compress(data)        // Compress data using deflate algorithm
    foreign static decompress(data)      // Decompress data using deflate algorithm
    foreign static encodeBase64(data)    // Encode data using base64
    foreign static decodeBase64(data)    // Decode data using base64
    foreign static encodeHex(data)       // Encode data using hex
    foreign static decodeHex(data)       // Decode data using hex
    foreign static hash(data)            // Hash data using SHA256
}

class Directory {
    foreign static exists(path)    // Check if directory exists
    foreign static list(path)      // Get list of directory items
}

class File {
    foreign static exists(path)          // Check if file exists
    foreign static size(path)            // Get file size
    foreign static read(path)            // Read data from file
    foreign static readEmbedded(path)    // Read data from egg file
    foreign static write(path, data)     // Write data to file
}

foreign class Buffer {
    foreign construct new(size)           // Allocate buffer of given size in bytes
    foreign construct from(data)          // Create new buffer from: array of bytes, string or buffer

    foreign fill(value)                   // Fill buffer with given value

    foreign readInt8(offset)              // Read int8 from buffer at given offset
    foreign readUint8(offset)             // Read uint8 from buffer at given offset
    foreign readInt16(offset)             // Read int16 from buffer at given offset
    foreign readUint16(offset)            // Read uint16 from buffer at given offset
    foreign readInt32(offset)             // Read int32 from buffer at given offset
    foreign readUint32(offset)            // Read uint32 from buffer at given offset
    foreign readInt64(offset)             // Read int64 from buffer at given offset
    foreign readUint64(offset)            // Read uint64 from buffer at given offset
    foreign readFloat(offset)             // Read float from buffer at given offset
    foreign readDouble(offset)            // Read double from buffer at given offset
    foreign readBool(offset)              // Read bool from buffer at given offset
    foreign readString(size, offset)      // Read string of specified size from buffer at given offset

    foreign writeInt8(value, offset)      // Write int8 to buffer at given offset, returns new offset
    foreign writeUint8(value, offset)     // Write uint8 to buffer at given offset, returns new offset
    foreign writeInt16(value, offset)     // Write int16 to buffer at given offset, returns new offset
    foreign writeUint16(value, offset)    // Write uint16 to buffer at given offset, returns new offset
    foreign writeInt32(value, offset)     // Write int32 to buffer at given offset, returns new offset
    foreign writeUint32(value, offset)    // Write uint32 to buffer at given offset, returns new offset
    foreign writeInt64(value, offset)     // Write int64 to buffer at given offset, returns new offset
    foreign writeUint64(value, offset)    // Write uint64 to buffer at given offset, returns new offset
    foreign writeFloat(value, offset)     // Write float to buffer at given offset, returns new offset
    foreign writeDouble(value, offset)    // Write double to buffer at given offset, returns new offset
    foreign writeBool(value, offset)      // Write bool to buffer at given offset, returns new offset
    foreign writeString(value, offset)    // Write string to buffer at given offset, returns new offset

    foreign size                          // Get buffer size in bytes
    foreign toString                      // Convert buffer to string
    foreign toList                        // Convert buffer to list

    foreign [index]
    foreign [index]=(v)
}

foreign class Request {
    foreign construct new(url)    // New http request

    foreign make()                // Make request (this method is async)

    foreign complete              // Check if request is complete
    foreign status                // Get request status
    foreign body                  // Get request body
}

//------------------------------
// ENet
//------------------------------

class ENet {
    foreign static init()     // Initialize ENet

    foreign static version    // Get ENet version
}

// This is a port of lua-enet. It tries to be as close as possible while adapting the api to wren.
// Check out the documentation here: https://leafo.net/lua-enet/

foreign class Host {
    foreign construct new(address, peerCount, channelCount, inBandwidth, outBandwidth)
    foreign construct new(address)

    foreign connect(address, channelCount, data)
    foreign service(timeout)
    foreign checkEvents()
    foreign compressWithRangeCoder()
    foreign flush()
    foreign broadcast(data, channel, flag)
    foreign setBandwidthLimit(incoming, outgoing)
    foreign getPeer(index)

    connect(address) { connect(address, 1, 0) }

    service() { service(0) }

    broadcast(data) {
        broadcast(data, 0, "reliable")
    }

    foreign totalSent
    foreign totalReceived
    foreign serviceTime
    foreign peerCount
    foreign address
    foreign channelLimit=(v)
}

foreign class Peer {
    foreign disconnect(data)
    foreign disconnectNow(data)
    foreign disconnectLater(data)
    foreign ping()
    foreign reset()
    foreign send(data, channel, flag)
    foreign receive()
    foreign configThrottle(interval, acceleration, deceleration)
    foreign setTimeout(limit, minimum, maximum)

    disconnect() {
        disconnect(0)
    }

    disconnectNow() {
        disconnectNow(0)
    }

    disconnectLater() {
        disconnectLater(0)
    }

    send(data) {
        send(data, 0, "reliable")
    }

    foreign connectId
    foreign index
    foreign state
    foreign rtt
    foreign lastRtt
    foreign timeout
    foreign toString
    foreign pingInterval=(v)
    foreign rtt=(v)
    foreign lastRtt=(v)
}
