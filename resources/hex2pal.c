
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage, blah... blah...\n");
		return EXIT_FAILURE;
	}

	FILE* fp = NULL;
	int i = 0;

	union
	{
		int v;
		struct
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};
	} color;

	if ((fp = fopen(argv[1], "r")) != NULL)
	{
		while (fscanf(fp, "%6x", &color.v) != 0)
		{
			if (feof(fp) != 0)
				break;

			uint8_t r = (uint8_t)((color.r * 63) / 255);
			uint8_t g = (uint8_t)((color.g * 63) / 255);
			uint8_t b = (uint8_t)((color.b * 63) / 255);

			fwrite(&r, sizeof(uint8_t), 1, stdout);
			fwrite(&g, sizeof(uint8_t), 1, stdout);
			fwrite(&b, sizeof(uint8_t), 1, stdout);

			i++;
		}

		fclose(fp);

		for (; i < 256; i++)
		{
			int fill = 0;
			fwrite(&fill, sizeof(uint8_t), 3, stdout);
		}
	}
	else
	{
		fprintf(stderr, "Error opening '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
