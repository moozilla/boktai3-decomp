#include "scene_pool.h"
s32 ScenePool_StateLateralWeave(void *manager, struct ScenePoolActor *actor,
                                struct ScenePoolEntry *entry)
{
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        if ((s32)entry->unknown5c > 0)
            entry->settingB = 1;
        else
            entry->settingB = -1;
        entry->turnStep = 0x180;
    }
    if (entry->stateFrame <= 7) {
        if (entry->settingB <= 0)
            actor->controlFlags |= 0x80;
        else
            actor->controlFlags |= 0x40;
    } else if (entry->stateFrame > 24) {
        if (entry->settingB > 0)
            actor->controlFlags |= 0x80;
        else
            actor->controlFlags |= 0x40;
    }
    if (entry->stateFrame > 59) {
        entry->turnStep = 0x200;
        switch (entry->kind) {
            case 0:
            case 1:
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
                break;
            case 2:
                if (entry->direction > 0) {
                    entry->state = 1;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                } else {
                    entry->state = 2;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                }
                break;
            case 3:
                if (entry->direction > 0) {
                    entry->state = 1;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                } else {
                    entry->state = 2;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                }
                break;
        }
        return 0;
    }
    entry->stateFrame++;
    return 0;
}
