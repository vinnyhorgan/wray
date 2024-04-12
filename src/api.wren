//------------------------------
// Audio
//------------------------------

class Audio {
    foreign static init()        // Initialize audio device
    foreign static close()       // Close audio device

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
    foreign static print(text, x, y, size, color)                        // Draw text
    foreign static pixel(x, y, color)                                    // Draw pixel
    foreign static line(x1, y1, x2, y2, thick, color)                    // Draw line
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

    static rectangle(x, y, width, height, color) {
        rectangle(x, y, width, height, 0, 0, 0, color)
    }

    static rectangleLine(x, y, width, height, color) {
        rectangleLine(x, y, width, height, 1, color)
    }

    static polygonLine(x, y, sides, radius, r, color) {
        polygonLine(x, y, sides, radius, r, 1, color)
    }

    foreign static noiseSeed=(v)                                         // Set noise seed
    foreign static lineSpacing=(v)                                       // Set vertical line spacing for text
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
    foreign construct new(path)                                                               // Load texture from file (PNG, BMP, JPG)

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
    foreign construct new(width, height)                                                      // New render texture

    foreign begin()                                                                           // Begin rendering to texture
    foreign end()                                                                             // End rendering to texture
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

class Mouse {
    foreign static down(button)
    foreign static pressed(button)
    foreign static released(button)
    foreign static setPosition(x, y)
    foreign static setOffset(x, y)
    foreign static setScale(x, y)

    foreign static x
    foreign static y
    foreign static dx
    foreign static dy
    foreign static wheel
    foreign static cursor=(v)
    foreign static hidden
    foreign static hidden=(v)
    foreign static enabled=(v)
    foreign static onScreen
}

class Keyboard {
    foreign static down(key)
    foreign static pressed(key)
    foreign static pressedRepeat(key)
    foreign static released(key)

    foreign static keyPressed
}

foreign class Gamepad {
    foreign static available(id)

    foreign construct new(id)

    foreign down(button)
    foreign pressed(button)
    foreign released(button)
    foreign axis(axis)

    foreign id
    foreign name
    foreign axisCount
}

//------------------------------
// System
//------------------------------

class Window {
    foreign static init(width, height, title)
    foreign static close()
    foreign static toggleFullscreen()
    foreign static maximize()
    foreign static minimize()
    foreign static restore()
    foreign static setPosition(x, y)
    foreign static setMinSize(width, height)
    foreign static setMaxSize(width, height)
    foreign static setSize(width, height)
    foreign static focus()

    foreign static closed
    foreign static fullscreen
    foreign static hidden
    foreign static minimized
    foreign static maximized
    foreign static focused
    foreign static resized
    foreign static title=(v)
    foreign static width
    foreign static height
    foreign static x
    foreign static y
    foreign static dpi

    foreign static resizable
    foreign static resizable=(v)
    foreign static vsync
    foreign static vsync=(v)

    foreign static targetFps=(v)
    foreign static dt
    foreign static time
    foreign static fps
}

foreign class Request {
    foreign construct new(url)

    foreign make()

    foreign complete
    foreign status
    foreign body
}

class OS {
    foreign static openUrl(url)
    foreign static readLine()

    foreign static args
    foreign static name
    foreign static clipboard
    foreign static clipboard=(v)
}

class Directory {
    foreign static exists(path)
    foreign static list(path)
}

class File {
    foreign static exists(path)
    foreign static write(path, data)
    foreign static read(path)
    foreign static size(path)
}
