## Overview

This is a CMake template project for building a Console application using C++. All
necessary libraries are downloaded and build automatically using the vcpkg
package manager. You only need an appropriate compiler (Visual Studio build
tools on Windows, g++ on Linux), CMake, Ninja and of course vcpkg installed on
your machine. With this template, you can easily create 32-/64-bit debug/release
binaries using static/dynamic linkage for Windows as well as Linux. All
dependencies that are needed to run the executable are automatically copied to
the install folder during the CMake install target. By having a self contained
install folder, you get a portable executable, that can easily be shared and
run across systems. The project also includes a github workflow, that builds
the application on a github runner and uploads the binaries in the artifacts
section of the github action.

## Getting Started

- First make sure that all tools (cl.exe/g++, CMake, Ninja, vcpkg) are on your
  PATH and that the VCPKG_ROOT environment variable is set. Then clone the
  repository:

  `git clone https://github.com/Cortana4/CMake-Cpp-Qt-Application.git`

- Rename the cloned folder, cd into it and delete the .git folder.

- Start fresh by initializing a new repository:

  `git init`<br>
  `git branch -M main`<br>
  `git remote add origin <URL>`<br>
  `git add --all`<br>
  `git commit -m "initial commit"`<br>
  `git push -u origin main`

- Now you can list all available presets for yor machine by running the
  following command in the root of your repository:

  `cmake --list-presets=all .`

- Choose a configure preset and configure the cache:

  `cmake --preset=<configure preset>`

- Choose the same preset for build:

  `cmake --build --preset=<build preset>`

- Or build and install in one step:

  `cmake --build --preset <build preset> --target install`

#### Windows Example

`cmake --preset=windows-dynamic-x64-release`<br>
`cmake --build --preset=windows-dynamic-x64-release --target install`

#### Linux Example

`cmake --preset=linux-static-x64-release`<br>
`cmake --build --preset=linux-static-x64-release --target install`

## Troubleshoot

- The path to your repository must not contain whitespaces or special chars.
- use a CMake version between 3.27 and 3.31: in the latest version (4.0 and
  above) compatibility with older versions was dropped and some vcpkg packages
  might depend on older features.

#### Windows specific

- The path to your repository must be as short as possible.
- Update PowerShell to latest version (especially if pkgconf fails to build).

#### Linux specific

- Check logs if packages from system package manager must be installed.
