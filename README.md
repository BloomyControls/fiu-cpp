# FIU Serial Driver

C/C++ driver for communicating with a Bloomy Controls Fault Insertion Units over
RS-485.

## Features

- Modern, native C++20 library
- Supports Windows and Linux
- Implements all commands and queries supported by the FIU
- Exceptionless error handling (see below)
- C wrapper (`include/bci/fiu/CInterface.h`) for use in C and other languages
- Easy inclusion in CMake projects

> [!NOTE]
> Exceptions are not handled internally unless it is necessary to do so. Any
> exceptions thrown by this library are considered critical exceptional
> situations, such as a failure to allocate memory.

## Example Usage (C++)

TODO

## Example Usage (C)

TODO

## Building

To build the driver as a shared library (.DLL or .so), first configure CMake,
then execute the build system. This project can also be built with Visual
Studio.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build build
```

## Adding to a CMake Project

In your CMakeLists.txt, fetch and link with the driver:

```cmake
FetchContent_Declare(fiu-cpp
  GIT_REPOSITORY https://github.com/BloomyControls/fiu-cpp.git
  GIT_TAG v1.0.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fiu-cpp)

target_link_libraries(my_program PRIVATE bci::fiu)
```

If you have already installed the driver to a standard location on your system,
you can use `find_package` instead:

```cmake
find_package(fiu-cpp 1.0.0 REQUIRED)

target_link_libraries(my_program PRIVATE bci::fiu)
```

> [!TIP]
> On Windows, the MSI installer will register the CMake package in the Windows
> registry, so the above example will build and link successfully. However,
> because the DLL is not located in the `PATH` environment variable, the DLL
> will fail to be found at runtime. To fix this, you can add a CMake command to
> copy the DLL to the build directory and you can add an install target to copy
> the DLL when installing your target.

## Dependencies

This library does not have any runtime dependencies aside from the standard
C/C++ runtime libraries on your system. However, it does have some build
dependencies. If these are installed on your system, CMake will find and use
them if your installed versions are compatible. Otherwise, it will automatically
fetch and build them as part of the build. The build dependencies are:

- Boost v1.88.0

These are linked statically, regardless of the build configuration.

## License

Copyright (c) 2026, Bloomy Controls, Inc. All rights reserved.

This software is distributed under the BSD-3-clause license. See the LICENSE
file or <https://opensource.org/license/BSD-3-Clause> for details.
