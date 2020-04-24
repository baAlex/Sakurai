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


class IRPixel
	attr_accessor :value
	attr_accessor :x

	def initialize(value, x)
		@value = value
		@x = x
	end
end


class IRRow
	attr_accessor :pixels
	attr_accessor :max_x
	attr_accessor :y


	def initialize(y)
		@pixels = Array.new()
		@max_x = 0
		@y = y
	end


	def append(value, x)

		if value > 0 then

			@pixels.push(IRPixel.new(value, x))

			if x > @max_x then
				@max_x = x
			end
		end
	end


	def emit_mov(accumulator)

		while accumulator > 0 do
			if accumulator >= 4 then
				print("\tmovsd\n")
				accumulator -= 4
			elsif accumulator >=2 then
				print("\tmovsw\n")
				accumulator -= 2
			else
				print("\tmovsb\n")
				accumulator -= 1
			end
		end
	end


	def emit_instructions(soup, prev_row, previous_data_offset)

		# Destination X, Y adjustment (DI)
		if prev_row != nil then
			print("\tadd di, #{320 * (@y - prev_row.y) - prev_row.max_x + @pixels[0].x - 1}")
		else
			print("\tadd di, #{320 * @y + @pixels[0].x}")
		end

		print(" ; Row #{@y}, x = #{@pixels[0].x}\n")

		# Source data offset adjustment (SI)
		data_offset = data_offset_at(soup)

		if data_offset > previous_data_offset then
			print("\tadd si, #{data_offset - previous_data_offset}")
			print(" ; #{data_offset}\n")
		elsif data_offset < previous_data_offset then
			print("\tsub si, #{previous_data_offset - data_offset}")
			print(" ; #{data_offset}\n")
		end

		# Iterate pixels an emit MOV instructions
		accumulator = 0
		last_x = (@pixels[0].x - 1)

		for p in @pixels do

			# Transparent pixel between (x adjustment)
			if p.x != (last_x + 1) then
				emit_mov(accumulator)
				data_offset += accumulator
				accumulator = 0

				print("\tadd di, #{p.x - (last_x + 1)}\n")
			end

			# Opaque pixels
			accumulator += 1

			if accumulator == 4 || p == @pixels.last() then
				emit_mov(accumulator)
				data_offset += accumulator
				accumulator = 0
			end

			last_x = p.x
		end

		# Bye!
		return data_offset
	end


	def subset_of?(other)

		if self == other || @pixels.size > other.pixels.size then
			return false
		end

		for a in 0...(other.pixels.size - @pixels.size) do
		for b in 0...@pixels.size do

			if @pixels[b].value != other.pixels[a + b].value then
				break
			end

			if b == (@pixels.size - 1) then
				return true
			end
		end
		end

		return false
	end


	def data_offset_at(array)

		if @pixels.size > array.size then
			return nil
		end

		# FIXME!, the '+1'
		for a in 0...(array.size - @pixels.size + 1) do
		for b in 0...@pixels.size do

			if @pixels[b].value != array[a + b].value then
				break
			end

			if b == (@pixels.size - 1) then
				return (a)
			end
		end
		end

		raise("This should not happen!")
		return nil
	end
end


class IRFrame
	attr_accessor :rows

	def initialize()
		@rows = Array.new()
	end

	def append(row)
		if row.pixels.size == 0 then return end
		@rows.push(row)
	end
end


def FramesFromIndividualFiles(filename_list)

	frame_list = Array.new()
	previous_header = nil

	# Iterate files creating intermediate objects
	for filename in filename_list do

		if filename == "+linear" ||
		   filename == "+pingpong" ||
		   filename == "+font" then
			next
		end

		# Read BMP data
		file = File.open(filename, "rb")
		header = ReadBmpHeader(file)

		if header[:bpp] != 8 then
			raise("True color images not supported")
		end

		if previous_header != nil then
			if previous_header[:width] != header[:width] ||
			   previous_header[:height] != header[:height] then
				raise("Animation frames can't have different dimensions")
			end
		end

		bmp_data = ReadBmpIndexedData(header, file)
		file.close()

		# Create a frame with rows
		frame = IRFrame.new()
		frame_list.push(frame)

		for r in 0...header[:height] do

			row = IRRow.new(r)

			for c in 0...header[:width] do
				row.append(bmp_data[header[:width] * r + c], c)
			end

			frame.append(row)
		end

		# Next step
		previous_header = header
	end

	return frame_list
end


def FramesFromFontSheet(filename_list)

	frame_list = Array.new()
	header = nil
	bmp_data = nil

	# Iterate files, the first one wins
	for filename in filename_list do

		if filename == "+linear" ||
		   filename == "+pingpong" ||
		   filename == "+font" then
			next
		end

		# Read BMP data
		file = File.open(filename, "rb")
		header = ReadBmpHeader(file)

		if header[:bpp] != 8 then
			raise("True color images not supported")
		end

		bmp_data = ReadBmpIndexedData(header, file)
		file.close()
		break
	end

	if bmp_data == nil
		raise("No valid input")
	end

	# Create frames subdividing the image by 16x16
	# (to get our 256 ascii characters)
	char_width = header[:width] / 16
	char_height = header[:height] / 16
	offset = 0

	while 1 do

		frame = IRFrame.new()
		frame_list.push(frame)

		for r in 0...char_height do

			row = IRRow.new(r)

			for c in 0...char_width do
				row.append(bmp_data[(header[:width]) * r + offset + c], c)
			end

			frame.append(row)
		end

		# Next frame
		offset += char_width

		if (offset % header[:width]) == 0 then
			offset += (char_height - 1) * header[:width]
		end

		if frame_list.size == 255 || # !
		   offset > (header[:width] * header[:height]) - (char_width) then
			break
		end

		# FIXME?, the modulo in the engine side didn't allow 255 frames
	end

	return frame_list
end


def OptimizedDataFromFrames(frame_list)

	soup = Array.new()

	# Brute force, close your eyes...
	for frame_a in frame_list do
	for row_a in frame_a.rows do

		superset = nil

		for frame_b in frame_list do
		for row_b in frame_b.rows do

			if row_a.subset_of?(row_b) == true then
				superset = row_b
				break
			end

		end
		end

		if superset == nil then
			soup += row_a.pixels
		end

	end
	end

	# Even with the 'superset' approach some bytes
	# on the soup ends being unused, lets delete them
	soup_mask = Array.new(soup.size, false)
	soup_clean = Array.new()

	for frame in frame_list do
	for row in frame.rows do

		offset = row.data_offset_at(soup)

		for i in offset...(offset + row.pixels.size) do
			soup_mask[i] = true
		end

	end
	end

	for i in 0...soup_mask.size do
		if soup_mask[i] == true then
			soup_clean.push(soup[i])
		end
	end

	return soup_clean
end


def EmitProgram(pingpong, font_sheet, frame_list, data_soup)

	# Print header
	print("; Thanks von Neumann!\n\n")
	print("dw (file_end) ; File size\n")
	print("dw (pixels) ; Offset to data\n")

	if pingpong == true && frame_list.size > 1 then
		print("dw #{frame_list.size * 2 - 2 - 1} ; Frames number -ping pong mode-\n")
	else
		print("dw #{frame_list.size - 1} ; Frames number\n")
	end

	print("\n")

	# Print frame code offsets
	for i in 0...frame_list.size do
		print("dw (code_f#{i} - $)\n")
	end

	# Extra frame codes offsets for pingpong playback
	if pingpong == true && frame_list.size > 1 then
		for i in 0...(frame_list.size - 2) do
			print("dw (code_f#{frame_list.size - i - 2} - $)\n")
		end
	end

	# Print code
	frame_list.each_with_index() do |frame, frame_no|

		print("\ncode_f#{frame_no}:\n")

		if frame_no == 0x20 then
			print("\tmov ax, 4\n")
			print("\tretf\n")
		end

		prev_row = nil
		data_offset = 0
		max_x = 0

		frame.rows.each() do |row|

			if row.max_x > max_x then
				max_x = row.max_x
			end

			data_offset = row.emit_instructions(data_soup, prev_row, data_offset)
			prev_row = row
		end

		if font_sheet == true then
			print("\tmov ax, #{max_x + 1}\n") # FIXME?!
		end

		print("\tretf\n")
	end

	# Print data
	print("\npixels:\n")

	for i in 0...data_soup.size do

		if (i % 16) == 0 then
			print("\tdb ")
		end

		print("#{data_soup[i].value}")

		if i != data_soup.size - 1 then
			printf("%s", ((i % 16) != 15) ? ", " : "\n")
		else
			print("\n")
		end
	end

	print("\n\nfile_end:\n")
end


def main(args)

	pingpong = false
	font_sheet = false

	for a in args do

		if a == "+pingpong" then
			pingpong = true
		end

		if a == "+linear" then
			pingpong = false
		end

		if a == "+font" then
			font_sheet = true
		end
	end

	if font_sheet == false then
		frame_list = FramesFromIndividualFiles(ARGV)
	else
		frame_list = FramesFromFontSheet(ARGV)
	end

	data_soup = OptimizedDataFromFrames(frame_list)

	EmitProgram(pingpong, font_sheet, frame_list, data_soup)
end


(ARGV.length > 0) ? main(ARGV) : raise("No Bmp input specified")
