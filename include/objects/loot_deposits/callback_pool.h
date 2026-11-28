#ifndef GUARD_CALLBACK_POOL_H
#define GUARD_CALLBACK_POOL_H
#include "engine/engine.h"
#include "scene/scene_animation.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_collider.h"
#include "engine/video/display_sequence.h"
#include "effects/shadows/scaled_shadow_types.h"
#include "player/player.h"
struct CallbackPoolEntry {
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    struct SceneAnimation graphics;
    struct ContactCollider collider;
    struct ScaledShadow shadow;
    struct ContactPosition savedPosition;
    struct ContactPosition motion;     /* 0x120: x sway turn step, y fall speed */
    struct ContactPosition swayRadius; /* 0x128 */
    s32 previousDistanceSquared, distanceSquared, triggerDistanceSquared;
    u32 spawnScript;
    void *healthDisplay;
    u16 damageThreshold;
    u8 spawnIndex, spawnTarget;
    u16 ground;
    s16 health, maxHealth;
    u8 unknown14E[2], unknown150, displayMode;
    u16 callbackIndex;
    s16 slot;
    u16 statusFlags;
    s8 effectIndex;
    u8 unknown159, animationRow, hitTimer;
    u16 frame;
    u8 defeated, shakeTimer;
    u32 script;
    s32 arguments[8];
};
struct CallbackPoolScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
struct CallbackPool {
    struct Task task;
    void *sequenceResource;
    struct CallbackPoolEntry entries[4];
    u32 activeMask;
};
void LootDepositCallbackPool_SpawnHitParticles(struct CallbackPoolEntry *);
void LootDepositCallbackPool_RefreshHealthDisplay(struct CallbackPoolEntry *);
void LootDepositFeedback_SetState(struct CallbackPoolEntry *, s32);
u8 LootDepositPool_TestStatusFlags(struct CallbackPoolEntry *, u32);
void LootDepositPool_AddStatusFlags(struct CallbackPoolEntry *, u16);
void LootDepositPool_RemoveStatusFlags(struct CallbackPoolEntry *, u16);
struct CallbackPool *LootDepositPool_CreateManager(void);
s32 LootDepositPool_InitManager(struct CallbackPool *);
struct CallbackPoolEntry *LootDepositPool_Allocate(struct CallbackPool *);
s32 LootDepositCallbackPool_UpdateEntries(struct CallbackPool *);
s32 LootDepositCallbackPool_DestroyEntries(struct CallbackPool *);
void LootDepositCallbackPool_UpdateShake(struct CallbackPoolEntry *);
void LootDepositCallbackPool_InitializeCollider(struct CallbackPoolEntry *, u32, u32, u32);
void LootDeposit_CallMemberScript(struct CallbackPoolEntry *);
void LootDeposit_EmitRemovalDebris(struct CallbackPoolEntry *);
void LootDeposit_UpdateMemberSway(struct CallbackPoolEntry *);
void LootDepositFeedback_CheckPlayerRange(struct CallbackPoolEntry *);
u8 LootDepositFeedback_IsPlayerInsideOriginRange(struct CallbackPoolEntry *);
void sub_08104748(struct CallbackPoolEntry *);
static inline u8 LootDepositCallbackPool_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
static inline void LootDepositCallbackPool_Subtract(struct ContactPosition *out,
                                         const struct ContactPosition *b, const struct SceneVec *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
#endif
