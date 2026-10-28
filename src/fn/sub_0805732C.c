#include "level_advance_display.h"
void Sound_PlaySong(u32);
s32 LevelAdvanceDisplay_UpdateManager(struct LevelAdvanceManager *manager)
{
    if (gPlayers[0]) {
        if (gPlayers[0]->mode == 4) {
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
            LevelAdvanceDisplay_UpdateAnimation(manager);
            if (LevelAdvanceDisplay_TryAdvanceLevel(manager)) {
                LevelAdvanceDisplay_StartLevelAnimation(manager);
                Sound_PlaySong(904);
            } else if (manager->kind == 0 &&
                       (kind = LevelAdvanceDisplay_PollSecondaryEvent(manager)) >= 0) {
                LevelAdvanceDisplay_StartSecondaryAnimation(manager, kind);
                Sound_PlaySong(897);
            }
        }
    }
    return 0;
}
