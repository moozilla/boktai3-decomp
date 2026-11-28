#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "player/player_slot_status.h"
#include "scene/areas/area_effect_link.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "scene/terrain/terrain_sampling.h"
/* Player update using the half-rate link charge and alternate action table (tentative). */
extern u32 gUnk_08E87A90[];
extern const s16 gUnk_086149C4[];
void Player_AccumulateActionCharge(struct Player *, s32);
void sub_082420D4(struct Player *);
s32 Player_SelectInputMode(struct Player *);
void sub_0823B728(struct Player *, s32);
void sub_082424D0(struct Player *);
void Player_SmoothCameraAnchor(struct Player *);
void LinkPlayer_ClampToRecoveryTerrainCell(struct Player *);
void Player_UpdateModifier08Drift(struct Player *);
u32 Player_IsFlaggedWithGlobalModifier08(u32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
s32 Callback_InvokeActor(void *, void *);

static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline u8 Charge(s32 slot)
{
    if (!gLinkSession || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}
static inline s32 IsScene5(struct Player *p)
{
    if (p->active != 0 && p->sceneMode == 5)
        return 1;
    return 0;
}
void Player_UpdateHalfChargeLinkKind(struct Player *p)
{
    s32 idx;
    Player_SetStatusBits(&p->statusFlags, 0x4000);
    switch (p->kind) {
        case 1: {
            s32 cond;
            if (p->statusFlags & 0x10) {
                Player_AccumulateActionCharge(p, (Charge(p->slot) + 1) >> 1);
            }
            sub_082420D4(p);
            idx = Player_SelectInputMode(p);
            sub_0823B728(p, idx);
            Callback_InvokeActor(p, (void *)gUnk_08E87A90[idx]);
            if (p->movementVelocityLocked)
                *(struct SceneVec *)&p->velocityX = p->lockedVelocity;
            sub_082424D0(p);
            if (p->cameraOffsetActive != 0)
                Player_SmoothCameraAnchor(p);
            LinkPlayer_ClampToRecoveryTerrainCell(p);
            Player_UpdateModifier08Drift(p);
            if (p != NULL && IsScene5(p))
                break;
            cond = 0;
            if (!Player_IsFlaggedWithGlobalModifier08(p->movementStatus) && p->tetherMode == 1 &&
                (p->movementContactFlags & p->tetherMode))
                cond = (p->facing & p->tetherMode) != 0;
            if (cond) {
                struct ContactPosition pt;
                u32 ang;
                u8 *cell;
                u32 flags;
                if (++*(u16 *)&p->unk4A6 != 5)
                    break;
                pt = *(struct ContactPosition *)&p->position;
                ang = ((p->facing + 5) & 7) << 5;
                pt.x += Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 0x32);
                pt.z += Scale(gUnk_086149C4[ang] * 0x32);
                if (Terrain_SampleHeight(&pt) < (u32)(p->position.y + 0xc8))
                    break;
                cell = cellProps(Terrain_GetCellIndex(&pt));
                flags = ((struct TerrainCell *)cell)->flags;
                flags &= 4;
                if (flags)
                    break;
                p->coordinateAxis = p->facing;
                p->actionFourEnabled = flags;
                p->tetherMode = 4;
                p->tetherPhase = flags;
            } else {
                *(u16 *)&p->unk4A6 = cond;
            }
            break;
        }
        case 8:
            Player_SetSecondaryActionFlags(p, 8);
            if (p->tetherMode == 0x21) {
                idx = p->tetherMode;
            } else {
                u32 special = p->contactSpecial;
                idx = 0x1f;
                if (special != 0)
                    idx = 0x20;
            }
            Callback_InvokeActor(p, (void *)gUnk_08E87A90[idx]);
            sub_082424D0(p);
            break;
        case 2:
            Player_SetSecondaryActionFlags(p, 8);
            Callback_InvokeActor(p, (void *)p->triggerAnimationCallback);
            sub_082424D0(p);
            break;
    }
}
