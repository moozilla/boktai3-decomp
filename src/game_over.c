#include "game_over.h"
#include "player.h"
#include "message_window.h"
#include "runtime.h"

void Sound_StopAllTrackedSongs(void);

void SceneLifecycle_RequestRestart(s32);

void Sprite_LinkToList(void *, s32);

void Anim_LinkToList(void *, s32);

void Sprite_InitUnlinked(void *, void *, s32);

void Widget_InitCleared(void *, void *, s32, s32, s32, s32, s32, void *);

void Hud_SetMode(s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void SpriteOam_QueueFlaggedProjectedSprites(void);

void Sprite_RenderFlipAwareList(void);

void sub_08219674(void);

void Video_SetRenderCallbacks(s32, void *, void *, void *);

void Player_AdvanceTetherPhase(struct Player *);

void Sound_FadeOutChannel10(s32);

extern void (*const gUnk_08610E44[])(struct GameOver *);

extern void (*const gUnk_08610E64[])(struct GameOver *);

static inline s32 AddScaledLetterSpacing(s32 spread, struct GameOver *panel)
{
    s32 scale = panel->extentX;
    return spread + (scale >> 2);
}

static inline void andm(u32 *flags, u32 mask)
{
    *flags &= mask;
}

void GameOver_InitExpansion(struct GameOver *panel)
{
    Sound_StopAllTrackedSongs();
    Sound_PlaySong(2);
    Sound_PlaySong(0x12c);
    panel->extentX = 10;
    panel->extentY = 0x40;
    panel->animationState = 1;
}

void GameOver_UpdateExpansion(struct GameOver *panel)
{
    panel->extentX++;
    panel->extentY += 0x1e;
    if (panel->extentY > 0x7e) {
        panel->extentY = 0x7f;
        panel->animationState = 2;
    }
}

void GameOver_UpdateLetterRebound(struct GameOver *panel)
{
    panel->extentX++;
    panel->extentY -= 3;
    if (panel->extentY <= 0x60) {
        panel->extentY = 0x60;
        panel->animationState = 3;
    }
}

void GameOver_UpdateLetterSettle(struct GameOver *panel)
{
    panel->extentX += 2;
    panel->extentY -= 3;
    if (panel->extentY <= 0x40) {
        panel->extentY = 0x40;
        panel->animationState = 4;
    }
}

void GameOver_UpdateLetterExpand(struct GameOver *panel)
{
    panel->extentX += 3;
    if (panel->extentX > 0x3f) {
        panel->extentX = 0x40;
        panel->animationState = 5;
        panel->timer = 0;
    }
}

void GameOver_UpdateLetterHold(struct GameOver *panel)
{
    panel->timer++;
    if (panel->timer > 0x13) {
        panel->animationState = 6;
        panel->timer = 0;
    }
}

void GameOver_UpdateOpening(struct GameOver *panel)
{
    u32 timer = panel->timer + 1;
    panel->timer = timer;
    if ((u16)timer == 30) {
        Sound_PlaySong(0x2F9);
    } else if ((u16)timer > 149) {
        if ((u16)timer == 150)
            Sound_PlaySong(0x12D);
        panel->extentX += 5;
        panel->extentY -= 5;
        if (panel->extentX > 127) {
            struct GameOverSprite *sprite;
            s32 i;
            panel->extentX = 1;
            panel->extentY = 1;
            {
                u32 mask = 1;
                sprite = panel->sprites;
                for (i = 7; i >= 0; i--) {
                    sprite->flags |= mask;
                    sprite++;
                }
            }
            {
                u32 zero = 0;
                panel->animationState = 7;
                panel->timer = zero;
            }
        }
    }
}

void GameOver_UpdateExitDelay(struct GameOver *panel)
{
    panel->timer++;
    if (panel->timer > 0x59)
        panel->animationState = 8;
}

void GameOver_UpdateLetterTransforms(struct GameOver *panel)
{
    s32 i;
    s32 spread;
    for (i = 0; i < 8; i++) {
        panel->sprites[i].scaleX = panel->extentX;
        panel->sprites[i].scaleY = panel->extentY;
    }
    i = 3;
    spread = 0;
    for (; i >= 0; i--) {
        if (panel->extentX > 0x40) {
            panel->sprites[i].position.x = panel->sprites[i].baseX - spread;
            spread = AddScaledLetterSpacing(spread - 16, panel);
        } else
            panel->sprites[i].position.x = panel->sprites[i].baseX;
    }
    spread = 0;
    for (i = 4; i < 8; i++) {
        if (panel->extentX > 0x40) {
            panel->sprites[i].position.x = panel->sprites[i].baseX + spread;
            spread = AddScaledLetterSpacing(spread - 16, panel);
        } else
            panel->sprites[i].position.x = panel->sprites[i].baseX;
    }
}

void GameOver_StopPresentation(void)
{
    andm(&gEngineFlags, ~0x400);
    Sound_StopSong(2);
    SceneLifecycle_RequestRestart(0);
}

void GameOver_UpdateLetterAnimation(struct GameOver *panel)
{
    gUnk_08610E44[panel->animationState](panel);
    GameOver_UpdateLetterTransforms(panel);
}

void GameOver_ShowGraphics(struct GameOver *panel)
{
    struct GameOverSprite *q = panel->sprites;
    s32 i = 7;
    do {
        Sprite_LinkToList(q, 0);
        q++;
        i--;
    } while (i >= 0);
    Anim_LinkToList(&panel->overlay, 0);
    panel->graphicsLinked = 1;
}

void GameOver_InitSprites(struct GameOver *panel)
{
    s32 i;
    struct GameOverSprite *sprite;
    panel->extentX = 1;
    panel->extentY = 1;
    for (i = 0; i < 8; i++) {
        s32 x = i * 16 + 56;
        sprite = &panel->sprites[i];
        sprite->baseX = x;
        sprite->baseY = 72;
        if (i > 3)
            sprite->baseX += 16;
        AnimData_Load(sprite->animation, 0x654B);
        Sprite_InitUnlinked(sprite, sprite->animation, 0);
        sprite->priority = 0;
        sprite->flags = 0x1033;
        sprite->position.x = sprite->baseX;
        sprite->position.y = sprite->baseY;
        sprite->scaleX = panel->extentX;
        sprite->scaleY = panel->extentY;
        sprite->frame = i;
    }
}

void GameOver_InitOverlay(struct GameOver *panel)
{
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    if (resource) {
        struct SceneInputPackedPosition position;
        panel->overlayResource = *resource;
        ResourceHeader_Relocate(&panel->overlayResource, resource);
        position.x = -8;
        position.y = -8;
        position.z = 0;
        Widget_InitCleared(&panel->overlay, &panel->overlayResource, 0x4B, 0x1091, 0, 0, 60,
                           &position);
        CpuSet(panel->overlay.palette, panel->overlayPalette, 0x04000008);
        panel->overlay.paletteBank = 0x3AA;
        panel->overlay.palette = panel->overlayPalette;
        panel->choice = 0;
    }
}

void GameOver_StartFromScript(void)
{
    struct GameOver *panel = gGameOver;
    if (panel != 0) {
        GameOver_ShowGraphics(panel);
        Hud_SetMode(7);
        panel->closeState = 1;
        if (Script_SeekToKeyword(0x66) != 0) {
            if (Script_GetValue() != 0)
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1fff, 2);
        }
    }
}

void GameOver_TryBeginClose(struct GameOver *panel)
{
    u32 mask = 0x400;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        GameOver_ShowGraphics(panel);
        Hud_SetMode(7);
        if ((u8)(gPlayers[0]->tetherMode - 0x21) <= 1) {
            panel->costIndex = 1;
        } else {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 2);
        }
        panel->closeState = 1;
    }
}

void GameOver_FinishDelay(struct GameOver *panel)
{
    if (++panel->timer > 31) {
        void *a = SpriteOam_QueueFlaggedProjectedSprites;
        void *b = Sprite_RenderFlipAwareList;
        void *c = sub_08219674;
        u32 mask = ~1;
        struct GameOverSprite *entry = panel->sprites;
        int i = 7;
        do {
            entry->flags &= mask;
            entry++;
        } while (--i >= 0);
        Video_SetRenderCallbacks(0, a, b, c);
        panel->closeState = 2;
    }
}

void GameOver_UpdateCostPrompt(struct GameOver *panel)
{
    GameOver_UpdateLetterAnimation(panel);
    if (panel->animationState == 6) {
        panel->overlay.flags &= ~1;
        Hud_SetMode(3);
        MessageWindow_SetBounds(1, 13, 28, 4);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(panel->text);
        MessageWindow_SetNumberArgument(0, panel->costs[panel->costIndex]);
        if (gGameState->unk7B0 >= panel->costs[panel->costIndex])
            MessageWindow_ShowMessage(0);
        else
            MessageWindow_ShowMessage(1);
        panel->closeState = 3;
    }
}

void GameOver_UpdateCostSelection(struct GameOver *panel)
{
    s32 decision = -1;
    if (gPadState[1] & 1) {
        u32 choice = panel->choice;
        decision = 1;
        if (choice != 0)
            decision = 0;
        Sound_PlaySong(221);
    } else if (gPadState[1] & 2) {
        decision = 0;
        Sound_PlaySong(222);
    }
    if (decision == 0) {
        u32 mask;
        panel->overlay.flags |= 1;
        mask = 1;
        MessageWindow_Clear();
        Hud_SetMode(7);
        Player_AdvanceTetherPhase(gPlayers[0]);
        Sound_FadeOutChannel10(2);
        panel->timer = decision;
        panel->choice = mask;
        panel->animationState = 6;
        panel->closeState = 4;
    } else if (decision == 1) {
        struct GameOverSprite *sprite;
        s32 i;
        u32 mask;
        Sound_FadeOutChannel10(2);
        panel->overlay.flags |= decision;
        mask = 1;
        sprite = panel->sprites;
        for (i = 7; i >= 0; i--) {
            sprite->flags |= mask;
            sprite++;
        }
        MessageWindow_Clear();
        Hud_SetMode(7);
        if (panel->procedure) {
            struct GameOverScriptArgs args;
            s32 value;
            args.count = 1;
            value = panel->costs[panel->costIndex];
            args.values = &value;
            Script_Call(panel->procedure, &args);
        }
        Task_Kill(panel);
    } else {
        u32 changed = 0;
        if (gPadState[1] & 32)
            changed = panel->choice != 0;
        else if ((gPadState[1] & 16) && !panel->choice)
            changed = 1;
        if (changed) {
            Sound_PlaySong(220);
            panel->choice = 1 - panel->choice;
            if (panel->choice)
                Anim_LoadKeyframe(&panel->overlay, &panel->overlayResource, 76, 1);
            else
                Anim_LoadKeyframe(&panel->overlay, &panel->overlayResource, 75, 1);
        }
        Palette_UpdateHighlightPulse(&panel->overlayPalette[12], &panel->pulseFrame);
    }
}

void GameOver_UpdateExit(struct GameOver *panel)
{
    GameOver_UpdateLetterAnimation(panel);
    if (panel->animationState == 8) {
        andm(&gEngineFlags, ~0x400);
        gPlayTimeCounterEnabled = 0;
        SceneLifecycle_RequestRestart(panel->choice);
        Task_Kill(panel);
    }
}

s32 GameOver_OnUpdate(struct GameOver *panel)
{
    gUnk_08610E64[panel->closeState](panel);
    return 0;
}

u32 GameOver_OnDestroy(struct GameOver *panel)
{
    struct GameOverSprite *q;
    s32 i;
    if (panel->graphicsLinked != 0) {
        q = panel->sprites;
        for (i = 7; i >= 0; i--) {
            Sprite_Remove(q);
            q++;
        }
        Anim_Remove(&panel->overlay);
    }
    gGameOver = 0;
    return 0;
}

s32 GameOver_Initialize(struct GameOver *panel)
{
    s32 present;
    GameOver_InitSprites(panel);
    GameOver_InitOverlay(panel);
    panel->closeState = 0;
    panel->animationState = 0;
    panel->timer = 0;
    panel->graphicsLinked = 0;
    if (Script_SeekToKeyword('r'))
        panel->text = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('c')) {
        panel->costs[0] = Script_GetValue();
        panel->costs[1] = Script_GetValue();
    } else {
        panel->costs[0] = 500;
        panel->costs[1] = 250;
    }
    panel->costIndex = 0;
    present = Script_SeekToKeyword('p');
    if (present)
        panel->procedure = Script_GetValue();
    else
        panel->procedure = present;
    gGameOver = (void *)panel;
    panel->pulseFrame = 0;
    return 0;
}

void *GameOver_Create(void)
{
    void *r = gGameOver;
    void *q;
    if (r == 0) {
        q = Task_Create(0xb, 0x354);
        if (q != 0) {
            Task_SetCallbacks(q, (void (*)(void *))GameOver_OnUpdate,
                              (void (*)(void *))GameOver_OnDestroy);
            if (GameOver_Initialize(q) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
