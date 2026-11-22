# cool-retro-term

|> Default Amber|C:\ IBM DOS|$ Default Green|
|---|---|---|
|![Default Amber Cool Retro Term](https://user-images.githubusercontent.com/121322/32070717-16708784-ba42-11e7-8572-a8fcc10d7f7d.gif)|![IBM DOS](https://user-images.githubusercontent.com/121322/32070716-16567e5c-ba42-11e7-9e64-ba96dfe9b64d.gif)|![Default Green Cool Retro Term](https://user-images.githubusercontent.com/121322/32070715-163a1c94-ba42-11e7-80bb-41fbf10fc634.gif)|

## Description
cool-retro-term is a terminal emulator which mimics the look and feel of the old cathode tube screens.
It has been designed to be eye-candy, customizable, and reasonably lightweight.

The terminal engine is a QML port of qtermwidget (Konsole), kept as a submodule: https://github.com/Rahelio/qmltermwidget.

This terminal emulator works under Linux and macOS and requires Qt 6.5 or newer.

Settings such as colors, fonts, and effects can be accessed via context menu.

## Screenshots
![Image](<https://i.imgur.com/TNumkDn.png>)
![Image](<https://i.imgur.com/hfjWOM4.png>)
![Image](<https://i.imgur.com/GYRDPzJ.jpg>)

## Terminal features

Besides the usual VT100/xterm emulation, the engine supports:

- Synchronized output (mode 2026), so full-screen programs such as nvim and tmux redraw without flicker
- OSC 52 clipboard: programs, including ones running over SSH, can copy to the system clipboard
- OSC 8 hyperlinks: linked text is underlined; Ctrl+click opens it (http, https, ftp, mailto and file links only)
- Color queries (OSC 10/11/12), so programs can pick a light or dark theme
- Bracketed paste, with protection against pasted text ending the paste early
- Shift+click to extend a selection
- Wide characters and emoji measured with utf8proc, when available

## Install

If you want to get a hold of the latest version, just go to the Releases page and grab the latest AppImage (Linux) or dmg (macOS).

Alternatively, most distributions such as Ubuntu, Fedora or Arch already package cool-retro-term in their official repositories. These packages may be older than this source tree.

## Building

cool-retro-term is built with CMake. It needs:

- CMake 3.21 or newer and a C++17 compiler
- Qt 6.5 or newer with Qt Declarative (Qt Quick, Qt Quick Controls), Qt Shader Tools and Qt SQL
- utf8proc (optional, recommended): without it character widths come from the C library

On Arch Linux:

```sh
sudo pacman -S --needed base-devel cmake qt6-base qt6-declarative qt6-shadertools libutf8proc
```

Then:

```sh
git clone --recursive https://github.com/Rahelio/cool-retro-term.git
cd cool-retro-term
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/cool-retro-term
```

If you already cloned without `--recursive`, run `git submodule update --init --recursive` first. If Qt is not in a standard location, pass `-DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/<platform>`. To build without utf8proc, pass `-DUSE_UTF8PROC=OFF`.

When you pull updates, also update the submodules with `git submodule update --init --recursive`, or pull with `git pull --recurse-submodules`. Otherwise the build can mix a new interface with an old terminal plugin, and the app quits at startup. Running `git config submodule.recurse true` once makes `git pull` do this automatically.

Only one instance runs at a time: starting cool-retro-term again opens a new window in the running instance. To try a fresh build while an installed copy is running, quit the installed copy first.

To install system-wide (the binary, the QML plugin, the desktop file, icons, the man page and the AppStream metadata):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j
sudo cmake --install build
```

On macOS, leave out `CMAKE_INSTALL_PREFIX`: it defaults to `/Applications` and the install puts `cool-retro-term.app` there, with the QML plugin inside the bundle. Don't use `/usr`, because System Integrity Protection blocks writes there even with `sudo`. Admin users don't need `sudo` to install into `/Applications`. The installed app links against your local Qt (for example Homebrew's `qt`). To get a self-contained app, use `scripts/build-dmg.sh`.

`scripts/build-appimage.sh` and `scripts/build-dmg.sh` produce the Linux AppImage and macOS DMG release packages.

## Usage

```
cool-retro-term [--default-settings] [--workdir <dir>] [-p|--profile <prof>] [--fullscreen] [--verbose] [-h|--help] [-v|--version] [-e <cmd> [args...]]
```

| Option | Description |
|---|---|
| `--default-settings` | Run with the default settings |
| `--workdir <dir>` | Start in `dir` |
| `-p`, `--profile <prof>` | Use the given profile |
| `--fullscreen` | Start in fullscreen |
| `-e <cmd> [args...]` | Run `cmd` instead of the shell; everything after it is passed to the command, so use it last |
| `--verbose` | Print additional information such as profiles and settings |
| `-h`, `--help` | Print the help |
| `-v`, `--version` | Print the version |

These options also apply when an instance is already running: the new window opens there with the given command, directory, profile and fullscreen setting. See `man cool-retro-term` once installed.
