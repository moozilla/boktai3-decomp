# ScenePanel subject initializer

Reviewed range: **0805D92C–0805DCAC**, one new **896-byte exact function (+0.0371 percentage points)**. `ScenePanelSubject_Initialize` retains thirteen locally minimized hints: eleven in the shared playback expressions and two for a scoped R2 limit pointer. It is marked as a register-pinning concession. No explicit R7 binding or instruction body is used. The DCAC creator starts the next function and is excluded.

Arguments are subject, home position, sequence row, sequence variant, initial state, kind, contact limit, additional collider setting, and appearance delay. The first four arrive in registers; the final five are stack arguments. Home is copied as eight bytes. Row, variant and kind narrow to bytes; contact limit and delay narrow to halfwords. The halfword atDC receives the narrowed contact limit divided by6.

Setup loads graphics resourceC038 and initializes the drawable, copies home to drawable position, obtains sequence resource922E:3449, configures the collider and installs the requested subject state. It then handles the resulting state byte:

| State | Contact progress | Palette bank | Additional setup |
|---:|---|---|---|
| 0 | 0 | 24C | Set drawable priority3 and hidden flag. |
| 2 | 0 | 24C | Select sequence1, perform one inline playback step, refresh collider. |
| 4 | Contact limit | 24D | Select sequence3, perform one inline playback step, refresh collider. |
| Other | Retained | Retained | No state-specific setup in this switch. |

All paths select the resulting palette bank, clear timerDE and changedF0, initialize the effect group104 bound to drawable position34, clear byteEE, and return0. State selection can also play the existing entry songsFB/FC before this switch.

The playback expressions preserve separate command reads for frame and flip bits, sequence direction/hold flags, u16 frame-index wrapping, command-halfword masking for frame weight and signed delay scaling by64. A narrowed zero duration is raised to1. The helper's completion result is unused here. Its separate volatile command reads and matching hints reproduce the reviewed decoder accesses; no broader concurrency claim is made.

Integration uses the consolidated ScenePanelSubjectView and the shared playback
helper. Palette fields and the selector follow main's verified palette-bank
names. The final initializer ABI uses const ContactPosition* and u32 values.
