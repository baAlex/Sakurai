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
# is a 40% of chances of fight a battle that has the number of a
# future one. An example: a player is fighting battles with two
# enemies, expecting to jump into those with three, but suddenly
# appears one that has four. Bad luck, a victim of those chances
NOISE_GATE = 40.0
NOISE_MIN = (BATTLES_NO.to_f / 4.0) / 2.0  # Minimum battles in the future
NOISE_MAX = (BATTLES_NO.to_f / 4.0)        # Maximum


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


def EnemiesNumber(battle, rng)

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


####


# Output chances
# ==============

chances = Array.new(ENEMIES.size)

print("#include \"utilities.h\"\n")
print("#include \"actor.h\"\n\n")
print("static uint8_t s_enemies_chances[#{BATTLES_NO * ENEMIES.size}] = {\n")

for b in 0...BATTLES_NO do

	sum = 0.0 # To normalize
	print("\t")

	for i in 0...chances.size do
		chances[i] = EnemyChances(i, b)
		sum += chances[i]
	end

	for i in 0...chances.size do
		chances[i] = (chances[i] / sum) * 100.0
		chances[i] = chances[i].round()

		print("#{chances[i]}, ")
	end

	print("\n")
end

print("};\n")


# Output numbers
# ==============

rng = Random.new(SEED)

print("\nstatic uint8_t s_enemies_number[#{BATTLES_NO}] = {\n")
print("\t")

for b in 0...BATTLES_NO do

	if b != 0 and (b % 8) == 0 then
		print("#{EnemiesNumber(b, rng)},\n\t")
	else
		if b != BATTLES_NO - 1 then
		print("#{EnemiesNumber(b, rng)}, ")
		else
		print("#{EnemiesNumber(b, rng)}\n")
		end
	end
end

print("};\n")


# Output function
# ===============
func = """
uint8_t Layout(uint8_t battle_no, uint8_t* out)
{
	uint8_t number = s_enemies_number[battle_no] % #{MAX_ENEMIES + 1};
	uint8_t off = (battle_no * #{ENEMIES.size});
	uint8_t enemy = 0;
	uint8_t i = 0;

	/* Chose enemies */
	for(i = 0; i < number; i++)
	{
	again:
		enemy = ((uint8_t)Random() % #{ENEMIES.size});

		if(((uint8_t)Random() % 100) < s_enemies_chances[off + enemy])
			out[i] = enemy + HEROES_NO;
		else
			goto again; /* TODO, limit this to some tries */
	}

	for(i = number; i < #{MAX_ENEMIES}; i++)
		out[i] = #{NO_ENEMY};

	/* Shuffle positions, ignoring right corner */
	for(i = 0; i < #{MAX_ENEMIES - 1}; i++)
	{
		off = (Random() % #{MAX_ENEMIES - 1});
		enemy = out[off];
		out[off] = out[i];
		out[i] = enemy;
	}

	return number;
}
"""

print(func)
