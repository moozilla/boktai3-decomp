# Armor layered animation

Tags: #matching #armor #compiler

ActorController_UpdateLayeredMid matches all 240 bytes at 08183CAC without
either register pin. Read the actor's old flag word first, then use the new bit
as the accumulator for the OR before writing the halfword.

ActorController_UpdateLayeredMidNoSound uses the same flag update. All 228 bytes
at 0818400C also match without its two pins.
