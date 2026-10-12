struct AffineBackground {
    u8 task[0x18];u16 id;u8 flags,layer;
    u16 v1C; s16 x,y,centerX,centerY,dx,dy;
    s16 speedX,speedY,pad2E; s32 accumulatedX,accumulatedY; s16 targetX,targetY;
    u8 pad3C[4];s16 width,height;u16 moveActive,moveTime,moveDuration,speedScale;
    u8 enabled,pad4D;u16 fadeActive,fadeDuration,blendX,blendY,startX,startY,targetBlendX,targetBlendY,stepX,stepY,pad62;
    u32 count;u16 types[8];s16 args[8][8];
};
