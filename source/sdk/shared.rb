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


# [shared.rb]
# - Alexander Brandt 2020

# https://ruby-doc.org/core-2.7.0/String.html#method-i-unpack
# https://www.fileformat.info/format/bmp/corion-rle8.htm

LITTLE_ENDIAN = 0
BIG_ENDIAN = 1

U8_T = 0
S8_T = 1
U16_T = 2
S16_T = 3
U32_T = 4
S32_T = 5


def UnpackU8(string)    return (string[0...1].unpack("C"))[0] end
def UnpackI8(string)    return (string[0...1].unpack("c"))[0] end

def UnpackU16LE(string) return (string[0...2].unpack("S<"))[0] end
def UnpackU16BE(string) return (string[0...2].unpack("S>"))[0] end
def UnpackS16LE(string) return (string[0...2].unpack("s<"))[0] end
def UnpackS16BE(string) return (string[0...2].unpack("s>"))[0] end

def UnpackU32LE(string) return (string[0...4].unpack("L<"))[0] end
def UnpackU32BE(string) return (string[0...4].unpack("L>"))[0] end
def UnpackS32LE(string) return (string[0...4].unpack("l<"))[0] end
def UnpackS32BE(string) return (string[0...4].unpack("l>"))[0] end


def ReadField(file, endianness, members)

	field = Hash.new()

	if endianness = LITTLE_ENDIAN then
		for i in members
			if i[0] == U8_T  then field[i[1]] = UnpackU8(file.read(1)) end
			if i[0] == S8_T  then field[i[1]] = UnpackS8(file.read(1)) end
			if i[0] == U16_T then field[i[1]] = UnpackU16LE(file.read(2)) end
			if i[0] == S16_T then field[i[1]] = UnpackS16LE(file.read(2)) end
			if i[0] == U32_T then field[i[1]] = UnpackU32LE(file.read(4)) end
			if i[0] == S32_T then field[i[1]] = UnpackS32LE(file.read(4)) end
		end
	else
		for i in members
			if i[0] == U8_T  then field[i[1]] = UnpackU8(file.read(1)) end
			if i[0] == S8_T  then field[i[1]] = UnpackS8(file.read(1)) end
			if i[0] == U16_T then field[i[1]] = UnpackU16BE(file.read(2)) end
			if i[0] == S16_T then field[i[1]] = UnpackS16BE(file.read(2)) end
			if i[0] == U32_T then field[i[1]] = UnpackU32BE(file.read(4)) end
			if i[0] == S32_T then field[i[1]] = UnpackS32BE(file.read(4)) end
		end
	end

	return field
end


def ReadBmpHeader(file)

	header = ReadField(file, LITTLE_ENDIAN, [
		[U16_T, :magic_word],
		[U32_T, :file_size],
		[U16_T, :reserved1],
		[U16_T, :reserved2],
		[U32_T, :data_offset],
		[U32_T, :info_size],

		# BITMAPINFOHEADER
		[S32_T, :width],
		[S32_T, :height],
		[U16_T, :planes], # Always 1
		[U16_T, :bpp], # Bits per pixel
		[U32_T, :compression],
		[U32_T, :data_size],
		[S32_T, :resolution_x],
		[S32_T, :resolution_y],
		[U32_T, :colors], # Zero or 2^n
		[U32_T, :important_colors]
	])

	if header[:magic_word] != 0x4D42 then
		raise("Not an Bmp file")
	end

	if header[:info_size] < 40 then
		raise("Bmp broken after bitmap header")
	end

	return header
end


def ReadBmpIndexedData(header, file)

	if header[:compression] > 1 then
		raise("Compresion algorithm not supported")
	end

	data = Array.new()
	file.seek(header[:data_offset], :SET)

	# Uncompressed
	if header[:compression] == 0 then

		width_padded = ((8 * header[:width]) / 32).ceil() * 4

		for row in 0...header[:height] do
			for col in 0...width_padded do

				pixel = UnpackU8(file.read(1))

				if col < header[:width] then
					data[(header[:height] - row - 1) * header[:width] + col] = pixel
				end
			end
		end
	end

	# 8 bits RLE
	if header[:compression] == 1 then

		x = 0
		y = header[:height] - 1

		while 1 do
			times = UnpackU8(file.read(1))

			# Copy pixel many times
			if times != 0 then
				pixel = UnpackU8(file.read(1))

				for i in 0...times do
					data[(header[:width] * y) + x] = pixel
					x += 1
				end
			end

			# Special instruction
			if times == 0 then
				instruction = UnpackU8(file.read(1))

				# EOL
				if instruction == 0 then
					x = 0
					y -= 1

				# EOF
				elsif instruction == 1 then
					return data

				# Delta
				elsif instruction == 2 then
					x += UnpackS8(file.read(1))
					y += UnpackS8(file.read(1))

				# Absolute mode (copy literally)
				else
					for i in 0...instruction do
						pixel = UnpackU8(file.read(1))
						data[(header[:width] * y) + x] = pixel
						x += 1
					end

					# «Each absolute run must be word-aligned»
					# Pure brute force here :/
					for i in 0...(instruction % 2) do
						pixel = UnpackU8(file.read(1))
					end
				end
			end # times == 0
		end # while
	end

	return data
end
