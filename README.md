# cool-retro-term

|> Default Amber|C:\ IBM DOS|$ Default Green|
|---|---|---|
|![Default Amber Cool Retro Term](https://user-images.githubusercontent.com/121322/32070717-16708784-ba42-11e7-8572-a8fcc10d7f7d.gif)|![IBM DOS](https://user-images.githubusercontent.com/121322/32070716-16567e5c-ba42-11e7-9e64-ba96dfe9b64d.gif)|![Default Green Cool Retro Term](https://user-images.githubusercontent.com/121322/32070715-163a1c94-ba42-11e7-80bb-41fbf10fc634.gif)|

## Description
cool-retro-term is a terminal emulator which mimics the look and feel of the old cathode tube screens.
It has been designed to be eye-candy, customizable, and reasonably lightweight.

It uses the QML port of qtermwidget (Konsole): https://github.com/Swordfish90/qmltermwidget.

This terminal emulator works under Linux and macOS and requires Qt6.

Settings such as colors, fonts, and effects can be accessed via context menu.

## Screenshots
![Image](<https://i.imgur.com/TNumkDn.png>)
![Image](<https://i.imgur.com/hfjWOM4.png>)
![Image](<https://i.imgur.com/GYRDPzJ.jpg>)

## Install

If you want to get a hold of the latest version, just go to the Releases page and grab the latest AppImage (Linux) or dmg (macOS).

Alternatively, most distributions such as Ubuntu, Fedora or Arch already package cool-retro-term in their official repositories.

## Building

cool-retro-term is built with CMake and requires Qt 6.5 or newer with the Qt Declarative (Qt Quick, Qt Quick Controls), Qt Shader Tools and Qt SQL modules.

```sh
git clone --recursive https://github.com/Swordfish90/cool-retro-term.git
cd cool-retro-term
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/cool-retro-term
```

If you already cloned without `--recursive`, run `git submodule update --init --recursive` first. To install system-wide, configure with `-DCMAKE_INSTALL_PREFIX=/usr` and run `sudo cmake --install build`. If Qt is not in a standard location, pass `-DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/<platform>`.

`scripts/build-appimage.sh` and `scripts/build-dmg.sh` produce the Linux AppImage and macOS DMG release packages.
