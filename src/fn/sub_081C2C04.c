#include "global.h"
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
} Entry;
struct SparkleEmitter {
    u8 pad0[0x718];
    u8 animation[0x734 - 0x718];
    Entry entry;
    u8 pad[0x768 - 0x734 - sizeof(Entry)];
    s32 palette;
    u8 pad76C[4];
    s32 timer;
    u16 angle;
    s16 speed;
    u8 pad778[4];
    void (*update)(struct SparkleEmitter *);
};
extern u8 *gUnk_0200047C;
void SceneMessages_SetPendingTrigger(void *, void *);
void Trigger_ConsumePending(void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void Sound_PauseChannel12(u32);
void SparkleEmitter_UpdateEmitting(struct SparkleEmitter *);
void SparkleEmitter_UpdateDraining(struct SparkleEmitter *);
static inline s32 Entry_MessageCount(Entry *entry)
{
    u8 *controller = gUnk_0200047C;
    return entry->count[controller[0x24]];
}
// confidence=low: handles the emitter's cutscene messages: 0xC starts emitting with a speed,
// direction and palette from the message, 0xD drains it.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM does not hoist the field addresses out of the loop (it uses only r4-r7), and cross-jumping merges the two loop tests.
void sub_081C2C04(struct SparkleEmitter *p)
{
    Entry *entry = &p->entry;
    s32 i;
    for (i = 0; i < Entry_MessageCount(entry); i++) {
        s16 *m = entry->messages[gUnk_0200047C[0x24]][i];
        SceneMessages_SetPendingTrigger(entry, m);
        switch (((u8 *)m)[6]) {
            case 12: {
                s32 speed = m[4];
                u8 direction = ((u8 *)m)[10];
                u32 palette = m[6];
                p->speed = speed;
                p->angle = (u8)((direction + 5) << 5);
                p->palette = palette;
                if (palette <= 14)
                    AnimData_SelectPaletteBank(p->animation, palette + 0x255);
                Sound_PlaySong(0x83);
                p->update = SparkleEmitter_UpdateEmitting;
                p->timer = 0;
                Trigger_ConsumePending(entry, 1);
                break;
            }

            case 13:
                Sound_PauseChannel12(8);
                p->update = SparkleEmitter_UpdateDraining;
                p->timer = 0;
                Trigger_ConsumePending(entry, 1);
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C2C04);
#endif
