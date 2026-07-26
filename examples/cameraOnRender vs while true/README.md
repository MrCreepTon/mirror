# cameraOnRender vs while true.lua

![Demontration](demo.gif)

Compares updating a camera's position and rotation from the `onCameraRender(camera)` callback against doing it from the script's main `while true do wait(0) end` loop. Two identical cameras are spawned side by side, both following the same offset from your character — the left half of the screen is driven from `onCameraRender`, the right half from the main loop — so you can see the difference in smoothness between the two approaches.

## Usage

`/diff` — toggle the side-by-side comparison on/off
