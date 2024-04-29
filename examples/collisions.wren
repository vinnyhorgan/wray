// Collisions by Javidx9!
// https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp

import "wray" for Color, Graphics, Keyboard, Mouse, RenderTexture, Window
import "vector" for Vector

class Rect {
    construct new(pos, size, vel) {
        _pos = pos
        _size = size
        _vel = vel
        _contact = []

        for (i in 0...4) {
            _contact.add(null)
        }
    }

    pos { _pos }
    size { _size }
    vel { _vel }
    contact { _contact }

    pos=(v) { _pos = v }
    size=(v) { _size = v }
    vel=(v) { _vel = v }
    contact=(v) { _contact = v }
}

class AABB {
    static pointVsRect(p, r) {
        return p.x >= r.pos.x && p.y >= r.pos.y && p.x < r.pos.x + r.size.x && p.y < r.pos.y + r.size.y
    }

    static rectVsRect(r1, r2) {
        return r1.pos.x < r2.pos.x + r2.size.x && r1.pos.x + r1.size.x > r2.pos.x && r1.pos.y < r2.pos.y + r2.size.y && r1.pos.y + r1.size.y > r2.pos.y
    }

    static rayVsRect(rayOrigin, rayDir, target) {
        var contactPoint = Vector.new(0, 0)
        var contactNormal = Vector.new(0, 0)
        var tHitNear = 0

        var invdir = Vector.new(1 / rayDir.x, 1 / rayDir.y)

        var tNear = (target.pos - rayOrigin) * invdir
        var tFar = (target.pos + target.size - rayOrigin) * invdir

        if (tFar.y.isNan || tFar.x.isNan) {
            return null
        }

        if (tNear.y.isNan || tNear.x.isNan) {
            return null
        }

        if (tNear.x > tFar.x) {
            var t = tNear.x
            tNear.x = tFar.x
            tFar.x = t
        }

        if (tNear.y > tFar.y) {
            var t = tNear.y
            tNear.y = tFar.y
            tFar.y = t
        }

        if (tNear.x > tFar.y || tNear.y > tFar.x) {
            return null
        }

        tHitNear = tNear.x.max(tNear.y)

        var tHitFar = tFar.x.min(tFar.y)

        if (tHitFar < 0) {
            return null
        }

        contactPoint = rayOrigin + rayDir * tHitNear

        if (tNear.x > tNear.y) {
            if (invdir.x < 0) {
                contactNormal = Vector.new(1, 0)
            } else {
                contactNormal = Vector.new(-1, 0)
            }
        } else if (tNear.x < tNear.y) {
            if (invdir.y < 0) {
                contactNormal = Vector.new(0, 1)
            } else {
                contactNormal = Vector.new(0, -1)
            }
        }

        return {
            "contactPoint": contactPoint,
            "contactNormal": contactNormal,
            "tHitNear": tHitNear
        }
    }

    static dynamicRectVsRect(rDynamic, dt, rStatic) {
        if (rDynamic.vel.x == 0 && rDynamic.vel.y == 0) {
            return null
        }

        var expandedTarget = Rect.new(rStatic.pos - rDynamic.size / 2, rStatic.size + rDynamic.size, Vector.new(0, 0))

        var collision = rayVsRect(rDynamic.pos + rDynamic.size / 2, rDynamic.vel * dt, expandedTarget)

        if (collision) {
            if (collision["tHitNear"] >= 0 && collision["tHitNear"] <= 1) {
                return collision
            }
        } else {
            return null
        }
    }

    static resolveDynamicRectVsRect(rDynamic, dt, rStatic) {
        var col = dynamicRectVsRect(rDynamic, dt, rStatic)

        if (col) {
            if (col["contactNormal"].y > 0) {
                rDynamic.contact[0] = rStatic
            } else {
                rDynamic.contact[0] = null
            }

            if (col["contactNormal"].x < 0) {
                rDynamic.contact[1] = rStatic
            } else {
                rDynamic.contact[1] = null
            }

            if (col["contactNormal"].y < 0) {
                rDynamic.contact[2] = rStatic
            } else {
                rDynamic.contact[2] = null
            }

            if (col["contactNormal"].x > 0) {
                rDynamic.contact[3] = rStatic
            } else {
                rDynamic.contact[3] = null
            }

            rDynamic.vel = rDynamic.vel + col["contactNormal"] * Vector.new(rDynamic.vel.x.abs, rDynamic.vel.y.abs) * (1 - col["tHitNear"])

            return true
        }

        return false
    }
}

Window.init(640, 480, "Collisions")
Window.resizable = true
Window.targetFps = 60
Window.setMinSize(640, 480)

var gameWidth = 320
var gameHeight = 240
var screen = RenderTexture.new(gameWidth, gameHeight)
var screenTexture = screen.texture

var player = Rect.new(Vector.new(170, 70), Vector.new(10, 40), Vector.new(0, 0))

var rects = []
rects.add(Rect.new(Vector.new(150, 50), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(150, 150), Vector.new(75, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(170, 50), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(190, 50), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(110, 50), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(50, 130), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(50, 150), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(50, 170), Vector.new(20, 20), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(150, 100), Vector.new(10, 1), Vector.new(0, 0)))
rects.add(Rect.new(Vector.new(200, 100), Vector.new(20, 60), Vector.new(0, 0)))

while (!Window.closed) {
    var scale = (Window.width / gameWidth).min(Window.height / gameHeight)
    Mouse.setOffset(-(Window.width - gameWidth * scale) / 2, -(Window.height - gameHeight * scale) / 2)
    Mouse.setScale(1 / scale, 1 / scale)

    var mouse = Vector.new(Mouse.x, Mouse.y)

    if (Keyboard.down("w")) {
        player.vel.y = -100
    } else if (Keyboard.down("s")) {
        player.vel.y = 100
    }

    if (Keyboard.down("a")) {
        player.vel.x = -100
    } else if (Keyboard.down("d")) {
        player.vel.x = 100
    }

    if (Mouse.down(0)) {
        player.vel = player.vel + (mouse - player.pos).unit * 10
    }

    var z = []

    for (i in 0...rects.count) {
        var col = AABB.dynamicRectVsRect(player, Window.dt, rects[i])

        if (col) {
            z.add({
                "id": i,
                "t": col["tHitNear"]
            })
        }
    }

    z.sort { |a, b| a["t"] < b["t"] }

    for (c in z) {
        AABB.resolveDynamicRectVsRect(player, Window.dt, rects[c["id"]])
    }

    player.pos = player.pos + player.vel * Window.dt

    screen.begin()

        Graphics.clear(Color.darkBlue)

        Graphics.rectangleLines(player.pos.x, player.pos.y, player.size.x, player.size.y, Color.white)

        for (r in rects) {
            Graphics.rectangleLines(r.pos.x, r.pos.y, r.size.x, r.size.y, Color.white)
        }

        for (i in 0...player.contact.count) {
            if (player.contact[i]) {
                Graphics.rectangleLines(player.contact[i].pos.x, player.contact[i].pos.y, player.contact[i].size.x, player.contact[i].size.y, Color.orange)
            }

            player.contact[i] = null
        }

        if (player.vel.length > 0) {
            var pos1 = player.pos + player.size / 2
            var pos2 = player.pos + player.size / 2 + player.vel.unit * player.vel.length / 10
            Graphics.line(pos1.x, pos1.y, pos2.x, pos2.y, Color.red)
        }

        Graphics.print("WASD to move!", 10, 10, 12, Color.white)

    screen.end()

    Graphics.begin()

        Graphics.clear(Color.black)

        screenTexture.draw((Window.width - gameWidth * scale) / 2, (Window.height - gameHeight * scale) / 2, 0, scale, -scale, 0, 0, Color.white)

    Graphics.end()
}
