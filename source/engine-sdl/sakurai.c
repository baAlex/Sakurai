/*-----------------------------

MIT License

Copyright (c) 2020 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [sakurai.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "japan-status.h"
#include "japan-version.h"


#define NAME "Sakurai"
#define VERSION "0.2-alpha"
#define NAME_FULL "Sakurai v0.2-alpha"


int main()
{
	struct jaStatus st = {0};
	struct ContextEvents evn = {0};

	struct Context* context = NULL;

	printf("%s v%s\n", NAME, VERSION);
	printf(" - LibJapan %i.%i.%i\n", jaVersionMajor(), jaVersionMinor(), jaVersionPatch());

	if ((context = ContextCreate(NULL, NAME_FULL, &st)) == NULL)
		goto return_failure;

	while (1)
	{
		if (ContextUpdate(context, &evn, &st) != 0)
			goto return_failure;

		if (evn.close == true)
			break;

		printf("%s, %s, %s, %s, %s, %s\n", (evn.a == true) ? "A" : "-", (evn.b == true) ? "B" : "-",
		       (evn.x == true) ? "X" : "-", (evn.y == true) ? "Y" : "-", (evn.lb == true) ? "LB" : "--",
		       (evn.rb == true) ? "RB" : "--");
	}

	// Bye!
	ContextDelete(context);
	return EXIT_SUCCESS;

return_failure:
	if (context != NULL)
		ContextDelete(context);
	jaStatusPrint(NAME, st);
	return EXIT_FAILURE;
}
