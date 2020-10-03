
Sakurai
=======
[![Ubuntu CI](https://github.com/baAlex/sakurai/workflows/Ubuntu/badge.svg)](https://github.com/baAlex/sakurai/actions?query=workflow%3AUbuntu)
[![Windows CI](https://github.com/baAlex/sakurai/workflows/Windows/badge.svg)](https://github.com/baAlex/sakurai/actions?query=workflow%3AWindows)

> 20/10/2: Window CI fails due to a [vcpkg issue](https://github.com/microsoft/vcpkg/issues/13856)

Turn based role-playing game for real mode DOS. **Currently in development**.

![](https://raw.githubusercontent.com/baAlex/Sakurai/master/resources/media/banner-gameplay.png)

____

This file is intended for development purposes, if isn't your case please check:
- Game website at: [baalex.itch.io/tanakas-magical-business](https://baalex.itch.io/tanakas-magical-business)
- Distributable readme in: [/resources/distributable-readme.txt](https://github.com/baAlex/Sakurai/blob/master/resources/distributable-readme.txt)


Compilation
-----------

### Modern platforms
Install `sdl2` and `cmake`:
 - In Ubuntu with: `sudo apt install libsdl2-dev`
 - Using `vcpkg` the same in Windows: `vcpkg install sdl2:x64-windows`
 - Other platforms should work in a similar fashion.

Finally clone and compile the repository with:
```
git clone https://github.com/baAlex/Sakurai.git sakurai
cd sakurai
git submodule init
git submodule update

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

For a release build the same procedure as above except for:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Assets and DOS
Sadly there is no elegant way of compile the assets or the DOS executable as both require a compilation involving different stages and programs. For the assets alone I recommend download them from [baalex.itch.io/tanakas-magical-business](https://baalex.itch.io/tanakas-magical-business).

In any case, the requirements are `fasm`, `bcc` and `ruby`. The whole process is contained in the file `./resources/compile-dos.sh` (functional only in Posix systems).


License
-------
Copyright (c) 2020 Alexander Brandt.

Source code under MIT License.

Files in folders "assets" and "assets-dev" distributed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. For more information see https://creativecommons.org/licenses/by-nc-sa/4.0/.
