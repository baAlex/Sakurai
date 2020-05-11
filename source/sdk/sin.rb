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


# [sin.rb]
# - Alexander Brandt 2020

include Math


ENTRIES = 128
COLUMNS = 16

print("static int8_t s_sin_table[#{ENTRIES}] = {")

for i in 1..ENTRIES

	value = Math.sin(i * (PI / ENTRIES)) * 127.0
	value = value.to_i()

	if i != ENTRIES then
	print(" #{value},")
	else
	print(" #{value}")
	end

	if (i % COLUMNS) == 0 then
		print("\n")
	end
end

print("};\n")
