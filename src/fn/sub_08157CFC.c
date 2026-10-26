#include "global.h"
#include "engine.h"
#include "player.h"
// confidence=low: action callback table and nearby interaction routines establish the family.
void Player_EmitPeriodicParticles(struct Player *);
s32 Player_CanUseSceneFeature(struct Player *);
void Player_UpdateTimedScriptEvent(struct Player *);
void Player_UpdateDirectionHistory(struct Player *);
void Player_SelectActionSequence(struct Player *);
void Player_UpdateCameraInput(struct Player *, s32);
void Player_RunPendingCallback(struct Player *);
void Player_ApplyDirectionalForce(struct Player *);
void Player_ApplyControlPresentation(struct Player *);
void Player_BlendCameraTarget(struct Player *);
void Player_ClampPositionToStoredTerrainCell(struct Player *);
void Player_UpdateLockedMovement(struct Player *);
s32 Player_IsStatusBit3ActionActive(u16);
void Player_AddControlFlags(struct Player *, u16);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdatePeriodicParticles(struct Player *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
extern void *const gUnk_08610C0C[];
extern const s16 gUnk_086149C4[];
union ProbePosition {
    struct SceneVec point;
    struct {
        u32 x : 16, y : 16, z : 16;
    } words;
};
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cellProperties(s32 index)
{
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        return cell + 4;
    return gTerrainMap->cells + 12 + index * 4;
}
static inline s32 groundHeight(struct SceneVec *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = cellProperties(index);
    u32 kind = *cell >> 4;
    height = (*cell & 15) << 8;
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
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u32 specialState(struct Player *p)
{
    u32 active = p->active;
    u32 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
static inline u32 nonzero(u32 value)
{
    return (-value | value) >> 31;
}
static inline u32 terrainFlag(struct SceneVec *pos)
{
    u32 flags = *(u16 *)(cellProperties(cellIndex(pos)) + 2);
    return flags & 4;
}
void Player_UpdateInteractionMovement(struct Player *p)
{
    union ProbePosition pos;
    u32 mask = 0x4000;
    p->statusFlags |= mask;
    switch (p->kind) {
        case 1:
            Player_EmitPeriodicParticles(p);
            if (Player_CanUseSceneFeature(p))
                Player_UpdateTimedScriptEvent(p);
            Player_UpdateDirectionHistory(p);
            Player_SelectActionSequence(p);
            Player_UpdateCameraInput(p, p->tetherMode);
            if (p->unk45C == 1)
                Player_RunPendingCallback(p);
            Callback_InvokeActor(p, gUnk_08610C0C[p->tetherMode]);
            if (p->movementVelocityLocked)
                *(struct SceneVec *)&p->velocityX = *(struct SceneVec *)((u8 *)p + 0x490);
            else
                Player_ApplyDirectionalForce(p);
            Player_ApplyControlPresentation(p);
            if (p->cameraOffsetActive)
                Player_BlendCameraTarget(p);
            Player_ClampPositionToStoredTerrainCell(p);
            Player_UpdateLockedMovement(p);
            if (p && specialState(p))
                break;
            {
                u32 enabled = 0;
                if (!Player_IsStatusBit3ActionActive(*(u16 *)((u8 *)p + 0x292)) &&
                    p->tetherMode == 1 && (*(u8 *)((u8 *)p + 0x284) & 1))
                    enabled = nonzero(p->facing & 1);
                if (enabled) {
                    if (++*(u16 *)p->unk4A6 == 5) {
                        s32 angle;
                        u32 flags;
                        pos.point = p->position;
                        angle = ((p->facing + 5) & 7) << 5;
                        pos.words.x += scale(gUnk_086149C4[(angle + 64) & 255] * 50);
                        pos.words.z += scale(gUnk_086149C4[angle] * 50);
                        if ((u32)groundHeight(&pos.point) < (u32)(p->position.y + 200))
                            break;
                        flags = terrainFlag(&pos.point);
                        if (flags)
                            break;
                        p->coordinateAxis = p->facing;
                        p->actionFourEnabled = flags;
                        ActorSequence_Begin(p, 4, 0);
                    }
                } else
                    *(u16 *)p->unk4A6 = enabled;
            }
            break;
        case 4:
            p->statusFlags |= 0x40000;
            Player_AddControlFlags(p, 8);
            if ((u8)(p->tetherMode - 0x1F) > 5) {
                if (p->contactSpecial)
                    ActorSequence_Begin(p, 0x20, 0);
                else
                    ActorSequence_Begin(p, 0x1F, 0);
            }
            if (p->unk45C == 1)
                Player_RunPendingCallback(p);
            Callback_InvokeActor(p, gUnk_08610C0C[p->tetherMode]);
            Player_ApplyControlPresentation(p);
            break;
        case 2:
            p->statusFlags |= 0x40000;
            Player_AddControlFlags(p, 8);
            if (p->unk45C == 1)
                Player_RunPendingCallback(p);
            Callback_InvokeActor(p, p->triggerAnimationCallback);
            Player_ApplyControlPresentation(p);
            PlayerCue_UpdatePeriodicParticles(p);
            break;
    }
}
