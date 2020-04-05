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
	attr_accessor :row_no
	attr_accessor :superset


	def initialize(row_no)
		@pixels = Array.new()
		@row_no = row_no
		@superset = nil
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


	def emit_instructions(soup, previous_delta_x, previous_data_offset)

		# Destination X, Y adjustment (DI)
		if @row_no > 0 then
			print("\tadd di, #{320 - previous_delta_x + @pixels[0].x - 1}")
			print(" ; Row #{@row_no}, x = #{@pixels[0].x}")

			if @superset then printf(", superset = #{@superset.row_no}") end
			printf("\n")
		end

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
			for y in 0...@pixels.size do # TODO, out of bounds?

				if @pixels[y].value != array[x + y].value then
					break
				end

				if y == (@pixels.size - 1) then
					return (x)
				end
			end
		end

		return nil
	end
end


def ProcessSprite(filename)

	file = File.open(filename, "rb")
	header = ReadBmpHeader(file)

	if header[:bpp] != 8 then
		raise("True color images not supported")
	end

	data = ReadBmpIndexedData(header, file)
	imrow_list = Array.new()

	file.close()

	# Create immediate rows
	for r in 0...header[:height] do

		imrow = IRRow.new(r)

		for c in 0...header[:width] do
			imrow.append(data[header[:width] * r + c], c)
		end

		if imrow.pixels.size != 0 then
			imrow_list.append(imrow)
		end
	end

	# Brute force from here, close your eyes...
	for irA in imrow_list do
		for irB in imrow_list do

			if offset = irA.subset_of?(irB) then
				irA.superset = irB
				break
			end
		end
	end

	soup = Array.new()

	for ir in imrow_list do
		if ir.superset == nil then
			soup += ir.pixels
		else
			# Unfold superset linkage
			# ('unfold' is the correct word?)
			while ir.superset.superset != nil do
				ir.superset = ir.superset.superset
			end
		end
	end

	# Print header
	print("; Thanks von Neumann!\n")
	print("dw (file_end) ; File size\n")
	print("dw (pixels)   ; Offset to data\n")

	# Print code
	print("\ncode:\n")
	delta_x = 0
	data_offset = 0

	for ir in imrow_list do
		delta_x, data_offset = ir.emit_instructions(soup, delta_x, data_offset)
	end

	print("\tretf\n")

	# Print data
	print("\npixels: db")

	for i in 0...soup.size do
		printf("%s", (i != 0) ? ", " : " ")
		print(soup[i].value)
	end

	print("\n\nfile_end:\n")
end

(ARGV.length > 0) ? ProcessSprite(ARGV[0]) : raise("No Bmp input specified")
