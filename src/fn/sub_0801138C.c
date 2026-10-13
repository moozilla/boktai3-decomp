#include "message_menu.h"
int MessageMenu_Update(struct MessageMenu *m)
{
    if (m->timer <= 40 && !(m->timer & 1))
        Sound_PlaySong(628);
    if (m->timer > 59 && (gUnk_03005260[1] & 8)) {
        Sound_PlaySong(221);
        if (m->script) {
            Script_Call(m->script, 0);
            m->script = 0;
        }
    }
    m->timer++;
    return 0;
}
