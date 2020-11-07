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
    * [Turbo Vision as a CMake subproject](#build-cmake)
* [Features](#features)
* [API changes](#apichanges)
* [Unicode support](#unicode)

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
cmake --build ./build # or `cd ./build && make`
```

The above produces the following files:

* `libtvision.a`, which is the Turbo Vision library.
* The demo applications `hello`, `tvdemo`, `tvedit`, `tvdir`, which were bundled with the original Turbo Vision (although some of them have a few improvements).
* The demo applications `mmenu` and `palette` from Borland's Technical Support.
* `tvhelp`, the Turbo Vision Help Compiler.

The library and executables can be found in `./build`. 

The build requirements are:

* A compiler supporting C++ 17.
* `libncursesw` (note the 'w').
* `libgpm` (for mouse support on the Linux console) (optional).

The minimal command line required to build a Turbo Vision application (e.g. `hello.cpp` with GCC) from this project's root is:

```sh
g++ -std=c++17 -o hello hello.cpp ./build/libtvision.a -Iinclude -lncursesw -lgpm
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

* Borland C++ 4.52 with the Borland DOS PowerPack.
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

### Turbo Vision as a CMake subproject (not Borland C++)

If you choose the CMake build system for your application, and you place Turbo Vision as a submodule in your repository, you can easily configure your application to link against Turbo Vision:

```cmake
add_subdirectory(tvision) # Assuming Turbo Vision is in the 'tvision' directory.
target_link_libraries(my_application tvision)
```

`<tvision/tv.h>` will be available in your application's include path during compilation. Additionally, your application will be linked against the necessary libraries (Ncurses, GPM...) automatically.

<div id="features"></div>

## Features

### Modern platforms (not Borland C++)

* UTF-8 support both in terminal I/O and the API. You can try Unicode support out in the `tvedit` and `tvdemo` applications.
* 16 colors.
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
* `TDrawBuffer` is no longer a fixed-length array. The equivalent of `sizeof(TDrawBuffer)/sizeof(ushort)` is the `.lenght()` method.
* `TTextDevice` is now buffered, so if you were using `otstream` you may have to send `std::flush` or `std::endl` through it for `do_sputn` to be invoked.
* The `buttons` field in `evMouseUp` events is no longer empty. It now indicates which button was released.
* `TRect` methods `move`, `grow`, `intersect` and `Union` now return `TRect&` instead of being `void`, so that they can be chained.
* `TOutlineViewer` now allows the root node to have siblings.
* New function `popupMenu(TPoint where, TMenuItem &aMenu, TGroup *receiver = 0)` which spawns a `TMenuPopup` on the desktop. See `source/tvision/popupmnu.cpp`.
* New class `TStringView`, which is a clone of `std::string_view`. You shouldn't need it unless you are programming in Borland C++, which has no `std::string_view`.
* Many methods which originally had null-terminated string parameters now receive `TStringView` instead. `TStringView` is compatible with `std::string_view`, `std::string` and `const char *` (even `nullptr`).
* New class `TSpan<T>`, a generic (and non-const) version of `TStringView`, inspired by `std::span`.
* New classes `TDrawSurface` and `TSurfaceView`, see `<tvision/surface.h>`.
* Unicode support, see below.

## Screenshots

You will find some screenshots [here](https://github.com/magiblot/tvision/issues/7). Feel free to add your own!

## Contributing

If you know of any Turbo Vision applications whose source code has not been lost and that could benefit from this, let me know.

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

So a Unicode character can be retrieved from `TEvent` in the following way:

```c++
switch (ev.keyDown.keyCode) {
    // ...
    default: {
        std::string_view s {ev.keyDown.text, ev.keyDown.textLength};
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

A new `TScreenCell` type is defined in `<tvision/scrncell.h>` which is capable of holding a UTF-8 sequence in addition to extended attributes (bold, underline, italic...). However, you should not write text into a `TScreenCell` directly but make use of Unicode-aware API functions instead.

### Text display rules

A character provided as argument to any of the Turbo Vision API functions that deal with displaying text is interpreted as follows:

* Non-printable characters in the range `0x00` to `0xFF` are interpreted as characters in the active codepage. For instance, `0x7F` is displayed as `⌂` and `0xF0` as `≡` if using CP437. As an exception, `0x00` is always displayed as a regular space. These characters are all one column wide.
* Character sequences which are not valid UTF-8 are interpreted as sequences of characters in the current codepage, as in the case above.
* Valid UTF-8 sequences with a display width other than one are taken care of in a special way, see below.

For example, the string `"╔[\xFE]╗"` may be displayed as `╔[■]╗`. This means that box-drawing characters can be mixed with UTF-8 in general, which is useful for backward compatibility. If you rely on this behaviour, though, you may get unexpected results: for instance, `"\xC4\xBF"` is a valid UTF-8 sequence and is displayed as `Ŀ` instead of `─┐`.

One of the issues of Unicode support is the existence of [multi-width](https://convertcase.net/vaporwave-wide-text-generator/) characters and [combining](https://en.wikipedia.org/wiki/Combining_Diacritical_Marks) characters. This conflicts with Turbo Vision's original assumption that the screen is a grid of cells occupied by a single character each. Nevertheless, these cases are handled in the following way:

* Multi-width characters can be drawn anywhere on the screen and nothing bad happens if they overlap partially with other characters.
* Zero-width characters are always drawn as `�`, so they become one column wide.
* No notable graphical glitches will occur as long as your terminal emulator respects character widths as measured by `wcwidth`.

Here is an example of such characters:
![Wide character display](https://user-images.githubusercontent.com/20713561/89314567-18e16400-d67a-11ea-950f-d4c35d40e5f7.png)

### Unicode-aware API functions

The usual way of writing to the screen is by using `TDrawBuffer`. A few methods have been added and others have changed their meaning:

```c++
void TDrawBuffer::moveChar(ushort indent, char c, ushort attr, ushort count);
void TDrawBuffer::putChar(ushort indent, ushort c);
```
`c` is always interpreted as a character in the active codepage.

```c++
void TDrawBuffer::moveStr(ushort indent, TStringView str, ushort attr);
void TDrawBuffer::moveCStr(ushort indent, TStringView str, ushort attrs);
```
`str` is interpreted according to the rules exposed previously.

```c++
ushort TDrawBuffer::moveStr(ushort indent, TStringView str, ushort attr, ushort width, ushort begin=0); // New
```
`str` is interpreted according to the rules exposed previously. However:
* `width` specifies the maximum number of display columns that should be read from `str`.
* `begin` specifies the number of display columns that should be skipped at the beginning of `str`. This is useful for horizontal scrolling. If `begin` is in the middle of a multi-width character, the remaining positions in that character are filled with spaces.

The return value is the number of display columns that were actually filled with text.

```c++
void TDrawBuffer::moveBuf(ushort indent, const void *source, ushort attr, ushort count);
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
#ifdef __BORLANDC__
size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, TCellAttribs attr);
#else
template <class Attr>
size_t TText::fill(TSpan<TScreenCell> cells, TStringView text, Attr &&attr);
#endif
bool TText::eat(TSpan<TScreenCell> cells, size_t &i, TStringView text, size_t &j);
void TText::next(TStringView text, size_t &index, size_t &width);
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

![Unicode Hello](https://user-images.githubusercontent.com/20713561/89353910-bd35cb80-d6b7-11ea-9ad6-0f608e5ae01c.png)

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

* `TDrawBuffer::moveStr(ushort, const char *, ushort)` takes a null-terminated string. In order to pass a substring of the current line, a copy is made into the array `s`, at the risk of a [buffer overrun](https://github.com/magiblot/tvision/commit/8aa2bf4af4474b85e86e340b08d7c56081b68986). The case where the line does not fit into `s` is not handled, so at most `maxLineLenght` characters will be copied. What's more, a multibyte character near position `maxLineLength` could be copied incompletely and become garbage when displayed.
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
The overload of `moveStr` used here is `TDrawBuffer::moveStr(ushort indent, TStringView str, ushort attr, ushort width, ushort begin)`. This function not only provides Unicode support, but also helps us write cleaner code and overcome some of the limitations previously present:

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
- [x] `THelpViewer` ([`81066ee5`](https://github.com/magiblot/tvision/commit/81066ee5c05496612dfcd9cf75df5702cbfb9679), [`8c7dac2a`](https://github.com/magiblot/tvision/commit/8c7dac2a61000f17e09cc31ebbb58b030f95c0e5)).
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
