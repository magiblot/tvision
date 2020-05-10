# Turbo Vision

An independent, backwards-compatible port of Turbo Vision 2.0 for modern Linux systems.

![tvedit on Konsole](https://user-images.githubusercontent.com/20713561/81506401-4fffdd80-92f6-11ea-8826-ee42612eb82a.png)

I started this as a personal project at the very end of 2018. It is now very close to feature parity with the original, and I am not spending much time on it anymore, so I'm making it open in case it may be useful to someone.

Even if the linux port cannot compete against other widespread solutions (such as SET's port, which has tons of additional features), I still consider the improvements made to the Windows implementation to be useful. For example, the TVEDIT application included here can be a good replacement for the missing `EDIT.COM` on 64-bit Windows.

The main goals of this project are:

* Making Turbo Vision work on Linux in the least intrusive way.
* Keeping it functional on DOS/Windows.
* Being as compatible as possible at the source code level with old Turbo Vision applications. This has led me to implement some of the Borland C++ RTL functions, as explained below.

Initially, I planned to add features like full-fledged Unicode support into Turbo Vision. However, I realized that doing so would require either extending the API or breaking backwards compatibility, and that a major rewrite would be most likely necessary.

As a GUI toolkit, Turbo Vision is pretty much outdated. Modern technologies tend to separate appearance specification from behaviour specification, and Turbo Vision is especially bad at that. Considering all its other limitations, it seems that Turbo Vision cannot fulfill the necessities of modern users and programmers. If some TUI library is to revolutionize our terminal applications in the future, that library won't be Turbo Vision, although it may be inspired by it.

## Build environment

### Linux

Turbo Vision can be built as an static library with CMake and GCC/Clang.

```sh
cmake .
make
```

The above produces the following files:

* `libtvision.a`, which is the Turbo Vision library.
* The demo applications `hello`, `tvdemo`, `tvedit` and `tvdir`, which were bundled with the original Turbo Vision (although some of them have a few improvements).

The build requirements are:

* A compiler supporting C++ 17.
* `libncursesw` (note the 'w').
* `libgpm` (for mouse support in the linux console) (optional).

The minimal command line required to build a Turbo Vision application (e.g. `hello.cpp` with GCC) from this project's root is (notice the shell expansion for the include directories):

```sh
g++ -o hello hello.cpp libtvision.a -Iinclude/{,tvision,override} -lncurses -lgpm
```

`-Iinclude/tvision` is only necessary if your application uses Turbo Vision 1.x includes (e.g. `#include <tv.h>` instead of `#include <tvision/tv.h>`). `-lgpm` is only necessary if Turbo Vision was built with `libgpm` support.

The `include/override` directory contains headers emulating the Borland C++ RTL (some come straight from Borland). Turbo Vision won't compile without them. This also means that including `tvision/tv.h` will bring several std names to the global namespace.

### Windows/DOS

Turbo Vision can still be built either as a DOS or Windows library with Borland C++.

I can confirm the build process works with:

* Borland C++ 4.52 with the Borland DOS PowerPack.
* Turbo Assembler 4.0.

You may face different problems depending on your build environment. For instance, Turbo Assembler needs a patch to work under Windows 95. On Windows XP everything seems to work fine. On Windows 10, MAKE may emit the error 'Fatal: Command arguments too long', which can be fixed by upgrading MAKE to the one bundled with Borland C++ 5.x.

Yes, this works in 64-bit Windows 10. What won't work is the Borland C++ installer, which is a 16-bit applications. You will have to run it on another environment or try your luck with winevdm.

A Borland Makefile can be found in the `project` directory. Build can be done by doing:

```sh
cd project
make.exe <options>
```

Where `<options>` can be:

* `-DDOS32` for 32-bit DPMI applications (which still works in 64-bit Windows).
* `-DWIN32` for 32-bit native Win32 applications (not possible for TVDEMO, which relies on `farcoreleft()` and other antiquities).
* `-DDEBUG` to build debug versions of the application and the library.
* `-DTVDEBUG` to link the applications with the debug version of the library.
* `-DOVERLAY`, `-DALIGNMENT={2,4}`, `-DEXCEPTION`, `-DNO_STREAMABLE`, `-DNOTAMS` for things I have nave never used but appeared in the original makefiles.

This will compile the library into a `LIB` directory next to `project`, and will compile executables for the demo applications in their respective `examples/*` directories.

I'm sorry, the root makefile assumes it is executed from the `project` directory. You can still run the original makefiles directly (in `source/tvision` and `examples/*`) if you want to use different settings.

## Features

### Linux

* Ncurses-based terminal support.
* Mouse and key modifiers support on the linux console.
* Overall better display performance than SET's or Sergio Sigala's ports.
* Reads UTF-8 input from the terminal and displays UTF-8 text, but still works with 8-bit ASCII characters internally.
* Implementation of some Borland C++ RTL functions: `findfirst`, `findnext`, `fnsplit`, `_dos_findfirst`, `_dos_findnext`, `getdisk`, `setdisk`, `getcurdir`, `filelenght`.
* Accepts both Unix and Windows-style file paths in 'Open File' dialogs.
* Simple segmentation fault handler that gives you the chance to 'continue running' the application if something goes wrong.

There are a few environment variables that affect the behaviour of all Turbo Vision applications:

* `TVISION_DISPLAY`: strategy for drawing to screen. Valid values are `ncurses` and `ansi`. Default is `ncurses`, which lets Ncurses do the dirty job. `ansi` is a custom strategy that avoids redundant buffering and offers better performance, but also has some issues, such as not clearing the text attributes when exiting the application.
* `TVISION_MAX_FPS`: limit of times screen changes are drawn to the terminal, default `60`. This helps keep the drawing performance reasonable. Special values for this option are `0`, to disable refresh rate limiting, and `-1`, to actually draw to the terminal in every call to THardwareInfo::screenWrite (useful for debugging).
* `TVISION_ESCDELAY`: the delay of time, in milliseconds, that should be waited after receiving an ESC key press. If another key is pressed during this delay, it will be interpreted as an Alt+Key combination.
* `TVISION_CODEPAGE`: the character set used internally by Turbo Vision. Only `437` and `850` are supported at the moment, although adding more costs as little as adding an array of translations in `source/linux/tables.cpp`.

### Windows

* Only compatible with the Win32 Console API.
* Applications fit the console window size instead of the buffer size.
* The console buffer is restored when exiting or suspending Turbo Vision.
* `kbCtrlC`, Shift+Arrow, `kbShiftTab` and AltGr key combinations work properly.
* No busy polling for events.

### All platforms

* Middle mouse button support (`mbMiddleButton`), even on DOS.
* Horizontal and vertical mouse wheel support (`evMouseWheel`), even on DOS.
* Arbitrary screen size support (with up to 65535 rows or columns).
* Graceful handling of screen resize events.
* Support for `kbCtrlUp` and `kbCtrlDown` key codes (which don't work on 16-bit DOS, don't ask me why). They can be used to move windows faster with the keyboard (as `kbCtrlLeft` and `kbCtrlRight` already did).
* Improved usability of menus: they can be closed by clicking twice on them, even submenus.
* Improved usability of scrollbars: dragging them also scrolls the page. Clicking on an empty area of the scrollbar moves the thumb right under the cursor. They are responsive by default to mouse wheel events.
* Views don't lose their sizes when extremely resized.
* Support for LF line endings in `tvdemo` and `tvedit`. `tvedit` preserves the line ending on file save but all newly created files use CRLF by default.
* `tvedit`: drag scroll with middle mouse button.
* `tvedit`: delete whole word with `kbCtrlBack` and `kbCtrlDel` (note to linux users: they might not work on terminal emulators, but they do work on the console).
* `tvedit`: smart Home key (toggles between beginning of line and beginning of indented text).
* `tvdemo`: event viewer applet useful for event debugging.
* `tvdemo`: option to change the background pattern.

## Not implemented

### Linux

* `evMouseAuto`.
* Help files.

## API changes

* `TDrawBuffer` is no longer an static array. The equivalent of `sizeof(TDrawBuffer)/sizeof(ushort)` is the `.lenght()` method.
* Several constructors and methods now receive or return `const char*` instead of `char*`.
* `TTextDevice` is now buffered, so if you were using `otstream` you may have to send `std::flush` or `std::endl` through it for `do_sputn` to be invoked.

## Screenshots

You will find some screenshots [here](https://github.com/magiblot/tvision/issues/7). Feel free to add your own!

## Contributing

If you know of any Turbo Vision applications whose source code has not been lost and that could benefit from this, let me know.
