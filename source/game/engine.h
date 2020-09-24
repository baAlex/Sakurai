/*-----------------------------

 [engine.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef ENGINE_H
#define ENGINE_H

#if defined(SAKURAI_DOS)
#include "engine-dos.h"
#else
#include "engine-sdl.h"
#endif

#endif
