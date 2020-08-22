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


# [sprite/write.rb]
# - Alexander Brandt 2020

require_relative "irframe.rb"


class IRRow

	def emit_instructions(soup, prev_row, previous_data_offset)

		def write_mov(accumulator)
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

		# Destination X, Y adjustment (DI)
		if prev_row != nil then
			print("\tadd di, #{320 * (@y - prev_row.y) - prev_row.max_x + @pixels_list[0].x - 1}")
		else
			print("\tadd di, #{320 * @y + @pixels_list[0].x}")
		end

		print(" ; Row #{@y}, x = #{@pixels_list[0].x}\n")

		# Source data offset adjustment (SI)
		data_offset = values_at(array: soup)

		if data_offset > previous_data_offset then
			print("\tadd si, #{data_offset - previous_data_offset}")
			print(" ; #{data_offset}\n")
		elsif data_offset < previous_data_offset then
			print("\tsub si, #{previous_data_offset - data_offset}")
			print(" ; #{data_offset}\n")
		end

		# Iterate pixels_list an emit MOV instructions
		accumulator = 0
		last_x = (@pixels_list[0].x - 1)

		for p in @pixels_list do

			# Transparent pixel between (x adjustment)
			if p.x != (last_x + 1) then
				write_mov(accumulator)
				data_offset += accumulator
				accumulator = 0

				print("\tadd di, #{p.x - (last_x + 1)}\n")
			end

			# Opaque pixels_list
			accumulator += 1

			if accumulator == 4 || p == @pixels_list.last() then
				write_mov(accumulator)
				data_offset += accumulator
				accumulator = 0
			end

			last_x = p.x
		end

		# Bye!
		return data_offset
	end
end


def WriteAsm(pingpong, font_sheet, frames, data_soup)

	# Discover dimensions in a inefficient way :)
	sprite_width = 0
	sprite_height = 0

	for f in frames do
		sprite_height = (f.max_y > sprite_height) ? f.max_y : sprite_height

		for r in f.rows_list do
			sprite_width = (r.max_x > sprite_width) ? r.max_x : sprite_width
		end
	end

	# Output local-header
	printf("\n; Load-header (8 bytes)\n")
	printf("dw (file_end) ; File size\n")
	printf("dw 0x%02X       ; Width (%i)\n", sprite_width + 1, sprite_width + 1)
	printf("dw 0x%02X       ; Height (%i)\n", sprite_height + 1, sprite_height + 1)
	printf("dw 0x00       ; Unused\n")

	# Output draw-header
	printf("\n; Draw-header:\n")
	printf("dw (pixels - $) ; Offset to data\n")

	if pingpong == true && frames.size > 1 then
		printf("dw 0x%02X         ; Frames number  (%i)\n", frames.size * 2 - 2 - 1, frames.size * 2 - 2 - 1)
	else
		printf("dw 0x%02X         ; Frames number (%i)\n", frames.size - 1, frames.size - 1)
	end

	# Output frame code offsets
	printf("\n; Code offsets:\n")

	for i in 0...frames.size do
		printf("dw (code_f#{i} - $)\n")
	end

	if pingpong == true && frames.size > 1 then
		for i in 0...(frames.size - 2) do
			printf("dw (code_f#{frames.size - i - 2} - $)\n")
		end
	end

	# Output code
	print("\n; Code:\n")
	frames.each_with_index() do |frame, frame_no|

		print("\ncode_f#{frame_no}:\n")

		if frame_no == 0x20 then
			print("\tmov ax, 4\n")
			print("\tretf\n")
			next
		end

		prev_row = nil
		data_offset = 0
		max_x = 0

		# Frame has content
		if frame.rows_list.size > 0 then
			frame.rows_list.each() do |row|

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

		# Empty frame
		else

			if frame == frames.last ||
			   frames[frame_no + 1].rows_list.size > 0 then
				if font_sheet == true then
					print("\tmov ax, #{max_x + 1}\n") # FIXME?!
				end

				print("\tretf\n")
			end
		end
	end

	# Output data
	printf("\npixels:\n")

	for i in 0...data_soup.size do

		if (i % 16) == 0 then
			print("db ")
		end

		printf("#{data_soup[i]}")

		if i != data_soup.size - 1 then
			printf("%s", ((i % 16) != 15) ? ", " : "\n")
		else
			printf("\n")
		end
	end

	printf("\nfile_end:\n")
end
