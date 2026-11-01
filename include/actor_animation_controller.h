#ifndef GUARD_ACTOR_ANIMATION_CONTROLLER_H
#define GUARD_ACTOR_ANIMATION_CONTROLLER_H

/* Tentative shared layout for the actor animation controller (081E34D0-081EAB64).
 * Offsets are cross-checked across many matched functions in this unit; field
 * names are provisional descriptions, not an identified character or a claim
 * that this is the game's entire animation system. Byte ranges whose role is
 * still unknown stay as unkNNNN padding. */

struct ActorChildSlot {
    u32 flags;
    u8 unk04[0x28];
    u8 sprite[0x80];
    void (*fn)(void *, void *);
    u8 index;
    u8 control;
    u8 unkB2[0x1c];
    u8 variant;
    u8 unkCF;
    u16 effect;
    u8 unkD2[0xa];
};

/* Same 16-entry array, viewed starting at each slot's callback field
 * (absolute offset 0x1C + 0xAC = 0xC8) instead of the slot's own start.
 * Kept distinct from ActorChildSlot (not a cast of it) so the already-matched
 * register-pinned callers that index from this phase keep their exact shape. */
struct ActorChildCallbackView {
    void (*fn)(void *, void *);
    u8 pad;
    u8 flags;
    u8 rest[0xdc - 6];
};

struct ActorSprite {
    u32 flags;
    u8 unk04[3];
    u8 priority;
    s8 scaleX, scaleY;
    u8 unk0A[6];
    u16 frame;
    u8 unk12[10];
    s16 x;
    s16 y; /* rotating-effect callers reinterpret this as a u16 "angle" via a local cast */
    s16 z;
};

struct ActorAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};

struct ActorAnimController {
    u8 unk00[0x1c];
    struct ActorChildSlot children[16]; /* 0x1C, stride 0xDC */
    struct ActorSprite obj;             /* 0xDDC; agbcc tail-pads this to 0x24 bytes */
    u8 unkE00[8];
    u8 effect[12];      /* 0xE08: palette bank index buffer, see AnimData_SelectPaletteBank */
    const u16 *palette; /* 0xE14: active 16-color palette pointer */
    u8 unkE18[12];
    struct ActorAnim anim; /* 0xE24 */
    u8 p2[0x40];           /* 0xE34: motion-related state, not yet identified */
    u32 arg;               /* 0xE74 */
    u16 count;             /* 0xE78 */
    s8 delta;              /* 0xE7A */
    u8 unkE7B[17];
    u16 dx, dy, dz; /* 0xE8C */
    u8 unkE92[0x36];
    s16 deltaX, deltaY, deltaZ; /* 0xEC8 */
    u8 unkECE[2];
    s16 targetX, targetY, targetZ; /* 0xED0 */
    u8 unkED6[2];
    s16 startX, startY, startZ; /* 0xED8 */
    u8 unkEDE[2];
    s16 savedX, savedY, savedZ; /* 0xEE0 */
    u8 unkEE6[2];
    u16 alternate[16];       /* 0xEE8: palette fade source colors */
    u16 base[16];            /* 0xF08: palette fade target colors */
    u16 output[16];          /* 0xF28: palette fade blended colors */
    const u16 *savedPalette; /* 0xF48 */
    s16 step;                /* 0xF4C */
    u16 duration;            /* 0xF4E */
    void *handle;            /* 0xF50 */
    void *completionHandle;  /* 0xF54 */
    u8 done, state;          /* 0xF58 */
    u16 mode : 4;            /* 0xF5A */
    u16 phase : 12;
    u16 oldMode;        /* 0xF5C */
    u16 speed, heading; /* 0xF5E */
    s16 interactionHeading; /* 0xF62 */
    u8 flags;       /* 0xF64: state-entry flag */
    u8 phaseSelect; /* 0xF65: provisional, selects a (mode,phase) pair on setup */
    u8 direction;   /* 0xF66 */
    s8 targetFacing;
    u8 facing;
    s8 bend;       /* 0xF69 */
    u8 option;     /* 0xF6A: provisional, copied from InteractionPartner.option */
    u8 flip;       /* 0xF6B */
    u8 last, next; /* 0xF6C */
    u8 unkF6E;
    u8 growFlag; /* 0xF6F: provisional, gates Actor_UpdateScaleGrowth */
    u8 unkF70[7];
    u8 variant8; /* 0xF77: provisional, not the same slot as .variant below */
    u8 unkF78[2];
    u16 savedY2; /* 0xF7A: provisional idle-restore save, distinct from sprite.y */
    u8 heightSettled;
    u8 interrupted; /* 0xF7D */
    u16 savedPhase; /* 0xF7E */
    u8 unkF80[4];
    u16 delay;                 /* 0xF84 */
    u8 savedFrame, savedTimer; /* 0xF86 */
    u8 delayCounter;           /* 0xF88: provisional, distinct from the delay/timer fields above */
    s8 group;                  /* 0xF89 */
    u16 resource;              /* 0xF8A */
    void (*previous)(void *);  /* 0xF8C */
    void (*fn)(void *);        /* 0xF90 */
    void (*scaleFn)(void *);   /* 0xF94 */
    void (*otherFn)(void *);   /* 0xF98 */
    s32 timer;                 /* 0xF9C */
    u32 variant;               /* 0xFA0 */
};

/* Tentative layout for the actor being interacted with (a smaller, partial
 * view of a different actor instance; not this controller's own fields). */
struct InteractionPartner {
    u8 unk00[0x1c];
    s16 x, y, z;
    u8 unk22[0x48 - 0x22];
    s16 worldX, worldY, worldZ;
    u8 unk4E[0xa4 - 0x4e];
    void *handle, *request;
    void (*fn)(void *, void *);
    u8 group, flags, orientation, option;
    u8 timer;
    u8 unkB5[0xd0 - 0xb5];
    u16 resource;
    u8 unkD2[2];
    s32 distance;
    u8 eligible;
};

#endif /* GUARD_ACTOR_ANIMATION_CONTROLLER_H */
