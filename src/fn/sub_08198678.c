#include "global.h"

struct Vec3 {
    s16 x, y, z;
};
struct PackedPosition {
    u32 x : 16, y : 16, z : 16, unused : 16;
};
struct SceneEffectActor {
    u8 unk_00[8];
    u8 resource[0x20];
    u8 animation[8];
    u32 flags;
    u8 unk_34[0x48 - 0x34];
    struct Vec3 position;
    /* Vec3 occupies eight bytes under the original ABI. */
    u8 unk_50[0x88 - 0x50];
    u8 shadow[0x154 - 0x88];
    u8 entered, timer, done, state;
};
struct PositionOwner {
    u8 unk_00[0x30];
    struct Vec3 position;
};
extern struct PositionOwner *gUnk_02000710;
extern struct Vec3 gUnk_03005418;
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ScaledShadow_Enable(void *);
s32 __divsi3(s32, s32);

static inline s32 TruncateShift(s32 value, s32 shift)
{
    s32 result;
    if (value >= 0)
        result = value >> shift;
    else
        result = -((-value) >> shift);
    return result;
}
static inline void ProjectPosition(struct Vec3 *out, struct Vec3 *in)
{
    s32 a, b, c, d, x = in->x, z = in->z;
    out->x = TruncateShift(3 * (x - z), 5);
    a = TruncateShift(3 * (x + z), 5);
    b = TruncateShift(3 * in->y, 5);
    c = a - b;
    d = a + b;
    out->x = out->x - gUnk_03005418.x + 120;
    out->y = c - gUnk_03005418.y + 80;
    out->z = d - gUnk_03005418.z;
}
static inline void UnprojectPosition(struct Vec3 *out, struct Vec3 *in, s32 height)
{
    s32 x, y, value;
    x = in->x + gUnk_03005418.x - 120;
    y = in->y + gUnk_03005418.y - 80;
    value = __divsi3((x + y) << 5, 3) + height;
    out->x = TruncateShift(value, 1);
    out->y = height;
    value = __divsi3((y - x) << 5, 3) + height;
    out->z = TruncateShift(value, 1);
}

// Tentative: enters from the screen edge before resuming its world-space animation.
void SceneEffect_UpdateArrival(struct SceneEffectActor *actor)
{
    struct PackedPosition input;
    struct Vec3 screen;
    struct Vec3 world;
    struct PositionOwner *player;
    s32 nextState;
    if (actor->entered == 0) {
        Anim_SetKeyframes(actor->animation, actor->resource, 5, 1, 0);
        actor->flags |= 0x10;
        player = gUnk_02000710;
        input.x = player->position.x;
        input.y = player->position.y;
        input.z = player->position.z;
        ProjectPosition(&screen, (struct Vec3 *)&input);
        if (actor->flags & 4)
            screen.x = 264;
        else
            screen.x = -24;
        actor->position.x = screen.x;
        actor->position.y = screen.y;
        actor->entered++;
    } else {
        if (actor->flags & 4) {
            actor->position.x -= 7;
            if (actor->position.x > 184)
                return;
            actor->position.x = 184;
        } else {
            actor->position.x += 7;
            if (actor->position.x < 56)
                return;
            actor->position.x = 56;
        }
        actor->flags &= ~0x10;
        UnprojectPosition(&world, &actor->position, gUnk_02000710->position.y);
        actor->position.x = world.x;
        actor->position.y = world.y;
        actor->position.z = world.z;
        Anim_SetKeyframes(actor->animation, actor->resource, 0, 2, 0);
        ScaledShadow_Enable(actor->shadow);
        nextState = 7;
        actor->state = nextState;
        actor->entered = 0;
        actor->timer = 0;
    }
}
