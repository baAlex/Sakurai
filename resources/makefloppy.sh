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

	mcopy -i ./sakurai.img ./assets/fx1.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/fx2.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/player.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/enemy-a.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-b.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-c.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-d.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-e.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-f.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/enemy-g.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/port-a.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/port-b.jvn ::/assets/

	mcopy -i ./sakurai.img ./assets/font1.jvn ::/assets/
	mcopy -i ./sakurai.img ./assets/font2.jvn ::/assets/
