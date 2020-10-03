#
# MIT License
#
# Copyright (c) 2020 Alexander Brandt
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


# [sprite.rb]
# - Alexander Brandt 2020

require_relative "shared.rb"

require_relative "sprite/irframe.rb"
require_relative "sprite/irrow.rb"
require_relative "sprite/irpixel.rb"

require_relative "sprite/soup.rb"
require_relative "sprite/read.rb"
require_relative "sprite/write.rb"


def Sprite(type, filename_output, inputs)

	output = File.open(filename_output, "wb")

	font_sheet = false

	if type == "linear" then font_sheet = false end
	if type == "font"   then font_sheet = true end

	# Read frames
	if font_sheet == false then
		frame_list = ReadFramesFromFiles(list: inputs)
	else
		frame_list = ReadFramesFromFontSheet(filename: inputs[0])
	end

	# Create an optimized 'data-soup', an array basically
	data_soup = DataSoupFromFrames(list: frame_list)

	# Write output
	asm = WriteAsm(font_sheet, frame_list, data_soup)
	output.print(asm)

	output.close()
end


if __FILE__ == $PROGRAM_NAME
	(ARGV.length > 2) ? Sprite(ARGV[0], ARGV[1], ARGV[2..]) : raise("Usage blah... blah... blah...")
end
