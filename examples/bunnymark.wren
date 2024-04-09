import "wray" for Color, Graphics, Mouse, Texture, Window
import "random" for Random

var random = Random.new()

class Bunny {
    construct new(x, y, vx, vy, color) {
        _x = x
        _y = y
        _vx = vx
        _vy = vy
        _color = color
    }

    x { _x }
    y { _y }
    vx { _vx }
    vy { _vy }
    color { _color }

    x=(x) { _x = x }
    y=(y) { _y = y }
    vx=(vx) { _vx = vx }
    vy=(vy) { _vy = vy }
}

Window.init(640, 480, "Bunnymark")
Window.targetFps = 60

var bunny = Texture.new("bunny.png")

var bunnies = []

while (!Window.closed) {
    if (Mouse.down(0)) {
        for (i in 1..100) {
            var newBunny = Bunny.new(
                Mouse.x, Mouse.y,
                random.float(-250, 250) / 60,
                random.float(-250, 250) / 60,
                Color.new(random.int(50, 240), random.int(80, 240), random.int(100, 240))
            )

            bunnies.add(newBunny)
        }
    }

    for (b in bunnies) {
        b.x = b.x + b.vx
        b.y = b.y + b.vy

        if (((b.x + 16) > Window.width) || ((b.x + 16) < 0)) {
            b.vx = -b.vx
        }

        if (((b.y + 16) > Window.height) || ((b.y + 16) < 0)) {
            b.vy = -b.vy
        }
    }

    Graphics.begin()

    Graphics.clear(Color.white)

    for (b in bunnies) {
        bunny.draw(b.x, b.y, b.color)
    }

    Graphics.rectangle(0, 0, Window.width, 40, Color.black)
    Graphics.print("Bunnies: %(bunnies.count)", 120, 10, 20, Color.white)

    if (Window.fps > 40) {
        Graphics.print("FPS: %(Window.fps)", 10, 10, 20, Color.green)
    } else if (Window.fps > 20) {
        Graphics.print("FPS: %(Window.fps)", 10, 10, 20, Color.yellow)
    } else {
        Graphics.print("FPS: %(Window.fps)", 10, 10, 20, Color.red)
    }

    Graphics.end()
}

Window.close()
