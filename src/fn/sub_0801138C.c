#include "global.h"
extern u16 gUnk_03005260[];
void Sound_PlaySong(int);
void Script_Call(int, int);
struct Menu {
    u8 task[24];
    u32 timer;
    u8 pad[6];
    u16 script;
};
int MessageMenu_Update(struct Menu *m)
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
