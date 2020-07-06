
Sakurai
=======

Turn based role-playing game for real mode DOS. **Currently in development**.

![](https://raw.githubusercontent.com/baAlex/Sakurai/master/resources/media/banner-gameplay.png)

____

What follows in this file is intended for development purposes. If is not the case please check:
- Distributable readme in: [resources/distributable-readme.txt](https://github.com/baAlex/Sakurai/blob/master/resources/distributable-readme.txt)
- Game information at: [baalex.itch.io/tanakas-magical-business](https://baalex.itch.io/tanakas-magical-business)


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
It happens that BCC don't perform many checks and most of the time invalid code ends compiled whitout any warning. So, the Cmake file is there just to compile the files in a modern enviroment.

No modern platform is supported at the moment.


License
-------
Copyright (c) 2020 Alexander Brandt.

Source code under MIT License.

Files in folders "assets" and "assets-dev" distributed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International Public License. For more information see https://creativecommons.org/licenses/by-nc-sa/4.0/.
