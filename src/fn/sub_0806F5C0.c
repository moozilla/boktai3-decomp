#include "global.h"
#include "engine.h"
#include "game_state.h"
struct SceneEffectEntry {
    s16 x, y, z;
    u8 pad6[2];
    s16 ax, ay, az;
    u8 padE[2];
    s16 delay, time;
    u16 name, flagged;
    u8 d, f;
    u8 pad1A[2];
    void *effect;
};
extern void *gUnk_0200019C;
extern u32 gUnk_030053F0;
struct SceneEffectEntry *SceneEffectSchedule_Allocate(void *);
void *SceneScheduledEffect_Create(struct SceneEffectEntry *, u16 name, s32 time, u8 d, u8 f, s32, s32 ax, s32 ay,
                   s32 az, s32 delay, u16 flagged);
s32 Div(s32, s32);
s32 Mod(s32, s32);
static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline s32 scaleByDifficulty(s32 value)
{
    return Div((10 - gGameState->unk876) * value, 10);
}
static inline s32 scaleUnlessFlagged(s32 value)
{
    if (gEngineFlags & 0x800)
        return value;
    return scaleByDifficulty(value);
}
/* Registry routine FD6B: schedules a scene effect (unless engine flag 0x400 is
   set): -p position, -a vector, -f kind then a random delay in [min, max), -n,
   -d, -t time, -s; time and delay shrink with game state +0x876 unless engine
   flag 0x800 is set (tentative). */
void SceneEffectSchedule_AddFromScript(void)
{
    struct SceneEffectEntry *e;
    s32 time, delay;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gUnk_030053F0) & mask)
            return;
    }
    if (!gUnk_0200019C)
        return;
    e = SceneEffectSchedule_Allocate(gUnk_0200019C);
    if (!e)
        return;
    if (Script_SeekToKeyword('p')) {
        e->x = Script_GetValue();
        e->y = Script_GetValue();
        e->z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        e->ax = Script_GetValue();
        e->ay = Script_GetValue();
        e->az = Script_GetValue();
    }
    if (Script_SeekToKeyword('f')) {
        s32 min, range;
        e->f = Script_GetValue();
        min = Script_GetValue();
        range = Script_GetValue() - min;
        if (range == 0)
            e->delay = range;
        else
            e->delay = min + Mod(Random(), range);
    }
    e->name = Script_GetKeywordValue('n', 0);
    e->d = Script_GetKeywordValue('d', 0);
    e->time = Script_GetKeywordValue('t', 0);
    e->flagged = 0;
    if (Script_GetKeywordValue('s', 0))
        e->flagged = 1;
    time = scaleUnlessFlagged(e->time);
    delay = scaleUnlessFlagged(e->delay);
    e->effect =
        SceneScheduledEffect_Create(e, e->name, time, e->d, e->f, -1, e->ax, e->ay, e->az, delay, e->flagged);
}
