// MATCHING_CONCESSION: register-pinning
#include "hit_switch.h"
void HitSwitch_AnimateBobbingCompletion(struct HitSwitchEntry *entry)
{
    register u16 *frame asm("r4") = &entry->frame;
    if (*frame == 0)
        Sound_PlaySong(426);
    {
        u32 next = *frame + 1;
        *frame = next;
        if ((u16)next <= 7) {
            entry->sprite.position = entry->world;
            entry->sprite.frame = 1;
        } else {
            entry->sprite.frame = 2;
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateCompletedBobbing);
            if (entry->completedScriptId) {
                s32 value;
                struct HitSwitchScriptArgs args;
                args.count = 1;
                args.values = &value;
                value = entry->tag;
                Script_Call(entry->completedScriptId, &args);
            }
        }
    }
}
