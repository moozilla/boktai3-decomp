#include "orbit_background.h"
// MATCHING_CONCESSION: register-pinning
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline s32 absolute(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}
void OrbitBackground_UpdatePosition(struct OrbitBackground *background)
{
    s32 finalZ;
    if (background->holdCamera) {
        if (background->blendFrame <= 6) {
            s32 weight, denominator;
            background->blendFrame++;
            weight = 8 - background->blendFrame;
            denominator = weight + 1;
            background->position.x =
                Div(background->position.x * weight + gUnk_030042A0.x, denominator);
            background->position.y =
                Div(background->position.y * weight + gUnk_030042A0.y, denominator);
            finalZ = Div(background->position.z * weight + gUnk_030042A0.z, denominator);
        } else {
            background->blendFrame = 8;
            background->position.x = gUnk_030042A0.x;
            background->position.y = gUnk_030042A0.y;
            finalZ = (u16)gUnk_030042A0.z;
        }
    } else {
        if (gUnk_02000580[background->playerIndex]->targetActive) {
            register u32 angle asm("r0");
            if (gUnk_02000580[background->playerIndex]->transition == 4)
                angle = ((u16)gGameState->unk85E + 128) & 255;
            else
                angle = (u16)gGameState->unk85E;
            background->angle = angle;
            if (background->blendFrame <= 6) {
                s32 weight, denominator;
                background->blendFrame++;
                weight = 8 - background->blendFrame;
                denominator = weight + 1;
                background->position.x = Div(background->position.x * weight +
                                                 gUnk_02000580[background->playerIndex]->target.x,
                                             denominator);
                background->position.y = Div(background->position.y * weight +
                                                 gUnk_02000580[background->playerIndex]->target.y,
                                             denominator);
                finalZ = Div(background->position.z * weight +
                                 gUnk_02000580[background->playerIndex]->target.z,
                             denominator);
            } else {
                background->blendFrame = 8;
                background->position.x = gUnk_02000580[background->playerIndex]->target.x;
                background->position.y = gUnk_02000580[background->playerIndex]->target.y;
                finalZ = (u16)gUnk_02000580[background->playerIndex]->target.z;
            }
        } else {
            register struct GameState **gameAddress asm("r2") = &gGameState;
            s32 source = (*gameAddress)->unk85E;
            s32 angle = background->angle;
            register s32 difference asm("r1");
            s32 reverse;
            {
                s32 adjusted = angle - 256;
                difference = (source - adjusted) & 255;
            }
            reverse = 128 - difference;
            if (absolute(reverse) <= 16) {
                if (difference != 128) {
                    register s32 next asm("r0");
                    if (difference <= 127) {
                        register s32 shifted asm("r0") = reverse >> 2;
                        register s32 step asm("r1");
                        step = shifted + 1;
                        next = angle + step;
                    } else {
                        next = difference - 128;
                        next >>= 2;
                        next = angle - next;
                        next += 255;
                    }
                    next &= 255;
                    background->angle = next;
                }
                {
                    s32 radius = (background->radius * 3 - 300) >> 2;
                    background->radius = radius;
                    if ((s16)radius < 0) {
                        background->radius = -radius;
                        background->angle = (*gameAddress)->unk85E;
                    }
                }
            } else {
                if (difference) {
                    register s32 next asm("r0");
                    if (difference <= 127) {
                        register s32 shifted asm("r0") = difference >> 2;
                        register s32 step asm("r1");
                        step = shifted + 1;
                        next = angle + step;
                    } else {
                        s32 full = 256;
                        s32 step = full - difference;
                        step >>= 2;
                        step += 1;
                        next = angle + full;
                        next -= step;
                    }
                    next &= 255;
                    background->angle = next;
                }
                if (background->radius <= 299)
                    background->radius = (background->radius * 3 + 300) >> 2;
            }
            {
                register u32 loadedFrame asm("r0") = background->blendFrame;
                u32 frame;
                asm("" : "+r"(loadedFrame));
                frame = loadedFrame;
                if (frame > 1) {
                    s32 denominator;
                    background->target.x =
                        (u16)gUnk_02000580[background->playerIndex]->position.x +
                        scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
                    {
                        struct OrbitBackgroundPlayer *player =
                            gUnk_02000580[background->playerIndex];
                        background->target.y =
                            (((128 - player->heightScale) * 150) >> 7) + (u16)player->position.y;
                    }
                    background->target.z =
                        (u16)gUnk_02000580[background->playerIndex]->position.z +
                        scale(gUnk_086149C4[background->angle & 255] * background->radius);
                    {
                        u32 nextFrame = frame - 1;
                        background->blendFrame = nextFrame;
                    }
                    denominator = background->blendFrame + 1;
                    background->position.x =
                        Div(background->position.x * background->blendFrame + background->target.x,
                            denominator);
                    background->position.y =
                        Div(background->position.y * background->blendFrame + background->target.y,
                            denominator);
                    finalZ =
                        Div(background->position.z * background->blendFrame + background->target.z,
                            denominator);
                } else {
                    background->blendFrame = 0;
                    background->position.x =
                        (u16)gUnk_02000580[background->playerIndex]->position.x +
                        scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
                    {
                        struct OrbitBackgroundPlayer *player =
                            gUnk_02000580[background->playerIndex];
                        background->position.y =
                            (((128 - player->heightScale) * 150) >> 7) + (u16)player->position.y;
                    }
                    finalZ = (u16)gUnk_02000580[background->playerIndex]->position.z +
                             scale(gUnk_086149C4[background->angle & 255] * background->radius);
                }
            }
        }
    }
    background->position.z = finalZ;
}
