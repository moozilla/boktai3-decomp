# Drawable playback in the monster reset callbacks

Tags: #monsters #matching

The eleven reset and playback callbacks below build without a drawable register pin or
an assembly barrier. They pass both the drawable and its embedded sequence to the
inlined step. The current-frame lookup reads the command table from the drawable and
the index from the sequence. This keeps the load of the drawable and its copy into the
long-lived owner register distinct. The existing single-argument step and the new
two-argument step share frame, flip, index and duration calculations.

- Ghoul_ResetRouteState
- Slime_ResetIdlePlayback
- Ghost_ResetMovementState
- Lun_ResetMovementState
- SceneActorRoute_ResetStep
- Golem_ResetCombatState
- Mummy_ResetRevivalMotion
- Dog_ResetActionState
- Mimic_ResetPursuitDrawable
- SceneActorControl_UpdateMotionPhase_080EBD0C
- Ivy_TickDrawable
