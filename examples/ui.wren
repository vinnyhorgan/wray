import "wray" for Buffer, Color, Graphics, UI, Window

Window.init(640, 480, "UI")
Window.resizable = true
Window.targetFps = 60

var logBuf = Buffer.new(64000)
var offset = 0
var inputBuf = Buffer.new(128)

var bg = Buffer.new(3 * 4)
bg.writeFloat(90, 0)
bg.writeFloat(95, 4)
bg.writeFloat(100, 8)

var checks = Buffer.new(3 * 4)
checks.writeUint32(1, 0)

var log = Fn.new {|text|
    offset = logBuf.writeString(text, offset)
    offset = logBuf.writeString("\n", offset)
}

while (!Window.closed) {
    UI.update()

    UI.begin()

    if (UI.beginWindow("Demo Window", 40, 40, 300, 450)) {
        var win = UI.getWindowInfo()
        UI.setWindowSize(win["width"].max(240), win["height"].max(300))

        if (UI.header("Window Info")) {
            UI.row(2, [60, -1], 0)
            UI.label("Position:")
            UI.label("%(win["x"]), %(win["y"])")
            UI.label("Size:")
            UI.label("%(win["width"]), %(win["height"])")
        }

        if (UI.header("Test Buttons", "expanded")) {
            UI.row(3, [100, -110, -1], 0)
            UI.label("Test buttons 1:")

            if (UI.button("Button 1")) {
                log.call("Pressed button 1")
            }
            if (UI.button("Button 2")) {
                log.call("Pressed button 2")
            }

            UI.label("Test buttons 2:")

            if (UI.button("Button 3")) {
                log.call("Pressed button 3")
            }
            if (UI.button("Popup")) {
                UI.openPopup("Test Popup")
            }

            if (UI.beginPopup("Test Popup")) {
                UI.button("Hello")
                UI.button("World")
                UI.endPopup()
            }
        }

        if (UI.header("Tree and Text", "expanded")) {
            UI.row(2, [150, -5], 0)
            UI.beginColumn()

            if (UI.beginTreenode("Test 1")) {
                if (UI.beginTreenode("Test 1a")) {
                    UI.label("Hello")
                    UI.label("world")
                    UI.endTreenode()
                }

                if (UI.beginTreenode("Test 1b")) {
                    if (UI.button("Button 1")) {
                        log.call("Pressed button 1")
                    }
                    if (UI.button("Button 2")) {
                        log.call("Pressed button 2")
                    }

                    UI.endTreenode()
                }

                UI.endTreenode()
            }

            if (UI.beginTreenode("Test 2")) {
                UI.row(2, [60, 60], 0)

                if (UI.button("Button 3")) {
                    log.call("Pressed button 3")
                }
                if (UI.button("Button 4")) {
                    log.call("Pressed button 4")
                }
                if (UI.button("Button 5")) {
                    log.call("Pressed button 5")
                }
                if (UI.button("Button 6")) {
                    log.call("Pressed button 6")
                }

                UI.endTreenode()
            }

            if (UI.beginTreenode("Test 3")) {
                UI.checkbox("Checkbox 1", checks, 0)
                UI.checkbox("Checkbox 2", checks, 4)
                UI.checkbox("Checkbox 3", checks, 8)
                UI.endTreenode()
            }

            UI.endColumn()

            UI.beginColumn()
            UI.row(1, [-1], 0)
            UI.text("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus ipsum, eu varius magna felis a nulla.")
            UI.endColumn()
        }

        if (UI.header("Background Color", "expanded")) {
            UI.row(2, [-78, -1], 74)

            UI.beginColumn()
            UI.row(2, [46, -1], 0)

            UI.label("Red:")
            UI.slider(bg, 0, 0, 255)

            UI.label("Green:")
            UI.slider(bg, 4, 0, 255)

            UI.label("Blue:")
            UI.slider(bg, 8, 0, 255)

            UI.endColumn()

            var color = Color.new(bg.readFloat(0), bg.readFloat(4), bg.readFloat(8))
            var rect = UI.next()
            UI.drawRect(rect[0], rect[1], rect[2], rect[3], color)
            UI.drawText("%(color.r) %(color.g) %(color.b)", rect[0], rect[1], rect[2], rect[3])
        }

        UI.endWindow()
    }

    if (UI.beginWindow("Log Window", 350, 40, 300, 200)) {
        UI.row(1, [-1], -25)

        UI.beginPanel("Log Output")
        UI.row(1, [-1], -1)
        UI.text(logBuf)
        UI.endPanel()

        var submitted = false
        UI.row(2, [-70, -1], 0)

        if (UI.textbox(inputBuf)) {
            UI.focus()
            submitted = true
        }

        if (UI.button("Submit")) {
            submitted = true
        }

        if (submitted) {
            log.call(inputBuf.toString)
            inputBuf[0] = 0
        }

        UI.endWindow()
    }

    UI.end()

    Graphics.begin()

        Graphics.clear(Color.new(bg.readFloat(0), bg.readFloat(4), bg.readFloat(8)))
        UI.draw()

    Graphics.end()
}
