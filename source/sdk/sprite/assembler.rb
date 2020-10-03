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

	if string[/[^(0-9)]/] != nil then
		return 1
	end

	return 0
end


def ValidateHex(string)

	if string[/[^(x)(A-F)(a-f)(0-9)]/] != nil then
		return 1
	end

	return 0
end


def ResolveReference(reference, symbols:)
	return symbols[reference].offset
end


class Instruction

	attr_accessor :tag
	attr_accessor :op
	attr_accessor :offset


	def initialize(line:, offset:)

		tokens = tokenize_line(line)

		@offset = offset;
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


def Assembler(string:, output_file:)

	program = Array.new()
	symbols = Hash.new()

	# First pass
	error = false
	current_offset = 0
	line_no = 1

	string.each_line { |line|

		if line.empty? == true then next end
		if line == "\n" then next end

		# Create instruction
		instruction = nil

		begin
			instruction = Instruction.new(line: line, offset: current_offset)
		rescue Exception => e
			printf("Error in line %04i : #{e.message}\n", line_no)
			error = true
			line_no += 1
			next
		end

		# Append it to program
		program << instruction

		# If has a tag, keep a reference in ::symbols[]
		if instruction.tag != nil then
			symbols[instruction.tag] = instruction
		end

		# If has an op, keep the offset updated
		if instruction.op != nil then
			current_offset += instruction.op.size()
		end

		line_no += 1
	}

	if error == true then raise() end

	# Second pass
	current_offset = 0

	for instruction in program do
		if instruction.op != nil then
			instruction.op.write(output: output_file, symbols: symbols, offset: current_offset)
			current_offset += instruction.op.size()
		end
	end
end
