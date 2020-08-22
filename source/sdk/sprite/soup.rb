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


# [sprite/soup.rb]
# - Alexander Brandt 2020

require_relative "irframe.rb"


def DataSoupFromFrames(list:)

	soup = Array.new()

	# Brute force, close your eyes...
	for frame_a in list do
	for row_a in frame_a.rows_list do

		superset = nil

		for frame_b in list do

			for row_b in frame_b.rows_list do
				if row_a.values_subset_of?(row: row_b) == true then
					superset = row_b
				end

				if superset != nil then break end
			end

			if superset != nil then break end
		end

		if superset == nil then
			for pixel in row_a.pixels_list do
				soup.append(pixel.value)
			end
		end

	end
	end

	# Even with the 'superset' approach some bytes
	# on the soup ends being unused, lets delete them
	soup_mask = Array.new(soup.size, false)
	soup_clean = Array.new()

	for frame in list do
	for row in frame.rows_list do

		offset = row.values_at(array: soup)

		if offset == nil then
			printf("soup:\n")
			puts(soup)
			printf("\nrow:\n")
			puts(row)
			printf("\n")

			raise("This sould not happen!!")
		end

		for i in offset...(offset + row.pixels_list.size) do
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
