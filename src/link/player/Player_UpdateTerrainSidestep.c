#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "scene/terrain/terrain_sampling.h"

void Player_SetTetherMode(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(void *, s32, s32);
void Player_PlayLocalSong(s32, u32);
s32 Player_IsFlaggedWithGlobalModifier08(u32);
void Player_EmitPositionEffect(void *, void *, u32, u32, void *, u32, u32);

extern const s16 gUnk_086149C4[];

static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline void SetFlags(u32 *field, u32 mask)
{
    u32 bits = mask;
    *field = bits | *field;
}
// confidence=low: terrain probes select sideways motion along the coordinate axis.
void Player_UpdateTerrainSidestep(struct Player *player)
{
    struct Player *p = player;
    struct ContactPosition pt;
    struct ContactPosition effect;
    s32 anim, direction, a, b;
    s32 height, index, speed, frame, delta;
    player->tetherMode = 4;
    if (p->tetherPhase == 1 && player->inputKeys[1] & 1)
        Player_SetTetherMode(p, 4, 2);
    switch (p->tetherPhase) {
        case 0:
            SetFlags(&p->statusFlags, 4);
            anim = ActorVariant_GetTableValue(p->actionMode, 2);
            if (p->animationIndex != anim) {
                Player_PlayLocalSong(p->slot, 0xD0);
                p->facing = (p->coordinateAxis + 4) & 7;
                if (p->coordinateAxis > 4) {
                    p->heading = (8 - p->coordinateAxis) >> 1;
                    p->side = 1;
                } else {
                    p->heading = p->coordinateAxis >> 1;
                    p->side = 0;
                }
            }
            if (Player_SetAnimationIndex(p, anim, 0x40))
                Player_SetTetherMode(p, 4, 1);
            break;
        case 1:
            SetFlags(&p->statusFlags, 8);
            p->actionFourEnabled = 1;
            direction = (s16)p->directionHistory[0];
            a = (p->coordinateAxis + 7) & 7;
            b = (p->coordinateAxis + 1) & 7;
            anim = ActorVariant_GetTableValue(p->actionMode, 3);
            if (direction == a || direction == b) {
                delta = (direction - p->coordinateAxis) * 2 + 8;
                direction = (p->coordinateAxis + delta) & 7;
                direction = (direction + 1) & 7;
                pt = *(struct ContactPosition *)&p->position;
                pt.x -= Scale(gUnk_086149C4[(direction * 32 + 64) & 255] * 50);
                pt.z -= Scale(gUnk_086149C4[(direction * 32) & 255] * 50);
                index = Terrain_GetCellIndex(&pt);
                height = Terrain_SampleHeight(&pt);
                if (height > p->position.y - 128 && height < p->position.y + 128 &&
                    !Player_IsFlaggedWithGlobalModifier08(
                        ((struct TerrainCell *)cellProps(index))->flags)) {
                    pt.x -= Scale(gUnk_086149C4[((((p->coordinateAxis + 1) & 7) * 32) + 64) & 255] *
                                  50);
                    pt.z -= Scale(gUnk_086149C4[((p->coordinateAxis + 1) & 7) * 32] * 50);
                    if ((s32)Terrain_SampleHeight(&pt) >= height + 200) {
                        u32 flags =
                            ((struct TerrainCell *)cellProps(Terrain_GetCellIndex(&pt)))->flags;
                        if (!(flags & 4)) {
                            anim = ActorVariant_GetTableValue(p->actionMode, 4);
                            frame = p->animation.frameIndex;
                            speed = 0;
                            if (frame != 0) {
                                speed = 3;
                                if (frame == 2)
                                    speed = 6;
                            }
                            p->velocityX =
                                -Scale(gUnk_086149C4[(direction * 32 + 64) & 255] * speed);
                            p->velocityZ = -Scale(gUnk_086149C4[(direction * 32) & 255] * speed);
                        }
                    }
                }
            }
            Player_SetAnimationIndex(p, anim, 0x40);
            if (p->animationIndex == 24 && p->animation.frameIndex == 0 &&
                p->animation.elapsed == 1)
                Player_PlayLocalSong(p->slot, 0xDB);
            break;
        case 2:
            SetFlags(&p->statusFlags, 8);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 5), 0x40)) {
                effect = *(struct ContactPosition *)&p->position;
                if (p->coordinateAxis == 1) {
                    effect.x += 28;
                    effect.y += 44;
                    effect.z -= 76;
                } else if (p->coordinateAxis == 3) {
                    effect.x -= 52;
                    effect.y += 0;
                    effect.z += 0;
                } else if (p->coordinateAxis == 5) {
                    effect.x += 12;
                    effect.y += 44;
                    effect.z -= 32;
                } else if (p->coordinateAxis == 7) {
                    effect.x -= 64;
                    effect.y += 68;
                    effect.z += 44;
                }
                { /* The object-push variant uses this position for its effect terrain test. */
                    struct ContactPosition probe;
                    probe = *(struct ContactPosition *)&p->position;
                    probe.x -= Scale(
                        gUnk_086149C4[((((p->coordinateAxis + 1) & 7) << 5) + 64) & 255] * 50);
                    probe.z -= Scale(gUnk_086149C4[((p->coordinateAxis + 1) & 7) << 5] * 50);
                }
                Player_EmitPositionEffect(p, &p->position, 256, 1, &effect, 0, 236);
                Player_SetTetherMode(p, 4, 1);
            }
            break;
    }
}
