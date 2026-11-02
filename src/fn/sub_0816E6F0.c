#include "scene_hud_display.h"
#include "engine_globals.h"
#include "ram.h"

#define HUD_OFFSET(field) ((u32) & ((struct SceneHudState *)0)->field)

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);

static inline s32 Halve(s32 value)
{
    return value >= 0 ? value >> 1 : -(-value >> 1);
}

static inline s32 Shift12(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline void RandomSpin_0816E6F0(s16 *spin)
{
    u16 *table = gRandomTable;
    u32 *index = &gRandomIndex;
    u32 mask;
    u32 i;
    s32 v;
    i = (*index) + 1;
    mask = 0x3FF;
    i &= mask;
    *index = i;
    v = Mod(table[i], 10) + 5;
    *spin = v;
    i = ((*index) + 1) & mask;
    *index = i;
    {
        u16 sample = table[i];
        if (sample & 1)
            *spin = -v;
    }
}

s32 sub_0816E6F0(u8 *m)
{
    s16 *pos = (s16 *)(m + HUD_OFFSET(widgetMotions[0].from));
    s16 *vel = (s16 *)(m + HUD_OFFSET(widgetMotions[0].to));
    u16 *angle = (u16 *)(m + HUD_OFFSET(widgetMotions[0].duration));
    s16 *spin = (s16 *)(m + HUD_OFFSET(widgetMotions[0].timer));
    s32 stopped;
    s32 a;
    s32 b;

    pos[0] += vel[0];
    pos[1] += vel[1];
    vel[1]++;
    if (!(++((struct SceneHudState *)m)->actionTransferDelay & 15)) {
        if (vel[0] > 0)
            vel[0]--;
        else if (vel[0] < 0)
            vel[0]++;
    }
    if (pos[0] < 8) {
        pos[0] = 8;
        vel[0] = Halve(-vel[0]);
        RandomSpin_0816E6F0(spin);
    } else if (pos[0] > 0xE8) {
        pos[0] = 0xE8;
        vel[0] = Halve(-vel[0]);
        RandomSpin_0816E6F0(spin);
    }
    stopped = 0;
    if (pos[1] > 0x77) {
        s32 v;
        s32 zero;
        pos[1] = 0x78;
        v = Halve(-vel[1]);
        zero = 0;
        vel[1] = v;
        if (v > (-2)) {
            vel[0] = zero;
            vel[1] = zero;
            *spin = zero;
            if (((u16)((*angle) - 0x40)) < 0x80)
                *angle = 0x80;
            else
                *angle = zero;
            stopped = 1;
        } else {
            RandomSpin_0816E6F0(spin);
        }
    }
    if (stopped) {
        *((u32 *)(m + HUD_OFFSET(motionWidgets[0].flags))) &= ~2;
        {
            u16 t = pos[0] - 8;
            *((u16 *)(m + HUD_OFFSET(motionWidgets[0].position.x))) = t;
        }
        {
            u16 t = pos[1] - 8;
            *((u16 *)(m + HUD_OFFSET(motionWidgets[0].position.y))) = t;
        }
        {
            struct SceneHudState *hud = (void *)m;
            hud->motionWidgets[0].rotation = *angle;
            if (!hud->motionWidgets[0].rotation)
                return 0;
        }
        Anim_LoadKeyframe(m + HUD_OFFSET(motionWidgets[0]), m + HUD_OFFSET(motionResource), 0xCE,
                          1);
        return 1;
    }
    *angle = (((*angle) + 0x100) + (*spin)) & 0xFF;
    a = Shift12(-(gUnk_086149C4[((*angle) + 0x40) & 0xFF] * 8));
    b = Shift12(-(gUnk_086149C4[*((u8 *)angle)] * 8));
    *((u32 *)(m + HUD_OFFSET(motionWidgets[0].flags))) |= 2;
    {
        u16 t = (pos[0] + b) + a;
        *((u16 *)(m + HUD_OFFSET(motionWidgets[0].position.x))) = t;
    }
    {
        u16 t = (pos[1] + a) - b;
        *((u16 *)(m + HUD_OFFSET(motionWidgets[0].position.y))) = t;
    }
    {
        u16 t = *angle;
        *((u16 *)(m + HUD_OFFSET(motionWidgets[0].rotation))) = t;
    }
    return -1;
}
