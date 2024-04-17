#include <enet/enet.h>

#include "lib/wren/wren.h"

void hostAllocate(WrenVM* vm)
{
}

void hostFinalize(void* data)
{
}

void hostNew(WrenVM* vm)
{
    enet_initialize();
}
