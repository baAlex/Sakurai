
Sakurai
=======
[![Ubuntu CI](https://github.com/baAlex/sakurai/workflows/Ubuntu/badge.svg)](https://github.com/baAlex/sakurai/actions?query=workflow%3AUbuntu)
[![Windows CI](https://github.com/baAlex/sakurai/workflows/Windows/badge.svg)](https://github.com/baAlex/sakurai/actions?query=workflow%3AWindows)

Turn based role-playing game for real mode DOS. **Currently in development... kinda**.

![](https://raw.githubusercontent.com/baAlex/Sakurai/master/resources/media/banner-gameplay.png)

____

This file is intended for development purposes, if isn't your case please check:
- Game website at: [baalex.itch.io/tanakas-magical-business][1]
- Distributable readme in: [/resources/distributable-readme.txt][2]


Compilation
-----------

### Modern platforms
Install `sdl2` and `cmake`:
 - In Ubuntu with: `apt install libsdl2-dev`
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

### DOS
The compilation requires a modern platform, a Posix compatible OS, `gcc-ia16-elf` and `fasm`.
 - In Ubuntu you can install them with: `apt install gcc-ia16-elf fasm`
 - Other platforms should work in a similar fashion.

A [bash script][3] covers the process, run:
```
./resources/compile-dos.sh
```

Is not an elegant process since the engine is written in Assembly and the game code in C, on top of that it compiles for real mode in a medium memory model. No extender will save us :(.

### Assets
Only `ruby` is required. The same assets works for both modern platforms and DOS.

A [ruby script][4] covers the process:
```
ruby ./resources/compile-assets.rb
```


License
-------
Copyright (c) 2020 Alexander Brandt.

Source code under MIT License.

Files in folders "assets" and "assets-dev" distributed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. For more information see https://creativecommons.org/licenses/by-nc-sa/4.0/.

____

[1]: https://baalex.itch.io/tanakas-magical-business
[2]: ./resources/distributable-readme.txt
[3]: ./resources/compile-dos.sh
[4]: ./resources/compile-assets.rb
