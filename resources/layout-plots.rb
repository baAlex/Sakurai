
require "gnuplot"

BATTLES_NO = 32
ENEMIES_NO = 7

NOISE_GATE = 40
NOISE_MIN = 2
NOISE_MAX = 2

RETURN_MEDIAN = 1
RETURN_SAWTOOTH = 2
RETURN_TRIANGLE = 3

CHANCES_ATTACK = 8      # In number of battles
CHANCES_DECAY = 3       # Same
BATTLES_DIV_ENEMIES = 4 # (BATTLES_NO / ENEMIES_NO)
OFFSET = 5


class Fixed
	# Copy and paste from 'source/game/fixed.c'

	def initialize(a, b = nil)

		# Assume that A is a raw value
		if b == nil then
			if a > 65535 then
				raise("This is an unsigned Q8 fixed point implementation")
			end

			@internal = a

		# Assume that A is the whole and B the fraction
		else
			if a > 255 || b > 255 then
				raise("This is an unsigned Q8 fixed point implementation")
			end

			@internal = (b) | (a << 8)
		end
	end

	def internal() return (@internal) end
	def whole() return (@internal >> 8) end
	def fraction() return (@internal & 0x00FF) end

	def >(other) return (@internal > other.internal) ? true : false end
	def <(other) return (@internal < other.internal) ? true : false end
	def >=(other) return (@internal >= other.internal) ? true : false end
	def <=(other) return (@internal <= other.internal) ? true : false end

	def +(other) return Fixed.new(@internal + other.internal) end
	def -(other) return Fixed.new(@internal - other.internal) end

	def *(other) return Fixed.new((@internal * other.internal) >> 8) end
	def /(other) return Fixed.new((@internal << 8) / other.internal) end
end

def FixedStep(edge0, edge1, v)
	edge1 = edge1 - edge0
	edge0 = v - edge0
	edge0 = edge0 / edge1
	return Fixed.new((edge0.internal).clamp(0, Fixed.new(1, 0).internal))
end


class RNG
	# Copy and paste from 'source/game/utilities.c'
	# http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html

	def initialize(seed = 1)
		@s_random_state = seed
	end

	def seed(seed)
		@s_random_state = seed
	end

	def random()
		@s_random_state ^= @s_random_state << 7
		@s_random_state ^= @s_random_state >> 9
		@s_random_state ^= @s_random_state << 8
		return @s_random_state
	end
end


class DummyRNG
	def initialize(seed = 1) end
	def seed(seed) end
	def random() return 0 end
end


def EnemiesNumber(battle_no, rng, ret)
	# Copy and paste from 'source/game/actor-layout.c'

	sawtooth = 0
	triangle = 0

	# First battle always has one enemy
	if ret == RETURN_MEDIAN && battle_no == 0 then
		return 1
	end

	# Sawtooth
	sawtooth = battle_no >> 1
	sawtooth = sawtooth % (ENEMIES_NO)
	sawtooth += 1

	# Triangle
	triangle = battle_no >> 1

	if triangle % ((ENEMIES_NO - 1) << 1) < (ENEMIES_NO - 1) then
		triangle = triangle % (ENEMIES_NO - 1)
	elsif
		triangle = (ENEMIES_NO - 1) - triangle % (ENEMIES_NO - 1)
	end

	triangle += 1

	# Add noise
	if (rng.random() % 100) < NOISE_GATE then
		triangle -= rng.random() % NOISE_MIN
		triangle += rng.random() % NOISE_MAX
		sawtooth -= rng.random() % NOISE_MIN
		sawtooth += rng.random() % NOISE_MAX
	end

	# Yay!
	if (ret == RETURN_SAWTOOTH) then return sawtooth end
	if (ret == RETURN_TRIANGLE) then return triangle end

	return ((triangle + sawtooth) >> 1).clamp(1, ENEMIES_NO)
end


def EnemyChances(enemy_i, battle_no)
	# Copy and paste from 'source/game/actor-layout.c'

	def ImaginaryLine(battle_no)
		chances = FixedStep(Fixed.new(0, 0), Fixed.new(BATTLES_NO, 0), battle_no)
		chances = chances / Fixed.new(ENEMIES_NO, 0)
		return chances
	end

	chances = Fixed.new(0, 0)

	attack_start = Fixed.new(BATTLES_DIV_ENEMIES, 0) * Fixed.new(enemy_i, 0)
	attack_end = attack_start + Fixed.new(CHANCES_ATTACK, 0)
	decay_end = attack_start + Fixed.new(CHANCES_ATTACK + CHANCES_DECAY, 0)

	battle_no += Fixed.new(OFFSET, 0)

	if battle_no >= attack_start then

		# 1 - Attack
		if battle_no <= attack_end then
			chances = FixedStep(attack_start, attack_end, battle_no)

		# 2 - Decay
		else
			chances = Fixed.new(1, 0) - FixedStep(attack_end, decay_end, battle_no)

			# Keep it over an imaginary line!
			if chances < ImaginaryLine(battle_no) then
				chances = ImaginaryLine(battle_no)
			end
		end
	end

	# Bye!
#	return (ImaginaryLine(battle_no) * Fixed.new(100, 0)).whole
	return (chances * Fixed.new(100, 0)).whole
end


# =============================


for i in 0...2 do

	Gnuplot.open do |gp|
	Gnuplot::Plot.new(gp) do |plot|

		rng = 0

		if i == 0 then
		plot.title  "Enemies number, modulating two waveforms"
		plot.output "./plot-enemies-number.png"
		rng = DummyRNG.new()
		else
		plot.title  "Enemies number, modulating two waveforms plus noise"
		plot.output "./plot-enemies-number-noise.png"
		rng = RNG.new(123)
		end

		plot.terminal "png"

		plot.xlabel "Battle"
		plot.yrange "[-1 to #{ENEMIES_NO + 2}]"

		x = (0...(BATTLES_NO * 2)).collect { |v| v }

		# Sawtooth
		rng.seed(123)
		y = x.collect { |v| EnemiesNumber(v, rng, RETURN_SAWTOOTH) }

		plot.data << Gnuplot::DataSet.new([x, y]) do |ds|
			ds.title = "Sawtooth"; ds.with = "lines"
		end

		# Triangle
		rng.seed(123)
		y = x.collect { |v| EnemiesNumber(v, rng, RETURN_TRIANGLE) }

		plot.data << Gnuplot::DataSet.new([x, y]) do |ds|
			ds.title = "Triangle"; ds.with = "lines"
		end

		# Median
		rng.seed(123)
		y = x.collect { |v| EnemiesNumber(v, rng, RETURN_MEDIAN) }

		plot.data << Gnuplot::DataSet.new([x, y]) do |ds|
			ds.title = "Median"; ds.with = "steps"; ds.linewidth = 3
		end

	end # Gnuplot::Plot
	end # Gnuplot.open
end


# =============================


Gnuplot.open do |gp|
Gnuplot::Plot.new(gp) do |plot|

	plot.title  "Enemies chances"
	plot.output "./plot-enemies-chances.png"

	plot.terminal "png"
	plot.xlabel "Battle"

	x = (0...(BATTLES_NO * 1.25)).collect { |v| v }

	for e in 0...ENEMIES_NO do

		y = x.collect { |v| EnemyChances(e, Fixed.new(v, 0)) }

		plot.data << Gnuplot::DataSet.new([x, y]) do |ds|
			ds.title = "Type #{e}"; ds.with = "lines"
		end
	end

end # Gnuplot::Plot
end # Gnuplot.open
