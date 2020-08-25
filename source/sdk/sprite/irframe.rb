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


# [sprite/irframe.rb]
# - Alexander Brandt 2020

require_relative "irrow.rb"


class IRFrame

	attr_accessor :rows_list
	attr_accessor :no


	def initialize(no:)
		@rows_list = Array.new()
		@no = no
	end


	def to_s()

		s = String.new()

		for row in @rows_list do
			s += "\t" + row.to_s()
			s += (row != @rows_list.last) ? ",\n" : "\n"
		end

		return "[Frame, no = #{@no} |\n" + s + "]"
	end


	def new_row(y:)

		row = IRRow.new(y: y)
		at = @rows_list.size + 1

		while (at -= 1) > 0 do
			if @rows_list[at - 1].y < y then
				break
			end
		end

		@rows_list.insert(at, row)
		return row
	end


	def min_y()

		if @rows_list[0] != nil then
			return @rows_list[0].y
		end

		return 999
	end


	def max_y()

		if @rows_list[-1] != nil then
			return @rows_list[-1].y
		end

		return -1
	end


	def purge()
		@rows_list.delete_if {|row| row.pixels_list.size == 0}
	end
end
