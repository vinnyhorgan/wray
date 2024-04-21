import "wray" for Color, Graphics, Mouse, RenderTexture, Texture, Window

Window.init(640, 480, "Letterbox")
Window.resizable = true
Window.targetFps = 60
Window.setMinSize(320, 240)

var gameWidth = 320
var gameHeight = 240
var screen = RenderTexture.new(gameWidth, gameHeight)
var screenTexture = screen.texture

var bunny = Texture.new("bunny.png")

while (!Window.closed) {
    var scale = (Window.width / gameWidth).min(Window.height / gameHeight)
    Mouse.setOffset(-(Window.width - gameWidth * scale) / 2, -(Window.height - gameHeight * scale) / 2)
    Mouse.setScale(1 / scale, 1 / scale)

    screen.begin()

    Graphics.clear(Color.black)

    Graphics.print("Resize the window!", 10, 10, 10, Color.white)
    Graphics.print("Virtual Mouse: [%(Mouse.x), %(Mouse.y)]", 10, 30, 10, Color.white)

    bunny.draw(100, 100)

    screen.end()

    Graphics.begin()

    Graphics.clear(Color.indigo)

    screenTexture.draw((Window.width - gameWidth * scale) / 2, (Window.height - gameHeight * scale) / 2, 0, scale, -scale, 0, 0, Color.white)

    Graphics.end()
}
