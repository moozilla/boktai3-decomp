#include "global.h"
#include "actor_animation_controller.h"
#include "render.h"
#include "input.h"
#include "ram.h"

// Provisional name: ActorPalette_BeginFade

void AnimData_SelectPaletteBank(void *, s32);

void ActorPalette_UpdateFade(struct ActorAnimController *);

void ActorPalette_UpdateFadeAndNotify(struct ActorAnimController *);

// Begin an actor palette transition; mode 2 also completes a waiting request.

void ActorPalette_BeginFade(struct ActorAnimController *p)
{
    p->step = 0;
    p->duration = 50;
    AnimData_SelectPaletteBank(p->effect, 344);
    p->savedPalette = p->palette;
    p->palette = p->output;
    /* local adaptation: mode is a bitfield, whose address can't be taken */
    if ((*(u8 *)((u8 *)p + 0xf5a) & 15) == 2)
        p->previous = ActorPalette_UpdateFadeAndNotify;
    else
        p->previous = ActorPalette_UpdateFade;
}

// Provisional name: ActorPalette_UpdateFade

s32 Div(s32, s32);

// Interpolate the actor palette over fifty updates, then restore its asset palette.

void ActorPalette_UpdateFade(struct ActorAnimController *p)
{
    s32 i;
    struct RGB {
        s32 r : 8, g : 8, b : 8;
    } a, b, c;
    for (i = 0; i < 16; i++) {
        a.r = p->base[i] & 31;
        a.g = (p->base[i] >> 5) & 31;
        a.b = (p->base[i] >> 10) & 31;
        b.r = p->alternate[i] & 31;
        b.g = (p->alternate[i] >> 5) & 31;
        b.b = (p->alternate[i] >> 10) & 31;
        c.r = Div((b.r - a.r) * p->step, p->duration) + a.r;
        c.g = a.g + Div((b.g - a.g) * p->step, p->duration);
        c.b = Div((b.b - a.b) * p->step, p->duration) + a.b;
        p->output[i] = (c.b << 10) | (c.g << 5) | c.r;
    }
    if (++p->step > p->duration) {
        p->previous = 0;
        AnimData_SelectPaletteBank(p->effect, 344);
        p->palette = p->savedPalette;
    }
}

// Provisional name: ActorPalette_UpdateFadeAndNotify

void Script_Call(void *, s32);

// Interpolate the actor palette over fifty updates, then restore its asset palette.

void ActorPalette_UpdateFadeAndNotify(struct ActorAnimController *p)
{
    s32 i;
    struct RGB {
        s32 r : 8, g : 8, b : 8;
    } a, b, c;
    for (i = 0; i < 16; i++) {
        a.r = p->base[i] & 31;
        a.g = (p->base[i] >> 5) & 31;
        a.b = (p->base[i] >> 10) & 31;
        b.r = p->alternate[i] & 31;
        b.g = (p->alternate[i] >> 5) & 31;
        b.b = (p->alternate[i] >> 10) & 31;
        c.r = Div((b.r - a.r) * p->step, p->duration) + a.r;
        c.g = a.g + Div((b.g - a.g) * p->step, p->duration);
        c.b = Div((b.b - a.b) * p->step, p->duration) + a.b;
        p->output[i] = (c.b << 10) | (c.g << 5) | c.r;
    }
    if (++p->step > p->duration) {
        p->previous = 0;
        AnimData_SelectPaletteBank(p->effect, 344);
        p->palette = p->savedPalette;
        if (p->completionHandle) {
            void *handle = p->completionHandle;
            p->completionHandle = 0;
            Script_Call(handle, 0);
        }
    }
}

// Provisional name: ActorPalette_BeginReverseFade

void ActorPalette_UpdateReverseFade(struct ActorAnimController *);

// Begin the reverse palette transition and retain the normal asset palette.

void ActorPalette_BeginReverseFade(struct ActorAnimController *p)
{
    const u16 *src;
    u16 *dst;
    s32 i;
    void *effect;
    p->step = 50;
    p->duration = 50;
    effect = p->effect;
    AnimData_SelectPaletteBank(effect, 343);
    p->savedPalette = p->palette;
    AnimData_SelectPaletteBank(effect, 344);
    src = p->palette;
    dst = p->output;
    for (i = 15; i >= 0; i--) {
        *dst++ = *src++;
    }
    p->palette = p->output;
    p->previous = ActorPalette_UpdateReverseFade;
}

// Provisional name: ActorPalette_UpdateReverseFade

static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    u32 result = mask & v;
    return result;
}

// Interpolate the actor palette over fifty updates, then restore its asset palette.

// MATCHING_CONCESSION: register-pinning
void ActorPalette_UpdateReverseFade(struct ActorAnimController *p)
{
    s32 i;
    struct RGB {
        s32 r : 8, g : 8, b : 8;
    } a, b, c;
    for (i = 0; i < 16; i++) {
        a.r = p->base[i] & 31;
        a.g = (p->base[i] >> 5) & 31;
        a.b = (p->base[i] >> 10) & 31;
        b.r = p->alternate[i] & 31;
        b.g = (p->alternate[i] >> 5) & 31;
        b.b = (p->alternate[i] >> 10) & 31;
        c.r = Div((b.r - a.r) * p->step, p->duration) + a.r;
        c.g = a.g + Div((b.g - a.g) * p->step, p->duration);
        c.b = Div((b.b - a.b) * p->step, p->duration) + a.b;
        p->output[i] = (c.b << 10) | (c.g << 5) | c.r;
    }
    if (--p->step < 0) {
        p->previous = 0;
        AnimData_SelectPaletteBank(p->effect, 343);
        p->palette = p->savedPalette;
        p->oldMode = p->mode;
        {
            u32 mode = 2;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0xd0;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            u8 one = 1;
            p->flags |= one;
        }
    }
}

// Provisional name: Actor_ProjectAndClampToScreen

struct Vec {
    s16 x, y, z;
};

static inline s32 project(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

static inline void screen(struct Vec *dst, struct Vec *src)
{
    s32 depth, height, y, z;
    s32 sx = src->x, sz = src->z;
    dst->x = project(sx - sz);
    depth = project(sx + sz);
    height = project(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gCameraOrigin[0] + 120;
    dst->y = y - gCameraOrigin[1] + 80;
    dst->z = z - gCameraOrigin[2];
}

// Project the actor into screen space and clamp it to the visible display.

void Actor_ProjectAndClampToScreen(struct ActorAnimController *p)
{
    struct Vec pos;
    p->obj.flags |= 0x30;
    screen(&pos, (struct Vec *)&p->obj.x);
    if (pos.x > 240)
        pos.x = 240;
    else if (pos.x < 0)
        pos.x = 0;
    if (pos.y > 160)
        pos.y = 160;
    else if (pos.y < 0)
        pos.y = 0;
    p->obj.x = pos.x;
    p->obj.y = pos.y;
    p->obj.z = 0;
}

// Provisional name: Actor_TryInteraction

extern u8 *gGameState;

u8 Math_Atan2(s32, s32);

void Actor_SetInteractionControlFlag(u8 *);

void Actor_BeginInteractionSoundSequence(u8 *);

void Actor_OnInteractionChildRelease(struct ActorAnimController *, struct ActorChildSlot *);

void Actor_OnInteractionChildDelay(struct ActorAnimController *, struct ActorChildSlot *);

// Tentative: accept a nearby interaction when facing, height and input permit it.

// MATCHING_CONCESSION: register-pinning
void Actor_TryInteraction(struct ActorAnimController *p, struct InteractionPartner *q)
{
    s32 dx, dz, xOffset, height, length;
    s32 zOffset;
    u32 radius;
    u32 a;
    u32 b, c;
    u32 sector;
    u8 *owner;
    if (q->group != p->group)
        return;
    if (q->orientation == 0) {
        xOffset = 0;
        zOffset = 128;
    } else {
        xOffset = 128;
        zOffset = 0;
    }
    owner = *(u8 **)((u8 *)p + 0x18);
    dx = *(s16 *)(owner + 0x30) - q->worldX - xOffset;
    dz = *(s16 *)(owner + 0x34) - q->worldZ - zOffset;
    length = dx * dx + dz * dz;
    radius = p->speed;
    if (length >= (s32)(radius * radius))
        return;
    if (q->orientation == 0) {
        a = 0;
        b = 6;
        c = 7;
    } else {
        a = 0;
        b = 1;
        c = 2;
    }
    {
        u32 angle = (u8)(Math_Atan2(dx, dz) - 16);
        s32 first = (s8)a;
        sector = angle >> 5;
        if (first != sector && (s8)b != sector && (s8)c != sector) {
            c = 255;
            b = 255;
            a = 255;
        }
    }
    if ((s8)a != *(s16 *)(gGameState + 0x85e) && (s8)b != *(s16 *)(gGameState + 0x85e) &&
        (s8)c != *(s16 *)(gGameState + 0x85e))
        return;
    height = q->worldY;
    owner = *(u8 **)((u8 *)p + 0x18);
    height -= *(s16 *)(owner + 0x32);
    if (height < 0)
        height = -height;
    if (height > 119)
        return;
    if (!(gPadState[1] & 1))
        return;
    if (gEnginePauseFlags & 4)
        return;
    if (*(u32 *)(owner + 0x1c) == 4)
        return;
    if (q->request)
        Script_Call(q->request, 0);
    p->obj.x = q->x;
    p->obj.y = q->y;
    p->obj.z = q->z;
    p->handle = q->handle;
    p->option = q->option;
    if (q->flags & 16) {
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0x30;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register u32 one asm("r1") = 1;
            register u32 flags asm("r2") = p->flags;
            p->flags = one | flags;
            {
                register void (*fn)(void *, void *) = Actor_OnInteractionChildRelease;
                q->fn = fn;
            }
            one |= q->flags;
            q->flags = one;
        }
        Actor_SetInteractionControlFlag(p);
    } else {
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0x20;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register u32 one asm("r1") = 1;
            register u32 flags asm("r2") = p->flags;
            p->flags = one | flags;
            {
                register void (*fn)(void *, void *) = Actor_OnInteractionChildDelay;
                q->fn = fn;
            }
            one |= q->flags;
            q->flags = one;
        }
        Actor_BeginInteractionSoundSequence(p);
    }
}

// Provisional name: Actor_SelectNearestInteraction

// Select the closest eligible interaction target and animate its selection marker.

// MATCHING_CONCESSION: register-pinning
void Actor_SelectNearestInteraction(struct ActorAnimController *p)
{
    s32 i;
    struct InteractionPartner *q;
    s32 dx, dz, xOffset, zOffset, length;
    register s32 height asm("r0");
    u32 radius;
    u32 a, b, c;
    u32 sector;
    u8 *owner;
    p->group = -1;
    for (i = 0; i < 16; i++) {
        q = (struct InteractionPartner *)((u8 *)p + 0x1c + i * 0xdc);
        q->eligible = 0;
        if (q->flags & 10)
            continue;
        if (q->orientation == 0) {
            xOffset = 0;
            zOffset = 128;
        } else {
            xOffset = 128;
            zOffset = 0;
        }
        owner = *(u8 **)((u8 *)p + 0x18);
        dx = *(s16 *)(owner + 0x30) - q->worldX - xOffset;
        dz = *(s16 *)(owner + 0x34) - q->worldZ - zOffset;
        length = dx * dx + dz * dz;
        radius = p->speed;
        if (length >= (s32)(radius * radius))
            continue;
        if (q->orientation == 0) {
            a = 0;
            b = 6;
            c = 7;
        } else {
            a = 0;
            b = 1;
            c = 2;
        }
        {
            u32 angle = (u8)(Math_Atan2(dx, dz) - 16);
            register s32 first asm("r1") = (s8)a;
            sector = angle >> 5;
            if (first != sector && (s8)b != sector && (s8)c != sector) {
                c = 255;
                b = 255;
                a = 255;
            }
        }
        if ((s8)a != *(s16 *)(gGameState + 0x85e) && (s8)b != *(s16 *)(gGameState + 0x85e) &&
            (s8)c != *(s16 *)(gGameState + 0x85e))
            continue;
        height = q->worldY;
        {
            u8 *parent = *(u8 **)((u8 *)p + 0x18);
            height -= *(s16 *)(parent + 0x32);
        }
        if (height < 0)
            height = -height;
        if (height > 119)
            continue;

        q->eligible = 1;
        q->distance = dx * dx + dz * dz;
        if (p->group == -1)
            p->group = i;
        else {
            s32 offset = p->group * 0xdc;
            u8 *base = (u8 *)p + 0xf0;
            if (q->distance < *(s32 *)(base + offset))
                p->group = i;
        }
    }
    for (i = 0; i < 16; i++) {
        q = (struct InteractionPartner *)((u8 *)p + 0x1c + i * 0xdc);
        if (i == p->group) {
            if (++q->timer > 10) {
                AnimData_SelectPaletteBank((u8 *)q + 0x2c, q->resource);
                q->timer = 10;
            } else
                AnimData_SelectPaletteBank((u8 *)q + 0x2c, q->resource + 1);
        } else {
            u8 old = q->timer--;
            if (old == 0) {
                q->timer = 0;
                AnimData_SelectPaletteBank((u8 *)q + 0x2c, q->resource + 2);
            } else
                AnimData_SelectPaletteBank((u8 *)q + 0x2c, q->resource + 1);
        }
    }
}

void ScriptUtility_SetControlFlag(u8);

void Actor_SetInteractionControlFlag(u8 *p)
{
    ScriptUtility_SetControlFlag(*(u8 *)(p + 0xF6A));
}

void Actor_RestoreSavedAnimationPlayback(u8 *p)
{
    u32 a = *(u8 *)(p + 0xF86);
    u32 b;
    *(u16 *)(p + 0xE2C) = a;
    b = *(u8 *)(p + 0xF87);
    *(u16 *)(p + 0xE32) = b;
}

/* Tentative role: arm the idle animation transition at its resting position. */

extern u8 gSunClock[];

/* Provisional name: Actor_UpdateIdleWait */

void Actor_UpdateIdleWait(struct ActorAnimController *);

static inline s32 absolute(s32 x)
{
    return x < 0 ? -x : x;
}

void Actor_UpdateScaleGrowth(struct ActorAnimController *p)
{
    s32 x = p->obj.scaleX;
    s32 y = p->obj.scaleY;
    x += 2;
    y += 2;
    if (x > 127) {
        y = 127;
        x = 127;
        p->scaleFn = 0;
    }
    p->obj.scaleX = x;
    p->obj.scaleY = y;
}

/* Tentative role: arm the idle animation transition at its resting position. */

void Actor_ArmRestIdleTransition(struct ActorAnimController *p)
{
    if (p->obj.x == 25 && p->obj.z == 0 && absolute(p->obj.y - 102) <= 9) {
        p->timer = 0;
        p->fn = Actor_UpdateIdleWait;
        if (gSunClock[14] == 5 || gSunClock[14] == 0 || gSunClock[14] == 4)
            p->delay = 1800;
        else
            p->delay = 10800;
        if (p->growFlag && p->obj.scaleX != 127 && (p->obj.flags & 16))
            p->scaleFn = Actor_UpdateScaleGrowth;
    } else
        p->fn = 0;
}

/* Provisional name: Actor_UpdateIdleWait */

/* Gap function (gbadisasm left it as data): counts down an idle delay, then
 * saves the current phase and arms Actor_UpdateIdlePhaseRestoration. */

void Actor_UpdateIdlePhaseRestoration(struct ActorAnimController *);

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateIdleWait(struct ActorAnimController *p)
{
    if (*(s32 *)gPadState) {
        p->timer = 0;
        if (gSunClock[14] == 5 || gSunClock[14] == 0 || gSunClock[14] == 4)
            p->delay = 1800;
        else
            p->delay = 10800;
    }
    if (++p->timer > p->delay) {
        if (p->done) {
            p->interrupted = 0;
            {
                register u32 offset asm("r1") = 0xF5A;
                u16 *field = (void *)((u8 *)p + offset);
                p->savedPhase = *field >> 4;
            }
            {
                u32 offset = 0xF5A;
                u16 *field;
                register u32 raw asm("r1");
                register u32 mask asm("r3");
                field = (void *)((u8 *)p + offset);
                raw = *field;
                mask = 15;
                *field = (mask & raw) | 0x50;
                if (p->oldMode == 3) {
                    register u32 bits asm("r0") = mode_bits((u8 *)field, mask);
                    if (bits == 3) {
                        p->savedFrame = p->anim.frame;
                        p->savedTimer = p->anim.timer;
                    }
                }
            }
            {
                u32 one = 1;
                u32 zero;
                u8 *field;
                u32 value;
                field = &p->flags;
                value = *field;
                zero = 0;
                one |= value;
                *field = one;
                p->fn = Actor_UpdateIdlePhaseRestoration;
                p->count = zero;
                p->savedY2 = p->obj.y;
            }
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

void Anim_SetSequence(void *, u32, u32, u32, u32);

void ActorShadow_Disable(void *);

static inline u8 take_v2(struct ActorAnimController *p)
{
    s32 m = 1;
    s32 v = p->flags;
    if (v & m) {
        p->flags = ~m & v;
        p->done = 0;
        return 1;
    }
    return 0;
}

static inline u32 frame_factor_v2(u16 v)
{
    u32 value = v;
    u32 mask = 15;
    return mask & value;
}

void Sound_PlaySong(s32);

void ActorShadow_Enable(void *);

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateIdlePhaseRestoration(struct ActorAnimController *p)
{
    if (*(s32 *)gPadState) {
        p->interrupted = 1;
        p->count = 0;
    }
    if (p->interrupted && p->done) {
        u16 phase;
        p->obj.y = p->savedY2 - 1;
        p->timer = 0;
        p->fn = Actor_UpdateIdleWait;
        phase = p->savedPhase;
        p->oldMode = p->mode;
        {
            u32 mode = 3;
            p->mode = mode;
        }
        {
            register u32 shifted asm("r3") = phase << 4;
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | shifted;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
    }
}

void Sound_PlaySong(s32);

void Actor_BeginInteractionSoundSequence(u8 *p)
{
    Sound_PlaySong(0x11E);
    *(u32 *)(p + 0xF98) = 0x081E44F9;
}

void Actor_UpdateInteractionSoundDelay(struct ActorAnimController *);

void Actor_InitInteractionSoundDelay(u8 *p)
{
    *(u8 *)(p + 0xF88) = 0xc;
    *(void (**)(void))(p + 0xF98) = Actor_UpdateInteractionSoundDelay;
}

/* Finish a delayed controller callback, selecting the time-of-day sound. */

void SunClock_GetBaseHour(void);

void Actor_UpdateInteractionSoundDelay(struct ActorAnimController *p)
{
    if (--p->delayCounter == 0) {
        p->otherFn = 0;
        if ((u8)(gSunClock[14] - 1) <= 2)
            Sound_PlaySong(0x120);
        else
            SunClock_GetBaseHour();
    }
}

u8 *Actor_EnsureController(u32, u32);

void Actor_BeginAnimationPhaseOne(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x10;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseTwo(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x20;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseThree(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x30;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseFour(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x40;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseFive(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x50;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseSix(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x60;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseSeven(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x70;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginAnimationPhaseEight(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x80;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

#define H(o) (*(u16 *)((u8 *)g + (o)))

#define B(o) (*(u8 *)((u8 *)g + (o)))

#define W(o) (*(u32 *)((u8 *)g + (o)))

#define OWNER(o) (*(u16 *)(*(u8 **)((u8 *)g + 0x18) + (o)))

s32 Script_SeekToKeyword(s32);

s32 Script_GetValue(void);

s32 Script_GetKeywordValue(s32, s32);

static inline u32 mode_bits_v2(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    return mask & v;
}

// MATCHING_CONCESSION: register-pinning
void Actor_BeginScriptedAnimationPhaseNine(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0)
        return;
    B(0xf76) = Script_GetKeywordValue(0x70, 0);
    if (W(0xddc) & 1)
        return;
    g->oldMode = g->mode;
    {
        u32 mode = 3;
        g->mode = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0x90;
        if (g->oldMode == 3) {
            register u32 bits asm("r0") = mode_bits_v2((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->savedFrame = g->anim.frame;
                g->savedTimer = g->anim.timer;
            }
        }
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Actor_SetAnimationPhase: Requests and saves the supplied phase with special cases for playing sound and clearing control */

#define Actor_SetAnimationPhase sub_081E4B0C

struct G {
    u8 f0[0xE2C];
    u16 e2c;
    u8 f1[3];
    u16 e32;
    u8 f2[0xF5A - 0xE34];
    u8 xa : 4;
    u8 xb : 4;
    u8 y;
    u16 b;
    u8 f3[0xF64 - 0xF5E];
    u8 c;
    u8 f4[0xF86 - 0xF65];
    u8 d;
    u8 e;
};

void ScriptUtility_ClearControlFlag(void);

void sub_081E4B0C(u32 phase)
{
    struct G *g = (struct G *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct G *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    if (phase == 12) {
        Sound_PlaySong(288);
        return;
    }
    if (phase == 14) {
        ScriptUtility_ClearControlFlag();
        return;
    }
    phase = (u16)phase;
    g->b = g->xa;
    g->xa = three;
    {
        u32 shifted = phase << 4;
        *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | shifted;
    }
    if (g->b == 3 && g->xa == 3) {
        g->d = g->e2c;
        g->e = g->e32;
    }
    {
        u8 one = 1;
        g->c |= one;
    }
    {
        u32 value = *(u16 *)((u8 *)g + 0xf5a) >> 4;
        *(u16 *)((u8 *)g + 0xf7e) = value;
    }
}

/* Actor_BeginAnimationPhaseTen: Ensures the controller exists and requests mode three phase ten with playback preservation */

#define Actor_BeginAnimationPhaseTen sub_081E4BD4

void sub_081E4BD4(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xa0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Actor_BeginAnimationPhaseEleven: Ensures the controller exists and requests mode three phase eleven with playback preservation */

#define Actor_BeginAnimationPhaseEleven sub_081E4C74

void sub_081E4C74(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xb0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Actor_BeginCompletionAnimation: Selects mode 3 phase 13 whose handler plays sequence 18 and invokes the completion script */

#define Actor_BeginCompletionAnimation sub_081E4D14

void sub_081E4D14(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 3;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xd0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

#define OWNER_V(o) (*(u16 *)(*(u8 *volatile *)((u8 *)g + 0x18) + (o)))

// MATCHING_CONCESSION: register-pinning
void Actor_BeginCurveToPosition(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    H(0xf8a) = Script_GetValue();
    if (Script_SeekToKeyword(0x74)) {
        H(0xed0) = Script_GetValue();
        {
            u32 value = Script_GetValue() + 160;
            H(0xed2) = value;
        }
        H(0xed4) = Script_GetValue();
    } else {
        {
            u32 value = OWNER_V(0x30);
            H(0xed0) = value;
        }
        {
            u32 value = OWNER_V(0x32) + 160;
            H(0xed2) = value;
        }
        {
            u32 value = OWNER_V(0x34);
            H(0xed4) = value;
        }
    }
    B(0xf67) = Script_GetKeywordValue(0x64, -1);
    W(0xf50) = Script_GetKeywordValue(0x70, 0);
    g->oldMode = g->mode;
    {
        u32 mode = 2;
        g->mode = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0xa0;
        if (g->oldMode == 3) {
            register u32 bits asm("r0") = mode_bits_v2((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->savedFrame = g->anim.frame;
                g->savedTimer = g->anim.timer;
            }
        }
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

void Actor_BeginCurveToRest(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    H(0xf8a) = Script_GetValue();
    B(0xf77) = Script_GetKeywordValue(0x75, 0);
    W(0xf50) = Script_GetKeywordValue(0x70, 0);
    Actor_ProjectAndClampToScreen(g);
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x80;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

// MATCHING_CONCESSION: register-pinning
void Actor_BeginScriptedCurve(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    W(0xf50) = Script_GetKeywordValue(0x70, 0);
    if (Script_SeekToKeyword(0x74)) {
        H(0xed0) = Script_GetValue();
        H(0xed2) = Script_GetValue();
    } else {
        H(0xed2) = 0;
        H(0xed0) = 0;
    }
    g->oldMode = g->mode;
    {
        u32 mode = 2;
        g->mode = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0x90;
        if (g->oldMode == 3) {
            register u32 bits asm("r0") = mode_bits_v2((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->savedFrame = g->anim.frame;
                g->savedTimer = g->anim.timer;
            }
        }
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

// MATCHING_CONCESSION: register-pinning
void Actor_SetFacingAndIdle(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    H(0xf8a) = Script_GetValue();
    {
        register u32 shifted asm("r0") = Script_GetKeywordValue(0x64, 0) << 24;
        u32 flip = 0;
        u32 mask = 0x07000000;
        u8 direction = (mask & shifted) >> 24;
        B(0xf68) = direction;
        if (direction > 4) {
            direction = 8 - direction;
            flip = 1;
        }
        B(0xf66) = direction;
        B(0xf6b) = flip;
    }
    Anim_SetSequence((u8 *)g + 0xe24, W(0xe74), 0, B(0xf66), B(0xf6b));
    g->oldMode = g->mode;
    {
        u32 mode = 2;
        g->mode = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0;
        if (g->oldMode == 3) {
            register u32 bits asm("r0") = mode_bits_v2((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->savedFrame = g->anim.frame;
                g->savedTimer = g->anim.timer;
            }
        }
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginFadeAnimation(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    *(u16 *)((u8 *)g + 0xf8a) = Script_GetValue();
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xb0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
    *(u32 *)((u8 *)g + 0xf54) = Script_GetKeywordValue(0x70, 0);
}

void Actor_BeginReverseFadeAnimation(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    *(u16 *)((u8 *)g + 0xf8a) = Script_GetValue();
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xC0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
    *(u32 *)((u8 *)g + 0xf54) = Script_GetKeywordValue(0x70, 0);
}

void Actor_BeginHide(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0x70;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

/* Script command: configure the singleton animation controller. */

// MATCHING_CONCESSION: register-pinning
void Actor_BeginPositionedEffect(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    if (Script_SeekToKeyword(0x70)) {
        H(0xdf8) = Script_GetValue();
        H(0xdfa) = Script_GetValue();
        H(0xdfc) = Script_GetValue();
    } else {
        {
            u32 value = OWNER_V(0x30);
            H(0xdf8) = value;
        }
        {
            u32 value = OWNER_V(0x32) + 160;
            H(0xdfa) = value;
        }
        {
            u32 value = OWNER_V(0x34);
            H(0xdfc) = value;
        }
    }
    g->oldMode = g->mode;
    {
        u32 mode = 2;
        g->mode = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0xe0;
        if (g->oldMode == 3) {
            register u32 bits asm("r0") = mode_bits_v2((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->savedFrame = g->anim.frame;
                g->savedTimer = g->anim.timer;
            }
        }
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_BeginRadialBurst(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    u32 three = 2;
    if (g == 0) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xBB34, 0);
        if (g == 0)
            return;
    }
    g->oldMode = g->mode;
    g->mode = three;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | 0xf0;
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

u8 *Actor_EnsureController(u32, u32);

u8 Actor_GetFacingDirection(void)
{
    u8 *p = gScriptedActorController;
    if (p == 0) {
        p = Actor_EnsureController(0xBB34, 0);
        if (p == 0)
            return 0;
    }
    return p[0xF68];
}

void Actor_SetSpritePriorityFromScript(void)
{
    u8 *p = gScriptedActorController;
    if (p == 0) {
        p = Actor_EnsureController(0xBB34, 0);
        if (p == 0)
            return;
    }
    p[0xDE3] = Script_GetKeywordValue(0x70, 0);
}

void Actor_SetPreviousAnimationStateFromScript(void)
{
    u8 *p = gScriptedActorController;
    if (p == 0) {
        p = Actor_EnsureController(0xBB34, 0);
        if (p == 0)
            return;
    }
    p[0xF6C] = Script_GetKeywordValue(0x72, 0);
}

void Actor_OnControllerIdle(void *controller, void *child)
{
}

/* Tentative role: Consume a child reset flag and update its movement. */

static inline u8 child_take(struct ActorChildSlot *c)
{
    s32 mask = 1;
    register s32 v asm("r1") = c->control;
    if (v & mask) {
        c->control = v & ~mask;
        return 1;
    }
    return 0;
}

// MATCHING_CONCESSION: register-pinning
void Actor_OnInteractionChildUpdate(struct ActorAnimController *input, struct ActorChildSlot *child)
{
    struct ActorChildSlot *c = child;
    struct ActorAnimController *p = input;
    if (child_take(c))
        c->flags &= ~1;
    Actor_TryInteraction(p, c);
}

/* Tentative role: Wait through a child-triggered delay, then enter mode 2 / phase 4. */

static inline u8 child_take_v2(struct ActorChildSlot *c)
{
    s32 mask = 1;
    u8 *flags = &c->control;
    register s32 v asm("r1") = *flags;
    if (v & mask) {
        *flags = v & ~mask;
        return 1;
    }
    return 0;
}

// MATCHING_CONCESSION: register-pinning
void Actor_OnInteractionChildDelay(struct ActorAnimController *input, struct ActorChildSlot *child)
{
    struct ActorChildSlot *c = child;
    struct ActorAnimController *p = input;
    if (child_take_v2(c)) {
        p->timer = 10;
        if (gSunClock[14] != 5 && gSunClock[14] != 0 && gSunClock[14] != 4)
            *((u8 *)p + 0xf6f) = 1;
        else
            *((u8 *)p + 0xf6f) = 0;
        {
            u8 *variant = &c->variant;
            p->variant8 = *variant;
        }
    }
    if (--p->timer == -1) {
        p->oldMode = p->mode;
        {
            u32 mode = 2;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 64;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
    }
}

/* Tentative role: Release the owner effect and reset this child callback. */

static inline u8 child_take_v3(struct ActorChildSlot *c)
{
    s32 mask = 1;
    register u8 *flags asm("r2") = &c->control;
    register s32 v asm("r1") = *flags;
    if (v & mask) {
        *flags = v & ~mask;
        return 1;
    }
    return 0;
}

// MATCHING_CONCESSION: register-pinning
void Actor_OnInteractionChildRelease(struct ActorAnimController *input,
                                     struct ActorChildSlot *child)
{
    struct ActorChildSlot *c = child;
    struct ActorAnimController *p = input;
    if (child_take_v3(c)) {
        register void *h asm("r2") = p->handle;
        if (h) {
            p->handle = 0;
            Script_Call(h, 0);
        }
        {
            register void (*fn)(void *, void *) = Actor_OnInteractionChildUpdate;
            c->fn = fn;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = c->control;
            c->control = mask | v;
        }
    }
}

/* Tentative role: Select a child sprite effect and clear its hidden flag. */

// MATCHING_CONCESSION: register-pinning
void Actor_OnEffectChildPaletteInit(struct ActorAnimController *p, struct ActorChildSlot *c)
{
    if (child_take(c)) {
        AnimData_SelectPaletteBank(c->sprite, c->effect + 2);
        c->flags &= ~1;
    }
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

static inline u8 take(struct ActorAnimController *p)
{
    s32 m = 1;
    register s32 v asm("r1") = p->flags;
    if (v & m) {
        p->flags = ~m & v;
        p->done = 0;
        return 1;
    }
    return 0;
}

struct Rotator {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad2[12];
    u16 angle;
    u8 pad20[0x7e];
    s8 delta;
};

static inline void rotate(struct ActorAnimController *p)
{
    struct Rotator *o = (struct Rotator *)&p->obj;
    if (!(p->count++ & 3)) {
        switch ((int)(p->anim.frame & 3)) {
            case 0:
            case 3:
                if (o->flags & 0x10)
                    o->delta = 1;
                else
                    o->delta = -8;
                break;
            case 1:
            case 2:
                if (o->flags & 0x10)
                    o->delta = -1;
                else
                    o->delta = 8;
                break;
        }
        o->angle += o->delta;
    }
}

static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    take(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateAppearanceAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 1;
        Anim_SetSequence(&p->anim, p->arg, 1, 0, p->next);
        p->obj.flags &= ~1;
        p->obj.flags &= ~16;
        p->obj.flags &= ~32;
        p->obj.scaleY = 64;
        p->obj.scaleX = 64;
        (*(u16 *)&p->obj.y) += 160;
        *(u16 *)((u8 *)p + 0xee2) = (*(u16 *)&p->obj.y);
        Sound_PlaySong(286);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        s32 zero;
        ActorShadow_Enable(p->p2);
        p->phase = 0;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->anim.frame;
            p->savedTimer = p->anim.timer;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            zero = 0;
            p->flags = mask | v;
        }
        Anim_SetSequence(&p->anim, p->arg, 0, 4, zero);
        if (p->handle) {
            void *h = p->handle;
            p->handle = (void *)zero;
            Script_Call(h, 0);
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateCompactEntryAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        u16 *first = (u16 *)((u8 *)p + 0xed0);
        s32 zero;
        zero = 0;
        *first = 25;
        *(u16 *)((u8 *)p + 0xed2) = 102;
        *(u16 *)((u8 *)p + 0xed4) = zero;
        p->state = 1;
        Anim_SetSequence(&p->anim, p->arg, 1, 0, p->next);
        p->obj.flags &= ~1;
        p->obj.flags |= 48;
        p->obj.scaleY = 64;
        p->obj.scaleX = 64;
        ActorShadow_Disable(p->p2);
        Sound_PlaySong(286);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        p->oldMode = p->mode;
        {
            u32 mode = 3;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 16;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        {
            register void *h asm("r2") = p->handle;
            if (h) {
                p->handle = 0;
                Script_Call(h, 0);
            }
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateProjectedEntryAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        u16 *first = (u16 *)((u8 *)p + 0xed0);
        s32 savedZero;
        register s32 zero asm("r2");
        savedZero = 0;
        zero = 0;
        *first = 25;
        *(u16 *)((u8 *)p + 0xed2) = 102;
        *(u16 *)((u8 *)p + 0xed4) = zero;
        p->state = 1;
        Anim_SetSequence(&p->anim, p->arg, 1, 0, p->next);
        *((u8 *)p + 0xf77) = savedZero;
        p->obj.flags &= ~1;
        p->obj.scaleY = 64;
        p->obj.scaleX = 64;
        Actor_ProjectAndClampToScreen(p);
        (*(u16 *)&p->obj.y) += 8;
        *(u16 *)((u8 *)p + 0xee2) = (*(u16 *)&p->obj.y);
        ActorShadow_Disable(p->p2);
        Sound_PlaySong(286);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        p->phase = 8;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->anim.frame;
            p->savedTimer = p->anim.timer;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateSilentProjectedEntryAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        u16 *first = (u16 *)((u8 *)p + 0xed0);
        s32 savedZero;
        register s32 zero asm("r2");
        savedZero = 0;
        zero = 0;
        *first = 25;
        *(u16 *)((u8 *)p + 0xed2) = 102;
        *(u16 *)((u8 *)p + 0xed4) = zero;
        p->state = 1;
        Anim_SetSequence(&p->anim, p->arg, 1, 0, p->next);
        *((u8 *)p + 0xf77) = savedZero;
        p->obj.flags &= ~1;
        p->obj.scaleY = 64;
        p->obj.scaleX = 64;
        Actor_ProjectAndClampToScreen(p);
        (*(u16 *)&p->obj.y) += 8;
        *(u16 *)((u8 *)p + 0xee2) = (*(u16 *)&p->obj.y);
        ActorShadow_Disable(p->p2);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        p->phase = 8;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->anim.frame;
            p->savedTimer = p->anim.timer;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateShadowedRestAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        s32 zero;
        p->obj.flags &= ~1;
        p->obj.flags &= ~16;
        p->obj.flags &= ~32;
        p->obj.scaleY = 64;
        p->obj.scaleX = 64;
        (*(u16 *)&p->obj.y) += 160;
        *(u16 *)((u8 *)p + 0xee2) = (*(u16 *)&p->obj.y);
        ActorShadow_Enable(p->p2);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
        p->phase = 0;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->anim.frame;
            p->savedTimer = p->anim.timer;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            zero = 0;
            p->flags = mask | v;
        }
        Anim_SetSequence(&p->anim, p->arg, 0, 4, zero);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    {
        register void *h asm("r2") = p->handle;
        if (h) {
            p->handle = 0;
            Script_Call(h, 0);
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateLargeRestAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        s32 zero;
        p->obj.flags &= ~1;
        p->obj.flags |= 48;
        p->obj.scaleY = 127;
        p->obj.scaleX = 127;
        ActorShadow_Disable(p->p2);
        Actor_SetInteractionControlFlag(p);
        AnimData_SelectPaletteBank(p->effect, 343);
        p->phase = 0;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->anim.frame;
            p->savedTimer = p->anim.timer;
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            zero = 0;
            p->flags = mask | v;
        }
        Anim_SetSequence(&p->anim, p->arg, 0, 4, zero);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    {
        register void *h asm("r2") = p->handle;
        if (h) {
            p->handle = 0;
            Script_Call(h, 0);
        }
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_HideAndResetChildren(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    u16 n;
    u8 done;
    if (take(p)) {
        ScriptUtility_ClearControlFlag();
        ActorShadow_Disable(p->p2);
        p->obj.flags |= 1;
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 32;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        {
            u32 active = 4;
            register void (*fn)(void *, void *) asm("r4") = Actor_OnInteractionChildUpdate;
            u32 reset = 1;
            struct ActorChildCallbackView *child =
                (struct ActorChildCallbackView *)((u8 *)p + 0xc8);
            s32 i = 15;
            do {
                u32 v = child->flags;
                if (active & v) {
                    register u32 flags asm("r0");
                    child->fn = fn;
                    flags = reset | v;
                    child->flags = flags;
                }
                child++;
            } while (--i >= 0);
        }
    }
}

// Provisional name: Actor_MoveAlongCurveToRest

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

extern const s16 gUnk_086149C4[];

static inline void set_direction(s16 angle, u8 *directionOut, u8 *flipOut)
{
    register s32 direction asm("r2");
    while (angle < 0)
        angle += 256;
    while (angle > 255)
        angle -= 256;
    {
        register s32 shifted asm("r0") = angle - 16;
        direction = (shifted >> 5) & 7;
    }
    {
        s32 choice = direction;
        if (choice == 4)
            *directionOut = 0;
        else if (choice == 0)
            *directionOut = 4;
        else if (choice < 4) {
            *directionOut = 4 - direction;
            *flipOut = 1;
        } else {
            register s32 result asm("r0") = direction - 4;
            *directionOut = result;
            *flipOut = 0;
        }
    }
}

// MATCHING_CONCESSION: register-pinning
void Actor_MoveAlongCurveToRest(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->timer = 0;
        p->startX = p->obj.x;
        p->startY = p->obj.y;
        p->startZ = p->obj.z;
        p->state = 0;
        p->bend = (p->variant & 1) ? 1 : -1;
        ActorShadow_Disable(p->p2);
        if (*((u8 *)p + 0xf6f) && p->obj.scaleX != 127 && (p->obj.flags & 16))
            p->scaleFn = Actor_UpdateScaleGrowth;
        if (!*((u8 *)p + 0xf77)) {
            p->targetX = 25;
            p->targetY = 102;
            p->targetZ = 0;
        } else if (gSunClock[14] != 5 && gSunClock[14] != 0 && gSunClock[14] != 4) {
            p->targetX = 25;
            p->targetY = 92;
            p->targetZ = 0;
        } else {
            p->targetX = 25;
            p->targetY = 75;
            p->targetZ = 0;
        }
    }
    p->heading = Math_Atan2(p->targetX - p->obj.x, p->targetY - p->obj.y);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (p->timer++ > 31) {
        p->obj.x = p->targetX;
        p->obj.y = p->targetY;
        p->oldMode = p->mode;
        {
            u32 mode = 3;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 16;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        if (p->handle) {
            void *handle = p->handle;
            p->handle = 0;
            Script_Call(handle, 0);
        }
    } else {
        set_direction(p->heading - 16, &p->direction, &p->flip);
        Anim_SetSequence(&p->anim, p->arg, p->state, p->direction, p->flip);
        {
            s32 delta = p->targetX - p->startX;
            p->obj.x = p->startX + Div(delta * p->timer, 32);
        }
        {
            s32 delta = p->targetY - p->startY;
            s32 y = (u16)p->startY + Div(delta * p->timer, 32);
            s32 bend = p->bend;
            p->obj.y = y + bend * (gUnk_086149C4[(p->timer * 4) & 255] >> 7);
        }
    }
}

// Provisional name: Actor_MoveAlongScriptedCurve

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_MoveAlongScriptedCurve(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->timer = 0;
        p->startX = p->obj.x;
        p->startY = p->obj.y;
        p->startZ = p->obj.z;
        p->deltaX = p->targetX - p->obj.x;
        p->deltaY = p->targetY - p->obj.y;
        p->state = 0;
        p->bend = (p->variant & 1) ? 1 : -1;
        ActorShadow_Disable(p->p2);
        if (p->obj.scaleX != 127 && (p->obj.flags & 16))
            p->scaleFn = Actor_UpdateScaleGrowth;
    }
    p->heading = Math_Atan2(p->targetX - p->obj.x, p->targetY - p->obj.y);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (p->timer++ > 31) {
        p->obj.x = p->targetX;
        p->obj.y = p->targetY;
        p->oldMode = p->mode;
        {
            u32 mode = 3;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 16;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        if (p->handle) {
            void *handle = p->handle;
            p->handle = 0;
            Script_Call(handle, 0);
        }
    } else {
        set_direction(p->heading - 16, &p->direction, &p->flip);
        Anim_SetSequence(&p->anim, p->arg, p->state, p->direction, p->flip);
        p->obj.x = p->startX + Div(p->deltaX * p->timer, 32);
        {
            s32 y = Div(p->deltaY * p->timer, 32);
            s32 sine = gUnk_086149C4[(p->timer * 4) & 255];
            s32 bend = p->bend;
            register s32 product asm("r0") = bend * 30;
            product *= sine;
            {
                register s32 curve asm("r2") = product >= 0 ? product >> 12 : -((-product) >> 12);
                {
                    s32 result = (u16)p->startY + y + curve;
                    p->obj.y = result;
                }
            }
        }
    }
}

extern const s16 gUnk_086149C4[];

/* Animation-tick step shared by the two places that advance the sequence (081E728C). */
static inline u8 sub_081E728C_Advance(struct ActorAnimController *p, u32 direction, u32 flip)
{
    struct ActorAnim *anim = &p->anim;
    struct ActorSprite *obj = &p->obj;
    const u16 *entry;
    u8 wrapped = 0;

    Anim_SetSequence(anim, p->arg, p->state, direction, flip);
    entry = anim->data + anim->frame;
    obj->frame = *entry >> 6;
    if ((anim->flags & 1) != ((*entry >> 4) & 1))
        obj->flags |= 4;
    else
        obj->flags &= ~4;
    if ((anim->flags & 2) != ((*entry >> 4) & 2))
        obj->flags |= 8;
    else
        obj->flags &= ~8;

    anim->timer++;
    if (anim->timer >= anim->rate) {
        anim->timer = 0;
        if (anim->flags & 4) {
            if (anim->frame != 0) {
                anim->frame--;
            } else {
                if (anim->flags & 8)
                    anim->timer = anim->rate;
                else
                    anim->frame = anim->count - 1;
                wrapped = 1;
            }
        } else {
            anim->frame++;
            if (anim->frame >= anim->count) {
                if (anim->flags & 8) {
                    anim->timer = anim->rate;
                    anim->frame = anim->count - 1;
                } else {
                    anim->frame = 0;
                }
                wrapped = 1;
            }
        }
        entry = anim->data + anim->frame;
        anim->factor = *entry & 15;
        anim->rate = (anim->scale * anim->factor) >> 6;
        if (anim->rate == 0)
            anim->rate = 1;
    }
    return wrapped;
}

/* Settle path shared by the near-target branch: copies the target, optionally advances, finishes. */
static inline void sub_081E728C_Finish(struct ActorAnimController *p)
{
    *(u16 *)((u8 *)p + 0xF5A) &= 0xF;
    if (p->oldMode == 3 && p->mode == 3) {
        p->savedFrame = p->anim.frame;
        p->savedTimer = p->anim.timer;
    }
    p->flags |= 1;
    if (p->handle) {
        void *handle = p->handle;
        p->handle = 0;
        Script_Call(handle, 0);
    }
}

#ifdef NONMATCHING
// fuzzy: 36.6%
// scope shape 331 (differences once low registers are set aside)
// confidence=low: steps an actor's animation toward its target: heading, sprite playback, and motion.
void sub_081E728C(struct ActorAnimController *p)
{
    s32 dx, dz, adx, adz;
    s16 targetY;
    u8 snap;

    dx = p->targetX - p->obj.x;
    dz = p->targetZ - p->obj.z;
    p->heading = Math_Atan2(dx, dz);

    if (take_v2(p)) {
        s32 idx;
        s16 v;

        p->state = 0;
        v = (s16)p->heading;
        while (v < 0)
            v += 256;
        while (v > 255)
            v -= 256;
        idx = ((v - 16) >> 5) & 7;
        if (idx == 4) {
            p->direction = 0;
        } else if (idx == 0) {
            p->direction = 4;
        } else if (idx <= 3) {
            p->direction = 4 - idx;
            p->flip = 1;
        } else {
            p->direction = idx - 4;
            p->flip = 0;
        }
        p->facing = (idx - 4) & 7;
        p->heightSettled = 0;
        ActorShadow_Enable(p->p2);
    }

    {
        u16 old = p->count;
        p->count = old + 1;
        if ((old & 3) == 0) {
            s32 sel = p->anim.frame & 3;
            if (sel == 0 || sel == 3) {
                p->delta = (p->obj.flags & 0x10) ? 1 : -8;
            } else {
                p->delta = (p->obj.flags & 0x10) ? -1 : 8;
            }
            p->obj.y += p->delta;
        }
    }

    p->done = sub_081E728C_Advance(p, p->direction, p->flip);

    adx = dx < 0 ? -dx : dx;
    adz = dz < 0 ? -dz : dz;
    if (adx + adz <= 9 && p->heightSettled != 0) {
        p->obj.x = p->targetX;
        p->obj.z = p->targetZ;
        if (p->targetFacing >= 0) {
            u32 m = p->targetFacing & 7;
            u32 flip = 0;
            p->facing = p->targetFacing;
            if (m > 4) {
                m = (u8)(8 - m);
                flip = 1;
            }
            p->done = sub_081E728C_Advance(p, m, flip);
        }
        sub_081E728C_Finish(p);
        return;
    }

    if (p->heightSettled == 0) {
        targetY = p->targetY;
        snap = 0;
        if (targetY > p->obj.y) {
            p->obj.y += 7;
            p->savedY += 7;
            if (targetY < p->obj.y)
                snap = 1;
        } else if (targetY < p->obj.y) {
            p->obj.y -= 7;
            p->savedY -= 7;
            if (targetY > p->obj.y)
                snap = 1;
        }
        if (snap) {
            p->obj.y = targetY;
            p->savedY = targetY;
            p->heightSettled = 1;
        }
    }

    if (dx == 0 && dz == 0)
        return;
    {
        const s16 *sine = gUnk_086149C4;
        s16 cosv = sine[((s16)p->heading + 0x40) & 0xFF];
        s16 sinv = sine[p->heading & 0xFF];
        p->deltaX = Div(cosv, 500);
        p->deltaZ = Div(sinv, 500);
        p->obj.x += p->deltaX;
        p->obj.z += p->deltaZ;
        p->savedX = p->obj.x;
        p->savedZ = p->obj.z;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081E728C);
#endif

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateFadeAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 11;
        Anim_SetSequence(&p->anim, p->arg, 11, 0, 0);
        ActorShadow_Enable(p->p2);
        p->previous = ActorPalette_BeginFade;
    }
    if (p->previous)
        p->previous(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done)
        p->anim.frame = 1;
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateReverseFadeAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 19;
        Anim_SetSequence(&p->anim, p->arg, 19, 0, 0);
        ActorShadow_Enable(p->p2);
        p->previous = ActorPalette_BeginReverseFade;
    }
    if (p->previous)
        p->previous(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateNotifyingAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 18;
        Anim_SetSequence(&p->anim, p->arg, 18, 0, 0);
        ActorShadow_Enable(p->p2);
    }
    if (p->previous)
        p->previous(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        register void *h asm("r2") = p->completionHandle;
        if (h) {
            p->completionHandle = 0;
            Script_Call(h, 0);
        }
        if (p->done)
            p->anim.frame = 1;
    }
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_InitPaletteEffectPose(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 12;
        Anim_SetSequence(&p->anim, p->arg, 12, 0, 0);
        AnimData_SelectPaletteBank((u8 *)p + 0xe08, 346);
        o = &p->obj;
        a = &p->anim;
        entry = a->data + a->frame;
        o->frame = *entry >> 6;
        {
            u32 input = a->flags;
            register u32 mask asm("r3") = 1;
            register u32 left asm("r1") = (u8)(input & mask);
            if (left != (((*entry & 0x30) >> 4) & mask))
                o->flags |= 4;
            else
                o->flags &= ~4;
        }
        {
            register u32 input asm("r0") = a->flags;
            u32 mask = 2;
            u32 left = (u8)(mask & input);
            if (left != (((*entry & 0x30) >> 4) & mask))
                o->flags |= 8;
            else
                o->flags &= ~8;
        }
        n = ++a->timer;
        if (n >= a->rate) {
            a->timer = 0;
            if (a->flags & 4) {
                if (a->frame == 0) {
                    if (a->flags & 8) {
                        a->timer = a->rate;
                    } else {
                        a->frame = a->count - 1;
                    }
                    done = 1;
                } else {
                    a->frame--;
                    done = 0;
                }
            } else {
                if (++a->frame >= a->count) {
                    if ((u8)(a->flags & 8)) {
                        a->timer = a->rate;
                        a->frame = a->count - 1;
                        done = 1;
                    } else {
                        a->frame = 0;
                        done = 1;
                    }
                } else
                    done = 0;
            }
            entry = a->data + a->frame;
            a->factor = frame_factor(*entry);
            a->rate = (a->factor * a->scale) >> 6;
            if (!a->rate)
                a->rate = 1;
        } else
            done = 0;
        *((u8 *)p + 0xde3) = 3;
        p->obj.flags &= ~1;
    }
}

/* Tentative role: Blink, emit a radial particle burst, and reset active child effects. */

s32 Mod(s32, s32);

void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);

void sub_080148CC(s32, s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateBlinkBurstDisappearance(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 13;
        Anim_SetSequence(&p->anim, p->arg, 13, 0, 0);
        AnimData_SelectPaletteBank((u8 *)p + 0xe08, 346);
        o = &p->obj;
        a = &p->anim;
        entry = a->data + a->frame;
        o->frame = *entry >> 6;
        {
            u32 input = a->flags;
            register u32 mask asm("r3") = 1;
            register u32 left asm("r1") = (u8)(input & mask);
            if (left != (((*entry & 0x30) >> 4) & mask))
                o->flags |= 4;
            else
                o->flags &= ~4;
        }
        {
            register u32 input asm("r0") = a->flags;
            u32 mask = 2;
            u32 left = (u8)(mask & input);
            if (left != (((*entry & 0x30) >> 4) & mask))
                o->flags |= 8;
            else
                o->flags &= ~8;
        }
        n = ++a->timer;
        if (n >= a->rate) {
            a->timer = 0;
            if (a->flags & 4) {
                if (a->frame == 0) {
                    if (a->flags & 8) {
                        a->timer = a->rate;
                    } else {
                        a->frame = a->count - 1;
                    }
                    done = 1;
                } else {
                    a->frame--;
                    done = 0;
                }
            } else {
                if (++a->frame >= a->count) {
                    if ((u8)(a->flags & 8)) {
                        a->timer = a->rate;
                        a->frame = a->count - 1;
                        done = 1;
                    } else {
                        a->frame = 0;
                        done = 1;
                    }
                } else
                    done = 0;
            }
            entry = a->data + a->frame;
            a->factor = frame_factor(*entry);
            a->rate = (a->factor * a->scale) >> 6;
            if (!a->rate)
                a->rate = 1;
        } else
            done = 0;
        *(u16 *)((u8 *)p + 0xf80) = 0;
        *(s16 *)((u8 *)p + 0xf82) = 15;
    }
    if (Mod(++*(u16 *)((u8 *)p + 0xf80), *(s16 *)((u8 *)p + 0xf82)) == 0) {
        *(u16 *)((u8 *)p + 0xf80) = 0;
        if (p->obj.flags & 1)
            p->obj.flags &= ~1;
        else
            p->obj.flags |= 1;
        if (--*(u16 *)((u8 *)p + 0xf82) == 0) {
            s32 duration = 10;
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            AnimatedParticles_EmitBurst((u8 *)p + 0xdf8, 1, 512, 48, 8, 8, 8, 4);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 32, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 64, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 96, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 128, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 160, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 192, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 224, 60, duration, 3, duration, 5, 60, duration);
            sub_080148CC(1, 3, (u8 *)p + 0xdf8, 256, 60, duration, 3, duration, 5, 60, duration);
            ActorShadow_Disable(p->p2);
            p->obj.flags |= 1;
            p->oldMode = p->mode;
            {
                u32 mode = 1;
                p->mode = mode;
            }
            {
                register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
                register u32 mask asm("r2") = 15;
                *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 32;
                if (p->oldMode == 3) {
                    register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                    if (bits == 3) {
                        p->savedFrame = p->anim.frame;
                        p->savedTimer = p->anim.timer;
                    }
                }
            }
            {
                register s32 mask asm("r0") = 1;
                register s32 v asm("r1") = p->flags;
                p->flags = mask | v;
            }
            {
                u32 active = 4;
                register void (*fn)(void *, void *) = Actor_OnInteractionChildUpdate;
                u32 reset = 1;
                struct ActorChildCallbackView *child =
                    (struct ActorChildCallbackView
                         *)((u8 *)p +
                            0xc8); /* MATCHING_CONCESSION: retain the original unused zero-register initialization. */
                register s32 reserved asm("r7") = 0;
                s32 i;
                for (i = 15; i >= 0; i--) {
                    register u32 v asm("r2") = child->flags;
                    if (active & v) {
                        u32 flags;
                        child->fn = fn;
                        flags = reset | v;
                        child->flags = flags;
                    }
                    child++;
                }
            }
        }
    }
}

/* Actor_OnAnimationNoOp: Empty function in the animation state block; tentative no-op animation callback */

#define Actor_OnAnimationNoOp sub_081E8370

void sub_081E8370(void)
{
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation2(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 2;
        Anim_SetSequence(&p->anim, p->arg, 2, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation3(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 3;
        Anim_SetSequence(&p->anim, p->arg, 3, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation4(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 4;
        Anim_SetSequence(&p->anim, p->arg, 4, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation5(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 5;
        Anim_SetSequence(&p->anim, p->arg, 5, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

static inline void rotate_v2(struct ActorAnimController *p)
{
    switch (p->anim.frame & 7) {
        case 0:
        case 1:
        case 6:
        case 7:
            p->delta = 1;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            p->delta = -1;
            break;
    }
    if (Mod(p->anim.timer, 5) == 0)
        (*(u16 *)&p->obj.y) += p->delta;
}

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation6(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 6;
        Anim_SetSequence(&p->anim, p->arg, 6, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
    }
    if (p->fn)
        p->fn(p);
    rotate_v2(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation7(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 7;
        Anim_SetSequence(&p->anim, p->arg, 7, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation8(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 8;
        Anim_SetSequence(&p->anim, p->arg, 8, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingAnimation9(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 9;
        Anim_SetSequence(&p->anim, p->arg, 9, 0, p->last);
        Actor_RestoreSavedAnimationPlayback(p);
        ActorShadow_Disable(p->p2);
        Actor_ArmRestIdleTransition(p);
    }
    if (p->fn)
        p->fn(p);
    rotate(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateFinishingAnimation(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 10;
        Anim_SetSequence(&p->anim, p->arg, 10, 0, p->last);
        ActorShadow_Disable(p->p2);
        Sound_PlaySong(287);
    }
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done) {
        if (*((u8 *)p + 0xf76))
            ScriptUtility_ClearControlFlag();
        p->obj.flags |= 1;
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 32;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        {
            u32 active = 4;
            register void (*fn)(void *, void *) = Actor_OnInteractionChildUpdate;
            u32 reset = 1;
            struct ActorChildCallbackView *child =
                (struct ActorChildCallbackView
                     *)((u8 *)p +
                        0xc8); /* MATCHING_CONCESSION: retain the original unused zero-register initialization. */
            register s32 reserved asm("r7") = 0;
            s32 i;
            for (i = 15; i >= 0; i--) {
                register u32 v asm("r2") = child->flags;
                if (active & v) {
                    u32 flags;
                    child->fn = fn;
                    flags = reset | v;
                    child->flags = flags;
                }
                child++;
            }
        }
    }
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingGrowth(struct ActorAnimController *p)
{
    s32 x, y;
    if (take(p))
        ActorShadow_Disable(p->p2);
    rotate(p);
    x = *(s8 *)((u8 *)&p->obj + 8);
    y = *(s8 *)((u8 *)&p->obj + 9);
    x++;
    y++;
    if (x > 127) {
        y = 127;
        x = 127;
    }
    *((u8 *)&p->obj + 8) = x;
    *((u8 *)&p->obj + 9) = y;
}

/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void Actor_UpdateRotatingShrink(struct ActorAnimController *p)
{
    s32 x, y;
    if (take(p))
        ActorShadow_Disable(p->p2);
    rotate(p);
    x = *(s8 *)((u8 *)&p->obj + 8);
    y = *(s8 *)((u8 *)&p->obj + 9);
    x--;
    y--;
    if (x < 64) {
        y = 64;
        x = 64;
    }
    *((u8 *)&p->obj + 8) = x;
    *((u8 *)&p->obj + 9) = y;
}

/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */

// MATCHING_CONCESSION: register-pinning
void sub_081E99EC(struct ActorAnimController *p)
{
    struct ActorSprite *o;
    struct ActorAnim *a;
    const u16 *entry;
    u32 flag;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->state = 11;
        Anim_SetSequence(&p->anim, p->arg, 11, 0, p->last);
        ActorShadow_Disable(p->p2);
        p->previous = ActorPalette_BeginFade;
    }
    if (p->previous)
        p->previous(p);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    p->done = done;
    if (done)
        p->anim.frame = 1;
}

/* Actor_UpdateInactiveAnimation: Empty handler provisionally interpreted as an inactive animation callback */

#define Actor_UpdateInactiveAnimation sub_081E9BA0

void sub_081E9BA0(void)
{
}

struct ActorRegionChild {
    u32 flags;
    u8 unk_04[0xa8];
    void (*callback)(void *, void *);
    u8 index;
    u8 control;
    u8 unk_B2[6];
    s32 regionCount;
    u32 regions[4];
    u8 unk_CC[3];
    u8 timer;
    u8 unk_D0[12];
};

struct ActorRegionController {
    u8 unk_00[0x1c];
    struct ActorRegionChild children[16];
};

struct ActorRegionPlayer {
    u8 unk_00[0x5a0];
    u32 region;
};

static inline u8 ActorChild_IsCurrentRegion(u32 current, u32 region)
{
    if (current == region)
        return TRUE;
    return FALSE;
}

/* Update child visibility for the current region, then tick active visible children. */
void Actor_UpdateRegionChildren(struct ActorRegionController *p)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        struct ActorRegionChild *children = p->children;
        s32 visible = 0;
        if (visible < p->children[i].regionCount) {
            u32 current = ((struct ActorRegionPlayer *)gGameState)->region;
            s32 j;
            for (j = 0; j < p->children[i].regionCount; j++) {
                if (ActorChild_IsCurrentRegion(current, p->children[i].regions[j]))
                    visible = 1;
            }
        }
        if (visible)
            children[i].flags &= ~1;
        else
            children[i].flags |= 1;
    }
    for (i = 0; i < 16; i++) {
        if (p->children[i].control & 4) {
            if (!(p->children[i].flags & 1)) {
                if (p->children[i].timer)
                    p->children[i].timer--;
                p->children[i].callback(p, &p->children[i]);
            }
        }
    }
}

/* Tentative role: Copy the saved position and dispatch the animation phase. */

extern void (*const gUnk_08613CF4[])(void *);

void Actor_UpdateMotionPhase(struct ActorAnimController *p)
{
    p->dx = p->savedX;
    p->dy = p->savedY;
    p->dz = p->savedZ;
    gUnk_08613CF4[p->phase](p);
}

extern const u32 gUnk_08613D34[];

void Callback_InvokeActor(u8 *, u32);

void Actor_DispatchAnimationPhase(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08613D34[*(u16 *)(p + 0xF5A) >> 4]);
}

/* Tentative role: Prepare the actor and dispatch its mode. */

extern void (*const gUnk_08613D70[])(void *);

void Actor_UpdateInteractionMode(struct ActorAnimController *p)
{
    Actor_SelectNearestInteraction(p);
    gUnk_08613D70[p->mode](p);
}

/* Tentative role: Check referenced resources and request actor removal for expired entries. */

struct Entry {
    u32 unknown;
    u16 *value;
};

s32 MessageQueue_GetMessagesById(u16, struct Entry **);

void Task_Kill(void *);

void Actor_CheckResourceLifetime(struct ActorAnimController *p)
{
    struct Entry *e;
    s32 n = MessageQueue_GetMessagesById(*(u16 *)((u8 *)p + 0xf8a), &e);
    e += n - 1;
    while (--n >= 0) {
        if (!*e->value)
            Task_Kill(p);
        e--;
    }
}

/* Tentative role: Dispatch the actor mode and auxiliary callbacks, then advance its tick counter. */

s32 Actor_UpdateController(struct ActorAnimController *p)
{
    Actor_CheckResourceLifetime(p);
    if (!p->mode)
        return 1;
    Actor_UpdateInteractionMode(p);
    if (p->scaleFn)
        p->scaleFn(p);
    if (p->otherFn)
        p->otherFn(p);
    p->variant++;
}

/* Tentative role: Release actor and child sprites, detach its resource, and clear the singleton. */

void Sprite_Remove(void *);

void Shadow_Destroy(void *);

s32 SceneBody_GetById(u16);

void SceneBody_Unregister(void *);

s32 Actor_DestroyController(struct ActorAnimController *p)
{
    s32 i;
    u8 *c;
    Sprite_Remove(&p->obj);
    Shadow_Destroy(p->p2);
    c = (u8 *)p + 0x1c;
    for (i = 15; i >= 0; i--) {
        Sprite_Remove(c);
        c += 0xdc;
    }
    if (SceneBody_GetById(*(u16 *)((u8 *)p + 0xf8a)))
        SceneBody_Unregister((u8 *)p + 0xe80);
    gScriptedActorController = 0;
}

struct InteractionChildHandler {
    void (*fn)(struct ActorAnimController *, struct ActorChildSlot *);
    u8 group, control, orientation, option;
    u8 timer;
    u8 unk_B5[3];
    s32 regionCount;
    s32 regions[4];
    u8 unk_CC[2];
    u8 variant;
    u8 unk_CF;
    u16 effect;
    u8 unk_D2[0xa];
};

struct ScriptInteractionChild {
    struct ActorSprite sprite;
    u8 unknown24[0x48 - 0x24];
    struct Vec world;
    u8 unknown4E[0xA4 - 0x4E];
    s32 completionScript, startScript;
    struct InteractionChildHandler handler;
};

void Terrain_GetBoxMinPosition(u16, void *);
u8 *Script_GetPc(void);
s32 Region_FindContaining(void *);

/* Save the previous mode before the scripted interaction starts. */
static inline void interaction_state(struct ActorAnimController *g, u32 mode, u32 phase)
{
    g->oldMode = g->mode;
    g->mode = mode;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 15) | (phase << 4);
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 flag = 1;
        g->flags |= flag;
    }
}

/* Tentative name: Actor_ConfigureInteractionChildFromScript.
 * Configure the first available interaction marker from the current script. */
#ifdef NONMATCHING
// fuzzy: 70.2%
void Actor_ConfigureInteractionChildFromScript(void)
{
    struct Vec position;
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    struct ScriptInteractionChild *children;
    s32 i, j;
    u32 interactionMode = 1;
    if (!g) {
        g = (struct ActorAnimController *)Actor_EnsureController(0xbb34, 0);
        if (!g)
            return;
    }
    children = (void *)g->children;
    for (i = 0; i < 16; i++) {
        struct InteractionChildHandler *handler = &children[i].handler;
        struct ActorSprite *sprite = &children[i].sprite;
        struct Vec *world = &children[i].world;
        if (handler->control & 2) {
            handler->control = 4;
            switch (Script_GetKeywordValue('t', 0)) {
                case 0:
                    handler->effect = 104;
                    interaction_state(g, interactionMode, 1);
                    handler->fn = Actor_OnInteractionChildUpdate;
                    handler->control |= 1;
                    break;
                case 1:
                    handler->control |= 8;
                    handler->effect = 104;
                    interaction_state(g, interactionMode, 4);
                    handler->fn = Actor_OnEffectChildPaletteInit;
                    handler->control |= 1;
                    break;
                case 2:
                    handler->control |= 16;
                    handler->effect = 107;
                    sprite->frame = 1;
                    interaction_state(g, interactionMode, 1);
                    handler->fn = Actor_OnInteractionChildUpdate;
                    handler->control |= 1;
                    break;
            }
            handler->orientation = Script_GetKeywordValue('r', 0);
            handler->option = Script_GetKeywordValue('A', 0);
            if (Script_SeekToKeyword('R')) {
                Terrain_GetBoxMinPosition(Script_GetValue(), &position);
            } else {
                if (!Script_SeekToKeyword('p'))
                    return;
                position.x = Script_GetValue();
                position.y = Script_GetValue() - 64;
                position.z = Script_GetValue();
            }
            if (handler->orientation) {
                world->x = position.x;
                sprite->x = position.x;
                world->y = position.y;
                sprite->y = position.y;
                world->z = position.z;
                sprite->z = position.z - 128;
            } else {
                world->x = position.x;
                sprite->x = position.x - 128;
                world->y = position.y;
                sprite->y = position.y;
                world->z = position.z;
                sprite->z = position.z;
            }
            sprite->x += 60;
            sprite->z += 60;
            sprite->y += 84;
            sprite->priority = 2;
            if (Script_SeekToKeyword('m')) {
                handler->regionCount = 0;
                while (Script_GetPc()) {
                    handler->regions[children[i].handler.regionCount] = Script_GetValue();
                    handler->regionCount++;
                }
            } else {
                handler->regionCount = 1;
                handler->regions[0] = Region_FindContaining(&position);
            }
            if (Script_SeekToKeyword('C'))
                children[i].completionScript = Script_GetValue();
            else
                children[i].completionScript = 0;
            if (Script_SeekToKeyword('a'))
                children[i].startScript = Script_GetValue();
            else
                children[i].startScript = 0;
            children[i].handler.variant = Script_GetKeywordValue('u', 0);
            if (!handler->orientation) {
                *(u16 *)g->unkF62 = -64;
            } else {
                children[i].sprite.flags |= 4;
                *(u16 *)g->unkF62 = 64;
            }
            j = handler->regionCount;
            if (j > 0) {
                do {
                    j--;
                } while (j != 0);
            }
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Actor_ConfigureInteractionChildFromScript);
#endif
// Provisional name: Actor_ConfigureFromScript

static inline void state(struct ActorAnimController *g, u32 mode, u32 phase)
{
    g->oldMode = g->mode;
    g->mode = mode;
    *(u16 *)((u8 *)g + 0xF5A) = (*(u16 *)((u8 *)g + 0xF5A) & 0xF) | (phase << 4);
    if (g->oldMode == 3 && g->mode == 3) {
        g->savedFrame = g->anim.frame;
        g->savedTimer = g->anim.timer;
    }
    {
        u8 one = 1;
        g->flags |= one;
    }
}

void Actor_ConfigureFromScript(void)
{
    struct ActorAnimController *g = (struct ActorAnimController *)gScriptedActorController;
    struct ActorSprite *obj;
    if (!g)
        g = (struct ActorAnimController *)Actor_EnsureController(0xbb34, 0);
    obj = (struct ActorSprite *)((u8 *)g + 0xddc);
    AnimData_SelectPaletteBank((u8 *)g + 0xe08, 343);
    g->obj.priority = 0;
    g->last = 0;
    g->resource = Script_GetValue();
    g->next = Script_GetKeywordValue(0x72, 0);
    g->option = Script_GetKeywordValue(0x41, 0);
    if (Script_SeekToKeyword(0x70)) {
        {
            s32 v = Script_GetValue();
            g->savedX = v;
            obj->x = v;
        }
        {
            s32 v = Script_GetValue();
            g->savedY = v;
            obj->y = v;
        }
        {
            s32 v = Script_GetValue();
            g->savedZ = v;
            obj->z = v;
        }
    } else {
        u8 *owner;
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedX = *(u16 *)(owner + 0x30);
        obj->x = *(u16 *)(owner + 0x30);
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedY = *(u16 *)(owner + 0x32);
        obj->y = *(u16 *)(owner + 0x32);
        owner = *(u8 **)((u8 *)g + 0x18);
        g->savedZ = *(u16 *)(owner + 0x34);
        obj->z = *(u16 *)(owner + 0x34);
    }
    if (Script_SeekToKeyword(0x63))
        W(0xf50) = Script_GetValue();
    g->phaseSelect = Script_GetKeywordValue(0x73, 0);
    if (gSunClock[14] != 5 && gSunClock[14] != 0 && gSunClock[14] != 4)
        g->growFlag = 1;
    else
        g->growFlag = 0;
    ActorShadow_Disable((u8 *)g + 0xe34);
    *(u32 *)((u8 *)g + 0xddc) |= 1;
    switch (g->phaseSelect) {
        case 0:
            state(g, 0, 0);
            break;
        case 1:
            state(g, 2, 1);
            break;
        case 2:
            state(g, 2, 2);
            break;
        case 3:
            state(g, 2, 3);
            break;
        case 4:
            state(g, 2, 5);
            break;
        case 5:
            state(g, 2, 6);
            break;
    }
}

/* Initialization view names the body and counters absent from the shared layout. */
struct ActorInitData {
    u8 unk_00[12];
    const u16 *palette;
    u8 unk_10[0x1c];
};
struct ActorInitController {
    u8 unk_00[0x18];
    void *owner;
    struct ActorChildSlot children[16];
    struct ActorSprite obj;
    u8 unk_E00[8];
    struct ActorInitData data[1];
    u8 shadow[0x40];
    void *animationResource;
    u16 count;
    u8 delta, unk_E7B;
    u16 counter;
    u8 unk_E7E[2];
    u8 body[0xc];
    u8 unk_E8C[0x3c];
    s16 deltaX, deltaY, deltaZ;
    u8 unk_ECE[2];
    s16 targetX, targetY, targetZ;
    u8 unk_ED6[2];
    s16 startX, startY, startZ;
    u8 unk_EDE[2];
    s16 savedX, savedY, savedZ;
    u8 unk_EE6[2];
    u16 alternate[16], fadeColors[32];
    const u16 *savedPalette;
    u8 unk_F4C[0x12];
    u16 speed, heading;
    u8 unk_F62[4];
    u8 direction;
    u8 unk_F67[4];
    u8 flip;
    u8 unk_F6C[0xe];
    u16 savedY2;
    u8 unk_F7C[0xe];
    u16 resource;
    u8 unk_F8C[8];
    void (*scaleFn)(void *), (*otherFn)(void *);
    s32 timer;
    u32 variant;
};

extern void *gUnknown_02000580;
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void Sprite_SetOriginStub(void *, s32, s32);
void Shadow_Initialize(void *, void *, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);

static inline void ActorInit_InitMainSprite(struct ActorInitController *p, struct ActorSprite *obj)
{
    void *effect;
    p->animationResource = ScriptProc_Start(0x922e, 0xc6a0);
    effect = p->data;
    AnimData_Load(effect, 0x97d3);
    Sprite_Init(obj, effect, 0);
    Sprite_SetOriginStub(obj, -8, -19);
    obj->priority = 0;
    obj->flags = 3;
}

static inline void ActorInit_SetChildControl(void (*callback)(void *, void *),
                                             struct ActorChildSlot *child)
{
    child->control = 2;
    child->fn = callback;
    child->control |= 1;
}

/* Initialize the singleton controller, its palette banks and child sprites. */
s32 Actor_InitializeController(void *arg0)
{
    struct ActorInitController *p = arg0;
    struct ActorSprite *obj;
    void *shadow;
    const u16 *palette, *src;
    u16 *dst;
    s32 i;
    u32 mask;
    gScriptedActorController = arg0;
    p->owner = gUnknown_02000580;
    obj = &p->obj;
    ActorInit_InitMainSprite(p, obj);
    shadow = p->shadow;
    Shadow_Initialize(shadow, &p->obj.x, 0);
    ActorShadow_Disable(shadow);
    p->delta = 1;
    p->count = 0;
    p->counter = 0;
    {
        struct ActorInitData *data = p->data;
        const u16 **saved = &p->savedPalette;
        *saved = data->palette;
    }
    palette = p->savedPalette;
    mask = 0xffff;
    src = palette;
    dst = p->fadeColors;
    for (i = 15; i >= 0; i--) {
        dst[16] = *src;
        *dst = *src & mask;
        src++;
        dst++;
    }
    {
        const u16 *src2 = palette + 16;
        u16 *dst2 = p->alternate;
        for (i = 15; i >= 0; i--) {
            *dst2 = *src2;
            src2++;
            dst2++;
        }
    }
    p->direction = 0;
    p->heading = 0;
    p->flip = 0;
    for (i = 0; i < 16; i++) {
        struct ActorChildSlot *child = &p->children[i];
        void *data = child->sprite;
        AnimData_Load(data, 0xa850);
        Sprite_Init(child, data, 0);
        Sprite_SetOriginStub(child, -8, -16);
        child->flags = 1;
        child->index = i;
        ActorInit_SetChildControl(Actor_OnControllerIdle, child);
    }
    p->scaleFn = 0;
    p->otherFn = 0;
    p->obj.x = 0;
    p->obj.y = 0;
    p->obj.z = 0;
    p->deltaZ = 0;
    p->deltaY = 0;
    p->deltaX = 0;
    p->savedY2 = 0;
    p->targetX = 25;
    p->targetY = 102;
    p->targetZ = 0;
    p->speed = 200;
    if (!SceneBody_GetById(p->resource))
        SceneBody_Init(p->body, p->resource, &p->savedX, 0, 7, p);
    p->variant = 0;
    return 0;
}

void Actor_RequestControllerRemoval(void)
{
    Task_Kill(gScriptedActorController);
}

/* Create the singleton animated controller and install its task callbacks. */

struct ActorAnimController *Task_Create(u32, u32);

void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));

s32 Actor_InitializeController(void *);

u8 *Actor_EnsureController(u32 resource, u32 unused)
{
    struct ActorAnimController *p;
    if (gScriptedActorController)
        return gScriptedActorController;
    p = Task_Create(11, 0xfa4);
    if (p) {
        p->resource = resource;
        Task_SetCallbacks(p, Actor_UpdateController, Actor_DestroyController);
        if (Actor_InitializeController(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 SunClock_GetPhase(void);

s32 TimeDisplay_GetLightPhaseGroup(void)
{
    s32 r;
    switch (SunClock_GetPhase()) {
        case 1:
        case 2:
        case 3:
            r = 0;
            break;
        case 0:
        case 4:
        case 5:
            r = 1;
            break;
        default:
            r = -1;
            break;
    }
    return r;
}

u32 SunClock_GetPackedDate(void);

void Clock_UnpackDate(u32 *, u32 *, u32 *, u32);

u32 TimeDisplay_GetCurrentMonth(void)
{
    u32 a;
    u32 b;
    u32 c;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    return b;
}

/* Initialize the controller's background layers and palette. */

void Bg_ClearTilemap(u32);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void *ScriptProc_Start(u32, u32);

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void Bg_SetPriority(u32, u32);

void TimeDisplay_InitBackgrounds(u8 *p)
{
    void *resource;
    u32 layer;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    resource = ScriptProc_Start(0xc091, 0xc5a7);
    *(void **)(p + 0x720) = resource;
    layer = 0;
    Bg_SetupLayers(1, 16, resource, 0, 0, 1, &layer);
    Bg_SetPriority(2, 2);
    layer = 1;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &layer);
    Bg_SetPriority(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet((u8 *)ScriptProc_Start(0x92b3, 0x91d) + 20, gBgPaletteBuffer, 256);
}

void ResourceHeader_Relocate(u8 *, void *);

void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);

void TimeDisplay_HideAnimatedObjects(u8 *);

void TimeDisplay_InitAnimatedObjects(u8 *s)
{
    void *p = ScriptProc_Start(0xCB05, 0x731D);
    u8 *obj = s + 0x4E0;
    u32 zero;
    u32 two;
    u32 one;
    ResourceHeader_Relocate(obj, p);
    Widget_InitLinked(s + 0x18, obj, 1, 0x10, zero = 0, 0, 0, 0);
    Widget_InitLinked(s + 0xA0, obj, 2, 0x10, zero, zero, zero, zero);
    Widget_InitLinked(s + 0x128, obj, 11, 0x10, two = 2, zero, zero, zero);
    Widget_InitLinked(s + 0x1B0, obj, 11, 0x10, two, zero, zero, zero);
    Widget_InitLinked(s + 0x458, obj, 0, 0x10, zero, zero, zero, zero);
    Widget_InitLinked(s + 0x238, obj, 7, 0x10, zero, zero, zero, zero);
    Widget_InitLinked(s + 0x2C0, obj, 3, 0x10, zero, zero, zero, zero);
    Widget_InitLinked(s + 0x348, obj, 7, 0x30, one = 1, zero, zero, zero);
    Widget_InitLinked(s + 0x3D0, obj, 3, 0x30, one, zero, zero, zero);
    TimeDisplay_HideAnimatedObjects(s);
}

/* Hide all nine animated display objects. */

void TimeDisplay_HideAnimatedObjects(u8 *p)
{
    *(u32 *)(p + 0x20) |= 1;
    *(u32 *)(p + 0x350) |= 1;
    *(u32 *)(p + 0x3d8) |= 1;
    *(u32 *)(p + 0xa8) |= 1;
    *(u32 *)(p + 0x1b8) |= 1;
    *(u32 *)(p + 0x130) |= 1;
    *(u32 *)(p + 0x240) |= 1;
    *(u32 *)(p + 0x2c8) |= 1;
    *(u32 *)(p + 0x460) |= 1;
}

/* Release the nine animated display objects. */

void Anim_Remove(void *);

void TimeDisplay_RemoveAnimatedObjects(u8 *p)
{
    Anim_Remove(p + 0x3d0);
    Anim_Remove(p + 0x348);
    Anim_Remove(p + 0x458);
    Anim_Remove(p + 0x1b0);
    Anim_Remove(p + 0x128);
    Anim_Remove(p + 0x2c0);
    Anim_Remove(p + 0x238);
    Anim_Remove(p + 0xa0);
    Anim_Remove(p + 0x18);
}

void Anim_LoadKeyframe(u8 *, u8 *, u16, s32);

void TimeDisplay_SetSelectionKeyframe(u8 *p)
{
    u8 *a = p + 0x128;
    u8 *b = p + 0x4E0;
    u16 v = (u16)(*(u32 *)(p + 0x724) + 0xC);
    Anim_LoadKeyframe(a, b, v, 0);
}

/* Show the paired foreground objects and their associated objects. */

struct Obj {
    u32 a, b, flags;
};

void Actor_HideAuxiliarySprites(u8 *p)
{
    ((struct Obj *)(p + 0x238))->flags &= ~1;
    ((struct Obj *)(p + 0x2c0))->flags &= ~1;
    *(u32 *)(p + 0x350) &= ~1;
    *(u32 *)(p + 0x3d8) &= ~1;
}

/* Advance a paired four-frame display every tenth update. */

extern u8 gUnk_0824FDC8[];

void memcpy(void *, const void *, u32);

void Actor_UpdatePairedSpriteAnimation(u8 *p)
{
    u8 frames[4][2];
    if (Mod(++*(s32 *)(p + 0x744), 10) == 0) {
        memcpy(frames, gUnk_0824FDC8, 8);
        if (++*(s32 *)(p + 0x740) > 3)
            *(s32 *)(p + 0x740) = 0;
        Anim_LoadKeyframe(p + 0x238, p + 0x4e0, frames[*(s32 *)(p + 0x740)][0], 0);
        Anim_LoadKeyframe(p + 0x2c0, p + 0x4e0, frames[*(s32 *)(p + 0x740)][1], 0);
    }
}
