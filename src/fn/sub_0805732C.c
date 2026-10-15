#include "level_advance_display.h"
#define LevelAdvanceDisplay_UpdateManager sub_0805732C
void sub_0822B2F8(u32);
s32 LevelAdvanceDisplay_UpdateManager(struct LevelAdvanceManager *manager)
{
    if (gUnk_02000580[0]) {
        if (gUnk_02000580[0]->mode == 4) {
            if (manager->active == 1) {
                s32 i;
                manager->sprite.flags |= 1;
                manager->active = 0;
                for (i = 0; i < 8; i++) {
                    manager->particles[i].flags |= 1;
                    manager->particles[i].active = 0;
                }
            }
        } else {
            s32 kind;
            sub_08057158(manager);
            if (sub_08056F30(manager)) {
                sub_080570BC(manager);
                sub_0822B2F8(904);
            } else if (manager->kind == 0 && (kind = sub_08056F80(manager)) >= 0) {
                sub_08057114(manager, kind);
                sub_0822B2F8(897);
            }
        }
    }
    return 0;
}
