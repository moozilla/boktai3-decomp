#include "palette_controller.h"

void sub_08002C38(struct PaletteController *, struct PaletteTransition *);
void sub_08002E00(struct PaletteController *, struct PaletteTransition *);
void sub_08002D58(struct PaletteController *, struct PaletteTransition *);

void sub_08002E54(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    sub_08002C38(p, t);
    t->level = 0x40;
    sub_08002E00(p, t);
    sub_08002D58(p, t);
}
