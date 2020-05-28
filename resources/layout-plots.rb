
require "gnuplot"

BATTLES_NO = 64
ENEMIES_NO = 4

NOISE_GATE = 40
NOISE_MIN = 2
NOISE_MAX = 2

RETURN_MEDIAN = 1
RETURN_SAWTOOTH = 2
RETURN_TRIANGLE = 3


class SakuraiRNG
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
	# Copy and paste from 'source/game/actor.c'

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
		rng = SakuraiRNG.new(123)
		end

		plot.terminal "png"

		plot.xlabel "Battle"
		plot.yrange "[-1 to #{ENEMIES_NO + 2}]"

		x = (0...(BATTLES_NO)).collect { |v| v }

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
