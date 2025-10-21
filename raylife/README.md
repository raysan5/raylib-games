# RayLife

A simple implementation of the [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [raylib](https://www.raylib.com/).

## 🎮 Controls

- **Mouse wheel**: zoom in/out
- **Space Key**: toggle between play and draw modes. (If the number of generations exceeds the maximum, step to the next generation.)
- **Left mouse button**: toggle cell
- **Right mouse button**: move camera
- **Up Arrow**: Increase generation interval
- **Down Arrow**: Decrease generation interval
- **R Key**: reset all

## 🖥️ Build

You must have [raylib](https://www.raylib.com/) installed.

1. Run `make`

    ```bash
    make
    ```

2. Run `./raylife`

    ```bash
    ./raylife <generations> # default: 500
    ```

## 🎥 Example

Here’s a preview of the game:

![RayLife GIF](./screenshots/gif_raylife.gif)

## 🔑 License

[MIT](https://opensource.org/license/mit/) - Created by [**Paco Algar**](https://github.com/Pacatro).
