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


# [sprite/assembler-op.rb]
# - Alexander Brandt 2020


class OpDefineData

	attr_accessor :type
	attr_accessor :data


	def initialize(tokens:)

		# db <imm/reference>
		# db <imm/reference> , <imm/reference> , <imm/reference> , <etc>

		@data = Array.new()

		if    tokens[0].downcase() == "db" then @type = "db"
		elsif tokens[0].downcase() == "dw" then @type = "dw"
		end

		if tokens.size == 1 then
			raise("Missing value/s to define")
		end

		# One-value syntax
		if tokens.size == 2 then
			if    ValidateNumber(tokens[1]) != 0 then @data << tokens[1].to_i()
			elsif ValidateTag(tokens[1]) != 0    then @data << tokens[1]
			else
				raise("Expected a value or a reference to define, found '#{tokens[1]}' instead")
			end

			return
		end

		# Multiple-values syntax
		token_no = 1

		for t in tokens[1...] do

			# Expecting a Comma
			if (token_no % 2) == 0 then
				if t != "," then
					raise("Expected a comma after token '#{tokens[token_no - 1]}' (#{token_no})")
				end

			# Expecting number/reference
			else
				if    ValidateNumber(t) != 0 then @data << t.to_i()
				elsif ValidateTag(t) != 0    then @data << t
				else
					raise("Expected a value or a reference after token '#{tokens[token_no - 1]}' (#{token_no})")
				end
			end

			token_no += 1
		end

		if tokens[-1] == "," then
			raise("Trailing commas not allowed")
		end
	end


	def write(output:, symbols:)
		# TODO
	end


	def size()
		# TODO
	end
end


class OpMov

	attr_accessor :imm

	def initialize(tokens:)

		# mov al , <imm>

		if tokens.size < 4 then raise("Missing symbols") end
		if tokens.size > 4 then raise("Unexpected symbols") end

		if tokens[1].downcase() != "al" then
			raise("No other destination than AL allowed for MOV")
		end

		if tokens[2] != "," then
			raise("Unexpected token '#{tokens[2]}'")
		end

		if ValidateNumber(tokens[3]) != 0 then
			raise("Only immediate values allowed for MOV")
		end

		@imm = tokens[3].to_i()

		if @imm > 255 then
			raise("Only 8 bits values allowed for MOV")
		end
	end


	def write(output:, symbols:)
		output.write([0xB0].pack("c"))
		output.write([@imm].pack("c"))
	end


	def size() return 2 end
end


class OpAdd

	attr_accessor :dest
	attr_accessor :imm


	def initialize(tokens:)

		# add si , <imm>
		# add di , <imm>

		if tokens.size < 4 then raise("Missing symbols") end
		if tokens.size > 4 then raise("Unexpected symbols") end

		if    tokens[1].downcase() == "si" then @dest = :si
		elsif tokens[1].downcase() == "di" then @dest = :di
		else  raise("Only destinations SI and DI allowed for ADD")
		end

		if tokens[2] != "," then
			raise("Unexpected token '#{tokens[2]}'")
		end

		if ValidateNumber(tokens[3]) != 0 then
			raise("Only immediate values allowed for ADD")
		end

		@imm = tokens[3].to_i()
	end


	def write(output:, symbols:)
		output.write([(@imm > 255) ? 0x81 : 0x83].pack("c"))
		output.write([(@dest == :si) ? 0xC6 : 0xC7].pack("c"))
		output.write([@imm].pack((@imm > 255) ? "s" : "c"))
	end


	def size() return (@imm > 255) ? 4 : 3 end
end


class OpSub

	attr_accessor :dest
	attr_accessor :imm


	def initialize(tokens:)

		# sub si , <imm>
		# sub di , <imm>

		if tokens.size < 4 then raise("Missing symbols") end
		if tokens.size > 4 then raise("Unexpected symbols") end

		if    tokens[1].downcase() == "si" then @dest = :si
		elsif tokens[1].downcase() == "di" then @dest = :di
		else  raise("Only destinations SI and DI allowed for SUB")
		end

		if tokens[2] != "," then
			raise("Unexpected token '#{tokens[2]}'")
		end

		if ValidateNumber(tokens[3]) != 0 then
			raise("Only immediate values allowed for SUB")
		end

		@imm = tokens[3].to_i()
	end


	def write(output:, symbols:)
		output.write([(@imm > 255) ? 0x81 : 0x83].pack("c"))
		output.write([(@dest == :si) ? 0xEE : 0x0EF].pack("c"))
		output.write([@imm].pack((@imm > 255) ? "s" : "c"))
	end


	def size() return (@imm > 255) ? 4 : 3 end
end


class OpRetf

	def initialize(tokens:)
		if tokens.size > 1 then raise("Unexpected symbols") end
	end

	def write(output:, symbols:)
		output.write([0xCB].pack("c"))
	end

	def size() return 1 end
end


class OpMovsb

	def initialize(tokens:)
		if tokens.size > 1 then raise("Unexpected symbols") end
	end

	def write(output:, symbols:)
		output.write([0xA4].pack("c"))
	end

	def size() return 1 end
end


class OpMovsw

	def initialize(tokens:)
		if tokens.size > 1 then raise("Unexpected symbols") end
	end

	def write(output:, symbols:)
		output.write([0xA5].pack("c"))
	end

	def size() return 1 end
end


class OpMovsd

	def initialize(tokens:)
		if tokens.size > 1 then raise("Unexpected symbols") end
	end

	def write(output:, symbols:)
		output.write([0x66].pack("c"))
		output.write([0xA5].pack("c"))
	end

	def size() return 2 end
end
