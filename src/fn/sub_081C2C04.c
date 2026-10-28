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
extern u8 *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(void *, void *);
void Trigger_ConsumePending(void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void Sound_PauseChannel12(u32);
void SparkleEmitter_UpdateEmitting(struct SparkleEmitter *);
void SparkleEmitter_UpdateDraining(struct SparkleEmitter *);
static inline u8 Entry_MessageCount(Entry *entry)
{
    u8 *controller = gSceneEventDispatchState;
    return entry->count[controller[0x24]];
}

static inline void SparkleEmitter_SetUpdate(struct SparkleEmitter *p,
                                            void (*update)(struct SparkleEmitter *))
{
    p->update = update;
}

static inline void SparkleEmitter_SetTimer(struct SparkleEmitter *p, s32 timer)
{
    p->timer = timer;
}

static inline s16 *Entry_GetMessage(Entry *entry, s32 i)
{
    u8 *controller = gSceneEventDispatchState;
    return entry->messages[controller[0x24]][i];
}

static inline u8 SparkleEmitter_QuantizeDirection(s16 direction)
{
    return (direction + 5) << 5;
}

// confidence=low: handles the emitter's cutscene messages: 0xC starts emitting with a speed,
// direction and palette from the message, 0xD drains it.
void sub_081C2C04(struct SparkleEmitter *p)
{
    Entry *entry = &p->entry;
    s32 i;
    for (i = 0; i < Entry_MessageCount(entry); i++) {
        s16 *m = Entry_GetMessage(entry, i);
        SceneMessages_SetPendingTrigger(entry, m);
        switch (((u8 *)m)[6]) {
            case 12: {
                s32 speed = m[4];
                s16 direction = SparkleEmitter_QuantizeDirection(((u8 *)m)[10]);
                u32 palette = m[6];
                p->speed = speed;
                p->angle = direction;
                p->palette = palette;
                if (palette <= 14)
                    AnimData_SelectPaletteBank(p->animation, palette + 0x255);
                Sound_PlaySong(0x83);
                SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
                SparkleEmitter_SetTimer(p, 0);
                Trigger_ConsumePending(entry, 1);
                break;
            }

            case 13:
                Sound_PauseChannel12(8);
                SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateDraining);
                p->timer = 0;
                Trigger_ConsumePending(entry, 1);
                break;
        }
    }
}
