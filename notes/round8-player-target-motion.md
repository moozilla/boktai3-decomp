# Player_UpdateTargetMotion

Tags: #matching #player #compiler #round8

The stock compiler reproduces all 4,176 bytes at 0814B1F8. Cases 4 and 6 each
step the secondary animation and increment the tether frame. Writing that tail in
both cases lets the compiler share it after register allocation. The previous
explicit jump shared it too early and changed the scratch registers for four
address calculations in case 5. The animation flag reads and writes are unchanged.
