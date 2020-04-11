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


	def initialize()
		@pixels = Array.new()
	end


	def append(value, x)
		if value == 0 then return end
		@pixels.push(IRPixel.new(value, x))
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


	def emit_instructions(soup, previous_delta_x, previous_data_offset, row_no)

		# Destination X, Y adjustment (DI)
		if row_no > 0 then
			print("\tadd di, #{320 - previous_delta_x + @pixels[0].x - 1}")
		else
			print("\tadd di, #{@pixels[0].x - 1}")
		end

		print(" ; Row #{row_no}, x = #{@pixels[0].x}\n")

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
		return last_x, data_offset
	end


	def subset_of?(other)

		if self == other || @pixels.size > other.pixels.size then
			return false
		end

		for x in 0...(other.pixels.size - @pixels.size) do
			for y in 0...@pixels.size do

				if @pixels[y].value != other.pixels[x + y].value then
					break
				end

				if y == (@pixels.size - 1) then
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

		for x in 0...array.size do
			for y in 0...(array.size - @pixels.size) do

				if @pixels[y].value != array[x + y].value then
					break
				end

				if y == (@pixels.size - 1) then
					return (x)
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


def ProcessSprite(filename_list)

	frame_list = Array.new()
	previous_header = nil

	# Iterate files creating intermediate objects
	for filename in filename_list do

		# Create frame
		frame = IRFrame.new()
		frame_list.push(frame)

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

		data = ReadBmpIndexedData(header, file)
		file.close()

		# Create rows
		for r in 0...header[:height] do

			row = IRRow.new()

			for c in 0...header[:width] do
				row.append(data[header[:width] * r + c], c)
			end

			frame.append(row)
		end

		# Next step
		previous_header = header
	end

	# Brute force, close your eyes...
	soup = Array.new()

	for frame_a in frame_list do
	for row_a in frame_a.rows do

		superset = nil

		for frame_b in frame_list do
		for row_b in frame_a.rows do

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

	# Print header
	print("; Thanks von Neumann!\n\n")
	print("dw (file_end) ; File size\n")
	print("dw (pixels) ; Offset to data\n")
	print("dw #{frame_list.size} ; Frames number\n")

	print("\n")

	for i in 0...frame_list.size do
		print("dw (code_f#{i + 1} - $)\n")
	end

	# Print code
	frame_list.each_with_index() do |frame, frame_no|

		print("\ncode_f#{frame_no + 1}:\n")
		delta_x = 0
		data_offset = 0

		frame.rows.each_with_index() do |row, row_no|
			delta_x, data_offset = row.emit_instructions(soup_clean, delta_x, data_offset, row_no)
		end

		print("\tretf\n")
	end

	# Print data
	print("\npixels:\n")

	for i in 0...soup_clean.size do

		if (i % 16) == 0 then print("\tdb ") end

		print("#{soup_clean[i].value}")

		if i != soup_clean.size - 1 then
			printf("%s", ((i % 16) != 15) ? ", " : "\n")
		else
			print("\n")
		end
	end

	print("\n\nfile_end:\n")
end

(ARGV.length > 0) ? ProcessSprite(ARGV) : raise("No Bmp input specified")
