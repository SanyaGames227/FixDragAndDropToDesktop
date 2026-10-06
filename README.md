# FixDragAndDropToDesktop

Tiny background tool for Windows. Fixes the desktop bug where files dragged
from 7-Zip (or similar apps) land next to the last icon instead of where
you dropped them.

<img width="634" height="380" alt="зображення" src="https://github.com/user-attachments/assets/361ebaee-a556-4631-8901-2279ae4a63e5" />

## The problem

You drag a file from 7-Zip onto the desktop. It shows up in the wrong place,
and you have to right-click → **Refresh** to fix it.

## The fix

This tool watches your desktop. When a new file or folder appears, it
instantly triggers a refresh (same as pressing F5). No more manual refreshing.

## Usage

1. Download `FixDragAndDropToDesktop.exe` from Releases.
2. Run it. It works silently in the background.
3. To start with Windows: press `Win + R`, type `shell:startup`, and put
   a shortcut to the exe there.

To stop it, end the process in Task Manager.

Debug mode (shows a console with a log):

    FixDragAndDropToDesktop.exe -enable_console

## Build

1. Open `FixDragAndDropToDesktop.sln` in Visual Studio.
2. Select the **Release** configuration.
3. Press **Build → Build Solution**.

## Notes

- Uses almost no resources.
- Doesn't change any files or settings.
- Only watches the top level of your desktop.

## License

[GNU GPL v3](LICENSE)
