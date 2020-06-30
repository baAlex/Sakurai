
Sakurai
=======

Turn based role-playing game for real mode DOS.
Submission for [MS-DOS game jam #2](https://itch.io/jam/dos-game-jam-2).

**Currently in development**.

![](https://raw.githubusercontent.com/baAlex/Sakurai/master/resources/media/banner-gameplay.png)


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


License
-------
Copyright (c) 2020 Alexander Brandt.

Source code under MIT License.

Files in folders "assets" and "assets-dev" distributed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. For more information see https://creativecommons.org/licenses/by-nc-sa/4.0/.
