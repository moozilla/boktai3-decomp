#include "global.h"
extern u16 gUnk_03005260[];
void sub_0822B2F8(int);
void sub_0821AD08(int, int);
struct Menu {
    u8 task[24];
    u32 timer;
    u8 pad[6];
    u16 script;
};
int MessageMenu_Update(struct Menu *m)
{
    if (m->timer <= 40 && !(m->timer & 1))
        sub_0822B2F8(628);
    if (m->timer > 59 && (gUnk_03005260[1] & 8)) {
        sub_0822B2F8(221);
        if (m->script) {
            sub_0821AD08(m->script, 0);
            m->script = 0;
        }
    }
    m->timer++;
    return 0;
}
