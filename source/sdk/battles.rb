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


# [battles.rb]
# - Alexander Brandt 2020

# « Baka!... is not like I can't write an overcomplicated
# algorithm to chose enemies! » (Me, probably)

include Math

SEED = 1234 # TODO, use a custom rng!

BATTLES_NO = 32
MAX_ENEMIES = 4
ENEMIES = ["TYPE_A", "TYPE_B", "TYPE_C", "TYPE_D", "TYPE_E", "TYPE_F", "TYPE_G"]
NO_ENEMY = "__NN__"


# Beyond the 90% all battles uses MAX_ENEMIES number of enemies
# Note that this is not a clamp, but where the linear function ends,
# also this is calculated before round the result
FULL_LAYOUT_AT = 90.0


# The number of enemies grows linear in every battle, but, there
# is a 30% of chances of fight a battle that has the number of a
# future one. An example: a player is fighting battles with two
# enemies, expecting to jump into those with three, but suddenly
# appears one that has four. Bad luck, a victim of those chances
NOISE_GATE = 30.0
NOISE_MIN = (BATTLES_NO.to_f / 3.0) / 2.0  # Minimum battles in the future
NOISE_MAX = (BATTLES_NO.to_f / 3.0)        # Maximum


# Too lazy to describe!, but imagine that every enemy "chances" or
# probabilities to appear, forms kind of a sawtooth waveshape
ATTACK = 2.0
DECAY = 1.0


def SmoothStep(edge0, edge1, v)
	t = ((v - edge0) / (edge1 - edge0)).clamp(0.0, 1.0)
	return t * t * (3.0 - 2.0 * t)
end


def LinearStep(edge0, edge1, v)
	return ((v - edge0) / (edge1 - edge0)).clamp(0.0, 1.0)
end


def EnemiesNo(battle, rng)

	battle = battle.to_f() + 1.0

	# First battle always has one enemy
	if battle == 1.0 then
		return 1
	end

	# Add noise
	if rng.rand(100) < NOISE_GATE then
		battle += rng.rand(NOISE_MIN..NOISE_MAX)
	end

	# Linear amount
	a = LinearStep(0.0, (BATTLES_NO.to_f() / 100.0) * FULL_LAYOUT_AT, battle.to_f())
	a = a * MAX_ENEMIES.to_f()

	# Yay!
	a = a.round()
	a = a.clamp(1, MAX_ENEMIES)
	return a
end


def EnemyChances(enemy_no, battle)

	v = 0.0
	enemy_no += 0.5

	a1 = BATTLES_NO.to_f / ENEMIES.size.to_f
	a2 = BATTLES_NO.to_f / ENEMIES.size.to_f
	d = BATTLES_NO.to_f / ENEMIES.size.to_f

	a2 *= (enemy_no) # Attack ends (zenith)
	a1 *= (enemy_no - ATTACK)
	d  *= (enemy_no + DECAY)

	if d > BATTLES_NO.to_f then
		d = BATTLES_NO.to_f
	end

	# 1- Attack
	if battle <= a2 then
		v = SmoothStep(a1, a2, battle.to_f)

		# 2- Decay
	elsif battle <= d then
		v = 1.0 - LinearStep(a2, d, battle.to_f)

		# Keep over an imaginary line!
		if v < (LinearStep(0.0, BATTLES_NO.to_f, battle.to_f)/ENEMIES.size.to_f) then
			v = LinearStep(0.0, BATTLES_NO.to_f, battle.to_f)/ENEMIES.size.to_f
		end

		# 3- A new attack using the previous imaginary line
	elsif battle <= BATTLES_NO then
		v = LinearStep(0.0, BATTLES_NO.to_f, battle.to_f)
		v /= ENEMIES.size.to_f

	# 4- Random chances
	else
		v = 1.0 / ENEMIES.size.to_f
	end

	return v * 100.0
end


def Type(battle, rng)

	# TODO, rather than export an already filled table, I can export
	# the chances and let the engine do the following procedure

	chances = Array.new(ENEMIES.size)
	sum = 0.0 # To normalize

	for i in 0...chances.size do
		chances[i] = EnemyChances(i, battle)
		sum += chances[i]
	end

	for i in 0...chances.size do
		chances[i] = (chances[i] / sum) * 100.0
	end

	# Chose one
	while 1 do

		i = rng.rand(0...ENEMIES.size)

		if rng.rand(100) < chances[i] then
			return ENEMIES[i]
		end
	end

	return "?"
end


####


rng1 = Random.new(SEED)
rng2 = Random.new(SEED)

layout = Array.new(MAX_ENEMIES)
prev_layout = Array.new(MAX_ENEMIES)

print("#include \"game-private.h\"\n\n")
print("static unsigned char s_battles_table[#{BATTLES_NO * MAX_ENEMIES}] = {\n")

for b in 0...BATTLES_NO do

	enemies_no = EnemiesNo(b, rng1)
	printf("\t/* Battle %02i (%i enemies) */ ", b, enemies_no)

	while 1 do
		# Fill a layout
		for i in 0...enemies_no do
			prev_layout[i] = layout[i]
			layout[i] = Type(b, rng2)
		end

		for i in enemies_no...MAX_ENEMIES do
			prev_layout[i] = layout[i]
			layout[i] = NO_ENEMY
		end

		# Different to the previous one?
		if not ((layout-prev_layout) + (prev_layout-layout)).empty? then
			break
		end
	end

	for i in 0...MAX_ENEMIES do
	printf("%s, ", layout[i])
	end

	print("\n")
end

print("};\n")

print("\nunsigned char Layout(unsigned char i, unsigned char battle_no)\n{\n")
	print("\treturn s_battles_table[(battle_no << 2) + i];\n")
print("}\n")
