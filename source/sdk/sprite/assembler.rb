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


# [sprite/assembler.rb]
# - Alexander Brandt 2020

require_relative "assembler-op.rb"


KNOWN_OPERATIONS = Hash.new()
KNOWN_OPERATIONS["db"] = OpDefineData
KNOWN_OPERATIONS["dw"] = OpDefineData
KNOWN_OPERATIONS["mov"] = OpMov
KNOWN_OPERATIONS["add"] = OpAdd
KNOWN_OPERATIONS["sub"] = OpSub
KNOWN_OPERATIONS["retf"] = OpRetf
KNOWN_OPERATIONS["movsb"] = OpMovsb
KNOWN_OPERATIONS["movsw"] = OpMovsw
KNOWN_OPERATIONS["movsd"] = OpMovsd


def ValidateTag(string)

	if string[0].match?(/[0-9]/) == true ||
	   string[/[^(A-Z)(a-z)(0-9)(_)]/] != nil then
		return 1
	end

	return 0
end


def ValidateNumber(string)

	if string[/[^(x)(A-F)(a-f)(0-9)]/] != nil
		return 1
	end

	return 0
end


class Instruction

	attr_accessor :tag
	attr_accessor :op


	def initialize(line:)

		tokens = tokenize_line(line)
		@tag = nil
		@op = nil

		if tokens.size == 0 then return end

		# Tag
		@tag = parse_tag(tokens)

		if @tag != nil
			tokens = tokens[2..] # Ignore tags tokens from here
		end

		# Operation
		if tokens.size != 0 then

			op_class = KNOWN_OPERATIONS[tokens[0].downcase()]

			if op_class == nil then
				raise("Unknown instruction '#{tokens[0]}'")
			end

			@op = op_class.new(tokens: tokens)
		end
	end


	def tokenize_line(line)

		# Remove comment
		comment_index = line.index(";")

		if comment_index != nil then
			line = line[0...comment_index]
		end

		# Leading and trailing whitespaces
		line = line.strip()

		# Tokenize
		tokens = Array.new()
		line.scan(/:|,|[^(\s|:|,)]+/) { |w| tokens << w }

		return tokens
	end


	def parse_tag(tokens)

		if KNOWN_OPERATIONS[tokens[0].downcase()] != nil then
			return nil
		end

		if tokens.size < 2 || tokens[1] != ":" then
			raise("Unknown instruction '#{tokens[0]}' / Missing ':' to specify a tag")
		end

		if ValidateTag(tokens[0]) != 0 then
			raise("Token '#{tokens[0]}' contains invalid characters for a tag")
		end

		return tokens[0]
	end
end


def Assembler(string:)

	program = Array.new()
	symbols = Hash.new()

	# First pass
	line_no = 1
	string.each_line { |line|

		if line.empty? == true then next end
		if line == "\n" then next end

		begin
			instruction = Instruction.new(line: line)
			program << instruction

			if instruction.tag != nil then
				symbols[instruction.tag] = instruction
			end

		rescue Exception => e
			printf("Error in line %04i : #{e.message}\n", line_no)
		end

		line_no += 1
	}

	# Second pass
	fp = File.open("test", "wb")

	for instruction in program do
		if instruction.op != nil then
			instruction.op.write(output: fp, symbols: symbols)
		end
	end

	fp.close()
end


####


Assembler(string: "	add di, 321 ; Row 1, x = 1
add si, 2 ; 2
movsw
	ADD di, 317; Row 2, x = 0
sub si , 2	; 2
 movsd
add di,317 ; Row 3, x = 1
sub si, 4 ; 2
movsw
add di, 317 ; Row 4, x = 0
  sub	si	,	2 ; 2
movsd
add di ,317 ; Row 5, x = 1
sub si, 4; 2
movsw
mov al, 4
retf

pixels:
db 8, 8, 8, 64, 8, 8, 64, 8, 8, 64, 8, 8, 64, 8, 64, 8
dw file_end ; File size
dw 0x06 ; Width (6)
dw 0x07 ; Height (7)
dw 0x00 ; Unused
file_end:")
