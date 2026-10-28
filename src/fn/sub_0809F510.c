#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"
#include "ram.h"

// confidence=low: hops within the stored home terrain cell.

extern const s16 gUnk_086149C4[];
struct CrowHopState {
    u8 unknown0[0x3E8];
    u16 homeCell;
    u8 unknown3EA[0x42];
    struct ContactPosition velocity;
};
static inline u32 Random(const u16 *table)
{
    u32 i = (gRandomIndex + 1) & 1023;
    gRandomIndex = i;
    i *= 2;
    return *(const u16 *)((u32)table + i);
}
static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 255] * speed);
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *a,
                       const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline void CopyWords(u32 control, const u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, control);
}
static inline void ZeroVector(u32 *zero, struct ContactPosition *dst)
{
    *zero = 0;
    CopyWords(0x05000002, zero, dst);
}
static inline void ClearVector(struct ContactPosition *v)
{
    u32 zero = 0;
    CpuSet(&zero, v, 0x05000002);
}
static inline void CellPosition(struct ContactPosition *p, s32 cell)
{
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    p->x = (x << 8) + 128;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    p->y = (*data & 15) << 8;
    p->z = (z << 8) + 128;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u8 TakeEntry(struct SceneActorRegistryResetView *actor, u8 **saved)
{
    u8 *field = &actor->field2B1;
    u32 value = *field;
    *saved = field;
    if (value) {
        *field = 0;
        return 1;
    }
    return 0;
}
// confidence=low: random horizontal motion and gravity keep the hop inside its home cell.
void Crow_HopWithinHomeCell(struct SceneActorRegistryResetView *actor)
{
    struct CrowHopState *state = actor->context;
    struct {
        u32 zero;
        struct ContactPosition home, next;
    } local;
    s32 ground = Chase_HeightAt(actor->terrainCell, &actor->position);
    u8 *arrived;
    s32 cell;
    if (TakeEntry(actor, &arrived)) {
        u32 angle;
        SetControl(actor, actor->savedTarget, 0);
        CellPosition(&local.home, state->homeCell);
        actor->direction = angle = Random(gRandomTable) & 255;
        ZeroVector(&local.zero, &state->velocity);
        Polar(&state->velocity, angle, (Random(gRandomTable) & 7) + 1);
        state->velocity.y = 14;
    }
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    Add(&local.next, &actor->position, &state->velocity);
    cell = Chase_CellAt(&local.next);
    if (state->homeCell != cell) {
        state->velocity.x = 0;
        state->velocity.z = 0;
    }
    actor->motion.x += state->velocity.x;
    actor->motion.z += state->velocity.z;
    actor->field2EA += state->velocity.y;
    state->velocity.y -= 2;
    if (ground >= (s16)actor->field2EA) {
        actor->position.y = ground;
        actor->field2EA = ground;
        ZeroVector(&local.zero, &state->velocity);
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        *arrived = 1;
    } else
        actor->counter2AC++;
}
