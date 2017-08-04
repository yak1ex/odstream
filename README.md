odstream
========

C++ iostream using `OutputDebugString()` Win32 API.

Usage
-----

### header-only or not ###

This library can use as a header-only library or an ordinary library. By default, header-only usage is chosen because of no need of preparation. To use as an ordinary library, you need to define `YAK_DEBUG_NO_HEADER_ONLY` when including `odstream.hpp`, 

### preparation (for not header-only usage) ###

#### Unix-ish ###

    make

builds `libodstream.a`. Include `odstream.hpp` and link with `libodstream.a`

### Windows(MSVC) ###

    nmake

builds `odstream.lib` and `odstream_s.lib`. `odstream.lib` is for `-MD` runtime option and `odstream_s.lib` for `-MT` runtime option. Include `odstream.hpp` and link with an appropriate library.

### code ###

1. Direct style

        yak::debug::ods() << "Trace messages"; yak::debug::ods().flush();
        // yak::debug::ods() << "Trace messages" << std::flush; // requires #include <iostream>

    If `DEBUG` macro is defined, the message is output by `OutputDebugString()` when a flush occurs. Otherwise, ignored.  Instead of calling `yak::debug::ods().flush()`, you can flush by using stream manipulators like `std::flush` and `std::endl`. However, such manipulators require including `iostream` header reagardless whether `DEBUG` is defined or not, and it might cause a burden of executable size.

2. Macro style

        ODS_NOFLUSH(<< "Trace messages1"); // without flush
        ODS(<< "Trace messages2"); // with flush

    This is simliar as direct style but needs not including `iostream` when `DEBUG` is not defined, even if manipulators are used.

    To tell the truth, including `iostream` when `DEBUG` is defined is done by `odstream.hpp` header, so you need not it by yourself. To define `ODSTREAM_NO_INCLUDE_IOSTREAM` suppresses this behavior.

Notes
-----

### executable size ###

Here are examples of executable size. These vary in different options and environments, especially for header-only usage.

#### GCC-6.3.0 with -O2 on Cygwin, stripped ####

Cygwin     |header-only<br>DEBUG|header-only<br>no DEBUG|no header-only<br>DEBUG|no header-only<br>no DEBUG|blank main
-----------|---------|---------|---------|---------|---------
normal     |  13,838 |   8,718 |  15,886 |   8,718 |   8,718
static link| 934,414 | 886,798 | 934,926 |   8,718 |   8,718

#### VC14(VS2015) ####

VC14       |header-only<br>DEBUG|header-only<br>no DEBUG|no header-only<br>DEBUG|no header-only<br>no DEBUG|blank main
-----------|---------|---------|---------|---------|---------
normal     |  27,648 |   8,704 |  28,672 |   8,704 |   8,192
static link| 179,200 |  74,752 | 179,712 |  74,752 |  74,752

#### VC11(VS2012) ####

VC11       |header-only<br>DEBUG|header-only<br>no DEBUG|no header-only<br>DEBUG|no header-only<br>no DEBUG|blank main
-----------|---------|---------|---------|---------|---------
normal     |  23,040 |  12,800 |  25,088 |   6,144 |   6,144
static link| 153,088 |  74,752 | 154,112 |  52,224 |  52,224

### namespace alias and implementation switch ###

To be described.

License
-------

This software is distributed under the terms of a zlib/libpng License.

> Copyright (c) 2013,2017 Yak! / Yasutaka ATARASHI
>
> This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
>
> Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
>
> 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
>
> 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
>
> 3. This notice may not be removed or altered from any source distribution.
