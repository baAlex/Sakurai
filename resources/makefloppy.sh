dd if=/dev/zero of=./sakurai.img count=1440 bs=1k
mkfs.msdos ./sakurai.img

mlabel -i ./sakurai.img ::sakurai

mcopy -i ./sakurai.img ./sakurai.exe ::/
mcopy -i ./sakurai.img ./game.dll ::/
mcopy -i ./sakurai.img ./README.md ::/readme.txt

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

	mcopy -i ./sakurai.img ./assets/sprite1.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/sprite2.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/player.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-a.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-b.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-c.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-d.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-e.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-f.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/type-g.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/port-a.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/port-b.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/font1.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/font2.jvn ::/assets/
