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


# [sprite/irrow.rb]
# - Alexander Brandt 2020

require_relative "irpixel.rb"


class IRRow

	attr_accessor :pixels_list
	attr_accessor :y


	def initialize(y:)
		@pixels_list = Array.new()
		@y = y
	end


	def to_s()

		s = String.new()

		for pixel in @pixels_list do
			s += pixel.to_s()
			s += (pixel != @pixels_list.last) ? ", " : ""
		end

		return "[Row, y = #{@y} | " + s + "]"
	end


	def new_pixel(x:, value:)

		if value == 0 then return end

		pixel = IRPixel.new(x: x, value: value)
		at = @pixels_list.size + 1

		while (at -= 1) > 0 do

			if @pixels_list[at - 1].x < x then
				break
			end
		end

		@pixels_list.insert(at, pixel)
		return pixel
	end


	def min_x()

		if @pixels_list[0] != nil then
			return @pixels_list[0].x
		end

		return 999
	end


	def max_x()

		if @pixels_list[-1] != nil then
			return @pixels_list[-1].x
		end

		return -1
	end


	def values_subset_of?(row:)

		if self == row || @pixels_list.size > row.pixels_list.size then
			return false
		end

		for a in 0...(row.pixels_list.size - @pixels_list.size) do
		for b in 0...@pixels_list.size do

			if @pixels_list[b].value != row.pixels_list[a + b].value then
				break
			end

			if b == (@pixels_list.size - 1) then
				return true
			end
		end
		end

		return false
	end


	def values_at(array:)

		if @pixels_list.size > array.size then
			return nil
		end

		# FIXME!, the '+1'
		for a in 0...(array.size - @pixels_list.size + 1) do
		for b in 0...@pixels_list.size do

			if @pixels_list[b].value != array[a + b] then
				break
			end

			if b == (@pixels_list.size - 1) then
				return a
			end
		end
		end

		return nil
	end
end
