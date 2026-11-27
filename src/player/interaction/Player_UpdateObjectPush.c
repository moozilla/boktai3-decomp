#include "player/player.h"
#include "engine/engine.h"
#include "player/player_slot_status.h"

// confidence=low: action phases push neighboring scene objects and emit position effects.

struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct TerrainCell {
    u16 unk0;
    u16 flags;
};

struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32 song);
s32 PuzzleBlock_TryPushNeighborByIndex(void *position, u32 index);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_SetActionPhase(void *p, u8 v);
u32 Player_IsStatusBit3ActionActive(u32 x);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_PlayPositionEffect(struct Player *, struct SceneVec *, s16, u32, void *, s32, u32);
s32 SlidingDoors_HasTypeTwoResource(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_IsWithinCoordinateBand(void *);
s32 Player_CheckFacingTerrainClearance(void *);

static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline int tileIndex(struct PackedVec *const v)
{
    int x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline int heightAt(struct PackedVec *const v, int i)
{
    u8 *c = cell(i);
    int slope = *c >> 4;
    int height = (*c & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)v->z;
            break;
        case 2:
            height -= (u8)v->x;
            break;
    }
    return height;
}

static inline int isTwo(struct Player *p)
{
    int r;
    u16 q = p->active;
    r = 0;
    if (q != 0 && p->sceneMode == 2)
        r = 1;
    return r;
}

static inline u32 CellHasFlag(u16 flags, u32 mask)
{
    u32 m = mask;
    return flags & m;
}

void Player_UpdateObjectPush(struct Player *p)
{
    s32 animation;
    u8 state;
    struct PackedVec v;
    struct PackedVec a;
    struct PackedVec b;
    struct PackedVec c;
    if (!Player_CheckFacingTerrainClearance(p)) {
        ActorSequence_Begin(p, 0, 0);
        return;
    }
    if ((p->tetherPhase == 1) && (p->inputKeys[1] & 1))
        Player_SetActionPhase(p, 2);
    state = p->tetherPhase;
    switch (state) {
        case 0:
            Player_SetStatusBits(&p->statusFlags, 4);
            animation = ActorVariant_GetTableValue(p->actionMode, 2);
            if (p->animationIndex != animation) {
                Sound_PlaySong(0xd0);
                p->facing = (p->coordinateAxis + 4) & 7;
                if (p->coordinateAxis > 4) {
                    p->heading = (8 - p->coordinateAxis) >> 1;
                    p->side = 1;
                } else {
                    p->heading = p->coordinateAxis >> 1;
                    p->side = state;
                }
            }
            if (Player_SetAnimation(p, animation, 0x40, 1) == 0)
                return;
            Player_SetActionPhase(p, 1);
            return;

        case 1: {
            int t;
            int previousDirection;
            int nextDirection;
            s32 height;
            int nextHeight;
            Player_SetStatusBits(&p->statusFlags, 8);
            p->actionFourEnabled = state;
            t = (s16)p->directionHistory[0];
            previousDirection = (p->coordinateAxis + 7) & 7;
            nextDirection = (p->coordinateAxis + 1) & 7;
            animation = ActorVariant_GetTableValue(p->actionMode, 3);
            p->visualEffects7A0[0].pending = state;
            if ((t != previousDirection) && (t != nextDirection)) {
                if (Player_IsWithinCoordinateBand(p)) {
                    if (PuzzleBlock_TryPushNeighborByIndex(&p->position, p->facing) > 0) {
                        if ((p != 0) && isTwo(p)) {
                            ActorSequence_Begin(p, 0, 0);
                        } else {
                            ActorSequence_Begin(p, 5, 0);
                            p->actionFourEnabled = 0;
                        }
                    }
                }
            } else {
                int s;
                int idx;
                int d;
                int angle, scaled;
                d = p->coordinateAxis;
                {
                    int turn = (t - d) * 2 + 8;
                    d += turn;
                    d &= 7;
                    t = d + 1;
                    t &= 7;
                }
                v = *(struct PackedVec *)&p->position;
                v.x -= scale(gUnk_086149C4[((t << 5) + 0x40) & 0xff] * 50);
                angle = t << 5;
                scaled = gUnk_086149C4[angle & 0xff] * 50;
                s = angle;
                v.z -= scale(scaled);
                idx = tileIndex(&v);
                {
                    struct PackedVec *probe = &v;
                    height = heightAt(probe, tileIndex(probe));
                }
                if ((height > (p->position.y - 0x80)) && (height < (p->position.y + 0x80))) {
                    if (!Player_IsStatusBit3ActionActive(
                            ((struct TerrainCell *)cell(idx))->flags)) {
                        v.x -= scale(
                            gUnk_086149C4[((((p->coordinateAxis + 1) & 7) << 5) + 0x40) & 0xff] *
                            50);
                        v.z -= scale(gUnk_086149C4[((p->coordinateAxis + 1) & 7) << 5] * 50);
                        {
                            struct PackedVec *probe = &v;
                            nextHeight = heightAt(probe, tileIndex(probe));
                        }
                        if (nextHeight >= (height + 0xc8)) {
                            if (!(CellHasFlag(((struct TerrainCell *)cell(tileIndex(&v)))->flags,
                                              4))) {
                                int m;
                                u16 q;
                                animation = ActorVariant_GetTableValue(p->actionMode, 4);
                                q = p->animation.frameIndex;
                                m = 0;
                                if (q) {
                                    m = 3;
                                    if (q == 2)
                                        m = 6;
                                }
                                p->velocityX = -scale(gUnk_086149C4[(s + 0x40) & 0xff] * m);
                                p->velocityZ = -scale(gUnk_086149C4[s & 0xff] * m);
                            }
                        }
                    }
                }
            }
            Player_SetAnimation(p, animation, 0x40, 1);
            if (((p->animationIndex == ActorVariant_GetTableValue(p->actionMode, 4)) &&
                 (p->animation.frameIndex == 0)) &&
                (p->animation.elapsed == 1))
                Sound_PlaySong(0xdb);
            return;
        }

        case 2: {
            int d;
            Player_SetStatusBits(&p->statusFlags, 8);
            p->visualEffects7A0[0].pending = 1;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 5), 0x40, 1))
                return;
            a = *(struct PackedVec *)&p->position;
            b = *(struct PackedVec *)&p->position;
            d = p->coordinateAxis;
            if (d == 1) {
                a.x += 0x1c;
                a.y += 0x2c;
                a.z -= 0x4c;
                b.x += 0x40;
                b.z -= 0x80;
            } else if (d == 3) {
                a.x -= 0x34;
                a.y += 0;
                a.z += 0;
                b.x += 0x80;
                b.z += 0x40;
            } else if (d == 5) {
                a.x += 0xc;
                a.y += 0x2c;
                a.z -= 0x20;
                b.x += 0x40;
                b.z += 0x80;
            } else if (d == 7) {
                a.x -= 0x40;
                a.y += 0x44;
                a.z += 0x2c;
                b.x -= 0x80;
                b.z += 0x40;
            }
            c = *(struct PackedVec *)&p->position;
            c.x -= scale(gUnk_086149C4[((((p->coordinateAxis + 1) & 7) << 5) + 0x40) & 0xff] * 50);
            c.z -= scale(gUnk_086149C4[((p->coordinateAxis + 1) & 7) << 5] * 50);
            if (CellHasFlag(((struct TerrainCell *)cell(tileIndex(&c)))->flags, 0x100))
                Player_PlayPositionEffect(p, &p->position, 0x100, 1, &a, 1, 0xeb);
            else
                Player_PlayPositionEffect(p, &p->position, 0x100, 1, &a, 0, 0xec);
            if (SlidingDoors_HasTypeTwoResource(tileIndex(&b)))
                Sound_PlaySong(0x184);
            Player_SetActionPhase(p, 1);
            return;
        }
    }
}
