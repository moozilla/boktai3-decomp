#include "hit_switch.h"
#define HitSwitch_AnimateStaticCompletion sub_08054394
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
        sub_08054294(entry, sub_0805440C);
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
