#include "engine.h"
void sub_0811D358(void);
void sub_0811DB6C(void);
void sub_0811D54C(void);
static inline u16 *SceneState_HalfwordAt(u8 *base, s32 offset)
{
    return (u16 *)(base + offset);
}

void SceneState_ResetRuntimeFields(void)
{
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7A0) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7A2) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7A4) = 0;
    *((s16 *)(((u8 *)gUnk_02000710) + 0x7A6)) = -1;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x81A) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7B2) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7B4) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7B6) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7FA) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7FC) = 0;
    *SceneState_HalfwordAt((u8 *)gUnk_02000710, 0x7FE) = 0;
    *((u16 *)(((u8 *)gUnk_02000710) + 0x818)) = 2000;
    *((u32 *)(((u8 *)gUnk_02000710) + 0x7B8)) = 1;
    *((u32 *)(((u8 *)gUnk_02000710) + 0x7BC)) = 1;
    *((u32 *)(((u8 *)gUnk_02000710) + 0x7C0)) = 1;
    *((u32 *)(((u8 *)gUnk_02000710) + 0x7C4)) = 0;
    *((u16 *)(((u8 *)gUnk_02000710) + 0x7F8)) = 1;
    *((u32 *)(((u8 *)gUnk_02000710) + 0x81C)) = 0;
    sub_0811D358();
    sub_0811DB6C();
    sub_0811D54C();
    *((u32 *)(((u8 *)gUnk_02000710) + 0x820)) = 0;
}
