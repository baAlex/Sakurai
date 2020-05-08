dd if=/dev/zero of=./tanakas.img count=1440 bs=1k
mkfs.msdos ./tanakas.img

mlabel -i ./tanakas.img ::tanakas

mcopy -i ./tanakas.img ./tanakas.exe ::/
mcopy -i ./tanakas.img ./game.dll ::/
mcopy -i ./tanakas.img ./README.md ::/readme.txt

mmd -i ./tanakas.img ::/assets

	mcopy -i ./tanakas.img ./assets/palette.raw ::/assets/
	mcopy -i ./tanakas.img ./assets/bkg1.raw ::/assets/
	mcopy -i ./tanakas.img ./assets/bkg2.raw ::/assets/
	mcopy -i ./tanakas.img ./assets/bkg3.raw ::/assets/
	mcopy -i ./tanakas.img ./assets/bkg4.raw ::/assets/
	mcopy -i ./tanakas.img ./assets/title.raw ::/assets/

	mcopy -i ./tanakas.img ./assets/fx1.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/fx2.jvn ::/assets/

	mcopy -i ./tanakas.img ./assets/sayori.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/kuro.jvn ::/assets/

	mcopy -i ./tanakas.img ./assets/ui-ports.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/ui-items.jvn ::/assets/

	mcopy -i ./tanakas.img ./assets/enemy-a.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-b.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-c.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-d.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-e.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-f.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/enemy-g.jvn ::/assets/

	mcopy -i ./tanakas.img ./assets/font1.jvn ::/assets/
	mcopy -i ./tanakas.img ./assets/font2.jvn ::/assets/
