#!/bin/bash

cp "./sakurai.exe" "./tanakas.exe"
cp "./resources/distributable-license.txt" "./license.txt"
cp "./resources/distributable-readme.txt" "./readme.txt"

root_files="./tanakas.exe
            ./game.dll
            ./readme.txt
            ./license.txt"

assets_files="./assets/bkg1.raw
              ./assets/bkg2.raw
              ./assets/bkg3.raw
              ./assets/bkg4.raw
              ./assets/enemy-a.jvn
              ./assets/enemy-b.jvn
              ./assets/enemy-c.jvn
              ./assets/enemy-d.jvn
              ./assets/enemy-e.jvn
              ./assets/enemy-f.jvn
              ./assets/enemy-g.jvn
              ./assets/font1.jvn
              ./assets/font1a.jvn
              ./assets/font2.jvn
              ./assets/fx1.jvn
              ./assets/fx2.jvn
              ./assets/kuro.jvn
              ./assets/palette.raw
              ./assets/sayori.jvn
              ./assets/sprite1.jvn
              ./assets/sprite2.jvn
              ./assets/title.raw
              ./assets/ui-items.jvn
              ./assets/ui-ports.jvn"

# Make a floppy image
dd if=/dev/zero of="./floppy-image.img" count=1440 bs=1k
mkfs.msdos "./floppy-image.img"
mlabel -i "./floppy-image.img" ::tanakas
mmd -i "./floppy-image.img" ::/assets

for f in $root_files
do
mcopy -i "./floppy-image.img" $f ::/
done

for f in $assets_files
do
mcopy -i "./floppy-image.img" $f ::/assets/
done

# Make zip
zip -9 "./tanakas.zip" "./floppy-image.img" $root_files $assets_files

# Bye!
rm "./tanakas.exe" "license.txt" "readme.txt" "./floppy-image.img"
