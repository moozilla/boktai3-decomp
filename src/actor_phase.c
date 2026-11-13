#include "global.h"
#include "player.h"

struct PhaseSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6, pad7;
    s8 scaleX, scaleY;
};

struct PhaseCollision {
    u8 pad0[6];
    u16 flags;
};

struct PhaseActor {
    u8 pad0[4];
    struct PhaseSprite *sprite;
    u8 pad8[0xfc];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x1c];
    struct PhaseCollision collision134;
    u8 pad13c[0x4c];
    struct PhaseCollision collision188;
    u8 pad190[0x8c];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0xa];
    u16 flags2c0;
    u8 pad2c2[6];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};

static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}

static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

struct PhaseVector {
    s32 x, y;
};

void ActorShadow_Disable(void *);

void Sound_PlaySong(u32);

void ActorPhase_EmitTrailSprite(struct PhaseActor *, void *, u32);

void Axe_UpdateTrailColliders(struct PhaseActor *);

void Axe_UpdateAttackColliderPositions(struct PhaseActor *);

void Axe_ReleaseAuxiliarySprite(struct PhaseActor *);

void SceneActorControl_ResetRotation(void);

void ActorPhase_UpdateAcceleratingTrail(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        *(u32 *)(data + 0xa38) |= 2;
        data[0xcab] = 0;
        data[0xa35] = 0;
        *(struct PhaseVector *)(data + 0xa24) = *(struct PhaseVector *)(data + 0xa54);
        {
            u32 angle = data[0xca8];
            data[0xa34] = angle;
        }
        data[0xa36] = 4;
        ActorShadow_Disable(data + 0xcac);
        *(u32 *)(data + 0xa38) = 1;
    }
    {
        s32 angle = data[0xa34];
        s32 next = angle - data[0xa36];
        data[0xa34] = next;
        if (angle < next)
            Sound_PlaySong(0x169);
    }
    if ((1 & frame) && data[0xa36] <= 19)
        data[0xa36]++;
    ActorPhase_EmitTrailSprite(s, data + 0xa24, data[0xa34]);
    Axe_UpdateTrailColliders(s);
    Axe_UpdateAttackColliderPositions(s);
    if ((*(u32 *)(data + 0xa38) & 1) && frame > 30) {
        Axe_ReleaseAuxiliarySprite(s);
        {
            void (*next)(void) = SceneActorControl_ResetRotation;
            u32 id = 0x34;
            s->entered = 1;
            s->phaseState = 0;
            s->phaseId = id;
            s->phase = next;
            s->timer = 0;
            clearFlags(&s->flags114, 1);
            s->f104 = 0;
        }
    }
}

#define ActorPhase_SetAnimation Axe_SetAuxiliarySpriteParameters

u8 ActorPhase_UpdateReverseFlickerTimer(u32 *, s32, s32, s32);

void Axe_SetAuxiliarySpriteParameters(struct PhaseActor *, void *, u32);

void ActorPhase_UpdateReverseFlickerExit();

void ActorPhase_UpdateReverseFlicker(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    u32 flags;
    if (consumeEntry(s)) {
        data[0xcab] = 0;
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
    if (ActorPhase_UpdateReverseFlickerTimer(&flags, frame, 10, 30)) {
        void (*next)(void) = ActorPhase_UpdateReverseFlickerExit;
        u32 id = 0x30;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        goto visible;
    } else if (flags & 1) {
    visible:
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
}

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void ActorPhase_UpdateReverseFlickerExit(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    if (consumeEntry(s))
        data[0xcab] = 0;
    if (Sprite_BlinkUntilExpired(data + 0xa38, frame, 10, 30)) {
        Axe_ReleaseAuxiliarySprite(s);
        {
            void (*next)(void) = SceneActorControl_ResetRotation;
            u32 id = 0x34;
            s->entered = 1;
            s->phaseState = 0;
            s->phaseId = id;
            s->phase = next;
            s->timer = 0;
            clearFlags(&s->flags114, 1);
            s->f104 = 0;
        }
    }
}

void ActorPhase_WaitBeforeRelocation();

void ActorPhase_UpdateSpriteCollapse(struct PhaseActor *s)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags |= 2;
        {
            u32 mask = 4;
            *(u32 *)(data + 0xe4c) |= mask;
        }
        Sound_PlaySong(0x16b);
        {
            u32 mask = 0x1800;
            s->flags2c0 = mask | s->flags2c0;
        }
    }
    if (s->sprite->scaleX > 32) {
        s->sprite->scaleX -= 4;
    } else if (s->sprite->scaleX > 8) {
        s->sprite->scaleX -= 2;
        s->sprite->scaleY += 4;
        ActorShadow_Disable((u8 *)s + 0x320);
    } else if (s->sprite->scaleY <= 119) {
        s->sprite->scaleY += 8;
        ActorShadow_Disable((u8 *)s + 0x320);
    } else {
        void (*next)(void);
        u32 id;
        s->sprite->flags = 1;
        next = ActorPhase_WaitBeforeRelocation;
        id = 0x24;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        ActorShadow_Disable((u8 *)s + 0x320);
        {
            u32 mask = 0x40;
            *(u32 *)(data + 0xe4c) |= mask;
        }
    }
}

void ActorPhase_SelectDistantWaypoint(struct PhaseActor *, void *, void *, s32);

void ActorPhase_UpdateSpriteRestore();

void ActorPhase_WaitBeforeRelocation(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags &= ~2;
        {
            struct PhaseCollision *collision = &s->collision188;
            collision->flags |= 4;
        }
        {
            struct PhaseCollision *collision = &s->collision134;
            collision->flags |= 4;
        }
    }
    s->sprite->flags |= 1;
    ActorShadow_Disable((u8 *)s + 0x320);
    if (frame > data[0xe40]) {
        ActorPhase_SelectDistantWaypoint(s, (u8 *)s + 0x5c, (*(u8 **)gPlayers) + 0x30, 0x40000);
        {
            void (*next)(void) = ActorPhase_UpdateSpriteRestore;
            u32 id = 0x25;
            s->entered = 1;
            s->phaseState = 0;
            s->phaseId = id;
            s->phase = next;
            s->timer = 0;
            clearFlags(&s->flags114, 1);
            s->f104 = 0;
        }
    }
}

void ActorShadow_Enable(void *);

static inline void clearCollision(struct PhaseCollision *p, u32 mask)
{
    p->flags &= ~mask;
}

static inline void clearFlags16(u16 *p, u32 mask)
{
    *p &= ~mask;
}

void ActorPhase_UpdateSpriteRestore(struct PhaseActor *s)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        s->sprite->flags = (s->sprite->flags | 2) & ~1;
        s->sprite->field6 = 0;
        clearCollision(&s->collision188, 4);
        clearCollision(&s->collision134, 4);
        Sound_PlaySong(0x16c);
    }
    if (s->sprite->scaleY > 112) {
        s->sprite->scaleY -= 8;
    } else if (s->sprite->scaleY > 64) {
        s->sprite->scaleX += 2;
        s->sprite->scaleY -= 4;
        ActorShadow_Enable((u8 *)s + 0x320);
    } else if (s->sprite->scaleX <= 63) {
        s->sprite->scaleX += 4;
    } else {
        s->sprite->scaleX = 64;
        s->sprite->scaleY = 64;
        clearFlags((u32 *)(data + 0xe4c), 0x44);
        s->phaseState = 1;
        clearFlags16(&s->flags2c0, 0x1800);
    }
}
