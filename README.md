
Sakurai
=======

Turn based role-playing game for real mode DOS. **Currently in development**.

![](https://raw.githubusercontent.com/baAlex/Sakurai/master/resources/media/banner-gameplay.png)

____

This file is intended for development purposes, if isn't your case please check:
- Game website at: [baalex.itch.io/tanakas-magical-business](https://baalex.itch.io/tanakas-magical-business)
- Distributable readme in: [/resources/distributable-readme.txt](https://github.com/baAlex/Sakurai/blob/master/resources/distributable-readme.txt)


Compilation
-----------
The compilation requires `fasm`, `bcc`, `ruby` and `ninja`.
On Ubuntu you can install these with:
```
sudo apt install fasm bcc ruby ninja-build
```

To clone and compile the repository with:
```
git clone https://github.com/baAlex/Sakurai.git
cd Sakurai
ninja -f ./resources/build.ninja
```

### About the Cmake file
It happens that `bcc` don't perform many checks and most of the time invalid code ends compiled without any warning. The Cmake file is there just to compile the game logic code with a modern toolset. Note that the engine (wrote in assembly) do not support any other platform rather than DOS.


Project name
------------
«Tanaka's magical business» is the project name, «Sakurai» the codename. There is no legal implication in any of the two, and is not the idea make a distinction between modules or editions. Both names refer to the same thing.

The codename is shorter, thus used in development. The name longer and explicit in a hope of allowing translations without meaning loss. I know that everybody will use the English name and also that there is a trend of video games being named with catchy single words. Well, I went old-school here hoping to see a «Negocios mágicos de Tanaka» some day.


License
-------
Copyright (c) 2020 Alexander Brandt.

Source code under MIT License.

Files in folders "assets" and "assets-dev" distributed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. For more information see https://creativecommons.org/licenses/by-nc-sa/4.0/.
