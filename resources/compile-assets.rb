#!/bin/ruby

require_relative "../source/sdk/background.rb"
require_relative "../source/sdk/palette.rb"
require_relative "../source/sdk/sprite.rb"

Background("./assets/bkg1.raw", "./assets-dev/bkg1.bmp")
Background("./assets/bkg2.raw", "./assets-dev/bkg2.bmp")
Background("./assets/bkg3.raw", "./assets-dev/bkg3.bmp")
Background("./assets/bkg4.raw", "./assets-dev/bkg4.bmp")
Background("./assets/title.raw", "./assets-dev/title.bmp")

Palette("./assets/palette.raw", "./assets-dev/palette.bmp")

Sprite("linear", "./assets/fx1.jvn",
[
	"./assets-dev/fx1.001.bmp",
	"./assets-dev/fx1.002.bmp",
	"./assets-dev/fx1.003.bmp",
	"./assets-dev/fx1.004.bmp",
	"./assets-dev/fx1.005.bmp",
	"./assets-dev/fx1.006.bmp"
])

Sprite("linear", "./assets/fx2.jvn",
[
	"./assets-dev/fx2.001.bmp",
	"./assets-dev/fx2.002.bmp",
	"./assets-dev/fx2.003.bmp",
	"./assets-dev/fx2.004.bmp",
	"./assets-dev/fx2.005.bmp",
	"./assets-dev/fx2.006.bmp"
])

Sprite("linear", "./assets/sprite1.jvn", ["./assets-dev/sprite1.bmp"])
Sprite("linear", "./assets/sprite2.jvn", ["./assets-dev/sprite2.bmp"])

Sprite("linear", "./assets/sayori.jvn",
[
	"./assets-dev/sayori.001.bmp",
	"./assets-dev/sayori.002.bmp"
])

Sprite("linear", "./assets/kuro.jvn",
[
	"./assets-dev/kuro.001.bmp",
	"./assets-dev/kuro.002.bmp"
])

Sprite("linear", "./assets/ui-ports.jvn", ["./assets-dev/ui-ports.bmp"])

Sprite("linear", "./assets/ui-items.jvn",
[
	"./assets-dev/ui-items.001.bmp",
	"./assets-dev/ui-items.002.bmp",
	"./assets-dev/ui-items.003.bmp"
])

Sprite("linear", "./assets/ferment.jvn",
[
	"./assets-dev/ferment.001.bmp",
	"./assets-dev/ferment.002.bmp"
])

Sprite("linear", "./assets/windeye.jvn",
[
	"./assets-dev/windeye.001.bmp",
	"./assets-dev/windeye.002.bmp"
])

Sprite("linear", "./assets/kingpin.jvn",
[
	"./assets-dev/kingpin.001.bmp",
	"./assets-dev/kingpin.002.bmp"
])

Sprite("linear", "./assets/phibia.jvn",
[
	"./assets-dev/phibia.001.bmp",
	"./assets-dev/phibia.002.bmp"
])

Sprite("linear", "./assets/destroyr.jvn",
[
	"./assets-dev/destroyr.001.bmp",
	"./assets-dev/destroyr.002.bmp"
])

Sprite("linear", "./assets/viridi.jvn",
[
	"./assets-dev/viridi.001.bmp",
	"./assets-dev/viridi.002.bmp"
])

Sprite("linear", "./assets/ni.jvn",
[
	"./assets-dev/ni.001.bmp",
	"./assets-dev/ni.002.bmp"
])

Sprite("font", "./assets/font1.jvn", ["./assets-dev/font1.bmp"])
Sprite("font", "./assets/font1a.jvn", ["./assets-dev/font1a.bmp"])
Sprite("font", "./assets/font2.jvn", ["./assets-dev/font2.bmp"])
