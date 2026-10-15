#include "hit_switch.h"
void HitSwitch_AnimateStaticCompletion(struct HitSwitchEntry *entry)
{
    entry->sprite.renderMode = 2;
    if (entry->frame == 0)
        Sound_PlaySong(426);
    entry->frame++;
    if (entry->frame <= 7)
        entry->sprite.frame = 1;
    else {
        entry->sprite.frame = 2;
        HitSwitch_SetUpdate(entry, HitSwitch_TickCompletedStatic);
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
