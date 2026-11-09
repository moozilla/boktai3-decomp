s32 DirectionalController_CheckRemaining(struct DirectionalManager *manager,
                                         struct DirectionalController *p)
{
    DirectionalController_ConsumeEntered(p);
    if (manager->actor) {
        u32 mask = 6;
        u32 flags;
        flags = gUnk_02000560;
        if (flags & mask)
            goto done;
        flags &= 1;
        if (!flags)
            goto done;
        {
            s32 dx = p->position.x - manager->position.x, dz = p->position.z - manager->position.z;
            u32 distance = dx * dx + dz * dz;
            s32 angle = Math_Atan2(dx, dz);
            u32 direction = (((u16)angle + 112) >> 5) & 7;
            if (distance < p->distance) {
                s32 a, b, c;
                if (p->orientation) {
                    a = 0;
                    b = 1;
                    c = 2;
                } else {
                    a = 6;
                    b = 7;
                    c = 0;
                }
                if (a == direction || b == direction || c == direction) {
                    u32 facing = manager->actor->facing;
                    if (facingAllowed(facing, a, b, c))
                        return angle;
                }
            }
        }
    }
done:
    DirectionalController_SetState(p, 0);
    return 0;
}
