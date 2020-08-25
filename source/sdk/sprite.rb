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


def main(args)

	pingpong = false
	font_sheet = false

	for a in args do
		if a == "+pingpong" then pingpong = true end
		if a == "+linear" then   pingpong = false end
		if a == "+font" then     font_sheet = true end
	end

	args.delete("+pingpong")
	args.delete("+linear")
	args.delete("+font")

	# Read frames
	if font_sheet == false then
		frame_list = ReadFramesFromFiles(list: ARGV)
	else
		frame_list = ReadFramesFromFontSheet(filename: ARGV[0])
	end

	# Create an optimized 'data-soup', an array basically
	data_soup = DataSoupFromFrames(list: frame_list)

	# Write output
	WriteAsm(pingpong, font_sheet, frame_list, data_soup)
end


(ARGV.length > 0) ? main(ARGV) : raise("No Bmp input specified")
