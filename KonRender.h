#ifndef KONHEADER_H_INCLUDED
#define KONHEADER_H_INCLUDED

#define x86_sse 1

//include only if sse is availiable
#ifdef x86_sse 1
#include "render_sse.h"
#else
#include "render.h"
#endif

#include "import.h"


#endif // KONHEADER_H_INCLUDED
