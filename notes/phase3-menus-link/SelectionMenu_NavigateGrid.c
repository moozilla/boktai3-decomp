int SelectionMenu_NavigateGrid(struct SelectionMenu *p)
{
    int index = p->selectedSlot;
    int direction = 0;
    int pressedUp = gPadState[1] & 0x40;
    u16 *held = gPadState;
    if (pressedUp)
        direction = 1;
    else if (gPadState[1] & 0x80)
        direction = 2;
    else if (gPadState[1] & 0x20)
        direction = 4;
    else if (gPadState[1] & 0x10)
        direction = 8;
    if (!direction) {
        if (p->repeatTicks > 7) {
            if (*held & 0x40)
                direction = 1;
            else if (*held & 0x80)
                direction = 2;
            if (held[0] & 0x20)
                direction |= 4;
            else if (held[0] & 0x10)
                direction |= 8;
        } else if (gPadState[0] & 0xf0)
            p->repeatTicks++;
    }
    if (direction & 1) {
        if (!(index >> 2))
            index = (index & 3) + 12;
        else
            index -= 4;
        p->repeatTicks = 0;
    } else if (direction & 2) {
        int row = index >> 2;
        if (row == 3)
            index &= row;
        else
            index += 4;
        p->repeatTicks = 0;
    } else if (direction & 4) {
        if (!(index & 3))
            index = (index & 0xfc) + 3;
        else
            index--;
        p->repeatTicks = 0;
    } else if (direction & 8) {
        if ((index & 3) == 3)
            index &= 0xfc;
        else
            index++;
        p->repeatTicks = 0;
    }
    return index;
}
