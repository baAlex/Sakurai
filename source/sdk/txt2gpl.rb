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


# [txt2gpl.rb]
# - Alexander Brandt 2020

# GIMP is incredibly bad dealing with palettes

def ProcessTxt(filename_list)

	print("GIMP Palette\n")
	for filename in filename_list do

		file = File.open(filename, "rb")
		while (file.eof? == false) do

			line = file.readline()
			line = line.sub("#", "")

			b = line.to_i(base=16) & 0x0000FF
			g = (line.to_i(base=16) & 0x00FF00) >> 8
			r = (line.to_i(base=16) & 0xFF0000) >> 16

			print("#{r}\t#{g}\t#{b}\n")
		end

		file.close()
	end
end

(ARGV.length > 0) ? ProcessTxt(ARGV) : raise("No Txt input specified")
