#include "global.h"

void Registry3F74_SetState(void *, void (*)(void));
void Registry3F74_UpdateSelection(void);

void Registry3F74_StartSelection(void *p)
{
    Registry3F74_SetState(p, Registry3F74_UpdateSelection);
}
