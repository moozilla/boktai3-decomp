#include "global.h"
#include "engine.h"
#include "player.h"
/* Per-update handler for the player in link-play kinds one, two and eight (tentative). */
u8 *TerrainModifiers_FindCellOverride(s32, s32);
struct LinkRefs {
    u8 unk0[0x5c];
    u8 charge[8];
};
extern struct LinkRefs *gUnk_020004B4;
extern u32 gUnk_08E87A10[];
extern const s16 gUnk_086149C4[];
void Player_AccumulateActionCharge(struct Player *, u32);
void sub_082420D4(struct Player *);
s32 Player_SelectInputMode(struct Player *);
void sub_0823B728(struct Player *, s32);
void sub_082424D0(struct Player *);
void sub_0823B7A4(struct Player *);
void Player_ClampPositionToStoredTerrainCell_0823B938(struct Player *);
void Player_UpdateModifier08Drift(struct Player *);
u32 Player_IsFlaggedWithGlobalModifier08(u32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
s32 Callback_InvokeActor(void *, void *);

struct FloorPoint {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline void FlagOr(u32 m, u32 *f)
{
    *f |= m;
}
static inline u8 Charge(s32 slot)
{
    if (!gUnk_020004B4 || slot < 0)
        return 0;
    return gUnk_020004B4->charge[slot];
}
static inline s32 IsScene5(struct Player *p)
{
    if (p->active != 0 && p->sceneMode == 5)
        return 1;
    return 0;
}
static inline u8 *CellAt(struct FloorPoint *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    s32 index;
    u8 *cell;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        return cell + 4;
    return gTerrainMap->cells + 12 + index * 4;
}
static inline u32 GroundHeight(struct FloorPoint *position)
{
    u8 *cell = CellAt(position);
    u32 kind = *cell >> 4;
    s32 height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
void Player_UpdateLinkKind(struct Player *p)
{
    s32 idx;
    FlagOr(0x4000, &p->statusFlags);
    switch (p->kind) {
        case 1: {
            s32 cond;
            if (p->actionMode == 1 && (p->statusFlags & 0x10)) {
                u32 charge = Charge(p->slot);
                Player_AccumulateActionCharge(p, charge);
            }
            sub_082420D4(p);
            idx = Player_SelectInputMode(p);
            sub_0823B728(p, idx);
            Callback_InvokeActor(p, (void *)gUnk_08E87A10[idx]);
            if (p->movementVelocityLocked)
                *(struct SceneVec *)&p->velocityX = *(struct SceneVec *)((u8 *)p + 0x490);
            sub_082424D0(p);
            if (p->cameraOffsetActive != 0)
                sub_0823B7A4(p);
            Player_ClampPositionToStoredTerrainCell_0823B938(p);
            Player_UpdateModifier08Drift(p);
            if (p != NULL && IsScene5(p))
                break;
            cond = 0;
            if (!Player_IsFlaggedWithGlobalModifier08(*(u16 *)((u8 *)p + 0x292)) &&
                p->tetherMode == 1 && (*(u8 *)((u8 *)p + 0x284) & p->tetherMode))
                cond = (p->facing & p->tetherMode) != 0;
            if (cond) {
                struct PackedVec pt;
                u32 ang;
                u8 *cell;
                u32 flags, four;
                if (++*(u16 *)&p->unk4A6 != 5)
                    break;
                pt = *(struct PackedVec *)&p->position;
                ang = ((p->facing + 5) & 7) << 5;
                pt.x += Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 0x32);
                pt.z += Scale(gUnk_086149C4[ang] * 0x32);
                if (GroundHeight((struct FloorPoint *)&pt) < (u32)(p->position.y + 0xc8))
                    break;
                cell = CellAt((struct FloorPoint *)&pt);
                flags = *(u16 *)(cell + 2);
                four = 4;
                flags &= four;
                if (flags)
                    break;
                p->coordinateAxis = p->facing;
                p->actionFourEnabled = flags;
                p->tetherMode = four;
                p->tetherPhase = flags;
            } else {
                *(u16 *)&p->unk4A6 = cond;
            }
            break;
        }
        case 8: {
            Player_SetSecondaryActionFlags(p, 8);
            if (p->tetherMode == 0x21) {
                idx = p->tetherMode;
            } else {
                u32 special = p->contactSpecial;
                idx = 0x1f;
                if (special != 0)
                    idx = 0x20;
            }
            Callback_InvokeActor(p, (void *)gUnk_08E87A10[idx]);
            sub_082424D0(p);
            break;
        }
        case 2:
            Player_SetSecondaryActionFlags(p, 8);
            Callback_InvokeActor(p, (void *)p->triggerAnimationCallback);
            sub_082424D0(p);
            break;
    }
}
