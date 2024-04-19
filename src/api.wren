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

//------------------------------
// Graphics
//------------------------------

class Graphics {
    foreign static begin()                                                                                    // Begin rendering to screen
    foreign static end()                                                                                      // End rendering to screen
    foreign static beginBlend(mode)                                                                           // Begin blending mode ("alpha", "additive", "multiplied", "addColors", "subtractColors", "alphaPremultiply")
    foreign static endBlend()                                                                                 // End blending mode (returns to "alpha")
    foreign static beginScissor(x, y, width, height)                                                          // Begin scissor mode
    foreign static endScissor()                                                                               // End scissor mode
    foreign static screenshot(path)                                                                           // Save screenshot to file
    foreign static measure(text, size)                                                                        // Measure text width using default font
    foreign static noise(x, y, frequency, depth)                                                              // Get perlin noise value

    foreign static clear(color)                                                                               // Clear the screen
    foreign static print(text, x, y, size, color)                                                             // Draw text
    foreign static pixel(x, y, color)                                                                         // Draw pixel
    foreign static line(x1, y1, x2, y2, thick, color)                                                         // Draw line
    foreign static circle(x, y, radius, color)                                                                // Draw circle
    foreign static circleLines(x, y, radius, color)                                                           // Draw circle outline
    foreign static ellipse(x, y, rx, ry, color)                                                               // Draw ellipse
    foreign static ellipseLines(x, y, rx, ry, color)                                                          // Draw ellipse outline
    foreign static rectangle(x, y, width, height, r, ox, oy, color)                                           // Draw rectangle
    foreign static rectangleLines(x, y, width, height, thick, color)                                          // Draw rectangle outline
    foreign static triangle(x1, y1, x2, y2, x3, y3, color)                                                    // Draw triangle
    foreign static triangleLines(x1, y1, x2, y2, x3, y3, color)                                               // Draw triangle outline
    foreign static polygon(x, y, sides, radius, r, color)                                                     // Draw polygon
    foreign static polygonLines(x, y, sides, radius, r, thick, color)                                         // Draw polygon outline
    foreign static draw(texture, x, y, r, sx, sy, ox, oy, color)                                              // Draw texture
    foreign static drawRec(texture, srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)    // Draw part of texture

    static line(x1, y1, x2, y2, color) {
        line(x1, y1, x2, y2, 1, color)
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

    static draw(texture, x, y) {
        draw(texture, x, y, 0, 1, 1, 0, 0, Color.white)
    }

    static draw(texture, x, y, color) {
        draw(texture, x, y, 0, 1, 1, 0, 0, color)
    }

    static drawRec(texture, srcX, srcY, srcWidth, srcHeight, dstX, dstY) {
        drawRec(texture, srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)
    }

    static drawRec(texture, srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {
        drawRec(texture, srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)
    }

    foreign static noiseSeed=(v)                                                                              // Set noise seed
    foreign static lineSpacing=(v)                                                                            // Set vertical line spacing for text
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

foreign class Texture {
    foreign construct new(path)    // Load texture from file (PNG, BMP, JPG)

    foreign width                  // Get texture width
    foreign height                 // Get texture height
    foreign filter=(v)             // Set texture filter ("point", "bilinear")
    foreign wrap=(v)               // Set texture wrap ("repeat", "clamp")
}

foreign class RenderTexture {
    foreign construct new(width, height)    // New render texture

    foreign begin()                         // Begin rendering to texture
    foreign end()                           // End rendering to texture

    foreign texture                         // Get texture
}

foreign class Font {
    foreign construct new(path, size)              // Load font from file (FNT, TTF, OTF)

    foreign print(text, x, y, r, ox, oy, color)    // Draw text
    foreign measure(text)                          // Measure text width

    print(text, x, y, color) {
        print(text, x, y, 0, 0, 0, color)
    }

    foreign size                                   // Get font height
}

foreign class Camera {
    foreign construct new(x, y)    // New camera

    foreign begin()                // Begin camera mode
    foreign end()                  // End camera mode
    foreign screenToWorld(x, y)    // Get world space position from screen space (returns map with "x" and "y")
    foreign worldToScreen(x, y)    // Get screen space position from world space (returns map with "x" and "y")

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
    foreign construct new(vs, fs)    // Load shader from vertex and fragment files
    foreign construct new(fs)        // Load shader from fragment file (uses default vertex shader)

    foreign begin()                  // Begin shader mode
    foreign end()                    // End shader mode
    foreign set(name, value)         // Set uniform value
}

//------------------------------
// Input
//------------------------------

class Keyboard {
    foreign static pressed(key)          // Check if key is pressed once
    foreign static pressedRepeat(key)    // Check if key is pressed again
    foreign static down(key)             // Check if key is being pressed
    foreign static released(key)         // Check if key is released once

    foreign static keyPressed            // Get latest key pressed
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
    foreign static maximize()                    // Maximize window
    foreign static minimize()                    // Minimize window
    foreign static restore()                     // Restore window from minimized or maximized state
    foreign static setPosition(x, y)             // Set window position
    foreign static setMinSize(width, height)     // Set minimum window dimensions
    foreign static setMaxSize(width, height)     // Set maximum window dimensions
    foreign static setSize(width, height)        // Set window dimensions
    foreign static focus()                       // Set window focus
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
    foreign static x                             // Get window position x
    foreign static y                             // Get window position y
    foreign static dpi                           // Get window dpi
    foreign static fileDropped                   // Check if any file has been dropped
    foreign static title=(v)                     // Set window title
    foreign static icon=(v)                      // Set window icon from texture
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
    foreign static clipboard        // Get clipboard text
    foreign static clipboard=(v)    // Set clipboard text
}

class Directory {
    foreign static exists(path)    // Check if directory exists
    foreign static list(path)      // Get list of directory items
}

class File {
    foreign static exists(path)         // Check if file exists
    foreign static size(path)           // Get file size
    foreign static read(path)           // Read data from file
    foreign static write(path, data)    // Write data to file
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
    foreign static init()
    foreign static close()

    foreign static version
}

foreign class Host {
    foreign construct new()
    foreign construct new(port)

    foreign service(timeout)
    foreign connect(address, port)
}

foreign class Peer {
    foreign disconnect()
    foreign send(data)

    foreign toString
}
