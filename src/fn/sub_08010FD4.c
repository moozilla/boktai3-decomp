#include "group_objects.h"
int GroupObjects_Update(struct GroupObjects *m)
{
    int i;
    struct GroupPiece *p;
    BoardPieces_DispatchCommands(m);
    if (m->active) {
        if (m->actorId)
            m->actor = sub_08225984(m->actorId);
        if (m->paletteActive == 0) {
            if (++m->paletteMask >= 63) {
                m->paletteMask = 63;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 1) {
            if (--m->paletteMask <= 0) {
                m->paletteMask = 0;
                m->paletteActive = -1;
            }
        } else if (m->paletteActive == 2) {
            if (++m->paletteMask > 31) {
                m->paletteMask = 32;
                m->paletteActive = -1;
            }
        }
        if (m->paletteActive >= 0) {
            void *first = gUnk_030042E4 + 0x5A60;
            void *second = gUnk_030042E4 + 0x5AA0;
            sub_08221110(m->palette, first, second, m->paletteMask, 6);
        }
        p = m->pieces;
        for (i = 0; i < 16; i++, p++) {
            gUnk_08604B48[p->state](m, p, i);
            BoardPiece_UpdateAnimation(p);
        }
        m->timer++;
    }
    return 0;
}
