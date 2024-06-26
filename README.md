![wray](assets/wray.png)

# wray

> **The tiny wren runtime.**

Wray can be used to create any type of program: from basic command line tools up to fully fledged graphical applications and games.
Programs written with it are lightweight, fast and cross platform.
All of this is possible thanks to wray's main dependencies: the amazing [raylib](https://raylib.com) library and the [wren](https://wren.io) programming language.

The project takes A LOT of inspiration from the [love2d](https://love2d.org) framework, but trying to be even more lightweight and friendly to use.
Wray aims to be a self-contained, scripting tool with minimal dependencies and a lot of educational value.

## Features

- **Simple**: the API is friendly and can be learned by anyone.
- **Fast**: thanks to the speed of wren and raylib.
- **Cross Platform**: wray works on all major desktop platforms.
- **Tiny**: the runtime is self-contained and minimal in size.
- **Educational**: easy to get up and running with writing code.
- **Versatile**: can be used to create a vast array of different programs.

## Getting Started

To get started simply download the latest release for your operating system.
Wray has a simple CLI which allows to create, package and run projects.

```
wray new my_project
wray my_project
```

This should output: "Hello, World!".

If so you are ready to start writing code!
Just open the new folder in a code editor.
To learn everything you can do with wray, look at the [api](src/api.wren) file in the source directory.
Also check out wren's [documentation](https://wren.io/syntax.html).

The following code opens a window:

``` javascript
import "wray" for Window, Graphics, Color

Window.init(640, 480, "Hello Wray!")
Window.targetFps = 60

while (!Window.closed) {
    Graphics.begin()

    Graphics.clear(Color.blue)

    Graphics.end()
}
```

For more examples check the [examples](examples) folder in the repository!
Also look at the [modules](modules) directory for some wren libraries to use in your projects!

## Distribution

Once you've completed your project, you have to package it up.
First you should create an `egg` file.
This is an archive that contains all the project files and assets.

```
wray nest my_project
```

This file can now be distributed, but users will need to have the wray runtime installed.
You can also create completely indipendent executables by fusing the runtime and the egg file.

```
wray fuse my_project.egg
```

This will create the file `my_project.exe`, which can be distributed.

## Building

To build wray from source you need [cmake](https://cmake.org).

```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Acknowledgements

Thanks to all these amazing projects:

- [raylib](https://raylib.com)
- [wren](https://wren.io)
- [love2d](https://love2d.org)
- [raylib-wren](https://github.com/TSnake41/raylib-wren)
- [dome](https://domeengine.com)
- [enet](https://github.com/lsalzman/enet)
- [argparse](https://github.com/cofyc/argparse)
- [map](https://github.com/rxi/map)
- [microui](https://github.com/rxi/microui)
- [naett](https://github.com/erkkah/naett)
- [zip](https://github.com/kuba--/zip)
- [icon](https://www.flaticon.com/free-icon/robin_1230870?term=bird&page=1&position=8&origin=search&related_id=1230870)

## License

Wray is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
