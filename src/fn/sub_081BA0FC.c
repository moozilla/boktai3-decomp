#include "global.h"

struct Q {
    u8 f[0x52a];
    u16 h52a;
    u8 g[0x2c];
    void *p558;
};
void MenuCursor_EnableInput(void *);
void Registry2388_SetUpdate(void *, void (*)(void));
void Registry2388_UpdateSelection(void);

void Registry2388_BeginSelection(struct Q *p)
{
    if (p->h52a == 0)
        MenuCursor_EnableInput(p->p558);
    Registry2388_SetUpdate(p, Registry2388_UpdateSelection);
}
