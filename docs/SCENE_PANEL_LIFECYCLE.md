# Scene panel lifecycle

The reviewed `0805C8D8–0805CB38` span contains three new exact functions,
608 ordinary-C bytes (+0.0252 percentage points). The partial lifecycle view
joins the adjacent configuration, interaction, rig and command contracts.

Initialization reads optional d into the main-sprite skip byte (default zero),
saves the current player and tether pointers at 4F4/4F8, initializes anchors
and initial mode, then reads n into subject count (default four). It creates
subjects, initializes interaction, particles and time palette, reads the
script property, and registers the command receiver. Finally it copies the
signed global mode at game-state 876 to the panel byte C10 and publishes the
panel global. It returns zero. The count is not clamped by this code.

The update callback always dispatches commands, selects presentation mode,
and counts active subjects before examining player kind. Player kind four
returns zero immediately. In initial mode one, a player kind one with resource
618 equal to zero forces interaction state fourteen and rig state nine unless
already in fourteen; it invokes the no-op action and pending script argument
eight. The selected interaction callback from 08605ABC then runs.

Initial mode zero updates the palette state. Other modes update the rig,
submit the midpoint, draw the background layers, load the mode palette and
update the approach sprite. A set activation latch at 1342 produces sound
only when main presentation is enabled: resource zero selects warningSounds[2]
at 1348, otherwise the warningStage entry. A nonzero selected sound is played,
then sound 29B always follows. The latch clears even when presentation skips
sound. Particles update last. The non-void callback retains the original
fall-through ABI; no reliable return value is asserted for that path.

Cleanup removes the main palette sprite only when its active halfword is set
in initial mode zero. Other modes remove all rig sprites and the auxiliary
sprite, then call 08215A74(1,1). Both paths destroy particles, remove the
receiver, clear the published global and return zero. The three functions are
ordinary C without pins, barriers or handwritten instruction bodies.
