// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "large_scene_animation_controller.h"
#include "game_state.h"
#include "engine.h"

void SceneAnimation_UpdateSlotAnimation10(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xa, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation11(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xb, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation0(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation2(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 2, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation1(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 1, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation3(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 3, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation12(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xc, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation13(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xd, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation14(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xe, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation15(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0xf, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation16(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x10, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation17(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x11, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation18(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x12, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation19(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x13, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation20(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x14, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation21(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x15, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation22(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x16, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation23(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x17, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void SceneAnimation_UpdateSlotAnimation45(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2d, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void SceneAnimation_UpdateSlotAnimation46(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2e, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void Nidhoggr_UpdateLoopAnimation29(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x29, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void Nidhoggr_UpdateLoopAnimation2B(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2b, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void Nidhoggr_UpdateLoopAnimation2A(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2a, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void Nidhoggr_UpdateLoopAnimation2C(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    s = t + i;
    if (*s == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2c, 1, 4);
        (*s)++;
        p->rowDoneFlag[i] = 1;
    }
    Anim_Step(&p->rows[i], p->animHdr);
}

void Nidhoggr_UpdateOneShotAnimation2F(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x2f, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation30(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x30, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation31(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x31, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation32(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x32, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation33(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x33, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation34(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x34, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation35(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x35, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation36(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x36, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation37(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x37, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation15(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x15, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation39(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x39, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation3A(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x3a, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation18(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x18, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation19(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x19, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation1A(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1a, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdateOneShotAnimation1B(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1b, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation1C(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1c, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation1D(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1d, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation1E(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1e, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation1F(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x1f, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation20(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x20, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation21(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x21, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation22(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x22, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation23(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x23, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation24(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x24, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation25(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x25, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation26(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x26, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation28(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x28, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

void Nidhoggr_UpdatePartAnimation27(struct Nidhoggr *p, s32 i)
{
    u8 *s;
    u8 *t = p->rowInitFlag;
    u8 z;
    s = t + i;
    z = *s;
    if (z == 0) {
        p->rows[i].flags &= ~1;
        Anim_SetKeyframes(&p->rows[i], p->animHdr, 0x27, 2, 4);
        (*s)++;
        p->rowDoneFlag[i] = z;
    }
    if (Anim_Step(&p->rows[i], p->animHdr) != 0) {
        *s = 0xff;
        p->rowDoneFlag[i] = 1;
    }
}

/* Tentative: called once per frame from Nidhoggr_Update. For each of the 26 parts it looks up
 * a callback in gUnk_0824F364 keyed by t->unknown71C2[i] and runs it through sub_08249240
 * (or resets the row latches when there is none), blinks the row via rowBlink, then
 * repositions each row's offset (rows[i].pos) and collider from t->pos plus constant offset
 * tables. confidence=low. */

extern void (*const gUnk_0824F364[])(void);
extern const struct SceneVec gUnk_0824F2E4[];
extern const struct SceneVec gUnk_0824F324[];
extern const struct SceneVec gUnk_0824F334[];
extern const struct SceneVec gUnk_0824F33C[];
extern const struct SceneVec gUnk_0824F344[];

void Nidhoggr_UpdatePartMotion(struct Nidhoggr *t)
{
    void **q;
    s32 i;
    s32 j;
    struct SceneVec *p21, *p20;
    struct SceneVec *pa, *pb, *pc, *pd, *pe, *pf;
    for (i = 0; i <= 25; i++) {
        if (gUnk_0824F364[t->unknown71C2[i]] == 0) {
            t->rows[i].flags |= 1;
            t->rowInitFlag[i] = 0;
            t->rowCounter[i] = 0;
            t->rowDoneFlag[i] = 1;
        } else {
            if (t->rowBlink[i] != 0) {
                t->rowBlink[i]--;
                if (i & 1)
                    t->rows[i].flags |= 0x200;
                else
                    t->rows[i].flags |= 0x400;
            } else {
                t->rows[i].flags &= ~0x600;
            }
            sub_08249240(t, (void *)i, gUnk_0824F364[t->unknown71C2[i]]);
            t->rowCounter[i]++;
        }
    }
    for (i = 0, p21 = &t->rows[21].pos, p20 = &t->rows[20].pos, pa = &t->pos; i <= 1; i++) {
        t->rows[i].pos.x = pa->x + gUnk_0824F344->x;
        t->rows[i].pos.y = pa->y + gUnk_0824F344->y;
        t->rows[i].pos.z = pa->z + gUnk_0824F344->z;
        Collider_SetPosition(&t->colliders[i], pa, 0);
        Collider_SetPosition(&t->altColliders[i + 1], pa, 0);
    }
    for (i = 2, j = 0, pb = &t->pos; i <= 9; i++, j++) {
        t->rows[i].pos.x = pb->x + j[gUnk_0824F2E4].x;
        t->rows[i].pos.y = pb->y + j[gUnk_0824F2E4].y;
        t->rows[i].pos.z = pb->z + j[gUnk_0824F2E4].z;
        Collider_SetPosition(&t->colliders[i], &t->rows[i].pos, 0);
    }
    for (i = 10, j = 0, pc = &t->pos; i <= 17; i++, j++) {
        t->rows[i].pos.x = pc->x + j[gUnk_0824F2E4].x;
        t->rows[i].pos.y = pc->y + j[gUnk_0824F2E4].y;
        t->rows[i].pos.z = pc->z + j[gUnk_0824F2E4].z;
        Collider_SetPosition(&t->colliders[i], &t->rows[i].pos, 0);
    }
    for (i = 18, j = 0, pd = &t->pos; i <= 19; i++, j++) {
        t->rows[i].pos.x = pd->x + j[gUnk_0824F324].x;
        t->rows[i].pos.y = pd->y + j[gUnk_0824F324].y;
        t->rows[i].pos.z = pd->z + j[gUnk_0824F324].z;
        Collider_SetPosition(&t->colliders[i], &t->rows[i].pos, 0);
    }
    pe = &t->pos;
    p21->x = pe->x + gUnk_0824F334->x;
    p21->y = pe->y + gUnk_0824F334->y;
    p21->z = pe->z + gUnk_0824F334->z;
    p20->x = pe->x + gUnk_0824F33C->x;
    p20->y = pe->y + gUnk_0824F33C->y;
    p20->z = pe->z + gUnk_0824F33C->z;
    i = 22;
    for (; i <= 23; i++) {
        t->rows[i].pos.x = (&t->pos)->x + gUnk_0824F344->x;
        t->rows[i].pos.y = (&t->pos)->y + gUnk_0824F344->y;
        t->rows[i].pos.z = (&t->pos)->z + gUnk_0824F344->z;
        Collider_SetPosition(&t->colliders[i], &t->rows[i].pos, 0);
    }
    q = (void **)t->unknown72C4;
    if (*q != 0)
        Callback_InvokeActor(q, *q);
}

extern u16 gUnk_03004BD8;

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void Bg_SetPriority(u32, u32);

static inline void Or16(u32 m, u16 *p)
{
    *p |= m;
}

static inline void And16(u32 m, u16 *p)
{
    *p &= m;
}

void Nidhoggr_InitBackgrounds(struct Nidhoggr *t)
{
    u32 index;
    t->bgScript = ScriptProc_Start(0xc091, 0xe356);
    index = 0;
    Bg_SetupLayers(2, 0, t->bgScript, 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    Bg_SetPriority(3, 2);
    index = 2;
    Bg_SetupLayers(3, 0, t->bgScript, 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0);
    Or16(0xc00, &gUnk_03004BD8);
    t->bgScrollX = 0;
    t->bgScrollY = 0;
    t->bgMode = 0;
}

struct Vec081935B8 {
    s16 x, y, z;
};

extern u16 gUnk_03005418[];

void BgGfx_LoadTilemapAtPosition(u32, s32, s32);

static inline s32 Shift5(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    return r;
}

static inline void Project(struct Vec081935B8 *out, struct Vec081935B8 *v)
{
    s32 s, h;
    s32 x, z;
    x = v->x;
    z = v->z;
    out->x = Shift5((x - z) * 3);
    s = Shift5((x + z) * 3);
    h = Shift5(v->y * 3);
    out->y = s - h;
    out->z = s + h;
}

void Nidhoggr_UpdateBackgroundScroll(struct Nidhoggr *t)
{
    struct Vec081935B8 d;
    Project(&d, (struct Vec081935B8 *)&t->pos);
    d.x = gUnk_03005418[0] - d.x;
    d.y = gUnk_03005418[1] + 10 - d.y;
    d.z = d.z - gUnk_03005418[2];
    {
        s16 tmp = d.x;
        t->bgScrollX = tmp;
    }
    t->bgScrollY = d.y;
    BgGfx_LoadTilemapAtPosition(3, t->bgScrollX, t->bgScrollY);
    BgGfx_LoadTilemapAtPosition(1, t->bgScrollX, t->bgScrollY);
}

void SceneAnimation_SetSlotState(void *, u32, u32);

void Nidhoggr_SetBackgroundMode(struct Nidhoggr *t, u32 mode)
{
    u8 *cur = &t->bgMode;
    if (*cur != mode) {
        u32 index = mode;
        u32 zero = 0;
        *cur = mode;
        Bg_SetupLayers(2, 0, t->bgScript, zero, 0, 1, &index);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        if (*cur) {
            SceneAnimation_SetSlotState(t, 0x15, 0x33);
            SceneAnimation_SetSlotState(t, 0x14, 0x34);
        } else {
            SceneAnimation_SetSlotState(t, 0x15, 0x10);
            SceneAnimation_SetSlotState(t, 0x14, 0x11);
        }
    }
}

struct Vec081936F8 {
    s16 x, y, z, w;
};

extern const struct Vec081936F8 gUnk_0824F478[];

void TerrainModifiers_RegisterCell(void *, u32, u32, s32, u32, u32);

static inline u32 CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || x >= gUnk_030052F8 || z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

void Nidhoggr_RegisterTerrainCells(struct Nidhoggr *t)
{
    if (!t->terrainCellsRegistered) {
        TerrainModifiers_RegisterCell(&t->terrainCells[0],
                                      CellIndex(gUnk_0824F478[0].x >> 8, gUnk_0824F478[0].z >> 8),
                                      0, gUnk_0824F478[0].y >> 8, 0xff, 0x1000);
        TerrainModifiers_RegisterCell(&t->terrainCells[1],
                                      CellIndex(gUnk_0824F478[1].x >> 8, gUnk_0824F478[1].z >> 8),
                                      0, gUnk_0824F478[1].y >> 8, 0xff, 0x1000);
        t->terrainCellsRegistered = 1;
    }
}

void TerrainModifiers_UnregisterCell(void *);

void Nidhoggr_UnregisterTerrainCells(struct Nidhoggr *p)
{
    if (p->terrainCellsRegistered) {
        p->terrainCellsRegistered = 0;
        TerrainModifiers_UnregisterCell(&p->terrainCells[0]);
        TerrainModifiers_UnregisterCell(&p->terrainCells[1]);
    }
}

void Nidhoggr_SetTerrainCellFlag40(struct Nidhoggr *p)
{
    p->terrainCells[0].flags |= 0x40;
    p->terrainCells[1].flags = 0x40 | p->terrainCells[1].flags;
}

extern const u8 gUnk_0824F488[];

void SparkParticle_Spawn(u32, u32, const u8 *, u32, u32, u32, u32, u32);

void Nidhoggr_EmitSparks(u32 a)
{
    SparkParticle_Spawn(a, 1, gUnk_0824F488, 0x20, 3, 8, 2, 2);
}

/* Tentative: a per-part "hit reaction" callback, assigned as Nidhoggr_InitParts's anim-row
 * destroy/update callback (referenced five times from its literal pool) and probably
 * invoked through Nidhoggr_UpdatePartMotion's function-pointer table. Computes a collider-relative
 * part index via sub_08249420(colliderOffset, 0x54), adds contact damage
 * (PlayerStats_AddContactHundredths) scaled by t->optionS (0x7274), decrements that
 * part's hp (t->hp[i]) and checks t->groupHpA/groupHpB (0x728A/0x728C), plays sounds
 * (0x211 among them) and updates t->attackActive (0x71BE) gating. 1216 bytes, several
 * undiscovered globals/helpers; not attempted byte-for-byte this pass. confidence=low. */

/* Life drain: subtract dmg from boss life, clamp it to the group hp total, and when it
 * reaches zero take the overflow out of dmg and switch behavior. */

/* Publish boss life, then show the damage number and the sprite override for the row. */

/* Hit flags for the part's contact state: t->flags[2] bits and the t->flags[3 + slot] level. */

// confidence=low: Nidhoggr body-part hit callback; collider c is one of t->colliders, src is the attacker.

/* Life drain: subtract dmg from boss life, clamp it to the group hp total, and when it
 * reaches zero take the overflow out of dmg and switch behavior. */

/* Publish boss life, then show the damage number and the sprite override for the row. */

/* Hit flags for the part's contact state: t->flags[2] bits and the t->flags[3 + slot] level. */

// confidence=low: Nidhoggr body-part hit callback; collider c is one of t->colliders, src is the attacker.

/* Life drain: subtract dmg from boss life, clamp it to the group hp total, and when it
 * reaches zero take the overflow out of dmg and switch behavior. */

/* Publish boss life, then show the damage number and the sprite override for the row. */

/* Hit flags for the part's contact state: t->flags[2] bits and the t->flags[3 + slot] level. */

// confidence=low: Nidhoggr body-part hit callback; collider c is one of t->colliders, src is the attacker.
struct Sub08193874Hit {
    u8 unk_00[0x3E];
    u16 damage;
    u8 unk_40[0x43 - 0x40];
    u8 flags;
    u8 unk_44[0x54 - 0x44];
};
struct Sub08193874Source {
    u8 unk_00[0x44];
    u16 power;
};
void Collision_CalculateDamageResponse(void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
s32 sub_08249420(s32, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void PaletteEffect_StartSpriteOverride(void *, s32, s32);
void Nidhoggr_CreateBurstEffects(void *, void *);
void Sound_PlaySong(u32);
void Sound_PlaySongChecked(u32);

/* Life drain: subtract dmg from boss life, clamp it to the group hp total, and when it
 * reaches zero take the overflow out of dmg and switch behavior. */
static inline s32 Sub08193874_Life(struct Nidhoggr *t, s32 dmg, s32 total)
{
    t->life -= dmg;
    if (t->life < total)
        t->life = total;
    if (t->life <= 0) {
        dmg += t->life;
        t->life = 0;
        if (t->mode != 6)
            Nidhoggr_SetBehavior(t, 12);
    }
    return dmg;
}

/* Publish boss life, then show the damage number and the sprite override for the row. */
static inline void Sub08193874_Report(struct Nidhoggr *t, struct Sub08193874Hit *c, s32 part,
                                      s32 dmg, s32 flag)
{
    *(s32 *)((u8 *)gUnk_02000710 + 0x618) = t->life;
    if (dmg >= 0) {
        DamageNumberDisplay_Show(c, dmg, 0, (u8 *)c + 0xC, (u8 *)c + 0x14, 1);
        PaletteEffect_StartSpriteOverride(&t->rows[part], 8, flag);
    }
}

/* Hit flags for the part's contact state: t->flags[2] bits and the t->flags[3 + slot] level. */
static inline void Sub08193874_Level(struct Nidhoggr *t, struct Sub08193874Hit *c, s32 slot, s32 hp,
                                     s32 th1, s32 th2, s32 th3)
{
    u8 *fl = &t->flags[3];
    if (hp > th3) {
        if (fl[slot] != 0)
            fl[slot] = 0;
        return;
    }
    if (hp > th2) {
        if (fl[slot] != 1)
            fl[slot] = 1;
        return;
    }
    if (hp > th1) {
        if (fl[slot] != 2)
            fl[slot] = 2;
        return;
    }
    if (hp > 0) {
        if (fl[slot] != 3)
            fl[slot] = 3;
        return;
    }
    if (fl[slot] != 4)
        fl[slot] = 4;
}

#ifdef NONMATCHING
// fuzzy: 34.9%
// scope shape 285 (differences once low registers are set aside)
// confidence=low: Nidhoggr body-part hit callback; collider c is one of t->colliders, src is the attacker.
void sub_08193874(void *src, struct Sub08193874Hit *c, struct Nidhoggr *t)
{
    s32 part;
    s32 slot = 0;
    s32 sl = 0;
    s32 dmg;
    s32 total;
    s32 th1, th2, th3;
    s16 *gh;
    s32 saved;

    Collision_CalculateDamageResponse(src);
    dmg = c->damage;
    PlayerStats_AddContactHundredths(src, c, t->optionS);
    part = sub_08249420((u8 *)c - 0x66D8 - (u8 *)t, 0x54);
    if (t->rowBlink[part] != 0)
        return;
    t->rowBlink[part] = ((struct Sub08193874Source *)src)->power;

    gh = &t->groupHpA;
    total = gh[0] + gh[1];
    if ((u32)part <= 1) {
        s32 y = t->optionY;
        th1 = y >> 2;
        th2 = y >> 1;
        th3 = (y * 3) >> 2;
        slot = part;
        if (gh[slot] <= 0)
            return;
        if (t->attackActive == 0) {
            Sound_PlaySongChecked(0x211);
            return;
        }
        saved = gh[slot];
        gh[slot] = saved - dmg;
        if (gh[slot] < 0) {
            dmg += gh[slot];
            gh[slot] = sl;
        }
        total = gh[0] + gh[1];
        if (total <= 0 && t->mode != 6)
            Nidhoggr_SetBehavior(t, 12);
        dmg = Sub08193874_Life(t, dmg, total);
        Sub08193874_Report(t, c, part, dmg, 1);
        if (gh[slot] > 0) {
            if (slot == 0)
                t->flags[2] &= 0xFB;
            else
                t->flags[2] &= 0xF7;
            if (dmg != 0)
                Sound_PlaySongChecked(0x13E);
            else
                Sound_PlaySong(0x211);
            if (c->flags & 4)
                Nidhoggr_EmitSparks((u32)c + 0x24);
        } else {
            if (slot == 0)
                t->flags[2] |= 4;
            else
                t->flags[2] |= 8;
            Sound_PlaySongChecked(0x1BB);
            Nidhoggr_CreateBurstEffects(&t->effectPool, (u8 *)c + 0x24);
        }
        Sub08193874_Level(t, c, slot, gh[slot], th1, th2, th3);
        return;
    }

    switch (t->mode) {
        case 2:
        case 3:
        case 8:
            if ((u32)(part - 6) <= 3) {
                slot = part - 6;
                sl = 5;
            } else if ((u32)(part - 14) <= 3) {
                slot = part - 10;
                sl = 10;
            } else if (part == 19) {
                slot = 8;
                sl = 15;
            } else {
                return;
            }
            break;
        case 4:
        case 5:
        case 9:
            if ((u32)(part - 2) <= 3) {
                slot = part - 2;
                sl = 5;
            } else if ((u32)(part - 10) <= 3) {
                slot = part - 6;
                sl = 10;
            } else if (part == 18) {
                slot = 8;
                sl = 15;
            } else {
                return;
            }
            break;
        default:
            slot = 0;
            sl = 0;
            break;
    }

    if (t->hp[slot] <= 0)
        return;
    saved = t->hp[slot];
    t->hp[slot] = saved - dmg;
    if (t->hp[slot] > 0) {
        if (dmg != 0)
            Sound_PlaySongChecked(0x13E);
        else
            Sound_PlaySong(0x211);
        if (c->flags & 4)
            Nidhoggr_EmitSparks((u32)c + 0x24);
    } else {
        dmg += t->hp[slot];
        t->hp[slot] = 0;
        t->dead[slot] = 0;
        SceneAnimation_SetSlotState(t, part, sl);
        if ((u8)(t->state - 2) <= 1)
            t->posOffsetXVelocity = -0x600;
    }
    total = t->groupHpA + t->groupHpB;
    dmg = Sub08193874_Life(t, dmg, total);
    Sub08193874_Report(t, c, part, dmg, 2);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08193874);
#endif

/* Tentative: sibling of sub_08193874 (also assigned via Nidhoggr_InitParts's literal pool, used
 * once rather than five times), taking the same (task, index, damage) shape and operating
 * on t->groupHpA/groupHpB (0x728A/0x728C) and t->maxHp-derived scaling from t->optionY
 * (0x726C). 964 bytes; not attempted byte-for-byte this pass. confidence=low. */

/* Applies collider damage to Nidhoggr's two health groups or nine independent parts.
 * Damage is clamped against part and total health before displaying the hit. */
struct NidhoggrDamageCollider {
    u8 unk_00[0xC];
    struct SceneVec position;
    struct SceneVec displayOffset;
    u8 unk_1C[0x50 - 0x1C];
    struct Nidhoggr *owner;
};
/* The two grouped-health pools and their damage stages are indexed together. */
struct NidhoggrGroupHealth {
    u8 unk_00[0x724F];
    u8 state[2];
    u8 unk_7251[0x728A - 0x7251];
    s16 hp[2];
};

void PaletteEffect_StartSpriteOverride(void *, s32, s32);

#ifdef NONMATCHING
// fuzzy: 91.9%
// scope shape 8 (differences once low registers are set aside)
s32 Nidhoggr_ApplyPartDamage(struct NidhoggrDamageCollider *collider, s32 kind, s32 amount)
{
    struct Nidhoggr *p;
    u32 slot;
    s32 damage;
    u32 part;
    u32 slotState;
    if (kind != 0)
        return 0;
    part = 0;
    slotState = 0;
    p = collider->owner;
    damage = amount;
    slot = (u32)collider - (u32)&p->colliders[0];
    slot /= sizeof(p->colliders[0]);
    if (slot <= 1) {
        s32 quarter, half, threeQuarters;
        quarter = (s16)p->optionY >> 2;
        half = (s16)p->optionY >> 1;
        threeQuarters = ((s16)p->optionY * 3) >> 2;
        part = slot;
        if (((struct NidhoggrGroupHealth *)p)->hp[slot] <= 0)
            return 0;
        ((struct NidhoggrGroupHealth *)p)->hp[slot] -= damage;
        if (((struct NidhoggrGroupHealth *)p)->hp[slot] < 0) {
            damage += ((struct NidhoggrGroupHealth *)p)->hp[slot];
            ((struct NidhoggrGroupHealth *)p)->hp[slot] = 0;
        }
        if (p->groupHpA <= 0 && p->groupHpB <= 0 && p->mode != 6)
            Nidhoggr_SetBehavior(p, 12);
        p->life -= damage;
        if (p->life <= 0) {
            damage += p->life;
            p->life = 0;
            if (p->mode != 6)
                Nidhoggr_SetBehavior(p, 12);
        }
        gUnk_02000710->unk618 = p->life;
        if (damage > 0) {
            DamageNumberDisplay_Show(collider, damage, 0, &collider->position,
                                     &collider->displayOffset, 1);
            PaletteEffect_StartSpriteOverride(&p->rows[slot], 8, 1);
        }
        if (((struct NidhoggrGroupHealth *)p)->hp[part] > 0) {
            if (part)
                p->flags[2] &= ~8;
            else
                p->flags[2] &= ~4;
        } else {
            if (part)
                p->flags[2] |= 8;
            else
                p->flags[2] |= 4;
        }
        if (((struct NidhoggrGroupHealth *)p)->hp[part] > threeQuarters) {
            if (((struct NidhoggrGroupHealth *)p)->state[part] != 0)
                ((struct NidhoggrGroupHealth *)p)->state[part] = 0;
        } else if (((struct NidhoggrGroupHealth *)p)->hp[part] > half) {
            if (((struct NidhoggrGroupHealth *)p)->state[part] != 1)
                ((struct NidhoggrGroupHealth *)p)->state[part] = 1;
        } else if (((struct NidhoggrGroupHealth *)p)->hp[part] > quarter) {
            if (((struct NidhoggrGroupHealth *)p)->state[part] != 2)
                ((struct NidhoggrGroupHealth *)p)->state[part] = 2;
        } else if (((struct NidhoggrGroupHealth *)p)->hp[part] > 0) {
            if (((struct NidhoggrGroupHealth *)p)->state[part] != 3)
                ((struct NidhoggrGroupHealth *)p)->state[part] = 3;
        } else {
            if (((struct NidhoggrGroupHealth *)p)->state[part] != 4)
                ((struct NidhoggrGroupHealth *)p)->state[part] = 4;
        }
    } else {
        switch (p->mode) {
            case 2:
            case 3:
            case 8:
                if (slot - 6 <= 3) {
                    part = slot - 6;
                    slotState = 5;
                } else if (slot - 14 <= 3) {
                    part = slot - 10;
                    slotState = 10;
                } else if (slot == 19) {
                    part = 8;
                    slotState = 15;
                } else
                    return 0;
                break;
            case 4:
            case 5:
            case 9:
                if (slot - 2 <= 3) {
                    part = slot - 2;
                    slotState = 5;
                } else if (slot - 10 <= 3) {
                    part = slot - 6;
                    slotState = 10;
                } else if (slot == 18) {
                    part = 8;
                    slotState = 15;
                } else
                    return 0;
                break;
            case 6:
            case 7:
                return 0;
        }
        if (p->hp[part] <= 0)
            return 0;
        p->hp[part] -= damage;
        if (p->hp[part] <= 0) {
            damage += p->hp[part];
            p->hp[part] = 0;
            p->dead[part] = 0;
            SceneAnimation_SetSlotState(p, slot, slotState);
            if ((u8)(p->state - 2) <= 1)
                p->posOffsetXVelocity = -0x600;
        }
        p->life -= damage;
        if (p->life <= 0) {
            damage += p->life;
            p->life = 0;
            if (p->mode != 6)
                Nidhoggr_SetBehavior(p, 12);
        }
        gUnk_02000710->unk618 = p->life;
        if (damage > 0) {
            DamageNumberDisplay_Show(collider, damage, 0, &collider->position,
                                     &collider->displayOffset, 1);
            PaletteEffect_StartSpriteOverride(&p->rows[slot], 8, 2);
        }
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Nidhoggr_ApplyPartDamage);
#endif

/* Initialize Nidhoggr's part animations, attack/body colliders, target records,
 * and the time-dependent palette used by part 21. */

struct NidhoggrResourceHeader {
    u32 words[8];
};
struct NidhoggrPartVector {
    u32 words[2];
};
struct NidhoggrPartCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[12];
    struct NidhoggrPartVector offset, bounds;
    u8 unk_24[0x54 - 0x24];
};
struct NidhoggrTargetRecord {
    void *collider;
    u32 flags;
    void *callback;
    void *next;
};
struct NidhoggrPaletteRow {
    u8 unk_00[0x3a];
    u16 paletteIndex;
    u8 unk_3c[12];
    void *palette;
};
extern const u8 gUnk_0824F490[];
extern u8 gUnk_0824F4AC[];
extern struct NidhoggrPartVector gUnk_0824F4B4, gUnk_0824F4BC;
extern struct NidhoggrPartVector gUnk_0824F4C4, gUnk_0824F4CC;
extern struct NidhoggrPartVector gUnk_0824F4D4, gUnk_0824F4DC;
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
void TargetRegistry_RegisterSceneActor(void *, void *, void *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void TimePalette_InitTransition(void *, void *, void *, void *);
s32 Nidhoggr_ApplyPartDamage(struct NidhoggrDamageCollider *collider, s32 kind, s32 amount);

static inline void Nidhoggr_MaskPartFlags(struct NidhoggrPartCollider *p, u32 mask)
{
    p->flags &= mask;
}
static inline void Nidhoggr_SetPaletteIndex(u16 *p, u32 value)
{
    *p = value;
}
static inline void Nidhoggr_SetPalette(void **p, void *value)
{
    *p = value;
}
static inline void Nidhoggr_OrPartFlags(struct NidhoggrPartCollider *p, u32 mask)
{
    p->flags |= mask;
}
static inline void Nidhoggr_AddTargetFlags(u32 *p, u32 mask)
{
    *p |= mask;
}

struct NidhoggrPartInit {
    u8 unk_00[0x4c];
    u32 actorId;
    u8 unk_50[0x541c - 0x50];
    struct NidhoggrResourceHeader animHdr;
    struct NidhoggrAnimRow rows[26];
    struct NidhoggrPartCollider timedCollider;
    struct NidhoggrPartCollider attackColliders[26];
    struct NidhoggrPartCollider colliders[26];
    struct NidhoggrTargetRecord partExtra[26];
    u8 unk_7100[0x722a - 0x7100];
    u8 rowBlink[26];
    u8 unk_7244[0x7270 - 0x7244];
    u16 optionD;
    u8 unk_7272[0x72c4 - 0x7272];
    u8 paletteTransition[0x38];
};

#ifdef NONMATCHING
// fuzzy: 69.9%
// scope shape 46 (differences once low registers are set aside)
void Nidhoggr_InitParts(void *arg0)
{
    struct NidhoggrPartInit *t = arg0;
    struct NidhoggrResourceHeader *src;
    s32 i;
    struct NidhoggrAnimRow *row;
    struct NidhoggrTargetRecord *target;
    struct NidhoggrPartCollider *collider;
    u32 damage;
    src = ScriptProc_Start(0xcb05, 0x546b);
    t->animHdr = *src;
    ResourceHeader_Relocate(&t->animHdr, src);
    i = 0;
    {
        void *zeroVector = gUnk_0824F4AC;
        u32 extent = 16;
        row = t->rows;
        target = t->partExtra;
        do {
            Widget_InitLinked(row, &t->animHdr, 0, 1, 2, 2, 60, zeroVector);
            SceneAnimation_SetSlotState(t, i, gUnk_0824F490[i]);
            t->rowBlink[i] = 0;
            Collider_Init(&t->attackColliders[i], t->actorId, 0x2005, 0, extent, zeroVector,
                          zeroVector);
            collider = &t->colliders[i];
            Collider_Init(collider, t->actorId, 0x4005, 0, extent, zeroVector, zeroVector);
            Collider_Refresh(collider);
            TargetRegistry_RegisterSceneActor(target, collider, Nidhoggr_ApplyPartDamage);
            Nidhoggr_AddTargetFlags(&target->flags, 1);
            row++;
            target++;
            i++;
        } while (i <= 25);
    }
    {
        struct NidhoggrPartVector *bounds;
        struct NidhoggrPartVector *offset;
        damage = t->optionD >> 1;
        {
            struct NidhoggrPartCollider *collider = &t->colliders[0];
            bounds = &gUnk_0824F4CC;
            collider->bounds = *bounds;
            offset = &gUnk_0824F4C4;
            collider->offset = *offset;
            Nidhoggr_MaskPartFlags(collider, ~4);
            Collider_SetBodyProperties(collider, damage, 0x120, 16);
            Collider_SetCallback(collider, sub_08193874, t);
        }
        {
            struct NidhoggrPartCollider *collider = &t->attackColliders[0];
            bounds = &gUnk_0824F4CC;
            collider->bounds = *bounds;
            offset = &gUnk_0824F4C4;
            collider->offset = *offset;
            Nidhoggr_MaskPartFlags(collider, ~4);
            Collider_SetAttackProperties(collider, 10, 10, 0, 0x40000, 30);
            Collider_SetCallback(collider, 0, 0);
        }
        {
            struct NidhoggrPartCollider *collider = &t->colliders[1];
            bounds = &gUnk_0824F4DC;
            collider->bounds = *bounds;
            offset = &gUnk_0824F4D4;
            collider->offset = *offset;
            Nidhoggr_MaskPartFlags(collider, ~4);
            Collider_SetBodyProperties(collider, damage, 0x120, 16);
            Collider_SetCallback(collider, sub_08193874, t);
        }
        {
            struct NidhoggrPartCollider *collider = &t->attackColliders[1];
            bounds = &gUnk_0824F4DC;
            collider->bounds = *bounds;
            offset = &gUnk_0824F4D4;
            collider->offset = *offset;
            Nidhoggr_MaskPartFlags(collider, ~4);
            Collider_SetAttackProperties(collider, 10, 10, 0, 0x40000, 30);
            Collider_SetCallback(collider, 0, 0);
        }
    }
    damage = t->optionD;
    {
        struct NidhoggrPartCollider *collider = &t->colliders[2];
        u32 flags = 0x1008;
        for (i = 7; i >= 0; collider++, i--) {
            collider->bounds = gUnk_0824F4BC;
            collider->offset = gUnk_0824F4B4;
            Nidhoggr_OrPartFlags(collider, flags);
            Collider_SetBodyProperties(collider, damage, 0x120, 16);
            Collider_SetCallback(collider, sub_08193874, t);
        }
    }
    {
        struct NidhoggrPartCollider *collider = &t->colliders[10];
        u32 flags = 0x1008;
        for (i = 7; i >= 0; collider++, i--) {
            collider->bounds = gUnk_0824F4BC;
            collider->offset = gUnk_0824F4B4;
            Collider_SetBodyProperties(collider, damage, 0x120, 16);
            Collider_SetCallback(collider, sub_08193874, t);
            Nidhoggr_OrPartFlags(collider, flags);
        }
    }
    {
        struct NidhoggrPartCollider *collider = &t->colliders[18];
        u32 flags = 0x1008;
        for (i = 1; i >= 0; collider++, i--) {
            collider->bounds = gUnk_0824F4BC;
            collider->offset = gUnk_0824F4B4;
            Collider_SetBodyProperties(collider, damage, 0x120, 16);
            Collider_SetCallback(collider, sub_08193874, t);
            Nidhoggr_OrPartFlags(collider, flags);
        }
    }
    TimePalette_InitTransition(t->paletteTransition, gUnk_030042E4 + 0x3500, gUnk_030042E4 + 0x3560,
                               gUnk_030042E4 + 0x3580);
    Nidhoggr_SetPaletteIndex(&((struct NidhoggrPaletteRow *)&t->rows[21])->paletteIndex, 0xf1a3);
    Nidhoggr_SetPalette(&((struct NidhoggrPaletteRow *)&t->rows[21])->palette, &t->rowBlink[18]);
}
#else
INCLUDE_ASM("asm/nonmatching", Nidhoggr_InitParts);
#endif

void Anim_Remove(void *);

void Collider_Remove(void *);

void SceneLink_RemoveNode(void *);

void Nidhoggr_DestroyParts(struct Nidhoggr *p)
{
    s32 i = 0;
    u8 *a = (u8 *)p->partExtra;
    u8 *b = (u8 *)p->rows;
    do {
        Anim_Remove(b);
        Collider_Remove(&p->colliders[i]);
        SceneLink_RemoveNode(a);
        a += 0x10;
        b += 0x60;
        i++;
    } while (i <= 0x19);
}

struct Pos081944C4 {
    u32 a, b;
};

extern const struct Pos081944C4 gUnk_0824F4E4;

extern const u8 gUnk_0824F4EC[], gUnk_0824F4F4[];

void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);

void Nidhoggr_InitSuspendedObject(struct Nidhoggr *t)
{
    u8 *anim = t->suspendedAnim;
    u8 *sprite;
    u32 h;
    u8 *collider;
    u16 hz;
    u8 bz;
    AnimData_Load(anim, 0xc7f0);
    AnimData_SelectPaletteBank(anim, 0x1a3);
    sprite = t->suspendedSprite;
    Sprite_Init(sprite, anim, 0);
    *(u32 *)sprite |= 1;
    hz = 0;
    bz = 0;
    *(u16 *)(sprite + 0x10) = hz;
    *(struct Pos081944C4 *)&t->suspendedPosX = gUnk_0824F4E4;
    t->suspendedState = bz;
    t->terrainCellsRegistered = bz;
    t->suspendedTimer = hz;
    h = t->attackPower >> 1;
    collider = t->suspendedCollider;
    Collider_Init(collider, t->actorId, 0x2001, 0, 16, (void *)gUnk_0824F4EC,
                  (void *)gUnk_0824F4F4);
    Collider_SetAttackProperties(collider, h, 10, 0, 0x40000, 30);
    Collider_SetCallback(collider, NULL, NULL);
}

void Nidhoggr_DestroySuspendedObjectSprite(struct Nidhoggr *p)
{
    Sprite_Remove(p->suspendedSprite);
}

void Nidhoggr_StartSuspendedObjectDescent(struct Nidhoggr *p)
{
    u8 *q = &p->suspendedState;
    u8 v = *q;
    if (v == 0) {
        *q = 1;
        p->suspendedTimer = v;
    }
}

void Nidhoggr_StartSuspendedObjectRetraction(struct Nidhoggr *p)
{
    u8 *q = &p->suspendedState;
    if (*q == 4 || *q == 6) {
        u32 z = 0;
        *q = 7;
        p->suspendedTimer = z;
    }
}

void Nidhoggr_StartSuspendedObjectShake(struct Nidhoggr *p)
{
    u8 *q = &p->suspendedState;
    if (*q == 4) {
        u32 z = 0;
        *q = 5;
        p->suspendedTimer = z;
    }
}

u32 Nidhoggr_IsSuspendedObjectReady(struct Nidhoggr *p)
{
    u32 r = 0;
    if (p->suspendedState == 4) {
        r = 1;
    }
    return r;
}

u32 Nidhoggr_IsSuspendedObjectHidden(struct Nidhoggr *p)
{
    u32 r = 0;
    if (p->suspendedState == 0) {
        r = 1;
    }
    return r;
}

/* Tentative: the suspended object's per-frame state machine (states advanced by the
 * Nidhoggr_Start.../Is...SuspendedObject helpers). A 9-case switch over t->suspendedState
 * (0x71BC) manipulating t->suspendedSprite's flags and a "jitter" pair of s16 fields at
 * t->suspendedBasePosX/+2 (0x7158/0x715A), the collider at t->suspendedCollider, and
 * t->suspendedTimer (0x71C0); case 6 adds per-tick offsets from a small table
 * (gUnk_0824F504) keyed by suspendedTimer & 7; cases 1 and 8 call
 * Nidhoggr_CreateThreePointBurst(gUnk_0824F4FC). A full decode of the logic was written
 * (see git history/notes) but only reached 48.5% fuzzy -- the compiler keeps the task
 * pointer directly in a register and indexes the jitter pair through it rather than via
 * a separate pointer variable, which this draft did not reproduce. confidence=low. */

struct SuspendedJitter {
    s16 x, y, z;
};

struct SuspendedOffset {
    s16 x;
    s16 pad;
    s16 z;
    s16 pad2;
};

extern const u8 gUnk_0824F4FC[];
extern const struct SuspendedOffset gUnk_0824F504[];
void Nidhoggr_CreateThreePointBurst(const void *);

void Nidhoggr_UpdateSuspendedObject(struct Nidhoggr *t)
{
    u8 *sp;
    switch (t->suspendedState) {
        case 0: {
            u8 *sp = t->suspendedSprite;
            *(u32 *)sp |= 1;
        } break;
        case 1:
            sp = t->suspendedSprite;
            *(u32 *)sp &= ~1;
            *(u16 *)(sp + 0x10) = 1;
            t->suspendedPosX = 0xdc0;
            t->suspendedPosY = 0xfcc0;
            t->suspendedPosZ = 0xa80;
            t->suspendedState++;
            t->suspendedTimer = 0x14;
            Nidhoggr_CreateThreePointBurst(gUnk_0824F4FC);
            break;
        case 2:
            t->suspendedPosY += 0x33;
            if (t->suspendedPosY > -0x140)
                t->suspendedPosY = 0xfec0;
            if ((s16)--t->suspendedTimer <= 0)
                t->suspendedState++;
            break;
        case 3: {
            u8 *sp2 = t->suspendedSprite;
            *(u16 *)(sp2 + 0x10) = 0;
        }
            Collider_Refresh(t->suspendedCollider);
            Collider_SetPosition(t->suspendedCollider, &t->suspendedPosX, 0);
            t->suspendedState++;
            Sound_PlaySongChecked(0x1be);
            break;
        case 4:
            break;
        case 5:
            t->suspendedTimer = 0;
            t->suspendedState++;
            break;
        case 6: {
            u16 *tm = &t->suspendedTimer;
            u32 i = *tm & 7;
            const struct SuspendedOffset *tab = gUnk_0824F504;
            const struct SuspendedOffset *e;
            e = tab + i;
            t->suspendedPosX = 0xdc0 + e->x;
            t->suspendedPosZ = 0xa80 + e->z;
            (*tm)++;
            break;
        }
        case 7: {
            u8 *sp3 = t->suspendedSprite;
            *(u16 *)(sp3 + 0x10) = 1;
        }
            t->suspendedState++;
            break;
        case 8:
            t->suspendedPosY -= 0x33;
            if (t->suspendedPosY < -0x340) {
                {
                    u8 *sp = t->suspendedSprite;
                    *(u32 *)sp |= 1;
                }
                t->suspendedPosY = 0xfcc0;
                t->suspendedState = 0;
                Nidhoggr_CreateThreePointBurst(gUnk_0824F4FC);
            }
            break;
    }
}

/* Companion to sub_0819494C (part index 1): on bgMode, calls Nidhoggr_ChangeCompletedPartState for
 * part 0 or 1, copies two 8-byte blobs from gUnk_0824F544/4C or gUnk_0824F554/5C into collider and
 * altCollider fields +0x14/+0x1c (unknown collider sub-fields), then unconditionally sets or clears
 * a flag bit on colliders[0]/altColliders[0]/partExtra[0] based on `active`. First attempt (literal
 * translation of the disassembly) was far off (33% fuzzy) -- the shared-tail code the two branches
 * jump into, and the collider's internal field layout, need more investigation than fits this pass. */

struct PartExtraView {
    u32 pad, flags;
};
struct CopyPair {
    u32 a, b;
};
struct PartColliderView {
    u8 pad[6];
    u16 flags;
    u8 pad8[12];
    struct CopyPair first, second;
};
extern const struct CopyPair gUnk_0824F544, gUnk_0824F54C, gUnk_0824F554, gUnk_0824F55C;
static inline void SetExtra(struct PartExtraView *p, u32 mask)
{
    p->flags |= mask;
}
// confidence=low: restores first-part collider ranges after the completion state.
#define Nidhoggr_RestoreFirstPartRanges sub_08194858
void Nidhoggr_RestoreFirstPartRanges(struct Nidhoggr *p)
{
    struct PartColliderView *c;
    register const struct CopyPair *a asm("r4");
    const struct CopyPair *b;
    if (p->bgMode) {
        if (Nidhoggr_ChangeCompletedPartState(p, 0, 0x37)) {
            c = (void *)((u8 *)p + 0x672C);
            a = &gUnk_0824F54C;
            c->second = *a;
            b = &gUnk_0824F544;
            c->first = *b;
            c = (void *)((u8 *)p + 0x5EA4);
            goto copy;
        }
    } else {
        if (Nidhoggr_ChangeCompletedPartState(p, 0, 0x12)) {
            c = (void *)((u8 *)p + 0x66D8);
            a = &gUnk_0824F55C;
            c->second = *a;
            b = &gUnk_0824F554;
            c->first = *b;
            c = (void *)((u8 *)p + 0x5E50);
        copy:
            c->second = *a;
            c->first = *b;
        }
    }
    if (p->active) {
        ((struct PartColliderView *)((u8 *)p + 0x66D8))->flags |= 4;
        ((struct PartColliderView *)((u8 *)p + 0x5E50))->flags |= 4;
        SetExtra((void *)((u8 *)p + 0x6F60), 1);
    } else {
        struct PartColliderView *first = (void *)((u8 *)p + 0x66D8);
        s32 mask = ~4;
        first->flags &= mask;
        ((struct PartColliderView *)((u8 *)p + 0x5E50))->flags &= mask;
    }
}

/* Companion to sub_08194858 (part index 0); same shape, operating on parts 1/0 and
 * colliders[0]/[1] respectively. See sub_08194858's note. */

extern const struct CopyPair gUnk_0824F564, gUnk_0824F56C, gUnk_0824F574, gUnk_0824F57C;
// confidence=low: restores second-part collider ranges after the completion state.
#define Nidhoggr_RestoreSecondPartRanges sub_0819494C
void Nidhoggr_RestoreSecondPartRanges(struct Nidhoggr *p)
{
    struct PartColliderView *c;
    register const struct CopyPair *a asm("r4");
    const struct CopyPair *b;
    if (p->bgMode) {
        if (Nidhoggr_ChangeCompletedPartState(p, 1, 0x35)) {
            c = (void *)((u8 *)p + 0x66D8);
            a = &gUnk_0824F56C;
            c->second = *a;
            b = &gUnk_0824F564;
            c->first = *b;
            c = (void *)((u8 *)p + 0x5E50);
            goto copy;
        }
    } else {
        if (Nidhoggr_ChangeCompletedPartState(p, 1, 0x14)) {
            c = (void *)((u8 *)p + 0x672C);
            a = &gUnk_0824F57C;
            c->second = *a;
            b = &gUnk_0824F574;
            c->first = *b;
            c = (void *)((u8 *)p + 0x5EA4);
        copy:
            c->second = *a;
            c->first = *b;
        }
    }
    if (p->active) {
        ((struct PartColliderView *)((u8 *)p + 0x672C))->flags |= 4;
        ((struct PartColliderView *)((u8 *)p + 0x5EA4))->flags |= 4;
        SetExtra((void *)((u8 *)p + 0x6F70), 1);
    } else {
        struct PartColliderView *first = (void *)((u8 *)p + 0x672C);
        s32 mask = ~4;
        first->flags &= mask;
        ((struct PartColliderView *)((u8 *)p + 0x5EA4))->flags &= mask;
    }
}

struct H {
    u8 f[6];
    u16 v;
};

struct W {
    u32 a;
    u32 v;
};

static inline void orr(struct W *a, u32 m)
{
    a->v |= m;
}

void Nidhoggr_StartFirstPartAction(struct Nidhoggr *p)
{
    if (p->bgMode != 0)
        Nidhoggr_ChangeCompletedPartState(p, 0, 0x38);
    else
        Nidhoggr_ChangeCompletedPartState(p, 0, 0x13);
    /* 0x66DE and 0x5E56: unidentified flag bits near colliders[0] and the anim-row gap */
    ((struct H *)p)[3291].v |= 4;
    ((struct H *)p)[3018].v |= 4;
    orr(&((struct W *)p)[3564], 1);
}

struct H2 {
    u8 f[6];
    u16 v;
    u8 g[4];
};

void Nidhoggr_StartSecondPartAction(struct Nidhoggr *p)
{
    if (p->bgMode != 0)
        Nidhoggr_ChangeCompletedPartState(p, 1, 0x36);
    else
        Nidhoggr_ChangeCompletedPartState(p, 1, 0x15);
    /* unidentified flag bits, near colliders[] and the anim-row gap (see StartFirstPartAction) */
    ((struct H2 *)p)[2201].v |= 4;
    ((struct H2 *)p)[2019].v |= 4;
    orr(&((struct W *)p)[3566], 1);
}

void Nidhoggr_ResetPart22Animation(u32 a)
{
    Nidhoggr_ChangeCompletedPartState(a, 0x16, 0);
}

void Nidhoggr_ResetPart23Animation(u32 a)
{
    Nidhoggr_ChangeCompletedPartState(a, 0x17, 0);
}

extern u32 gUnk_03005304;

extern const u16 gUnk_086141BC[];

void Nidhoggr_SelectRandomPart22Animation(struct Nidhoggr *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = &p->flags[7];
    *c = (t & 3) + (1 + *c);
    if (*c > 5)
        *c = 0;
    if (p->bgMode != 0)
        Nidhoggr_ChangeCompletedPartState(p, 0x16, *c + 0x3f);
    else
        Nidhoggr_ChangeCompletedPartState(p, 0x16, *c + 0x16);
}

void Nidhoggr_SelectRandomPart23Animation(struct Nidhoggr *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = &p->flags[8];
    *c = (t & 3) + (1 + *c);
    if (*c > 5)
        *c = 0;
    if (p->bgMode != 0)
        Nidhoggr_ChangeCompletedPartState(p, 0x17, *c + 0x39);
    else
        Nidhoggr_ChangeCompletedPartState(p, 0x17, *c + 0x1c);
}

void Nidhoggr_SelectRandomPart22AlternateAnimation(struct Nidhoggr *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = &p->flags[7];
    *c = (t & 3) + (1 + *c);
    if (*c > 5)
        *c = 0;
    Nidhoggr_ChangeCompletedPartState(p, 0x16, *c + 0x22);
}

void Nidhoggr_SelectRandomPart23AlternateAnimation(struct Nidhoggr *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = &p->flags[8];
    *c = (t & 3) + (1 + *c);
    if (*c > 5)
        *c = 0;
    Nidhoggr_ChangeCompletedPartState(p, 0x17, *c + 0x2a);
}

void Nidhoggr_SetPart23SpecialAnimation(u32 a)
{
    Nidhoggr_ChangeCompletedPartState(a, 0x17, 0x32);
}

struct Vec08194CB8 {
    s16 x, y, z, w;
};

void Sound_PlaySongChecked(u32);

static inline s32 Random2(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}

void Nidhoggr_PlayNearbyRandomSound(struct Vec08194CB8 *pos)
{
    struct Vec08194CB8 d;
    d.x = pos->x - gUnk_02000710->positionX;
    d.z = pos->z - gUnk_02000710->positionZ;
    if (d.x < 0)
        d.x = -d.x;
    if (d.z < 0)
        d.z = -d.z;
    if (d.x <= 0x900 && d.z <= 0xc00) {
        if (Random2() > 0x7fff)
            Sound_PlaySongChecked(0x1c0);
        else
            Sound_PlaySongChecked(0x1bf);
    }
}

/* Tentative: called once per frame from Nidhoggr_Update, drives a death-flash effect while
 * t->flags[1] (0x724D) counts down (Blend_SetAlpha from a fade table gUnk_0824F588, toggling
 * bg blend bits in gUnk_03004BD8, and refreshing t->altColliders[0] at 0x5DFC), otherwise
 * sets bits in t->flags[2] (0x724E); also calls sub_08194858/sub_0819494C (part completion)
 * further down. 564 bytes; not attempted byte-for-byte this pass. confidence=low. */

extern const u8 gUnk_0824F584[];
extern const u8 gUnk_0824F588[];

void Nidhoggr_UpdateDamageFlashAndPartAnimations(struct Nidhoggr *t)
{
    if (t->flags[1]) {
        s32 n = 0x1e - t->flags[1];
        if (n <= 0xf) {
            Or16(0x200, &gUnk_03004BD8);
            Blend_SetAlpha(1, 0x10, gUnk_0824F588[n]);
            Collider_Refresh(&t->altColliders[0]);
        } else {
            t->flags[2] |= 0x10;
            {
                u16 *g = &gUnk_03004BD8;
                u32 m = ~0x200;
                *g = *g & m;
            }
        }
        t->flags[2] |= 3;
        t->flags[1]--;
    } else {
        {
            u16 *g = &gUnk_03004BD8;
            u32 m = ~0x200;
            *g = *g & m;
        }
    }
    if (t->flags[2] & 4) {
        t->flags[2] &= ~1;
        Nidhoggr_StartFirstPartAction(t);
    } else {
        sub_08194858(t);
    }
    if (t->flags[2] & 8) {
        t->flags[2] &= ~2;
        Nidhoggr_StartSecondPartAction(t);
    } else {
        sub_0819494C(t);
    }
    if (t->flags[2] & 0x10) {
        Nidhoggr_SetPart23SpecialAnimation((u32)t);
    } else if (t->flags[2] & 1) {
        Nidhoggr_SelectRandomPart22AlternateAnimation(t);
        t->flags[2] &= ~1;
    } else {
        switch (t->flags[3]) {
            case 0:
                Nidhoggr_ResetPart22Animation((u32)t);
                break;
            case 1:
            case 2:
            case 3:
                if (t->flags[5]) {
                    Nidhoggr_ResetPart22Animation((u32)t);
                    t->flags[5]--;
                } else {
                    Nidhoggr_SelectRandomPart22Animation(t);
                    Nidhoggr_PlayNearbyRandomSound((struct Vec08194CB8 *)((u8 *)t + 0x66FC));
                    t->flags[5] = gUnk_0824F584[t->flags[3]];
                }
                break;
            case 4:
                Nidhoggr_ResetPart22Animation((u32)t);
                break;
        }
    }
    if (t->flags[2] & 0x10) {
        t->flags[2] &= ~0x10;
        goto reset23;
    } else if (t->flags[2] & 2) {
        Nidhoggr_SelectRandomPart23AlternateAnimation(t);
        t->flags[2] &= ~2;
    } else {
        switch (t->flags[4]) {
            case 0:
            reset23:
                Nidhoggr_ResetPart23Animation((u32)t);
                break;
            case 1:
            case 2:
            case 3:
                if (t->flags[6]) {
                    Nidhoggr_ResetPart23Animation((u32)t);
                    t->flags[6]--;
                } else {
                    Nidhoggr_SelectRandomPart23Animation(t);
                    Nidhoggr_PlayNearbyRandomSound((struct Vec08194CB8 *)((u8 *)t + 0x6750));
                    t->flags[6] = gUnk_0824F584[t->flags[4]];
                }
                break;
            case 4:
                Nidhoggr_ResetPart23Animation((u32)t);
                break;
        }
    }
}

void Nidhoggr_InitBobbingMotion(struct Nidhoggr *p, s32 x, s32 y)
{
    p->posOffsetX = (p->pos.x - x) << 8;
    p->swayX = 0;
    p->posOffsetZ = (p->pos.z - y) << 8;
    p->posOffsetXVelocity = 0x30;
    p->swayXVelocity = 0x88;
    p->posOffsetZVelocity = 0x400;
}

void Nidhoggr_InitRelativeMotion(struct Nidhoggr *p, s32 x, s32 y)
{
    p->posOffsetX = (p->pos.x - x) << 8;
    p->swayX = 0;
    p->posOffsetZ = (p->pos.z - y) << 8;
    p->posOffsetXVelocity = 0;
    p->posOffsetZVelocity = -0x400;
}

void Nidhoggr_UpdateAnchoredSway(struct Nidhoggr *t, s32 cx, s32 cz)
{
    s32 x, z;
    {
        s32 *pos = &t->swayX;
        s32 *vel = &t->swayXVelocity;
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x1000) {
                *pos = -0x1000;
                *vel = 0x88;
            }
        } else if (v > 0x1000) {
            *pos = 0x1000;
            *vel = -0x88;
        }
    }
    {
        s32 *pos = &t->posOffsetZ;
        s32 *vel = &t->posOffsetZVelocity;
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x18000)
                *vel = 0x400;
        } else if (v > 0x18000)
            *vel = -0x400;
    }
    x = t->posOffsetX + t->swayX;
    z = t->posOffsetZ;
    if (x > 0)
        x = 0;
    {
        s16 v = cx + (x >> 8);
        t->pos.x = v;
        v = cz + (z >> 8);
        t->pos.z = v;
    }
}

void Nidhoggr_UpdateHorizontalSway(struct Nidhoggr *p)
{
    s32 *a = &p->swayX;
    s32 *b = &p->swayXVelocity;
    s32 v = *a + *b;
    *a = v;
    if (*b < 0) {
        if (v < -0x1000) {
            *a = -0x1000;
            *b = 0x88;
        }
    } else {
        if (v > 0x1000) {
            *a = 0x1000;
            *b = -0x88;
        }
    }
}

void Nidhoggr_UpdateWideSway(struct Nidhoggr *t)
{
    s32 x, z;
    {
        s32 *pos = &t->swayX;
        s32 *vel = &t->swayXVelocity;
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x8000) {
                *pos = -0x8000;
                *vel = 0x88;
            }
        } else if (v > 0x8000) {
            *pos = 0x8000;
            *vel = -0x88;
        }
    }
    {
        s32 *pos = &t->posOffsetZ;
        s32 *vel = &t->posOffsetZVelocity;
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x5000)
                *vel = 0x400;
        } else if (v > 0x5000)
            *vel = -0x400;
    }
    x = t->posOffsetX + t->swayX;
    z = t->posOffsetZ;
    {
        s16 v = (x >> 8) + *(u16 *)&t->basePos.x;
        t->pos.x = v;
        v = (z >> 8) + *(u16 *)&t->basePos.z;
        t->pos.z = v;
    }
}

extern const struct SceneVec gUnk_0824F598, gUnk_0824F5A8, gUnk_0824F5B0, gUnk_0824F5B8;

void Nidhoggr_TrackPlayerDepth(struct Nidhoggr *t)
{
    s32 d = gUnk_02000710->positionZ - (t->basePos.z - gUnk_0824F598.z);
    s32 target;
    s16 *speed;
    if (d > 0x200)
        target = 40;
    else if (d > 0x40)
        target = 12;
    else if (d > 0)
        target = 4;
    else if (d == 0)
        target = 0;
    else if (d > -0x40)
        target = -4;
    else if (d > -0x200)
        target = -12;
    else
        target = -40;
    speed = &t->speed;
    d = target - *speed;
    if (d > 16)
        *speed += 4;
    else if (d > 4)
        *speed += 2;
    else if (d > 0)
        *speed += 1;
    else if (d != 0) {
        if (d > -4)
            *speed -= 1;
        else if (d > -16)
            *speed -= 2;
        else
            *speed -= 4;
    }
    t->basePos.z += t->speed;
}

/* Tentative: called once per frame from Nidhoggr_UpdateState, right after the behavior
 * dispatch table. Advances the per-part defeat sequences (dead[] latches 1..9): picks three
 * row groups from the current mode, and for each stage waits for the row's animation to
 * finish (rowDoneFlag/rowCounter) before requesting the next completed-part state.
 * confidence=low. */
void Nidhoggr_UpdatePartSequences(struct Nidhoggr *t)
{
    s32 a = 0, b = 0, c = 0;
    s32 i, p;
    switch (t->mode) {
        case 2:
        case 3:
        case 8:
            a = 6;
            b = 14;
            c = 0x13;
            break;
        case 4:
        case 5:
        case 9:
            a = 2;
            b = 10;
            c = 0x12;
            break;
    }
    p = 0;
    for (i = 0; i < 4; i++, p++) {
        switch (t->dead[p]) {
            case 0:
                break;
            case 1:
                Nidhoggr_ChangeCompletedPartState(t, (a + i), 1);
                t->dead[p]++;
                break;
            case 2:
                if (t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 3);
                    t->dead[p]++;
                }
                break;
            case 4:
            case 6:
                if (t->rowCounter[(a + i)] > 0x27 && t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 3);
                    t->dead[p]++;
                }
                break;
            case 3:
            case 7:
                if (t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 2);
                    t->dead[p]++;
                }
                break;
            case 5:
                if (t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 2);
                    if (t->mode == 4)
                        t->dead[p]++;
                    else
                        t->dead[p] = 8;
                }
                break;
            case 8:
                if (t->rowCounter[(a + i)] > 0x27 && t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 4);
                    t->dead[p]++;
                }
                break;
            case 9:
                if (t->rowDoneFlag[(a + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (a + i), 0);
                    t->dead[p] = 0;
                }
                break;
        }
    }
    for (i = 0; i < 4; i++, p++) {
        switch (t->dead[p]) {
            case 0:
                break;
            case 1:
                Nidhoggr_ChangeCompletedPartState(t, (b + i), 6);
                t->dead[p]++;
                break;
            case 2:
                if (t->rowCounter[(b + i)] > 0x31 && t->rowDoneFlag[(b + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (b + i), 8);
                    t->dead[p]++;
                }
                break;
            case 3:
                if (t->rowDoneFlag[(b + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (b + i), 7);
                    t->dead[p]++;
                }
                break;
            case 4:
                if (t->rowCounter[(b + i)] > 0x31 && t->rowDoneFlag[(b + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (b + i), 9);
                    t->dead[p]++;
                }
                break;
            case 5:
                if (t->rowDoneFlag[(b + i)] != 0) {
                    Nidhoggr_ChangeCompletedPartState(t, (b + i), 0);
                    t->dead[p] = 0;
                }
                break;
        }
    }
    switch (t->dead[p]) {
        case 0:
            break;
        case 1:
            Nidhoggr_ChangeCompletedPartState(t, c, 0xb);
            t->dead[p]++;
            break;
        case 2:
            if (t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0xd);
                t->dead[p]++;
            }
            break;
        case 4:
        case 6:
            if (t->rowCounter[c] > 0x13 && t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0xd);
                t->dead[p]++;
            }
            break;
        case 3:
        case 7:
            if (t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0xc);
                t->dead[p]++;
            }
            break;
        case 5:
            if (t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0xc);
                if (t->mode == 4)
                    t->dead[p]++;
                else
                    t->dead[p] = 8;
            }
            break;
        case 8:
            if (t->rowCounter[c] > 0x13 && t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0xe);
                t->dead[p]++;
            }
            break;
        case 9:
            if (t->rowDoneFlag[c] != 0) {
                Nidhoggr_ChangeCompletedPartState(t, c, 0);
                t->dead[p] = 0;
            }
            break;
    }
}

s32 sub_08249350(s32, s32);

void Nidhoggr_ShufflePartOrder(struct Nidhoggr *p)
{
    const u16 *tbl;
    s32 i;
    u8 *q;
    u8 *a;
    p->shuffleCursor = 0;
    tbl = gUnk_086141BC;
    i = 8;
    q = &p->shuffleOrder[8];
    do {
        *q = i;
        q--;
        i--;
    } while (i >= 0);
    i = 0;
    do {
        u8 t, *e;
        s32 r;
        gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
        r = sub_08249350(tbl[gUnk_03005304], 9);
        a = p->shuffleOrder;
        e = a + i;
        t = *e;
        *e = a[r];
        a[r] = t;
        i++;
    } while (i <= 8);
}

s32 Nidhoggr_GetNextShuffledPart(struct Nidhoggr *p)
{
    u8 *q = &p->shuffleCursor;
    u8 *t;
    u8 v;
    u8 r;
    if (*q > 8) {
        Nidhoggr_ShufflePartOrder(p);
    }
    v = *q;
    t = p->shuffleOrder;
    r = t[v];
    *q = v + 1;
    return r;
}

void Nidhoggr_ActivateRandomPart(struct Nidhoggr *t)
{
    s32 tries;
    s32 i;
    if (t->timer <= 0x6d)
        return;
    t->timer = 0;
    tries = 9;
    for (;;) {
        i = Nidhoggr_GetNextShuffledPart(t);
        if (t->hp[i] > 0) {
            if (i <= 3) {
                if (!t->dead[i] && !t->dead[i + 4])
                    goto found;
            } else if (i <= 7) {
                if (!t->dead[i] && !t->dead[i - 4])
                    goto found;
            } else if (!t->dead[i])
                goto found;
        }
        if (--tries <= 0)
            return;
    }
found:
    t->dead[i] = 1;
}

/* Tentative: the second-phase counterpart of Nidhoggr_ActivateRandomPart (activates up to two
 * parts per call instead of one, and uses `timer` instead of `timer2`). confidence=low. */

void Nidhoggr_ActivateRandomParts2(struct Nidhoggr *t)
{
    s32 tries, limit;
    if (t->timer <= 0x59)
        return;
    t->timer = 0;
    tries = 9;
    limit = 2;
    do {
        s32 i = Nidhoggr_GetNextShuffledPart(t);
        if (t->hp[i] > 0) {
            if (i <= 3) {
                if (!t->dead[i] && !t->dead[i + 4]) {
                    t->dead[i] = 1;
                    if (--limit <= 0)
                        return;
                }
            } else if (i <= 7) {
                if (!t->dead[i] && !t->dead[i - 4]) {
                    t->dead[i] = 1;
                    if (--limit <= 0)
                        return;
                }
            } else if (!t->dead[i]) {
                t->dead[i] = 1;
                if (--limit <= 0)
                    return;
            }
        }
    } while (--tries > 0);
}

void Nidhoggr_InitFirstPhaseParts(struct Nidhoggr *t)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        t->hp[i] = t->maxHp;
        t->dead[i] = 0;
    }
    for (; i <= 7; i++) {
        t->hp[i] = 0;
        t->dead[i] = 0;
    }
    t->hp[i] = t->maxHp;
    t->dead[i] = 0;
}

void Nidhoggr_ResetAllPartHealth(struct Nidhoggr *p)
{
    s32 i = 0;
    u8 *a = (u8 *)&p->maxHp;
    u32 z = 0;
    u8 *b = p->dead;
    u16 *d = (u16 *)p->hp;
    do {
        *d = *(u16 *)a;
        b[i] = z;
        d++;
        i++;
    } while (i <= 8);
}

void Nidhoggr_ClearTargetValues(struct Nidhoggr *p)
{
    p->cameraTarget.x = 0;
    p->cameraTarget.y = 0;
    p->cameraTarget.z = 0;
}

void Nidhoggr_SetConfiguredTargetValues(struct Nidhoggr *p)
{
    p->cameraTarget.x = p->targetSourceX;
    p->cameraTarget.y = p->targetSourceY;
    p->cameraTarget.z = p->targetSourceZ;
}

void Nidhoggr_SetBehavior(struct Nidhoggr *p, s32 v)
{
    p->mode = v;
    p->state = 0;
}

void Nidhoggr_InitWaitingPose(struct Nidhoggr *p)
{
    u8 *q = &p->state;
    u32 z = *q;
    if (z == 0) {
        Nidhoggr_SetBackgroundMode(p, 0);
        p->pos.x = 0x260;
        p->pos.y = z;
        p->pos.z = 0x500;
        (*q)++;
    }
}

void Nidhoggr_UpdateEntrance(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_SetConfiguredTargetValues(t);
            t->pos.x = 0x260;
            t->pos.y = 0;
            t->pos.z = 0x500;
            Nidhoggr_InitRelativeMotion(t, 0x200, 0xf80);
            t->posOffsetXVelocity = -0x300;
            t->state++;
            Sound_PlaySongChecked(0x404);
            break;
        case 1: {
            s32 z;
            t->posOffsetX += t->posOffsetXVelocity;
            Nidhoggr_UpdateHorizontalSway(t);
            z = t->posOffsetZ += 0x2000;
            t->pos.x = ((t->posOffsetX + t->swayX) >> 8) + 0x200;
            t->pos.z = (z >> 8) + 0xf80;
            if (t->posOffsetZ >= 0) {
                t->state++;
                t->posOffsetXVelocity = -0xc00;
                t->swayXVelocity = 0x88;
                t->posOffsetZVelocity = 0x400;
            }
            break;
        }
        case 2:
            t->posOffsetX += t->posOffsetXVelocity;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->posOffsetX < -0x20000) {
                Nidhoggr_InitFirstPhaseParts(t);
                Nidhoggr_SetBehavior(t, 2);
                t->active = 0;
            }
            break;
    }
}

void BossAttack_ActivateCollider(void *);

void Nidhoggr_CreateLowerSweepBurst(void *);

void Nidhoggr_UpdateFirstPartPhase(struct Nidhoggr *t)
{
    s32 allDead, i;
    s16 *hp;
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_InitBobbingMotion(t, 0x200, 0xf80);
            if (t->posOffsetX < -0x20000)
                t->state = 1;
            else {
                t->state = 2;
                t->timer2 = 0;
            }
            break;
        case 1:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX > -0x20000) {
                t->state = 2;
                t->timer2 = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > 0x800)
                t->posOffsetXVelocity = 0x800;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            break;
        case 2:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 3:
            Nidhoggr_ActivateRandomPart(t);
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < -0x20000) {
                t->posOffsetX = -0x20000;
                t->posOffsetXVelocity = 0;
            } else if (t->posOffsetX > 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > 0x30)
                t->posOffsetXVelocity = 0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->pos.x == 0x200) {
                t->state = 4;
                t->posOffsetXVelocity = -0x600;
                BossAttack_ActivateCollider(t);
                Nidhoggr_CreateLowerSweepBurst(&t->pos.x);
            }
            break;
        case 4:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 5:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < -0x20000) {
                t->posOffsetX = -0x20000;
                t->posOffsetXVelocity = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > -0x30)
                t->posOffsetXVelocity = -0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->pos.x < 0) {
                t->state = 2;
                t->posOffsetXVelocity = 0x30;
            }
            break;
    }
    t->timer2++;
    allDead = 1;
    hp = t->hp;
    for (i = 8; i >= 0; hp++, i--) {
        if (*hp > 0)
            allDead = 0;
    }
    if (allDead)
        Nidhoggr_SetBehavior(t, 3);
}

void Nidhoggr_UpdateSecondPhaseTransition(struct Nidhoggr *t)
{
    u8 *state = &t->state;
    switch (*state) {
        case 0: {
            s32 *vel = &t->posOffsetXVelocity;
            s32 saved = *vel;
            Nidhoggr_InitRelativeMotion(t, 0x200, 0x980);
            *vel = saved;
            *state = 1;
            Sound_PlaySongChecked(0x408);
        }
        case 1: {
            s32 *pos, *vel;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            pos = &t->posOffsetX;
            vel = &t->posOffsetXVelocity;
            *pos += *vel;
            if (*pos < -0x20000) {
                *pos = -0x20000;
                *vel = 0;
            }
            if (t->posOffsetZVelocity > 0) {
                Nidhoggr_SetBehavior(t, 4);
                Nidhoggr_ResetAllPartHealth(t);
            }
            break;
        }
    }
}

void Nidhoggr_CreateUpperSweepBurst(void *);

void Nidhoggr_UpdateSecondPartPhase(struct Nidhoggr *t)
{
    s32 allDead, i;
    s16 *hp;
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_InitBobbingMotion(t, 0x200, 0x980);
            if (t->posOffsetX < -0x20000) {
                t->state = 1;
                Sound_PlaySongChecked(0x403);
            } else {
                t->state = 2;
                t->timer2 = 0;
            }
            break;
        case 1:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX > -0x20000) {
                t->state = 2;
                t->timer2 = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > 0x800)
                t->posOffsetXVelocity = 0x800;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            break;
        case 2:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 3:
            Nidhoggr_ActivateRandomParts2(t);
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < -0x20000) {
                t->posOffsetX = -0x20000;
                t->posOffsetXVelocity = 0;
            } else if (t->posOffsetX > 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > 0x30)
                t->posOffsetXVelocity = 0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            if (t->pos.x == 0x200) {
                t->state = 4;
                t->posOffsetXVelocity = -0x600;
                BossAttack_ActivateCollider(t);
                Nidhoggr_CreateUpperSweepBurst(&t->pos.x);
            }
            break;
        case 4:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 5:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < -0x20000) {
                t->posOffsetX = -0x20000;
                t->posOffsetXVelocity = 0;
            }
            t->posOffsetXVelocity += 6;
            if (t->posOffsetXVelocity > -0x30)
                t->posOffsetXVelocity = -0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            if (t->pos.x < 0) {
                t->state = 2;
                t->posOffsetXVelocity = 0x30;
            }
            break;
    }
    t->timer2++;
    allDead = 1;
    hp = t->hp;
    for (i = 8; i >= 0; hp++, i--) {
        if (*hp > 0)
            allDead = 0;
    }
    if (allDead)
        Nidhoggr_SetBehavior(t, 5);
}

void Nidhoggr_UpdateSlowReturn(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_InitRelativeMotion(t, 0x1e0, 0x500);
            if (t->posOffsetX < 0)
                t->posOffsetXVelocity = 0x90;
            else if (t->posOffsetX > 0)
                t->posOffsetXVelocity = 0x90;
            else
                t->posOffsetXVelocity = 0;
            t->state = 1;
        case 1:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX > 0 && t->posOffsetXVelocity > 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            } else if (t->posOffsetX < 0 && t->posOffsetXVelocity < 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            }
            Nidhoggr_UpdateAnchoredSway(t, 0x1e0, 0x500);
            if (t->posOffsetX == 0 && t->pos.x == 0x1e0) {
                t->pos.x = 0x1e0;
                t->swayX = 0;
                t->swayXVelocity = 0;
                if (t->pos.x == 0x1e0 && t->pos.z > 0x4fb && t->pos.z <= 0x504) {
                    t->pos.z = 0x500;
                    Nidhoggr_SetBehavior(t, 6);
                }
            }
            break;
    }
}

void Nidhoggr_UpdateFastReturn(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_InitRelativeMotion(t, 0x1e0, 0x500);
            if (t->posOffsetX < 0)
                t->posOffsetXVelocity = 0x800;
            else if (t->posOffsetX > 0)
                t->posOffsetXVelocity = 0x90;
            else
                t->posOffsetXVelocity = 0;
            t->state = 1;
            Sound_PlaySongChecked(0x40b);
        case 1:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX > 0 && t->posOffsetXVelocity > 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            } else if (t->posOffsetX < 0 && t->posOffsetXVelocity < 0) {
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            }
            Nidhoggr_UpdateAnchoredSway(t, 0x1e0, 0x500);
            if (t->posOffsetX == 0 && t->pos.x == 0x1e0) {
                t->pos.x = 0x1e0;
                t->swayX = 0;
                t->swayXVelocity = 0;
                if (t->pos.x == 0x1e0 && t->pos.z > 0x4fb && t->pos.z <= 0x504) {
                    t->pos.z = 0x500;
                    Nidhoggr_SetBehavior(t, 6);
                }
            }
            break;
    }
}

extern u32 gUnk_030053F4, gUnk_030053F0;

extern u32 gUnk_02000560;

void Camera_RequestShake(u32);

void SceneAnimation_InitTimedCollider(void *);

void Sound_StopSong(u32);

static inline u32 Tst2(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}

static inline u32 Tst32(u32 mask, u32 *p)
{
    return *p & mask;
}

static inline void Track(struct Nidhoggr *t)
{
    if (gUnk_02000710->positionX > 0xeff)
        Nidhoggr_SetConfiguredTargetValues(t);
    else
        Nidhoggr_ClearTargetValues(t);
}

void Nidhoggr_UpdateTimedAttackSequence(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_StartSuspendedObjectDescent(t);
            t->state++;
            break;
        case 1:
            if (Nidhoggr_IsSuspendedObjectReady(t)) {
                t->state++;
                t->timer = 0;
            }
            break;
        case 2:
            if (t->timer <= 9) {
                Camera_RequestShake(0x14);
                break;
            }
            Nidhoggr_RegisterTerrainCells(t);
            t->timer = 0;
            t->state++;
            t->attackActive = 1;
            break;
        case 3:
            Track(t);
            if (t->timer > 0x1a3) {
                Nidhoggr_StartSuspendedObjectShake(t);
                Sound_PlaySongChecked(0x1c1);
                t->state++;
            }
            break;
        case 4:
            Track(t);
            if (!(t->timer & 7))
                t->flags[2] |= 3;
            if (t->timer > 0x257) {
                t->attackActive = 0;
                if (gUnk_02000710->positionX > 0xeff)
                    Nidhoggr_UnregisterTerrainCells(t);
                else
                    Nidhoggr_SetTerrainCellFlag40(t);
                Nidhoggr_StartSuspendedObjectRetraction(t);
                Sound_StopSong(0x1c1);
                t->state++;
            }
            break;
        case 5:
            Track(t);
            t->flags[2] |= 3;
            if (Nidhoggr_IsSuspendedObjectHidden(t))
                t->state++;
            break;
        case 6:
            if (gUnk_02000710->positionX > 0xeff) {
                if ((t->groupHpA <= 0 && t->groupHpB <= 0) || t->life <= 0)
                    Nidhoggr_SetBehavior(t, 12);
                else
                    Nidhoggr_SetBehavior(t, 10);
            } else {
                SceneAnimation_InitTimedCollider(t);
                t->state++;
            }
            break;
        case 7:
            if (!t->flags[1]) {
                if (!Tst2(0x400, &gUnk_030053F4, &gUnk_030053F0) && !Tst32(4, &gUnk_02000560))
                    SceneAnimation_InitTimedCollider(t);
                else {
                    Nidhoggr_UnregisterTerrainCells(t);
                    t->state++;
                }
            }
            break;
        case 8:
            break;
    }
}

void Nidhoggr_UpdateRetreat(struct Nidhoggr *t)
{
    u8 *state = &t->state;
    switch (*state) {
        case 0:
            Nidhoggr_InitRelativeMotion(t, 0x200, 0x980);
            t->posOffsetXVelocity = -0x600;
            (*state)++;
        case 1: {
            s32 *pos = &t->posOffsetX;
            s32 *vel = &t->posOffsetXVelocity;
            *pos += *vel;
            if (*pos < -0x20000) {
                *pos = -0x20000;
                *vel = 0;
            }
            *vel += 6;
            if (*vel > -0x30)
                *vel = -0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            if (t->pos.x <= 0) {
                Nidhoggr_SetBehavior(t, 4);
                Nidhoggr_ResetAllPartHealth(t);
            }
            break;
        }
    }
}

extern const u8 gUnk_0824F5A0[];

void Nidhoggr_CreateDistanceLimitedEffect(void *, void *, const void *, void *);

void Nidhoggr_UpdateEffectAttack(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            t->basePos.x = -0xe80;
            t->basePos.y = t->pos.y;
            t->basePos.z = t->pos.z;
            Nidhoggr_InitRelativeMotion(t, t->basePos.x, t->basePos.z);
            t->posOffsetXVelocity = -0xe00;
            t->swayXVelocity = -0x88;
            t->state++;
            t->timer = 0;
            break;
        case 1:
            if (t->timer == 6)
                Sound_PlaySongChecked(0x409);
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < 0)
                t->state++;
            goto update;
        case 2:
            Nidhoggr_SetBackgroundMode(t, 1);
            t->basePos = gUnk_0824F598;
            t->basePos.z += gUnk_02000710->positionZ;
            t->speed = 0;
            t->pos = t->basePos;
            t->pos.x += -0x800;
            Nidhoggr_InitRelativeMotion(t, t->basePos.x, t->basePos.z);
            t->posOffsetXVelocity = 0xe00;
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 3:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX > 0) {
                t->state++;
                t->timer = 0;
                t->posOffsetX = 0;
                t->posOffsetXVelocity = 0;
            }
            goto update;
        case 4:
            if (t->timer > 0x28) {
                t->state++;
                t->timer = 0;
                Sound_PlaySongChecked(0x1b8);
            }
            goto update;
        case 5:
            if (!(t->timer & 1))
                Nidhoggr_CreateDistanceLimitedEffect(t->effectPool, &t->pos, gUnk_0824F5A0, t);
            Nidhoggr_TrackPlayerDepth(t);
            Nidhoggr_UpdateWideSway(t);
            if (t->timer > 0x78) {
                t->state++;
                Sound_StopSong(0x1b8);
                Sound_PlaySongChecked(0x40a);
            }
            break;
        case 6:
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < -0x80000)
                t->state++;
            t->posOffsetXVelocity -= 6;
            if (t->posOffsetXVelocity < -0xe00)
                t->posOffsetXVelocity = -0xe00;
        update:
            Nidhoggr_UpdateWideSway(t);
            break;
        case 7:
            Nidhoggr_SetBackgroundMode(t, 0);
            switch (t->mode) {
                case 8:
                    t->pos = gUnk_0824F5A8;
                    Nidhoggr_SetBehavior(t, 2);
                    break;
                case 9:
                    t->pos = gUnk_0824F5B0;
                    Nidhoggr_SetBehavior(t, 4);
                    break;
                case 10:
                    t->pos = gUnk_0824F5B8;
                    Nidhoggr_SetBehavior(t, 0xb);
                    break;
            }
            t->posOffsetXVelocity = 0;
            break;
    }
}

/* Tentative: disable the part colliders and retreat left before selecting the
 * offscreen pose behavior. The alternate collider range begins at slot one. */

struct RetreatCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[0x54 - 8];
};

struct RetreatPartExtra {
    u32 unk_00;
    u32 flags;
    u8 unk_08[8];
};

void Nidhoggr_UpdateLeftwardRetreat(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0: {
            s32 i;
            u16 *altFlags;
            register s32 altOffset asm("r1");
            s32 stride, extraMask;
            register s32 colliderMask asm("r4");
            u32 flags, altValue;
            u16 x;
            struct SceneVec *base;
            t->active = 1;
            i = 0;
            stride = sizeof(struct RetreatCollider);
            colliderMask = 4;
            extraMask = 1;
            altOffset = (u8 *)&((struct RetreatCollider *)&t->altColliders[1])->flags - (u8 *)t;
            asm("" : "+r"(altOffset));
            altFlags = (u16 *)((u8 *)t + altOffset);
        disablePart:
            altValue = *altFlags;
            altValue |= colliderMask;
            *altFlags = altValue;
            flags = ((struct RetreatCollider *)((u8 *)t->colliders + i * stride))->flags;
            flags |= colliderMask;
            ((struct RetreatCollider *)((u8 *)t->colliders + i * stride))->flags = flags;
            ((struct RetreatPartExtra *)t->partExtra)[i].flags |= extraMask;
            altFlags = (u16 *)((u8 *)altFlags + stride);
            i++;
            if (i < 26)
                goto disablePart;
            x = t->pos.x - 0x800;
            base = &t->basePos;
            colliderMask = 0;
            base->x = x;
            t->basePos.y = t->pos.y;
            t->basePos.z = t->pos.z;
            Nidhoggr_InitRelativeMotion(t, t->basePos.x, t->basePos.z);
            t->posOffsetXVelocity = -0xe00;
            t->swayXVelocity = -0x88;
            t->state++;
            t->timer = colliderMask;
            break;
        }
        case 1:
            if (t->timer == 6)
                Sound_PlaySongChecked(0x409);
            t->posOffsetX += t->posOffsetXVelocity;
            if (t->posOffsetX < 0)
                t->state++;
            Nidhoggr_UpdateWideSway(t);
            break;
        case 2:
            Nidhoggr_SetBehavior(t, 0x11);
            break;
    }
}

void Nidhoggr_InitOffscreenPose(struct Nidhoggr *p)
{
    u8 *q = &p->state;
    u32 z = *q;
    if (z == 0) {
        p->pos.x = z;
        p->pos.y = z;
        p->pos.z = 0xF980;
        (*q)++;
    }
}

void Nidhoggr_UpdateDescendingExit(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            t->pos.x = 0;
            t->pos.y = 0;
            t->pos.z = -0x680;
            Nidhoggr_InitRelativeMotion(t, 0x200, 0x980);
            t->state++;
            Sound_PlaySongChecked(0x400);
            break;
        case 1: {
            s32 z;
            Nidhoggr_UpdateHorizontalSway(t);
            z = t->posOffsetZ += 0x2000;
            t->pos.x = ((t->posOffsetX + t->swayX) >> 8) + 0x200;
            t->pos.z = (z >> 8) + 0x980;
            if (t->posOffsetZ >= 0) {
                t->state++;
                t->posOffsetXVelocity = -0xc00;
                t->swayXVelocity = 0x88;
                t->posOffsetZVelocity = 0x400;
            }
            break;
        }
        case 2: {
            s32 z;
            Nidhoggr_UpdateHorizontalSway(t);
            t->posOffsetX += t->posOffsetXVelocity;
            z = t->posOffsetZ += 0x2000;
            t->pos.x = ((t->posOffsetX + t->swayX) >> 8) + 0x200;
            t->pos.z = (z >> 8) + 0x980;
            if (t->posOffsetX < -0x60000) {
                t->state++;
                Trigger_ConsumePending(&t->receiver, 1);
            }
            break;
        }
    }
}

void Nidhoggr_UpdateRightwardPass(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 1);
            t->pos.x = -0xd80;
            t->pos.y = 0;
            t->pos.z = 0xb80;
            Nidhoggr_InitRelativeMotion(t, -0x680, 0xb80);
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 1: {
            s32 x, z;
            t->posOffsetX += 0x3000;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            t->pos.x = (x >> 8) - 0x680;
            t->pos.z = (z >> 8) + 0xb80;
            if (t->posOffsetX >= 0)
                t->state++;
            break;
        }
        case 2: {
            s32 x, z;
            t->posOffsetX += 0x3000;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            t->pos.x = (x >> 8) - 0x680;
            t->pos.z = (z >> 8) + 0xb80;
            if (t->posOffsetX > 0x2ffff) {
                t->state++;
                Trigger_ConsumePending(&t->receiver, 1);
            }
            break;
        }
    }
}

void Nidhoggr_UpdateLeftwardExit(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 1);
            t->pos.x = -0x680;
            t->pos.y = 0;
            t->pos.z = 0xb80;
            Nidhoggr_InitRelativeMotion(t, -0x680, 0xb80);
            t->state++;
            Sound_PlaySongChecked(0x40a);
            break;
        case 1: {
            s32 x, z;
            t->posOffsetX -= 0x1000;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            t->pos.x = (x >> 8) - 0x680;
            t->pos.z = (z >> 8) + 0xb80;
        }
            if (t->posOffsetX < -0x70000) {
                t->state++;
                Trigger_ConsumePending(&t->receiver, 1);
            }
            break;
    }
}

void Hud_SetMode(u32);

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);

void sub_08159164(u32);

void GameState_SetEventFlag(u32);

void Nidhoggr_UpdateDepartureTransition(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 1);
            t->pos.x = -0xd80;
            t->pos.y = 0;
            t->pos.z = 0x80;
            t->pos.z = gUnk_02000710->positionZ + 0x80;
            t->basePos = t->pos;
            t->basePos.x = -0x680;
            Nidhoggr_InitRelativeMotion(t, t->basePos.x, t->basePos.z);
            t->basePos.x += t->swayX >> 8;
            t->basePos.z += t->posOffsetZ >> 8;
            t->swayX = 0;
            t->posOffsetZ = 0;
            t->speed = 0;
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 1: {
            s32 x, z;
            t->posOffsetX += 0x3000;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            Nidhoggr_TrackPlayerDepth(t);
            t->pos.x = (x >> 8) + t->basePos.x;
            t->pos.z = (z >> 8) + t->basePos.z;
            if (t->posOffsetX >= 0) {
                t->state++;
                Hud_SetMode(7);
                ScreenTransition_SendCommand(3, 4, 0x1f, 0x1f, 0x1f, 0xffff, 0);
            }
            break;
        }
        case 2: {
            s32 x, z;
            t->posOffsetX += 0x3000;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            Nidhoggr_TrackPlayerDepth(t);
            t->pos.x = (x >> 8) + t->basePos.x;
            t->pos.z = (z >> 8) + t->basePos.z;
            if (t->posOffsetX > 0x2ffff)
                t->state++;
            break;
        }
        case 3:
            sub_08159164(t->optionE);
            GameState_SetEventFlag(0x41);
            if (t->script)
                Script_Call(t->script, 0);
            Task_Kill(t);
            break;
    }
}

void Nidhoggr_UpdateArrival(struct Nidhoggr *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            t->pos.x = 0;
            t->pos.y = 0;
            t->pos.z = -0x680;
            Nidhoggr_InitRelativeMotion(t, 0x260, 0x500);
            t->state++;
            Sound_PlaySongChecked(0x402);
            break;
        case 1: {
            s32 x, z;
            Nidhoggr_UpdateHorizontalSway(t);
            t->posOffsetX += 0xc00;
            if (t->posOffsetX > 0)
                t->posOffsetX = 0;
            t->posOffsetZ += 0x2000;
            if (t->posOffsetZ > 0)
                t->posOffsetZ = 0;
            x = t->posOffsetX + t->swayX;
            z = t->posOffsetZ;
            t->pos.x = (x >> 8) + 0x260;
            t->pos.z = (z >> 8) + 0x500;
            if (t->posOffsetX >= 0 && t->posOffsetZ >= 0) {
                t->state++;
                Trigger_ConsumePending(&t->receiver, 1);
            }
            break;
        }
    }
}

extern const u8 gUnk_0824F5C0[];

void Nidhoggr_UpdatePartSequences(struct Nidhoggr *);

void Nidhoggr_UpdateState(struct Nidhoggr *p)
{
    const u32 *tbl = (const u32 *)gUnk_0824F5C0;
    Callback_InvokeActor(p, tbl[p->mode]);
    Nidhoggr_UpdatePartSequences(p);
}

static inline s32 Approach(s32 d)
{
    s32 step = d >> 4;
    if (d > 0 && step == 0)
        step = 1;
    return step;
}

void Nidhoggr_ApproachCameraTarget(struct Nidhoggr *t)
{
    t->cameraPos.x += Approach(t->cameraTarget.x - t->cameraPos.x);
    t->cameraPos.y += Approach(t->cameraTarget.y - t->cameraPos.y);
    t->cameraPos.z += Approach(t->cameraTarget.z - t->cameraPos.z);
}

struct E {
    u32 a;
    u16 *b;
};

s32 MessageQueue_GetMessagesById(u32, struct E **);

void Nidhoggr_CheckActivationTriggers(struct Nidhoggr *p)
{
    struct E *e;
    s32 n = MessageQueue_GetMessagesById(p->actorId, &e);
    if (n > 0) {
        do {
            if (*e->b == 0 && p->mode == 0)
                Nidhoggr_SetBehavior(p, 1);
            n--;
            e++;
        } while (n > 0);
    }
}

void sub_0818ECFC(void *);

void Nidhoggr_InitParts(void *);

void Camera_SetPosition(void *);

void Camera_SetIsometricOffset(s32, s32);

s32 Nidhoggr_Update(struct Nidhoggr *t)
{
    Nidhoggr_CheckActivationTriggers(t);
    Nidhoggr_UpdateState(t);
    if (Tst2(0x400, &gUnk_030053F4, &gUnk_030053F0) || Tst32(4, &gUnk_02000560))
        t->flags[0] = 0;
    if (t->flags[0]) {
        Collider_Refresh(&t->altColliders[0]);
        Camera_RequestShake(0x80);
        t->flags[0]--;
    }
    Nidhoggr_UpdateDamageFlashAndPartAnimations(t);
    Nidhoggr_UpdateSuspendedObject(t);
    Nidhoggr_UpdateBackgroundScroll(t);
    Nidhoggr_UpdatePartMotion(t);
    sub_0818ECFC(t->effectPool);
    Nidhoggr_ApproachCameraTarget(t);
    if (t->targetMode)
        Camera_SetPosition(&t->cameraPos);
    else
        Camera_SetIsometricOffset(t->cameraPos.x, t->cameraPos.y);
    t->timer++;
    return 0;
}

void Nidhoggr_ClearEffectPools(void *);

s32 Nidhoggr_Destroy(struct Nidhoggr *t)
{
    Nidhoggr_ClearEffectPools(t->effectPool);
    Nidhoggr_DestroyParts(t);
    Nidhoggr_DestroySuspendedObjectSprite(t);
    Nidhoggr_UnregisterTerrainCells(t);
    return 0;
}

static inline int count(struct SceneEventReceiver *q)
{
    u8 *bank = &gUnk_0200047C->phase;
    return q->count[*bank];
}

s32 Nidhoggr_UpdateCutscene(struct Nidhoggr *t)
{
    struct SceneEventReceiver *queue = &t->receiver;
    int i;
    for (i = 0; i < count(queue); i++) {
        struct SceneEventDispatchState *g = gUnk_0200047C;
        struct SceneEventCommand *cmd = queue->pending[g->phase][i];
        switch (cmd->kind) {
            case 0:
                Nidhoggr_SetBehavior(t, 0xd);
                break;
            case 33:
                Nidhoggr_SetBehavior(t, 0xe);
                break;
            case 34:
                Nidhoggr_SetBehavior(t, 0xf);
                break;
            case 35:
                Nidhoggr_SetBehavior(t, 0x10);
                break;
            case 36:
                Nidhoggr_SetBehavior(t, 0x12);
                break;
        }
        SceneMessages_SetPendingTrigger(queue, cmd);
    }
    return Nidhoggr_Update(t);
}

void SceneMessages_UnregisterReceiver(void *);

s32 Nidhoggr_DestroyCutscene(struct Nidhoggr *t)
{
    SceneMessages_UnregisterReceiver(&t->receiver);
    return Nidhoggr_Destroy(t);
}

/* Script options of the Nidhoggr boss (both of its creators call this):
   -l (4000, kept twice, likely current and maximum life), -g, -Y, -e, -A,
   -D, -E, -S, and -c two or -f three values with a mode byte (0 or 1). */

void Nidhoggr_ReadOptions(struct Nidhoggr *p)
{
    u32 value;
    u8 zero8;
    u16 zero;
    p->maxLife = value = Script_GetKeywordValue('l', 4000);
    zero8 = 0;
    zero = 0;
    p->life = value;
    p->maxHp = Script_GetKeywordValue('g', 100);
    p->optionY = Script_GetKeywordValue('Y', 1000);
    p->script = Script_GetKeywordValue('e', 0);
    p->attackPower = Script_GetKeywordValue('A', 40);
    p->optionD = Script_GetKeywordValue('D', 20);
    p->optionE = Script_GetKeywordValue('E', 100);
    p->optionS = Script_GetKeywordValue('S', 0);
    p->targetSourceX = zero;
    p->targetSourceY = zero;
    p->targetSourceZ = zero;
    p->targetMode = zero8;
    if (Script_SeekToKeyword('c')) {
        p->targetSourceX = Script_GetValue();
        p->targetSourceY = Script_GetValue();
        p->targetMode = zero8;
    }
    if (Script_SeekToKeyword('f')) {
        p->targetSourceX = Script_GetValue();
        p->targetSourceY = Script_GetValue();
        p->targetSourceZ = Script_GetValue();
        p->targetMode = 1;
    }
}

void ScenePanelEffect_InitPool(void *);

static inline void Fill32(void *dst, u32 *tmp, u32 size)
{
    *tmp = 0;
    CpuSet(tmp, dst, 0x05000000 | size / 4);
}

s32 Nidhoggr_Init(struct Nidhoggr *t, s32 unusedA, s32 unusedB)
{
    s32 i;
    u8 *p;
    u32 tmp;
    t->active = 1;
    t->pos.x = -0xe00;
    t->pos.y = 0;
    t->pos.z = -0x80;
    t->timer = 0;
    t->shuffleCursor = 0xff;
    {
        u8 z = 0;
        for (i = 8, p = &t->dead[8]; i >= 0; p--, i--)
            *p = z;
    }
    ScenePanelEffect_InitPool(t->effectPool);
    Nidhoggr_InitBackgrounds(t);
    Nidhoggr_InitParts(t);
    Nidhoggr_InitSuspendedObject(t);
    Nidhoggr_SetBehavior(t, 0);
    t->life = t->maxLife;
    *(s32 *)((u8 *)gUnk_02000710 + 0x61c) = (s16)t->maxLife;
    *(s32 *)((u8 *)gUnk_02000710 + 0x618) = t->life;
    t->flags[0] = 0;
    t->flags[1] = 0;
    t->attackActive = 0;
    t->flags[2] = 0;
    t->flags[3] = 0;
    t->flags[4] = 0;
    t->flags[5] = 0;
    t->flags[6] = 0;
    t->flags[7] = 0;
    t->flags[8] = 0;
    t->groupHpA = t->optionY;
    t->groupHpB = t->optionY;
    Fill32(&t->cameraPos, &tmp, 8);
    Fill32(&t->cameraTarget, &tmp, 8);
    return 0;
}

struct Nidhoggr *Nidhoggr_Create(u16 a, u16 b)
{
    struct Nidhoggr *p = Task_Create(8, 0x72FC);
    if (p != 0) {
        p->actorId = a;
        Task_SetCallbacks(p, Nidhoggr_Update, Nidhoggr_Destroy);
        Nidhoggr_ReadOptions(p);
        if (Nidhoggr_Init(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void SceneMessages_RegisterReceiver(void *, u32, u32);

void *Nidhoggr_CreateCutscene(u16 id, u16 arg)
{
    struct Nidhoggr *t = Task_Create(8, 0x72fc);
    if (t) {
        t->actorId = id;
        Task_SetCallbacks(t, Nidhoggr_UpdateCutscene, Nidhoggr_DestroyCutscene);
        Nidhoggr_ReadOptions(t);
        if (Nidhoggr_Init(t, id, arg) < 0) {
            Task_Kill(t);
            return NULL;
        }
        {
            u16 tmp = t->targetSourceX;
            t->cameraPos.x = tmp;
        }
        /* the original writes x three times (y and z never set) */
        t->cameraPos.x = t->targetSourceY;
        t->cameraPos.x = t->targetSourceZ;
        t->cameraTarget = t->cameraPos;
        SceneMessages_RegisterReceiver(&t->receiver, t->actorId, 4);
        Nidhoggr_SetBehavior(t, 0xd);
    }
    return t;
}
