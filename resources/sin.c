
// $ clang ./resources/sin.c -lm -o sin

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
	float fi = 0.0f;

	fprintf(stdout, "static int8_t s_sin_table[128] = {\\\n");

	for (int i = 0; i < 128; i++)
	{
		fi += M_PI / 128.0f;
		float value = (sinf(fi) * 127);

		fprintf(stdout, " %i%s", (int)value, (i != 255) ? "," : "");
		fprintf(stdout, "%s", (i >= 16 && (i % 16) == 0) ? "\\\n" : "");
	}

	fprintf(stdout, "};\n");

	return EXIT_SUCCESS;
}
