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


def EmitMov(opaque_num)

	while opaque_num != 0 do

		if opaque_num >= 4 then
			print("\tmovsd\n")
			opaque_num -= 4
		elsif opaque_num >=2 then
			print("\tmovsw\n")
			opaque_num -= 2
		else
			print("\tmovsb\n")
			opaque_num -= 1
		end
	end
end


def ProcessSprite(filename)

	file = File.open(filename, "rb")
	header = ReadBmpHeader(file)

	if header[:bpp] != 8 then
		raise("True color images not supported")
	end

	data = ReadBmpIndexedData(header, file)

	print("; Thanks von Neumann!\n")
	print("dw (pixels)\n")

	# Code
	print("\ncode:\n")

	opaque_num = 0
	trans_num = 0
	max_di = 0
	first_opaque = 0

	for row in 0...header[:height] do

		# Adjust vertical offset with ADD in DI
		if row != 0 then

			# Cycle columns to check that the row is not empty
			# and to start ignoring initial transparent space
			for col in 0...header[:width] do

				if data[header[:width] * row + col] != 0 then
					print("\tadd di, #{320 + col - max_di} ; Row #{row}\n")

					first_opaque = col
					max_di = col
					trans_num = 0
					break
				end
			end
		end

		# Emit MOV instructions to draw opaque pixels
		# And ADD in DI to adjust transparent pixels
		for col in max_di...header[:width] do

			# Transparent pixel found
			if data[header[:width] * row + col] == 0 then
				trans_num += 1

				EmitMov(opaque_num)
				max_di += opaque_num
				opaque_num = 0

			# Opaque pixel
			else
				opaque_num += 1 # Sum whitout emit

				if trans_num != 0 then
					print("\tadd di, #{trans_num}\n")
					max_di += trans_num
					trans_num = 0
				end

				if opaque_num == 4 then # Enought!
					EmitMov(opaque_num)
					max_di += opaque_num
					opaque_num = 0
				end
			end
		end
	end

	print("\tretf\n")

	# Data
	print("\npixels:\n")
	first = false

	for row in 0...header[:height] do

		i = 0
		first = false

		for col in 0...header[:width] do

			if data[header[:width] * row + col] != 0 then

				if first == false then
					print("\nrow#{row}: db")
					first = true
				end

				printf("%s", (i == 0) ? " " : ", ")
				print("#{data[header[:width] * row + col]}")
				i += 1
			end
		end
	end

	print("\n")

	file.close()
end

(ARGV.length > 0) ? ProcessSprite(ARGV[0]) : raise("No Bmp input specified")