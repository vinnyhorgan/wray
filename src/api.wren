class Audio {
    foreign static init()
    foreign static close()

    foreign static volume
    foreign static volume=(v)
}

foreign class Sound {
    foreign construct new(path)

    foreign play()
    foreign stop()
    foreign pause()
    foreign resume()

    foreign playing
    foreign volume=(v)
    foreign pitch=(v)
    foreign pan=(v)
}

foreign class Font {
    foreign construct new(path, size)

    foreign print(text, x, y, r, ox, oy, color)

    print(text, x, y, color) {
        print(text, x, y, 0, 0, 0, color)
    }

    foreign measure(text)

    foreign size
}

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

class Graphics {
    foreign static begin()
    foreign static end()
    foreign static beginBlend(mode)
    foreign static endBlend()
    foreign static beginScissor(x, y, width, height)
    foreign static endScissor()
    foreign static clear(color)
    foreign static print(text, x, y, size, color)
    foreign static takeScreenshot(path)

    foreign static pixel(x, y, color)
    foreign static line(x1, y1, x2, y2, thick, color)
    foreign static circle(x, y, radius, color)
    foreign static circleLine(x, y, radius, color)
    foreign static ellipse(x, y, rx, ry, color)
    foreign static ellipseLine(x, y, rx, ry, color)
    foreign static rectangle(x, y, width, height, ox, oy, r, color)
    foreign static rectangleLine(x, y, width, height, thick, color)
    foreign static triangle(x1, y1, x2, y2, x3, y3, color)
    foreign static triangleLine(x1, y1, x2, y2, x3, y3, color)
    foreign static polygon(x, y, sides, radius, r, color)
    foreign static polygonLine(x, y, sides, radius, r, thick, color)

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

    foreign static noise(x, y, freq, depth)
    foreign static noiseSeed=(v)
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

foreign class Request {
    foreign construct new(url)

    foreign make()

    foreign complete
    foreign status
    foreign body
}

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

foreign class Color {
    foreign construct new(r, g, b, a)
    foreign construct new(r, g, b)

    foreign [index]
    foreign [index]=(v)

    r { this[0] }
    g { this[1] }
    b { this[2] }
    a { this[3] }

    r=(v) { this[0] = v }
    g=(v) { this[0] = v }
    b=(v) { this[0] = v }
    a=(v) { this[0] = v }

    toString { "Color (r: %(r), g: %(g), b: %(b), a: %(a))" }

    static none { new(0, 0, 0, 0) }
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
    foreign construct new(path)

    foreign draw(x, y, r, sx, sy, ox, oy, color)

    draw(x, y) {
        draw(x, y, 0, 1, 1, 0, 0, Color.white)
    }

    draw(x, y, color) {
        draw(x, y, 0, 1, 1, 0, 0, color)
    }

    foreign drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)

    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {
        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)
    }

    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {
        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)
    }

    foreign width
    foreign height
    foreign filter=(v)
    foreign wrap=(v)
}

foreign class RenderTexture {
    foreign construct new(width, height)

    foreign begin()
    foreign end()

    foreign draw(x, y, r, sx, sy, ox, oy, color)

    draw(x, y) {
        draw(x, y, 0, 1, 1, 0, 0, Color.white)
    }

    draw(x, y, color) {
        draw(x, y, 0, 1, 1, 0, 0, color)
    }

    foreign drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, r, sx, sy, ox, oy, color)

    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY) {
        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, Color.white)
    }

    drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, color) {
        drawRect(srcX, srcY, srcWidth, srcHeight, dstX, dstY, 0, 1, 1, 0, 0, color)
    }

    foreign width
    foreign height
    foreign filter=(v)
    foreign wrap=(v)
}

foreign class Camera {
    foreign construct new(x, y)

    foreign begin()
    foreign end()

    foreign x
    foreign x=(v)
    foreign y
    foreign y=(v)
    foreign ox
    foreign ox=(v)
    foreign oy
    foreign oy=(v)
    foreign r
    foreign r=(v)
    foreign zoom
    foreign zoom=(v)
}

foreign class Shader {
    foreign construct new(vs, fs)
    foreign construct new(fs)

    foreign begin()
    foreign end()
    foreign set(name, value)
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
