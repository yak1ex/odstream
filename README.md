odstream
========

C++ iostream using `OutputDebugString()` Win32 API. A bit part of this code uses C++11 features, which are explicit instantiation declaration and default template parameter for function templates. Thus, VC11(VS2012) can not be used.

Usage
-----

Typical usage: header only, macro style, with lock, and appropriate charcter type

    #define DEBUG // if DEBUG is not defined, they become no-ops.
    #include "odstream.hpp"
    ODS(<< "TEST" << std::endl); // for char
    WODS(<< L"TEST" << std::endl); // for wchar_t


### header-only or not ###

This library can use as a header-only library or an ordinary library. By default, header-only usage is chosen because of no need of preparation. To use as an ordinary library, you need to define `YAK_DEBUG_NO_HEADER_ONLY` when including `odstream.hpp`,

### preparation (for not header-only usage) ###

#### Unix-ish ###

    make

builds `libodstream.a`. Include `odstream.hpp` and link with `libodstream.a`

### Windows(MSVC) ###

    nmake

builds `odstream.lib` and `odstream_s.lib`. `odstream.lib` is for `-MD` runtime option and `odstream_s.lib` for `-MT` runtime option. Include `odstream.hpp` and link with an appropriate library.

### style ###

1. Direct style

        *yak::debug::ods() << "Trace messages"; yak::debug::ods_flush();
        // *yak::debug::ods() << "Trace messages" << std::flush; // requires #include <iostream>

    If `DEBUG` macro is defined, the message is output by `OutputDebugString()` when a flush occurs. Otherwise, ignored.  Instead of calling `yak::debug::ods_flush()`, you can flush by using stream manipulators like `std::flush` and `std::endl`. However, such manipulators require including `iostream` header reagardless whether `DEBUG` is defined or not, and it might cause a burden of executable size.

    If you want to output messages regardless of whether `DEBUG` marcro is defined or not, you can use `yak::debug_yes::ods()`.

2. Macro style

        ODS_NOFLUSH(<< "Trace messages1"); // without flush
        ODS(<< "Trace messages2"); // with flush

    This is simliar as direct style but needs not including `iostream` when `DEBUG` is not defined, even if manipulators are used.

    To tell the truth, including `iostream` when `DEBUG` is defined is done by `odstream.hpp` header, so you need not it by yourself. To define `ODSTREAM_NO_INCLUDE_IOSTREAM` suppresses this behavior.

### char type ###

You can choose char type from `char` and `wchar_t` as follows:

    // default is 'char'
    *yak::debug::ods()      << "char type is 'char'"; yak::debug::ods_flush();
    *yak::debug::ods<char>() << "char type is 'char'"; yak::debug::ods_flush();
    *yak::debug::ods<wchar_t>() << L"char type is 'wchar_t'"; yak::debug::ods_flush<wchar_t>()
    // Macro style
    ODS(<< "char type is 'char'")
    ODS_NOFLUSH(<< "char type is 'char'")
    WODS(<< L"char type is 'wchar_t'")
    WODS_NOFLUSH(<< L"char type is 'wchar_t'")

### lock ###

This library uses CRITICAL_SECTION in Win32 API to avoid interleave of output in different expressions. An order of output by expressions is generally unspecified, but they are not interleaved. If you need not to use CRITICAL_SECTION, you can use underline-postfixed version of interfaces like `ods_()`.

    // Thread1
    ODS(<< "Output" << 1 << " continues" << std::endl);
    // Thread2
    *yak::debug::ods() << "Output" << 2 << " continues" << std::endl;
    // They should not be interleaved, so results should be either of the belows:
    // Output1 continues
    // Output2 continues
    // or
    // Output2 continues
    // Output1 continues

Note that the lock is effective inside a full expression. If you have 2 or more full expressions, you may get interleaved results.

    // Thread1
    *yak::debug::ods() << "Output"; *yak::debug::ods() << 1;
    *yak::debug::ods() << " continues"; *yak::debug::ods() << std::endl;
    // Thread2
    *yak::debug::ods() << "Output"; *yak::debug::ods() << 1;
    *yak::debug::ods() << " continues"; *yak::debug::ods() << std::endl;

Limitation
----------

A returned value by `ods()` is not always a pointer to `std::ostream`. Dereference and to use stream operator: `operator<<` can be always used but member functions can't. If you need to call member functions, you can use operator->(). However, considering lock lifetime, manipurator with stream operator chain might be better. On the other hand, more headers may be required even if `DEBUG` is not defined, as described above.

    *yak::debug::ods() << "OK";      // Always OK
    yak::debug::ods_flush();         // Always OK
    //(*yak::debug::ods()).flush();  // OK only if DEBUG is defined
    yak::debug::ods()->flush();      // Always OK but #include <iostream> is necessary
    *yak::debug::ods() << setw(5);   // Always OK but #include <iomanip> is necessary

Notes
-----

### executable size ###

Here are examples of executable size in bytes. These vary in different options and environments, especially for header-only usage.

#### GCC-6.4.0 with -O2 on Cygwin, stripped ####

Cygwin     |header-only<br>DEBUG|header-only<br>no DEBUG|no header-only<br>DEBUG|no header-only<br>no DEBUG|blank main
-----------|---------|---------|---------|---------|---------
normal     |  22,030 |   8,718 |  28,174 |  25,102 |   8,718
static link| 939,022 | 887,310 | 944,142 | 942,606 |   8,718

#### VC14(VS2015) ####

VC14       |header-only<br>DEBUG|header-only<br>no DEBUG|no header-only<br>DEBUG|no header-only<br>no DEBUG|blank main
-----------|---------|---------|---------|---------|---------
normal     |  43,520 |   8,704 |  43,520 |   9,728 |   8,192
static link| 203,264 |  86,528 | 203,264 | 117,760 |  86,016

### namespace alias and implementation switch ###

 [Namespaces and Library Versioning](http://www.gotw.ca/publications/mill21.htm) by Hurb Sutter describes namespace alias and library versioning as "Option 2(d): Use an alias" at the bottom of the article. This is a convenient way for library versioning or option switching.

 Because actual names of entities are kept so that we can link without re-compile even though different configuration co-exists in object files and can refer needed entities directly if necessary, like `yak::debug_yes::ods()`. As desribed the article, there are limitations for namespace alias, such as specialization, re-opening namespace, and so on. These limitations are not concerns of this library. Thus, namespace alias is enough for this library.

 If these limitations are actual concerns, we can use inline namespace introduced in C++11 like the followings:
```cpp
#ifdef DEBUG
#define INLINE_YES inline
#define INLINE_NO
#else
#define INLINE_YES
#define INLINE_NO inline
#endif
namespace yak {
  namespace debug {
    INLINE_YES namespace yes {
    }
    INLINE_NO namespace no {
    }
  }
}
```

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
