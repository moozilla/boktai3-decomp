# Short-lived shaking keyframe icon

The tentative ShakeIconDisplay cohort spans08056D70–08056F20, five functions
and432 bytes. Two new exact functions add292 bytes /+0.0121 percentage points;
the three existing helpers (140 bytes) receive typed contracts without new
credit. Both new bodies match without hints after minimization. No runtime
coverage or particular in-game identity is claimed.

Each instance is a B0-byte list9 task, with a32-byte resource header at18,
a60-byte animation at38, state frame9A, lifetime9C, packed XYZ atA0, integer
completion script ID atA8 and per-frame callback atAC. There is no singleton.
Initialization requires CB05/1289; missing resource returns-1. Keyword t selects
frame75+t after halfword truncation, I sets lifetime(default60), and p sets
completion script ID(default0). Constructor flags16, priority1, argument0 and
scale60 are retained. Position begins at zero and initialization returns0.

The shake callback consumes one general-stream random sample for each of the
first six frames. It sets X to ((sample>>4)&15)-7. Later frames reset X and Y.
The task calls its callback, increments the halfword frame and, when the result
reaches lifetime, calls a nonzero completion ID with argument0 before deletion.
The original nonvoid fallthrough is retained; the task ignores its incidental
return value. Destruction unlinks the animation and returns0. Script creation
kills the new task if initialization returns nonzero.

All five complete reviewed extents pass exact checks. This simpler cohort shares
its resource and keyframe-animation views with the staged icon, while its task
size, random stream, timing and lifecycle are independent.
