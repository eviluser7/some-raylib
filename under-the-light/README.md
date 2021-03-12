![](./resources/logo2.png)
# You're afraid of the dark. Find the light and eat to stay alive.

## Gameplay
Use the arrow keys to move.
Get to the light tiles before the timer runs out.
Collect apples to avoid dying of hunger.
And do not leave the light.

## Installation
You'll need the raylib header file for this.
```
Ubuntu-based install:
sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev

Fedora install:
sudo dnf install alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel

All the packages can be found on pacman as well.
With pacman do:
sudo pacman -S raylib
```
Also check https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux for more information on Linux systems.
It seems raylib does not have a package on Debian-based systems, so you'll have to install it by hand.
```
git clone https://github.com/eviluser7/some-raylib
cd some-raylib/
cd under-the-light/
./build.sh && ./under-the-light
```

Web build coming soon
