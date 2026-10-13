struct BackgroundCommand {
    s16 args[8];
};
struct Background {
    u8 task[0x18];
    u16 id;
    u8 flags, layer;
    u32 resource;
    s32 x, y, startX, startY, scrollX, scrollY, unknown38, unknown3C;
    s32 targetX, targetY, originX, originY, offsetX, offsetY, speedX, speedY;
    s32 limitX, limitY, centerX, centerY;
    u16 moveActive, moveTime, moveDuration;
    s16 rateX, rateY;
    u16 scrollTime, scrollDuration, unknown7E;
    s32 scrollStartX, scrollStartY, scrollTargetX, scrollTargetY;
    u16 fadeActive, fadeDuration, blendX, blendY, blendStartX, blendStartY;
    u16 blendTargetX, blendTargetY, blendStepX, blendStepY;
    u32 unknownA4, count;
    u16 types[8];
    struct BackgroundCommand commands[8];
    u32 frame;
};
