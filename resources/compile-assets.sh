#!/bin/bash

mkdir -p "./objects/"
mkdir -p "./assets/"

ruby ./source/sdk/background.rb "./assets/bkg1.raw"  "./assets-dev/bkg1.bmp"
ruby ./source/sdk/background.rb "./assets/bkg2.raw"  "./assets-dev/bkg2.bmp"
ruby ./source/sdk/background.rb "./assets/bkg3.raw"  "./assets-dev/bkg3.bmp"
ruby ./source/sdk/background.rb "./assets/bkg4.raw"  "./assets-dev/bkg4.bmp"
ruby ./source/sdk/background.rb "./assets/title.raw" "./assets-dev/title.bmp"

ruby ./source/sdk/palette.rb "./assets/palette.raw" "./assets-dev/palette.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/fx1.asm"\
    "./assets-dev/fx1.001.bmp"\
    "./assets-dev/fx1.002.bmp"\
    "./assets-dev/fx1.003.bmp"\
    "./assets-dev/fx1.004.bmp"\
    "./assets-dev/fx1.005.bmp"\
    "./assets-dev/fx1.006.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/fx2.asm"\
    "./assets-dev/fx2.001.bmp"\
    "./assets-dev/fx2.002.bmp"\
    "./assets-dev/fx2.003.bmp"\
    "./assets-dev/fx2.004.bmp"\
    "./assets-dev/fx2.005.bmp"\
    "./assets-dev/fx2.006.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/sprite1.asm" "./assets-dev/sprite1.bmp"
ruby ./source/sdk/sprite.rb "linear" "./objects/sprite2.asm" "./assets-dev/sprite2.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/sayori.asm"\
    "./assets-dev/sayori.001.bmp"\
    "./assets-dev/sayori.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/kuro.asm"\
    "./assets-dev/kuro.001.bmp"\
    "./assets-dev/kuro.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/ui-ports.asm" "./assets-dev/ui-ports.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/ui-items.asm"\
    "./assets-dev/ui-items.001.bmp"\
    "./assets-dev/ui-items.002.bmp"\
    "./assets-dev/ui-items.003.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/ferment.asm"\
    "./assets-dev/ferment.001.bmp"\
    "./assets-dev/ferment.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/windeye.asm"\
    "./assets-dev/windeye.001.bmp"\
    "./assets-dev/windeye.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/kingpin.asm"\
    "./assets-dev/kingpin.001.bmp"\
    "./assets-dev/kingpin.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/phibia.asm"\
    "./assets-dev/phibia.001.bmp"\
    "./assets-dev/phibia.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/destroyr.asm"\
    "./assets-dev/destroyr.001.bmp"\
    "./assets-dev/destroyr.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/viridi.asm"\
    "./assets-dev/viridi.001.bmp"\
    "./assets-dev/viridi.002.bmp"

ruby ./source/sdk/sprite.rb "linear" "./objects/ni.asm"\
    "./assets-dev/ni.001.bmp"\
    "./assets-dev/ni.002.bmp"

ruby ./source/sdk/sprite.rb "font" "./objects/font1.asm"  "./assets-dev/font1.bmp"
ruby ./source/sdk/sprite.rb "font" "./objects/font1a.asm" "./assets-dev/font1a.bmp"
ruby ./source/sdk/sprite.rb "font" "./objects/font2.asm"  "./assets-dev/font2.bmp"

###

fasm "./objects/fx1.asm" "./assets/fx1.jvn"
fasm "./objects/fx2.asm" "./assets/fx2.jvn"
fasm "./objects/sprite1.asm" "./assets/sprite1.jvn"
fasm "./objects/sprite2.asm" "./assets/sprite2.jvn"
fasm "./objects/sayori.asm" "./assets/sayori.jvn"
fasm "./objects/kuro.asm" "./assets/kuro.jvn"
fasm "./objects/ui-ports.asm" "./assets/ui-ports.jvn"
fasm "./objects/ui-items.asm" "./assets/ui-items.jvn"
fasm "./objects/ferment.asm" "./assets/ferment.jvn"
fasm "./objects/windeye.asm" "./assets/windeye.jvn"
fasm "./objects/kingpin.asm" "./assets/kingpin.jvn"
fasm "./objects/phibia.asm" "./assets/phibia.jvn"
fasm "./objects/destroyr.asm" "./assets/destroyr.jvn"
fasm "./objects/viridi.asm" "./assets/viridi.jvn"
fasm "./objects/ni.asm" "./assets/ni.jvn"
fasm "./objects/font1.asm" "./assets/font1.jvn"
fasm "./objects/font1a.asm" "./assets/font1a.jvn"
fasm "./objects/font2.asm" "./assets/font2.jvn"
