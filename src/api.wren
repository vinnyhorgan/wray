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

class Window {
    foreign static init(width, height, title)
    foreign static close()

    foreign static closed
    foreign static width
    foreign static height
    foreign static fps
    foreign static targetFps=(fps)
}

class Graphics {
    foreign static begin()
    foreign static end()
    foreign static clear(color)
    foreign static rect(x, y, width, height, color)
    foreign static text(text, x, y, size, color)
}

class Mouse {
    foreign static pressed(button)
    foreign static down(button)

    foreign static x
    foreign static y
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

    foreign draw(x, y, color)

    foreign width
    foreign height
}
