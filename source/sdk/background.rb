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


# [background.rb]
# - Alexander Brandt 2020

require_relative "shared.rb"


def Background(filename_output, filename_input)

	output = File.open(filename_output, "wb")
	input = File.open(filename_input, "rb")
	header = ReadBmpHeader(input)

	if header[:bpp] != 8 then
		raise("True color images not supported")
	end

	data = ReadBmpIndexedData(header, input)
	input.close()

	for pixel in 0...(header[:height] * header[:width]) do
		output.write([data[pixel]].pack("c"))
	end

	output.close()
end


if __FILE__ == $PROGRAM_NAME
	(ARGV.length > 1) ? Background(ARGV[0], ARGV[1]) : raise("Usage blah... blah... blah...")
end
