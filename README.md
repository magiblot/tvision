# Turbo Vision

A modern port of Turbo Vision 2.0, the classical framework for text-based user interfaces. Now cross-platform and with Unicode support.

![tvedit on Konsole](https://user-images.githubusercontent.com/20713561/81506401-4fffdd80-92f6-11ea-8826-ee42612eb82a.png)

I started this as a personal project at the very end of 2018. By May 2020 I considered it was very close to feature parity with the original, and decided to make it open.

The original goals of this project were:

* Making Turbo Vision work on Linux by altering the legacy codebase as little as possible.
* Keeping it functional on DOS/Windows.
* Being as compatible as possible at the source code level with old Turbo Vision applications. This led me to implement some of the Borland C++ RTL functions, as explained below.

At one point I considered I had done enough, and that any attempts at revamping the library and overcoming its original limitations would require either extending the API or breaking backward compatibility, and that a major rewrite would be most likely necessary.

However, between July and August 2020 I found the way to integrate full-fledged Unicode support into the existing arquitecture, wrote the [Turbo](https://github.com/magiblot/turbo) text editor and also made the new features available on Windows. So I am confident that Turbo Vision can now meet many of the expectations of modern users and programmers.

# Table of contents

* [What is Turbo Vision good for?](#what-for)
* [How do I use Turbo Vision?](#how-to)
* Build environment
    * [Linux](#build-linux)
    * [Windows (MSVC)](#build-msvc)
    * [Windows (MinGW)](#build-mingw)
    * [Windows/DOS (Borland C++)](#build-borland)
    * [Turbo Vision as a CMake dependency](#build-cmake)
* [Features](#features)
* [API changes](#apichanges)
* [Applications using Turbo Vision](#applications)
* [Unicode support](#unicode)
* [Clipboard interaction](#clipboard)
* [Extended color support](#color)

<div id="what-for"></div>

## What is Turbo Vision good for?

A lot has changed since Borland created Turbo Vision in the early 90's. Many GUI tools today separate appearance specification from behaviour specification, use safer or dynamic languages which do not segfault on error, and support either concurrent or asynchronous programming, or both.

Turbo Vision does none of that, but it certainly overcomes many of the issues programmers still face today when writing terminal applications:

1. Forget about terminal capabilities and direct terminal I/O. When writing a Turbo Vision application, all you have to care about is what you want your application to behave and look like—there is no need to add workarounds in your code. Turbo Vision tries its best to produce the same results on all environments. For example: in order to get a bright background color on the Linux console, the *blink* attribute has to be set. Turbo Vision does this for you.

2. Reuse what has already been done. Turbo Vision provides many widget classes (also known as *views*), including resizable, overlapping windows, pull-down menus, dialog boxes, buttons, scroll bars, input boxes, check boxes and radio buttons. You may use and extend these; but even if you prefer creating your own, Turbo Vision already handles event dispatching, display of fullwidth Unicode characters, etc.: you do not need to waste time rewriting any of that.

3. Can you imagine writing a text-based interface that works both on Linux and Windows (and thus is cross-platform) out-of-the-box, with no `#ifdef`s? Turbo Vision makes this possible. First: Turbo Vision keeps on using `char` arrays instead of relying on the implementation-defined and platform-dependent `wchar_t` or `TCHAR`. Second: thanks to UTF-8 support in `setlocale` in [recent versions of Microsoft's RTL](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale#utf-8-support), code like the following will work as intended:
```c++
    std::ifstream f("コンピュータ.txt"); // On Windows, the RTL converts this to the system encoding on-the-fly.
```

<div id="how-to"></div>

## How do I use Turbo Vision?

You can get started with the [Turbo Vision For C++ User's Guide](https://archive.org/details/BorlandTurboVisionForCUserSGuide/mode/1up), and look at the sample applications [`hello`](https://github.com/magiblot/tvision/blob/master/hello.cpp), [`tvdemo`](https://github.com/magiblot/tvision/tree/master/examples/tvdemo) and [`tvedit`](https://github.com/magiblot/tvision/tree/master/examples/tvedit). Once you grasp the basics,
I suggest you take a look at the [Turbo Vision 2.0 Programming Guide](https://archive.org/details/bitsavers_borlandTurrogrammingGuide1992_25707423), which is, in my opinion, more intuitive and easier to understand, despite using Pascal. By then you will probably be interested in the [`palette`](https://github.com/magiblot/tvision/tree/master/examples/palette) example, which contains a detailed description of how palettes are used.

Don't forget to check out the <a href="#features">features</a> and <a href="#apichanges">API changes</a> sections as well.

## Build environment

<div id="build-linux"></div>

### Linux

Turbo Vision can be built as an static library with CMake and GCC/Clang.

```sh
cmake . -B ./build -DCMAKE_BUILD_TYPE=Release && # Could also be 'Debug', 'MinSizeRel' or 'RelWithDebInfo'.
cmake --build ./build # or `cd ./build; make`
```

CMake versions older than 3.13 may not support the `-B` option. You can try the following instead:

```sh
mkdir -p build; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release &&
cmake --build .
```

The above produces the following files:

* `libtvision.a`, which is the Turbo Vision library.
* The demo applications `hello`, `tvdemo`, `tvedit`, `tvdir`, which were bundled with the original Turbo Vision (although some of them have a few improvements).
* The demo applications `mmenu` and `palette` from Borland's Technical Support.
* `tvhc`, the Turbo Vision Help Compiler.

The library and executables can be found in `./build`.

The build requirements are:

* A compiler supporting C++14.
* `libncursesw` (note the 'w').
* `libgpm` (for mouse support on the Linux console) (optional).

If your distribution provides separate *devel* packages (e.g. `libncurses-dev`, `libgpm-dev` in Debian-based distros), install these too.

The minimal command line required to build a Turbo Vision application (e.g. `hello.cpp` with GCC) from this project's root is:

```sh
g++ -std=c++14 -o hello hello.cpp ./build/libtvision.a -Iinclude -lncursesw -lgpm
```

You may also need:

* `-Iinclude/tvision` if your application uses Turbo Vision 1.x includes (`#include <tv.h>` instead of `#include <tvision/tv.h>`).

* `-Iinclude/tvision/compat` if your application includes Borland headers (`dir.h`, `iostream.h`, etc.).

* On Gentoo (and possibly others): `-ltinfow` if both `libtinfo.so` and `libtinfow.so` are available in your system. Otherwise, you may get a segmentation fault when running Turbo Vision applications ([#11](https://github.com/magiblot/tvision/issues/11)). Note that `tinfo` is bundled with `ncurses`.

`-lgpm` is only necessary if Turbo Vision was built with `libgpm` support.

The backward-compatibility headers in `include/tvision/compat` emulate the Borland C++ RTL. Turbo Vision's source code still depends on them, and they could be useful if porting old applications. This also means that including `tvision/tv.h` will bring several `std` names to the global namespace.

<div id="build-msvc"></div>

### Windows (MSVC)

The build process with MSVC is slightly more complex, as there are more options to choose from. Note that you will need different build directories for different target architectures. For instance, to generate optimized binaries:

```sh
cmake . -B ./build && # Add '-A x64' (64-bit) or '-A Win32' (32-bit) to override the default platform.
cmake --build ./build --config Release # Could also be 'Debug', 'MinSizeRel' or 'RelWithDebInfo'.
```

In the example above, `tvision.lib` and the example applications will be placed at `./build/Release`.

If you wish to link Turbo Vision statically against Microsofts's run-time library (`/MT` instead of `/MD`), enable the `TV_USE_STATIC_RTL` option (`-DTV_USE_STATIC_RTL=ON` when calling `cmake`).

If you wish to link an application against Turbo Vision, note that MSVC won't allow you to mix `/MT` with `/MD` or debug with non-debug binaries. All components have to be linked against the RTL in the same way.

If you develop your own Turbo Vision application make sure to enable the following compiler flags, or else you will get compilation errors when including `<tvision/tv.h>`:

```
/permissive-
/Zc:__cplusplus
```

If you use [Turbo Vision as a CMake submodule](#build-cmake), these flags will be enabled automatically.

**Note:** Turbo Vision uses `setlocale` to set the [RTL functions in UTF-8 mode](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale#utf-8-support). This won't work if you use an old version of the RTL.

With the RTL statically linked in, and if UTF-8 is supported in `setlocale`, Turbo Vision applications are portable and work by default on **Windows Vista and later**.

<div id="build-mingw"></div>

### Windows (MinGW)

Once your MinGW environment is properly set up, build is done in a similar way to Linux:
```sh
cmake . -B ./build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release &&
cmake --build ./build
```
In the example above, `libtvision.a` and all examples are in `./build` if `TV_BUILD_EXAMPLES` option is `ON` (the default).

If you wish to link an application against Turbo Vision, simply add `-L./build/lib -ltvision` to your linker and `-I./include` to your compiler

<div id="build-borland"></div>

### Windows/DOS (Borland C++)

Turbo Vision can still be built either as a DOS or Windows library with Borland C++. Obviously, there is no Unicode support here.

I can confirm the build process works with:

* Borland C++ 4.52 with the Borland PowerPack for DOS.
* Turbo Assembler 4.0.

You may face different problems depending on your build environment. For instance, Turbo Assembler needs a patch to work under Windows 95. On Windows XP everything seems to work fine. On Windows 10, MAKE may emit the error `Fatal: Command arguments too long`, which can be fixed by upgrading MAKE to the one bundled with Borland C++ 5.x.

Yes, this works on 64-bit Windows 10. What won't work is the Borland C++ installer, which is a 16-bit application. You will have to run it on another environment or try your luck with [winevdm](https://github.com/otya128/winevdm).

A Borland Makefile can be found in the `project` directory. Build can be done by doing:

```sh
cd project
make.exe <options>
```

Where `<options>` can be:

* `-DDOS32` for 32-bit DPMI applications (which still work on 64-bit Windows).
* `-DWIN32` for 32-bit native Win32 applications (not possible for TVDEMO, which relies on `farcoreleft()` and other antiquities).
* `-DDEBUG` to build debug versions of the application and the library.
* `-DTVDEBUG` to link the applications with the debug version of the library.
* `-DOVERLAY`, `-DALIGNMENT={2,4}`, `-DEXCEPTION`, `-DNO_STREAMABLE`, `-DNOTASM` for things I have nave never used but appeared in the original makefiles.

This will compile the library into a `LIB` directory next to `project`, and will compile executables for the demo applications in their respective `examples/*` directories.

I'm sorry, the root makefile assumes it is executed from the `project` directory. You can still run the original makefiles directly (in `source/tvision` and `examples/*`) if you want to use different settings.

The minimal command line required to build a Turbo Vision application (e.g. `hello.cpp`) from this project's root is:

```sh
# 16-bit, real mode DOS.
BCC.EXE -ml -Iinclude hello.cpp lib/tv.lib

# 32-bit, native Win32 application.
BCC32.EXE -WC -Iinclude hello.cpp lib/tv32.lib

# 32-bit, protected mode DOS (DPMI32).
BCC32.EXE -WX -Iinclude hello.cpp lib/tv32.lib import32.lib
```

Note the extra `import32.lib` when using `-WX`. This file is provided by Borland C++, so it will be found as long as the compiler is configured properly (i.e. libraries and headers are looked up in `$(BCROOT)\LIB` and `$(BCROOT)\INCLUDE` by default). If you don't link against this file, you will get the following error:

```
Linker Error: Unresolved External 'SetConsoleActiveScreenBuffer' referenced from module hardwrvr.cpp
```

Additionally, you may get a warning like this one when running DPMI32 applications on an actual DOS environment:

```
32loader error: 'C:\HELLO.EXE'
    Invalid import references:
       - module 'kernel32.dll' entrypoint 'SetConsoleActiveScreenBuffer'
    Do you want to attempt running this program (Y/N)?
```

But don't worry, you can ignore them.

<div id="build-cmake"></div>

### Turbo Vision as a CMake dependency (not Borland C++)

If you choose the CMake build system for your application, there are two main ways to link against Turbo Vision:

* Installing Turbo Vision and importing it with `find_package`. Installation depends on the generator type:

    * First, decide an install prefix. The default one will work out-of-the-box, but usually requires admin privileges. On Unix systems, you can use `$HOME/.local` instead. On Windows, you can use any custom path you want but you'll have to add it to the `CMAKE_PREFIX_PATH` environment variable when building your application.
    * For mono-config generators (`Unix Makefiles`, `Ninja`...), you only have to build and install once:
        
        ```sh
        cmake . -B ./build # '-DCMAKE_INSTALL_PREFIX=...' to override the install prefix.
        cmake --build ./build
        cmake --install ./build
        ```
    * For multi-config generators (`Visual Studio`, `Ninja Multi-Config`...) you should build and install all configurations:

        ```sh
        cmake . -B ./build # '-DCMAKE_INSTALL_PREFIX=...' to override the install prefix.
        cmake --build ./build --config Release
        cmake --build ./build --config Debug --target tvision
        cmake --build ./build --config RelWithDebInfo --target tvision
        cmake --build ./build --config MinSizeRel --target tvision
        cmake --install ./build --config Release
        cmake --install ./build --config Debug --component library
        cmake --install ./build --config RelWithDebInfo --component library
        cmake --install ./build --config MinSizeRel --component library
        ```
    Then, in your application's `CMakeLists.txt`, you may import it like this:
    ```cmake
    find_package(tvision CONFIG)
    target_link_libraries(my_application tvision::tvision)
    ```

* Have Turbo Vision in a submodule in your repository, and import it with `add_subdirectory`:


    ```cmake
    add_subdirectory(tvision) # Assuming Turbo Vision is in the 'tvision' directory.
    target_link_libraries(my_application tvision)
    ```

In either case, `<tvision/tv.h>` will be available in your application's include path during compilation, and your application will be linked against the necessary libraries (Ncurses, GPM...) automatically.

<div id="features"></div>

## Features

### Modern platforms (not Borland C++)

* UTF-8 support both in terminal I/O and the API. You can try Unicode support out in the `tvedit` and `tvdemo` applications.
* True Color.
* Implementation of some Borland C++ RTL functions: `findfirst`, `findnext`, `fnsplit`, `_dos_findfirst`, `_dos_findnext`, `getdisk`, `setdisk`, `getcurdir`, `filelength`.
* Accepts both Unix and Windows-style file paths in 'Open File' dialogs.
* Compatibility with 32-bit help files.

### Linux

* Ncurses-based terminal support.
* Mouse and key modifiers support on the Linux console.
* Overall better display performance than SET's or Sergio Sigala's ports.
* Redirection of `stdin`/`stdout`/`stderr` does not interfere with terminal I/O.

There are a few environment variables that affect the behaviour of all Turbo Vision applications:

* `TVISION_DISPLAY`: strategy for drawing to screen. Valid values are `ncurses` and `ansi`. Default is `ansi`, which is a custom strategy that avoids redundant buffering and UTF-8 to wide char conversions. If you have issues, you may try `ncurses` instead.
* `TVISION_MAX_FPS`: limit of times screen changes are drawn to the terminal, default `60`. This helps keeping the draw performance reasonable. Special values for this option are `0`, to disable refresh rate limiting, and `-1`, to actually draw to the terminal in every call to THardwareInfo::screenWrite (useful for debugging).
* `TVISION_ESCDELAY`: the delay of time, in milliseconds, that should be waited after receiving an ESC key press. If another key is pressed during this delay, it will be interpreted as an Alt+Key combination.
* `TVISION_CODEPAGE`: the character set used internally by Turbo Vision to translate *extended ASCII* into Unicode. Only `437` and `850` are supported at the moment, although adding more costs as little as adding an array of translations in `source/platform/tables.cpp`.
* `TVISION_USE_STDIO`: if defined, terminal I/O is performed through `stdin`/`stdout`, so that it can be redirected from the shell. By default, Turbo Vision performs terminal I/O through `/dev/tty`, allowing the user to redirect `stdin`, `stdout` and `stderr` for their needs, without affecting the application's stability.

    For example, the following will leave `out.txt` empty:

    ```sh
    ./tvdemo | tee out.txt
    ```

    While the following will dump all the escape sequences and text printed by the application into `out.txt`:

    ```sh
    TVISION_USE_STDIO= ./tvdemo | tee out.txt
    ```

### Windows

* Only compatible with the Win32 Console API.
* Applications fit the console window size instead of the buffer size (no scrollbars are visible).
* The console buffer is restored when exiting or suspending Turbo Vision.
* `kbCtrlC`, Shift+Arrow, `kbShiftTab` and AltGr key combinations work properly.
* No busy polling for events (i.e. no 100% CPU consumption on idle, which is how it originally was).

The following are not available when compiling with Borland C++:

* The console's codepage is set to UTF-8 on startup and restored on exit.
* Microsoft's C runtime functions are set automatically to UTF-8 mode, so you do not need to use the `wchar_t` variants.
* If the console crashes, a new one is allocated automatically.

**Note:** Turbo Vision writes UTF-8 text directly to the Windows console. If the console is set in legacy mode and the bitmap font is being used, Unicode characters will not be displayed properly ([photo](https://user-images.githubusercontent.com/20713561/91917174-7a1f4600-ecbf-11ea-8c7a-2ec80d31d2ca.png)).

### All platforms

* Middle mouse button support (`mbMiddleButton`), even on DOS.
* Horizontal and vertical mouse wheel support (`evMouseWheel`), even on DOS.
* Arbitrary screen size support (with up to 65535 rows or columns).
* Notably improved memory safety.
* Graceful handling of screen resize events.
* Windows can be resized also from their bottom left corner.
* Windows can be dragged from empty areas with the middle mouse button.
* Support for `kbCtrlUp` and `kbCtrlDown` key codes (which don't work on 16-bit DOS, don't ask me why). They can be used to move windows faster with the keyboard (as `kbCtrlLeft` and `kbCtrlRight` already did).
* Improved usability of menus: they can be closed by clicking again on them, even submenus.
* Improved usability of scrollbars: dragging them also scrolls the page. Clicking on an empty area of the scrollbar moves the thumb right under the cursor. They are responsive by default to mouse wheel events.
* Views don't lose their sizes when extremely resized.
* `TInputLine`s no longer scroll the text display on focus/unfocus, allowing relevant text to stay visible.
* Support for LF line endings in `tvdemo` and `tvedit`. `tvedit` preserves the line ending on file save but all newly created files use CRLF by default.
* `tvedit`: context menu on right click.
* `tvedit`: drag scroll with middle mouse button.
* `tvedit`: delete whole word with `kbCtrlBack` and `kbCtrlDel` (note to Linux users: they might not work on terminal emulators, but they do work on the console).
* `tvedit`: smart Home key (toggles between beginning of line and beginning of indented text).
* `tvedit`: support for files bigger than 64 KiB on 32-bit or 64-bit builds.
* `tvdemo`: event viewer applet useful for event debugging.
* `tvdemo`: option to change the background pattern.

## Not implemented

### Linux

* `evMouseAuto`.

<div id="apichanges"></div>

## API changes

* Screen writes are buffered and are usually sent to the terminal once for every iteration of the active event loop (see also `TVISION_MAX_FPS`). If you need to update the screen during a busy loop, you may use `TScreen::flushScreen()`.
* `TDrawBuffer` is no longer a fixed-length array. The equivalent of `sizeof(TDrawBuffer)/sizeof(ushort)` is the `.length()` method.
* `TTextDevice` is now buffered, so if you were using `otstream` you may have to send `std::flush` or `std::endl` through it for `do_sputn` to be invoked.
* `TApplication` now provides `dosShell()`, `cascade()` and `tile()`, and handles `cmDosShell`, `cmCascade` and `cmTile` by default. These functions can be customized by overriding `getTileRect()` and `writeShellMsg()`. This is the same behaviour as in the Pascal version.
* The `buttons` field in `evMouseUp` events is no longer empty. It now indicates which button was released.
* Triple-click support: new mouse event flag `meTripleClick`.
* `TRect` methods `move`, `grow`, `intersect` and `Union` now return `TRect&` instead of being `void`, so that they can be chained.
* `TOutlineViewer` now allows the root node to have siblings.
* New function `ushort popupMenu(TPoint where, TMenuItem &aMenu, TGroup *receiver=0)` which spawns a `TMenuPopup` on the desktop. See `source/tvision/popupmnu.cpp`.
* New virtual method `TMenuItem& TEditor::initContextMenu(TPoint p)` that determines the entries of the right-click context menu in `TEditor`.
* `fexpand` can now take a second parameter `relativeTo`.
* New class `TStringView`, which is a clone of `std::string_view`. You shouldn't need it unless you are programming in Borland C++, which has no `std::string_view`.
* Many methods which originally had null-terminated string parameters now receive `TStringView` instead. `TStringView` is compatible with `std::string_view`, `std::string` and `const char *` (even `nullptr`).
* New class `TSpan<T>`, a generic (and non-const) version of `TStringView`, inspired by `std::span`.
* New classes `TDrawSurface` and `TSurfaceView`, see `<tvision/surface.h>`.
* New method `TView::textEvent` which allows receiving text in an efficient manner, see [Clipboard interaction](#clipboard).
* Unicode support, see [Unicode](#unicode).
* True Color support, see [extended colors](#color).

### Changes that you probably won't care about

I don't want extensions to be left undocumented, so I'll write them down just for the record.

* `TEvent::getKeyEvent()` now takes a `blocking` parameter which is `True` by default, making it possible to query input events in a non-blocking way.
* New method `TVMemMgr::reallocateDiscardable` which can be used along `allocateDiscardable` and `freeDiscardable`.

## Screenshots

You will find some screenshots [here](https://github.com/magiblot/tvision/issues/7). Feel free to add your own!

## Contributing

If you know of any Turbo Vision applications whose source code has not been lost and that could benefit from this, let me know.

<div id="applications"></div>

## Applications using Turbo Vision

If your application is based on this project and you'd like it to appear in the following list, just let me know.

* [Turbo](https://github.com/magiblot/turbo) by [magiblot](https://github.com/magiblot), a proof-of-concept text editor.
* [TMBASIC](https://github.com/electroly/tmbasic) by [Brian Luft](https://github.com/electroly), a programming language for creating console applications.

<div id="unicode"></div>

# Unicode support

The Turbo Vision API has been extended to allow receiving Unicode input and displaying Unicode text. The supported encoding is UTF-8, for a number of reasons:

* It is compatible with already present data types (`char *`), so it does not require intrusive modifications to existing code.
* It is the same encoding used for terminal I/O, so redundant conversions are avoided.
* Conformance to the [UTF-8 Everywhere Manifesto](http://utf8everywhere.org/), which exposes many other advantages.

Note that when built with Borland C++, Turbo Vision does not support Unicode. However, this does not affect the way Turbo Vision applications are written, since the API extensions are designed to allow for encoding-agnostic code.

## Reading Unicode input

The traditional way to get text from a key press event is as follows:

```c++
// 'ev' is a TEvent, and 'ev.what' equals 'evKeyDown'.
switch (ev.keyDown.keyCode) {
    // Key shortcuts are usually checked first.
    // ...
    default: {
        // The character is encoded in the current codepage
        // (CP437 by default).
        char c = ev.keyDown.charScan.charCode;
        // ...
    }
}
```

Some of the existing Turbo Vision classes that deal with text input still depend on this methodology, which has not changed. Single-byte characters, when representable in the current codepage, continue to be available in `ev.keyDown.charScan.charCode`.

Unicode support consists in two new fields in `ev.keyDown` (which is a `struct KeyDownEvent`):

* `char text[4]`, which may contain whatever was read from the terminal: usually a UTF-8 sequence, but possibly any kind of raw data.
* `uchar textLength`, which is the number of bytes of data available in `text`, from 0 to 4.

Note that the `text` field may contain garbage or uninitialized data from position `textLength` on.

You can also get a `TStringView` out of a `KeyDownEvent` with the `asText()` method.

So a Unicode character can be retrieved from `TEvent` in the following way:

```c++
switch (ev.keyDown.keyCode) {
    // ...
    default: {
        std::string_view s {ev.keyDown.asText()};
        processText(s);
    }
}
```

Let's see it from another perspective. If the user types `ñ`, a `TEvent` is generated with the following `keyDown` struct:

```c++
KeyDownEvent {
    union {
        .keyCode = 0xA4,
        .charScan = CharScanType {
            .charCode = 164 ('ñ'), // In CP437
            .scanCode = 0
        }
    },
    .controlKeyState = 0x200 (kbInsState),
    .text = {'\xC3', '\xB1', '\x00', '\x00'}, // In UTF-8
    .textLength = 2
}
```
However, if they type `€` the following will happen:
```c++
KeyDownEvent {
    union {
        .keyCode = 0x0 (kbNoKey), // '€' not part of CP437
        .charScan = CharScanType {
            .charCode = 0,
            .scanCode = 0
        }
    },
    .controlKeyState = 0x200 (kbInsState),
    .text = {'\xE2', '\x82', '\xAC', '\x00'}, // In UTF-8
    .textLength = 3
}
```
If a key shortcut is pressed instead, `text` is empty:
```c++
KeyDownEvent {
    union {
        .keyCode = 0xB (kbCtrlK),
        .charScan = CharScanType {
            .charCode = 11 ('♂'),
            .scanCode = 0
        }
    },
    .controlKeyState = 0x20C (kbCtrlShift | kbInsState),
    .text = {'\x00', '\x00', '\x00', '\x00'},
    .textLength = 0
}
```
So, in short: views designed without Unicode input in mind will continue to work exactly as they did before, and views which want to be Unicode-aware will have no issues in being so.

## Displaying Unicode text

The original design of Turbo Vision uses 16 bits to represent a *screen cell*—8 bit for a character and 8 bit for [BIOS color attributes](https://en.wikipedia.org/wiki/BIOS_color_attributes).

A new `TScreenCell` type is defined in `<tvision/scrncell.h>` which is capable of holding a limited number of UTF-8 codepoints in addition to extended attributes (bold, underline, italic...). However, you should not write text into a `TScreenCell` directly but make use of Unicode-aware API functions instead.

### Text display rules

A character provided as argument to any of the Turbo Vision API functions that deal with displaying text is interpreted as follows:

* Non-printable characters in the range `0x00` to `0xFF` are interpreted as characters in the active codepage. For instance, `0x7F` is displayed as `⌂` and `0xF0` as `≡` if using CP437. As an exception, `0x00` is always displayed as a regular space. These characters are all one column wide.
* Character sequences which are not valid UTF-8 are interpreted as sequences of characters in the current codepage, as in the case above.
* Valid UTF-8 sequences with a display width other than one are taken care of in a special way, see below.

For example, the string `"╔[\xFE]╗"` may be displayed as `╔[■]╗`. This means that box-drawing characters can be mixed with UTF-8 in general, which is useful for backward compatibility. If you rely on this behaviour, though, you may get unexpected results: for instance, `"\xC4\xBF"` is a valid UTF-8 sequence and is displayed as `Ŀ` instead of `─┐`.

One of the issues of Unicode support is the existence of [multi-width](https://convertcase.net/vaporwave-wide-text-generator/) characters and [combining](https://en.wikipedia.org/wiki/Combining_Diacritical_Marks) characters. This conflicts with Turbo Vision's original assumption that the screen is a grid of cells occupied by a single character each. Nevertheless, these cases are handled in the following way:

* Multi-width characters can be drawn anywhere on the screen and nothing bad happens if they overlap partially with other characters.
* Zero-width characters overlay the previous character. For example, the sequence `में` consists of the single-width character `म` and the combining characters `े` and `ं`. In this case, three Unicode codepoints are fit into the same cell.

    The `ZERO WIDTH JOINER` (`U+200D`) is always ommited, as it complicates things too much. For example, it can turn a string like `"👩👦"` (4 columns wide) into `"👩‍👦"` (2 columns wide). Not all terminal emulators respect the ZWJ, so, in order to produce predictable results, Turbo Vision will print both `"👩👦"` and `"👩‍👦"` as `👩👦`.
* No notable graphical glitches will occur as long as your terminal emulator respects character widths as measured by `wcwidth`.

Here is an example of such characters in the [Turbo](https://github.com/magiblot/turbo) text editor:
![Wide character display](https://user-images.githubusercontent.com/20713561/103179253-51344980-488a-11eb-9a29-79b9acb1b4b9.png)

### Unicode-aware API functions

The usual way of writing to the screen is by using `TDrawBuffer`. A few methods have been added and others have changed their meaning:

```c++
void TDrawBuffer::moveChar(ushort indent, char c, TColorAttr attr, ushort count);
void TDrawBuffer::putChar(ushort indent, ushort c);
```
`c` is always interpreted as a character in the active codepage.

```c++
ushort TDrawBuffer::moveStr(ushort indent, TStringView str, TColorAttr attr);
ushort TDrawBuffer::moveCStr(ushort indent, TStringView str, TAttrPair attrs);
```
`str` is interpreted according to the rules exposed previously.

```c++
ushort TDrawBuffer::moveStr(ushort indent, TStringView str, TColorAttr attr, ushort width, ushort begin=0); // New
```
`str` is interpreted according to the rules exposed previously. However:
* `width` specifies the maximum number of display columns that should be read from `str`.
* `begin` specifies the number of display columns that should be skipped at the beginning of `str`. This is useful for horizontal scrolling. If `begin` is in the middle of a multi-width character, the remaining positions in that character are filled with spaces.

The return values are the number of display columns that were actually filled with text.

```c++
void TDrawBuffer::moveBuf(ushort indent, const void *source, TColorAttr attr, ushort count);
```
This function's name is misleading. Even in its original implementation, `source` is treated as a string. So it is equivalent to `moveStr(indent, TStringView((const char*) source, count), attr)`.

```c++
void TDrawBuffer::moveBuf(ushort indent, const TScreenCell *source, ushort count); // New
```
In case you happen to have an array of `TScreenCell`, this method allows you to copy them into a `TDrawBuffer`.

There are other useful Unicode-aware functions:

```c++
int cstrlen(TStringView s);
```
Returns the displayed length of `s` according to the aforementioned rules, discarding `~` characters.

```c++
int strwidth(TStringView s); // New
```
Returns the displayed length of `s`.

On Borland C++, these methods assume a single-byte encoding and all characters being one column wide. This makes it possible to write encoding-agnostic `draw()` and `handleEvent()` methods that work on both platforms without a single `#ifdef`.

The functions above depend on the following lower-level functions. You will need them if you want to fill `TScreenCell` objects with text manually. You may find complete descriptions in `<tvision/ttext.h>`.

```c++
size_t TText::next(TStringView text);
size_t TText::prev(TStringView text, size_t index);
size_t TText::wseek(TStringView text, int count, Boolean incRemainder=True);
size_t TText::fill(TSpan<TScreenCell> cells, TStringView text);
size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, TColorAttr attr);
#ifndef __BORLANDC__
template <class Func>
size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, Func &&func);
#endif
bool TText::eat(TSpan<TScreenCell> cells, size_t &i, TStringView text, size_t &j);
bool TText::next(TStringView text, size_t &index, size_t &width);
bool TText::next(TStringView text, size_t &index);
void TText::wseek(TStringView text, size_t &index, size_t &remainder, int count);
```

For drawing `TScreenCell` buffers directly, the following methods are available:

```c++
void TView::writeBuf(short x, short y, short w, short h, const TScreenCell *b); // New
void TView::writeLine(short x, short y, short w, short h, const TScreenCell *b); // New
```

### Example: Unicode text in menus and status bars

It's as simple as it can be. Let's modify `hello.cpp` as follows:

```c++
TMenuBar *THelloApp::initMenuBar( TRect r )
{
    r.b.y = r.a.y+1;
    return new TMenuBar( r,
      *new TSubMenu( "~Ñ~ello", kbAltH ) +
        *new TMenuItem( "階~毎~料入報最...", GreetThemCmd, kbAltG ) +
        *new TMenuItem( "五劫~の~擦り切れ", cmYes, kbNoKey, hcNoContext ) +
        *new TMenuItem( "העברית ~א~ינטרנט", cmNo, kbNoKey, hcNoContext ) +
         newLine() +
        *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )
        );
}

TStatusLine *THelloApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "~Alt-Ç~ Exit", kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu )
            );
}
```
Here is what it looks like:

![Unicode Hello](https://user-images.githubusercontent.com/20713561/103179255-5396a380-488a-11eb-88ad-0192adbe233e.png)

### Example: writing Unicode-aware `draw()` methods

The following is an excerpt from an old implementation of `TFileViewer::draw()` (part of the `tvdemo` application), which does not draw Unicode text properly:

```c++
if (delta.y + i < fileLines->getCount()) {
    char s[maxLineLength+1];
    p = (char *)(fileLines->at(delta.y+i));
    if (p == 0 || strlen(p) < delta.x)
        s[0] = EOS;
    else
        strnzcpy(s, p+delta.x, maxLineLength+1);
    b.moveStr(0, s, c);
}
writeBuf( 0, i, size.x, 1, b );
```
All it does is move part of a string in `fileLines` into `b`, which is a `TDrawBuffer`. `delta` is a `TPoint` representing the scroll offset in the text view, and `i` is the index of the visible line being processed. `c` is the text color. A few issues are present:

* `TDrawBuffer::moveStr(ushort, const char *, TColorAttr)` takes a null-terminated string. In order to pass a substring of the current line, a copy is made into the array `s`, at the risk of a [buffer overrun](https://github.com/magiblot/tvision/commit/8aa2bf4af4474b85e86e340b08d7c56081b68986). The case where the line does not fit into `s` is not handled, so at most `maxLineLenght` characters will be copied. What's more, a multibyte character near position `maxLineLength` could be copied incompletely and become garbage when displayed.
* `delta.x` is the first visible column. With multibyte-encoded text, it is no longer true that such column begins at position `delta.x` in the string.

Below is a corrected version of the code above that handles Unicode properly:

```c++
if (delta.y + i < fileLines->getCount()) {
    p = (char *)(fileLines->at(delta.y+i));
    if (p)
        b.moveStr(0, p, c, size.x, delta.x);
}
writeBuf( 0, i, size.x, 1, b );
```
The overload of `moveStr` used here is `TDrawBuffer::moveStr(ushort indent, TStringView str, TColorAttr attr, ushort width, ushort begin)`. This function not only provides Unicode support, but also helps us write cleaner code and overcome some of the limitations previously present:

* The intermediary copy is avoided, so the displayed text is not limited to `maxLineLength` bytes.
* `moveStr` takes care of printing the string starting at column `delta.x`. We do not even need to worry about how many bytes correspond to `delta.x` columns.
* Similarly, `moveStr` is instructed to copy at most `size.x` columns of text without us having to care about how many bytes that is nor dealing with edge cases. The code is written in an encoding-agnostic way and will work whether multibyte characters are being considered or not.
* In case you hadn't realized yet, the intermediary copy in the previous version was completely unnecessary. It would have been necessary only if we had needed to trim the end of the line, but that was not the case: text occupies all of the view's width, and `TView::writeBuf` already takes care of not writing beyond it. Yet it is interesting to see how an unnecessary step not only was limiting functionality but also was prone to bugs.

## Unicode support across standard views

Support for creating Unicode-aware views is in place, and most views in the original Turbo Vision library have been adapted to handle Unicode.

The following views can display Unicode text properly. Some of them also do horizontal scrolling or word wrapping; all of that should work fine.

- [x] `TStaticText` ([`7b15d45d`](https://github.com/magiblot/tvision/commit/7b15d45da231f75f2677454021c2e34ad1149ca8)).
- [x] `TFrame` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679)).
- [x] `TStatusLine` ([`477b3ae9`](https://github.com/magiblot/tvision/commit/477b3ae91fd84eb1487dca18a87b3f7b8699c576)).
- [x] `THistoryViewer` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679)).
- [x] `THelpViewer` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679), [`8c7dac2a`](https://github.com/magiblot/tvision/commit/8c7dac2a61000f17e09cc31ebbb58b030f95c0e5), [`20f331e3`](https://github.com/magiblot/tvision/commit/20f331e362255d45859c36050ff75ffab078c3ab)).
- [x] `TListViewer` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679)).
- [x] `TMenuBox` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679)).
- [x] `TTerminal` ([`ee821b69`](https://github.com/magiblot/tvision/commit/ee821b69c5dd81c565fe1add1ac6f0a2f8a96a01)).
- [x] `TOutlineViewer` ([`6cc8cd38`](https://github.com/magiblot/tvision/commit/6cc8cd38da5841201544d6ba103f9662d7675213)).
- [x] `TFileViewer` (from the `tvdemo` application) ([`068bbf7a`](https://github.com/magiblot/tvision/commit/068bbf7a0a13482bda91f9f3411ec614f9a1e6ff)).
- [x] `TFilePane` (from the `tvdir` application) ([`9bcd897c`](https://github.com/magiblot/tvision/commit/9bcd897cb7cf010ef34d0281d42e9ea58345ce53)).

The following views can, in addition, process Unicode text or user input:

- [x] `TInputLine` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679), [`cb489d42`](https://github.com/magiblot/tvision/commit/cb489d42d522f7515c870942bcaa8f0f3dea3f35)).
- [x] `TEditor` ([`702114dc`](https://github.com/magiblot/tvision/commit/702114dc03a13ebce2b52504eb122c97f9892de9)). Instances are in UTF-8 mode by default. You may switch back to single-byte mode by pressing `Ctrl+P`. This only changes how the document is displayed and the encoding of user input; it does not alter the document. This class is used in the `tvedit` application; you may test it there.

Views not in this list may not have needed any corrections or I simply forgot to fix them. Please submit an issue if you notice anything not working as expected.

Use cases where Unicode is not supported (not an exhaustive list):

- [ ] Highlighted key shortcuts, in general (e.g. `TMenuBox`, `TStatusLine`, `TButton`...).

<div id="clipboard"></div>

# Clipboard interaction

Turbo Vision does not support accessing the system clipboard. So, in order to paste text into a Turbo Vision application, the user has to do so through the terminal emulator. Turbo Vision will then receive the text through standard input.

Unfortunately, each character is processed as a separate `evKeyDown` event. If the user pastes 5000 characters, the application will execute the same operations as if the user pressed the keyboard 5000 times. This involves drawing views, completing the event loop, updating the screen, etcetera. As you can imagine, this is far from optimal.

For the purpose of dealing with this situation, the Turbo Vision API has been extended with the following function:

```c++
Boolean TView::textEvent(TEvent &event, TSpan<char> dest, size_t &length);
```

`TEditor` takes advantage of this function to provide a good user experience when pasting text through the terminal. You can check it out in the `tvedit` application. As a developer, you may be interested in using it if you are implementing a text editing component. Otherwise, you don't need to care about it.

Just for the record, here is a more detailed explanation:

`textEvent()` tries to read text from standard input and stores it in a user-provided buffer `dest`. It returns `False` when no more events are available in the program's input queue or if a non-text event is found, in which case this event is saved with `putEvent()` so that it can be processed in the next iteration of the event loop.

The exact number of bytes read is stored in the output parameter `length`, which can never be greater than `dest.size()`.

It is intended to be used as follows:

```c++
// 'ev' is a TEvent, and 'ev.what' equals 'evKeyDown'.
switch (ev.keyDown.keyCode) {
    // Other cases we may want to check.
    // ...
    default:
        // Does the event contain text?
        if (ev.keyDown.textLength) {
            char buf[512];
            size_t length;
            // Fill 'buf' with text from the input queue,
            // including the text in 'ev'.
            while (textEvent(ev, buf, length)) {
                // Process 'length' bytes of text in 'buf'.
                // ...
            }
            // 'textEvent()' clears 'ev' after reading it the first time.
            // That is, by this point, 'ev.what' is 'evNothing'.
        }
}
```

<div id="color"></div>

# Extended color support

The Turbo Vision API has been extended to allow more than the original 16 colors.

Colors can be specified using any of the following formats:

* [BIOS color attributes](https://en.wikipedia.org/wiki/BIOS_color_attributes) (4-bit), the format used originally on MS-DOS.
* RGB (24-bit).
* `xterm-256color` palette indices (8-bit).
* The *terminal default* color. This is the color used by terminal emulators when no display attributes (bold, color...) are enabled (usually white for foreground and black for background).

Although Turbo Vision applications are likely to be ran in a terminal emulator, the API makes no assumptions about the display device. That is, the complexity of dealing with terminal emulators is hidden from the programmer and managed by Turbo Vision itself.

For example: color support varies among terminals. If the programmer uses a color format not supported by the terminal emulator, Turbo Vision will quantize it to what the terminal can display. The following images represent the quantization of a 24-bit RGB picture to 256, 16 and 8 color palettes:

| `COLORTERM=truecolor` (original) | `TERM=xterm-256color`|
|:-:|:-:|
|![mpv-shot0005](https://user-images.githubusercontent.com/20713561/111095336-7c4f4080-853d-11eb-8331-798898a2af68.png)|![mpv-shot0002](https://user-images.githubusercontent.com/20713561/111095333-7b1e1380-853d-11eb-8c4d-989fe24d0498.png)|

| `TERM=xterm-16color` | `TERM=xterm` (bold as bright) |
|:-:|:-:|
|![mpv-shot0003](https://user-images.githubusercontent.com/20713561/111095334-7bb6aa00-853d-11eb-9a3f-e7decc0bac7d.png)|![mpv-shot0004](https://user-images.githubusercontent.com/20713561/111095335-7bb6aa00-853d-11eb-9098-38d6f6c3c1da.png)|

Extended color support basically comes down to the following:
* Turbo Vision has originally used [BIOS color attributes](https://en.wikipedia.org/wiki/BIOS_color_attributes) stored in an `uchar`. `ushort` is used to represent attribute pairs. This is still the case when using Borland C++.
* In modern platforms there is a new type named `TColorAttr` which replaces `uchar`. It specifies a foreground and background colors and a style. Colors can be specified in different formats (BIOS color attributes, 24-bit RGB...). Styles are the typical ones (bold, italic, underline...). There's also `TAttrPair`, which replaces `ushort`.
* The `TDrawBuffer` methods, which used to take `uchar` or `ushort` parameters to specify color attributes, now take `TColorAttr` or `TAttrPair`.
* `TPalette`, which used to contain an array of `uchar`, now contains an array of `TColorAttr`. The `TView::mapColor` method also returns `TColorAttr` instead of `uchar`.
* `TView::mapColor` has been made virtual so that the palette system can be bypassed without having to rewrite any `draw` methods.
* `TColorAttr` and `TAttrPair` can be initialized with and casted into `uchar` and `ushort` in a way such that legacy code still compiles out-of-the-box without any change in functionality.

Below is a more detailed explanation aimed at programmers.

<details>
<summary>API reference of extended color support (<i>click to expand</i>).</summary>

## Data Types

In the first place we will explain the data types the programmer needs to know in order to take advantage of the extended color suport.

To get access to them, define the macro `Uses_TColorAttr` before including `<tvision/tv.h>`. You may not need to do this because other classes like `TView` or `TDrawBuffer` already depend on it.

All the types described in this section are *trivial*. This means that they can be `memset`'d and `memcpy`'d. But variables of these types are *uninitialized* when declared without initializer, just like primitive types. So make sure you don't manipulate them before initializing them.

### Color format types

Several types are defined which represent different color formats.
The reason why these types exist is to allow distinguishing color formats using the type system. Some of them also have public fields which make it easier to manipulate individual bits.

* `TColorBIOS` represents a BIOS color. It behaves the same as `uint8_t`, but allows accessing the `r`, `g`, `b` and `bright` bits individually.

    The memory layout is:

    * Bit 0: Blue (field `b`).
    * Bit 1: Green (field `g`).
    * Bit 2: Red (field `r`).
    * Bit 3: Bright (field `bright`).
    * Bits 4-7: unused.

    ```c++
    TColorBIOS bios = 0x4;  // 0x4: red.
    bios.bright = 1;        // 0xC: light red.
    bios.b = bios.r;        // 0xD: light magenta.
    bios = bios ^ 3;        // 0xE: yellow.
    uint8_t c = bios;       // Implicit conversion to integer types.
    ```

    In terminal emulators, BIOS colors are mapped to the basic 16 ANSI colors.

* `TColorRGB` represents a color in 24-bit RGB. It behaves the same as `uint32_t` but allows accessing the `r`, `g` and `b` bit fields individually.

    The memory layout is:

    * Bits 0-7: Blue (field `b`).
    * Bits 8-15: Green (field `g`).
    * Bits 16-23: Red (field `r`).
    * Bits 24-31: unused.

    ```c++
    TColorRGB rgb = 0x9370DB;   // 0xRRGGBB.
    rgb = {0x93, 0x70, 0xDB};   // {R, G, B}.
    rgb = rgb ^ 0xFFFFFF;       // Negated.
    rgb.g = rgb.r & 0x88;       // Access to individual components.
    uint32_t c = rgb;           // Implicit conversion to integer types.
    ```

* `TColorXTerm` represents an index into the `xterm-256color` color palette. It behaves the same as `uint8_t`.

### `TColorDesired`

`TColorDesired` represents a color which the programmer intends to show on screen, encoded in any of the supported color types.

A `TColorDesired` can be initialized in the following ways:

* As a BIOS color: with a `char` literal or a `TColorBIOS` object:

    ```c++
    TColorDesired bios1 = '\xF';
    TColorDesired bios2 = TColorBIOS(0xF);
    ```
* As a RGB color: with an `int` literal or a `TColorRGB` object:

    ```c++
    TColorDesired rgb1 = 0xFF7700; // 0xRRGGBB.
    TColorDesired rgb2 = TColorRGB(0xFF, 0x77, 0x00); // {R, G, B}.
    TColorDesired rgb3 = TColorRGB(0xFF7700); // 0xRRGGBB.
    ```
* As an XTerm palette index: with a `TColorXTerm` object.
* As the *terminal default* color: through zero-initialization:

    ```c++
    TColorDesired def1 {};
    // Or with 'memset':
    TColorDesired def2;
    memset(&def2, 0, sizeof(def2));
    ```

`TColorDesired` has methods to query the contained color, but you will usually not need to use them. See the struct definition in `<tvision/colors.h>` for more information.

Trivia: the name is inspired by [Scintilla](https://www.scintilla.org/index.html)'s `ColourDesired`.

### `TColorAttr`

`TColorAttr` describes the color attributes of a screen cell. This is the type you are most likely to interact with if you intend to change the colors in a view.

A `TColorAttr` is composed of:

* A foreground color, of type `TColorDesired`.
* A background color, of type `TColorDesired`.
* A style bitmask containing a combination of the following flags:

    * `slBold`.
    * `slItalic`.
    * `slUnderline`.
    * `slBlink`.
    * `slReverse`.
    * `slStrike`.

    These flags are based on the basic display attributes selectable through [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters). The results may vary between terminal emulators. `slReverse` is probably the least reliable of them: prefer using the `TColorAttr reverseAttribute(TColorAttr attr)` free function over setting this flag.

The most straight-forward way to create a `TColorAttr` is by means of the `TColorAttr(TColorDesired fg, TColorDesired bg, ushort style=0)` and `TColorAttr(int bios)` constructors:

```c++
// Foreground: RGB 0x892312
// Background: RGB 0x7F00BB
// Style: Normal.
TColorAttr a1 = {TColorRGB(0x89, 0x23, 0x12), TColorRGB(0x7F, 0x00, 0xBB)};

// Foreground: BIOS 0x7.
// Background: RGB 0x7F00BB.
// Style: Bold, Italic.
TColorAttr a2 = {'\x7', 0x7F00BB, slBold | slItalic};

// Foreground: Terminal default.
// Background: BIOS 0xF.
// Style: Normal.
TColorAttr a3 = {{}, TColorBIOS(0xF)};

// Foreground: Terminal default.
// Background: Terminal default.
// Style: Normal.
TColorAttr a4 = {};

// Foreground: BIOS 0x0
// Background: BIOS 0x7
// Style: Normal
TColorAttr a5 = 0x70;
```

The fields of a `TColorAttr` can be accessed with the following free functions:

```c++
TColorDesired getFore(const TColorAttr &attr);
TColorDesired getBack(const TColorAttr &attr);
ushort getStyle(const TColorAttr &attr);
void setFore(TColorAttr &attr, TColorDesired fg);
void setBack(TColorAttr &attr, TColorDesired bg);
void setStyle(TColorAttr &attr, ushort style);
```

### `TAttrPair`

`TAttrPair` is a pair of `TColorAttr`, used by some API functions to pass two attributes at once.

You may initialize a `TAttrPair` with the `TAttrPair(const TColorAttrs &lo, const TColorAttrs &hi)` constructor:

```c++
TColorAttr cNormal = {0x234983, 0x267232};
TColorAttr cHigh = {0x309283, 0x127844};
TAttrPair attrs = {cNormal, cHigh};
TDrawBuffer b;
b.moveCStr(0, "Normal text, ~Highlighted text~", attrs);
```

The attributes can be accessed with the `[0]` and `[1]` subindices:

```c++
TColorAttr lo = {0x892343, 0x271274};
TColorAttr hi = '\x93';
TAttrPair attrs = {lo, hi};
assert(lo == attrs[0]);
assert(hi == attrs[1]);
```

## Changing the appearance of a `TView`

Views are commonly drawn by means of a `TDrawBuffer`. Most `TDrawBuffer` member functions take color attributes by parameter. For example:

```c++
ushort TDrawBuffer::moveStr(ushort indent, TStringView str, TColorAttr attr);
ushort TDrawBuffer::moveCStr(ushort indent, TStringView str, TAttrPair attrs);
void TDrawBuffer::putAttribute(ushort indent, TColorAttr attr);
```

However, the views provided with Turbo Vision usually store their color information in palettes. A view's palette can be queried with the following member functions:

```c++
TColorAttr TView::mapColor(uchar index);
TAttrPair TView::getColor(ushort indices);
```

* `mapColor` looks up a single color attribute in the view's palette, given an index into the palette. Remember that the palette indices for each view class can be found in the Turbo Vision headers. For example, `<tvision/views.h>` says the following about `TScrollBar`:

    ```c++
    /* ---------------------------------------------------------------------- */
    /*      class TScrollBar                                                  */
    /*                                                                        */
    /*      Palette layout                                                    */
    /*        1 = Page areas                                                  */
    /*        2 = Arrows                                                      */
    /*        3 = Indicator                                                   */
    /* ---------------------------------------------------------------------- */
    ```

* `getColor` is a helper function that allows querying two cell attributes at once. Each byte in the `indices` parameter contains an index into the palette. The `TAttrPair` result contains the two cell attributes.

    For example, the following can be found in the `draw` method of `TMenuBar`:

    ```c++
    TAttrPair cNormal = getColor(0x0301);
    TAttrPair cSelect = getColor(0x0604);
    ```

    Which would be equivalent to this:

    ```c++
    TAttrPair cNormal = {mapColor(1), mapColor(3)};
    TAttrPair cSelect = {mapColor(4), mapColor(6)};
    ```

As an API extension, the `mapColor` method has been made `virtual`. This makes it possible to override Turbo Vision's hierarchical palette system with a custom solution without having to rewrite the `draw()` method.

So, in general, there are three ways to use extended colors in views:

1. By providing extended color attributes directly to `TDrawBuffer` methods, if the palette system is not being used. For example:

    ```c++
    // The 'TMyView' class inherits from 'TView' and overrides 'TView::draw'.
    void TMyView::draw()
    {
        TDrawBuffer b;
        TColorAttr color {0x1F1C1B, 0xFAFAFA, slBold};
        b.moveStr(0, "This is bold black text over a white background", color);
        /* ... */
    }
    ```

2. By modifying the palettes. There are two ways to do this:

    1. By modifying the application palette after it has been built. Note that the palette elements are `TColorAttr`. For example:

    ```c++
    void updateAppPalette()
    {
        TPalette &pal = TProgram::application->getPalete();
        pal[1] = {0x762892, 0x828712};              // TBackground.
        pal[2] = {0x874832, 0x249838, slBold};      // TMenuView normal text.
        pal[3] = {{}, {}, slItalic | slUnderline};  // TMenuView disabled text.
        /* ... */
    }
    ```

    2. By using extended color attributes in the application palette definition:

    ```c++
    static const TColorAttr cpMyApp[] =
    {
        {0x762892, 0x828712},               // TBackground.
        {0x874832, 0x249838, slBold},       // TMenuView normal text.
        {{}, {}, slItalic | slUnderline},   // TMenuView disabled text.
        /* ... */
    };

    // The 'TMyApp' class inherits from 'TApplication' and overrides 'TView::getPalette'.
    TPalette &TMyApp::getPalette() const
    {
        static TPalette palette(cpMyApp);
        return palette;
    }
    ```

3. By returning extended color attributes from an overriden `mapColor` method:

```c++
// The 'TMyScrollBar' class inherits from 'TScrollBar' and overrides 'TView::mapColor'.
TColorAttr TMyScrollBar::mapColor(uchar index)
{
    // In this example the values are hardcoded,
    // but they could be stored elsewhere if desired.
    switch (index)
    {
        case 1:     return {0x492983, 0x826124}; // Page areas.
        case 2:     return {0x438939, 0x091297}; // Arrows.
        case 3:     return {0x123783, 0x329812}; // Indicator.
        default:    return errorAttr;
    }
}
```

## Backward-compatibility

The types defined previously represent concepts that are also important when developing for Borland C++:

| Concept | Layout in Borland C++ | Layout in modern platforms |
|:-:|:-:|:-:|
| Color Attribute | `uchar`. A BIOS color attribute. | `struct TColorAttr`. |
| Color | A 4-bit number. | `struct TColorDesired`. |
| Attribute Pair | `ushort`. An attribute in each byte. | `struct TAttrPair`. |

One of this project's key principles is that the API should be used in the same way both in Borland C++ and modern platforms, that is, without the need for `#ifdef`s. Another principle is that legacy code should compile out-of-the-box, and adapting it to the new features should increase complexity as little as possible.

Backward-compatibility is accomplished in the following way:

* In Borland C++, `TColorAttr` and `TAttrPair` are `typedef`'d to `uchar` and `ushort`, respectively.
* In modern platforms, `TColorAttr` and `TAttrPair` can be used in place of `uchar` and `ushort`, respectively. That is, the assertions in the following code won't fail:

    ```c++
    // Any value which fits into a 'uchar' can be
    // losslessly passed through TColorAttr.
    uchar c = 0;
    do {
        assert(uchar(TColorAttr {c}) == c);
    } while (c++ < UCHAR_MAX);

    // Any value which fits into a 'ushort' can be
    // losslessly passed through TAttrPair.
    ushort s = 0;
    do {
        assert(ushort(TAttrPair {s}) == s);
    } while (s++ < USHRT_MAX);
    ```

    A `TColorAttr` initialized with `uchar` represents a BIOS color attribute. When converting back to `uchar`, the following happens:

    * If `fg` and `bg` are BIOS colors, and `style` is cleared, the resulting `uchar` represents the same BIOS color attribute contained in the `TColorAttr` (as in the code above).
    * Otherwise, the conversion results in a color attribute that stands out, i.e. white on magenta, meaning that the programmer should consider replacing `uchar`/`ushort` with `TColorAttr`/`TAttrPair` if they intend to support the extended color attributes.

    The same goes for `TAttrPair` and `ushort`, considering that it is composed of two `TColorAttr`.

A use case of backward-compatibility within Turbo Vision itself is the `TPalette` class, core of the palette system. In its original design, it used a single data type (`uchar`) to represent different things: array length, palette indices or color attributes.

The new design simply replaces `uchar` with `TColorAttr`. This means there are no changes in the way `TPalette` is used, yet `TPalette` is now able to store extended color attributes.

`TColorDialog` hasn't been remodeled yet, and thus it can't be used to pick extended color attributes at runtime.

### Example: adding extended color support to legacy code

The following pattern of code is common across `draw` methods of views:

```c++
void TMyView::draw()
{
    ushort cFrame, cTitle;
    if (state & sfDragging)
    {
        cFrame = 0x0505;
        cTitle = 0x0005;
    }
    else
    {
        cFrame = 0x0503;
        cTitle = 0x0004;
    }
    cFrame = getColor(cFrame);
    cTitle = getColor(cTitle);
    /* ... */
}
```

In this case, `ushort` is used both as a pair of palette indices and as a pair of color attributes. `getColor` now returns a `TAttrPair`, so even though this compiles out-of-the-box, extended attributes will be lost in the implicit conversion to `ushort`.

The code above still works just like it did originally. It's only non-BIOS color attributes that don't produce the expected result. Because of the compatibility between `TAttrPair` and `ushort`, the following is enough to enable support for extended color attributes:

```diff
-    ushort cFrame, cTitle;
+    TAttrPair cFrame, cTitle;
```

Nothing prevents you from using different variables for palette indices and color attributes, which is what should actually be done. The point of backward-compatibility is the ability to support new features without changing the program's logic, that is, minimizing the risk of increasing code complexity or introducing bugs.
</details>
