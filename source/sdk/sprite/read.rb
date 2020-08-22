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


# [sprite/read.rb]
# - Alexander Brandt 2020

require_relative "irrow.rb"


def ReadFramesFromFiles(list:)

	frame_list = Array.new()
	previous_header = nil

	# Iterate files creating intermediate objects
	for filename in list do

		# Read BMP data
		file = File.open(filename, "rb")
		header = ReadBmpHeader(file)

		if header[:bpp] != 8 then
			raise("Only 8 bit indexed images supported")
		end

		if previous_header != nil then
			if previous_header[:width] != header[:width] ||
			   previous_header[:height] != header[:height] then
				raise("Animation frames can't have different dimensions")
			end
		end

		bmp_data = ReadBmpIndexedData(header, file)
		file.close()

		# Create intermediate objects
		irframe = IRFrame.new(no: frame_list.size)

		for r in 0...header[:height] do

			irrow = irframe.new_row(y: r)

			for c in 0...header[:width] do
				irrow.new_pixel(value: bmp_data[header[:width] * r + c], x: c)
			end
		end

		irframe.purge()
		frame_list.push(irframe)

		# Preparations for next frame
		previous_header = header
	end

	return frame_list
end


def ReadFramesFromFontSheet(filename:)

	frame_list = Array.new()

	# Read BMP data
	file = File.open(filename, "rb")
	header = ReadBmpHeader(file)

	if header[:bpp] != 8 then
		raise("Only 8 bit indexed images supported")
	end

	bmp_data = ReadBmpIndexedData(header, file)
	file.close()

	# Create frames subdividing the image by 16x16
	# (to get our 256 ascii characters)
	char_width = header[:width] / 16
	char_height = header[:height] / 16
	offset = 0

	while 1 do

		irframe = IRFrame.new(no: frame_list.size)

		for r in 0...char_height do

			irrow = irframe.new_row(y: r)

			for c in 0...char_width do
				irrow.new_pixel(value: bmp_data[(header[:width]) * r + offset + c], x: c)
			end
		end

		irframe.purge()
		frame_list.push(irframe)

		# Preparations for next frame
		offset += char_width

		if (offset % header[:width]) == 0 then
			offset += (char_height - 1) * header[:width]
		end

		if frame_list.size == 255 || # !!!
		   offset > (header[:width] * header[:height]) - (char_width) then
			break
		end

		# TODO, the modulo in the engine allows 255 frames?
	end

	return frame_list
end
