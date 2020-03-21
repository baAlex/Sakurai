dd if=/dev/zero of=./sakurai.img count=1440 bs=1k
mkfs.msdos ./sakurai.img

mcopy -i ./sakurai.img ./sakurai.exe ::/
mcopy -i ./sakurai.img ./game.dll ::/

mmd -i ./sakurai.img ::/assets
mcopy -i ./sakurai.img ./assets/palette.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg1.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg2.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg3.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg4.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg5.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg6.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg7.raw ::/assets/
mcopy -i ./sakurai.img ./assets/bkg8.raw ::/assets/
