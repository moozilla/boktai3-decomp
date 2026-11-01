# Hit reaction particle bursts

Tags: #scene-actors #matching 0807D2E0

The guarded hit reaction now advances the ten-bit RNG index three times per contact
particle and once for each of the six defeat sparks. The six spark calls use the
actor's position, a randomized variant from 1 through 4, a spread of (80, 60, 60)
and six distinct headings. The old draft put the heading in the wrong argument and
passed a constant variant. The route reset clears the control mode, counter and timer.
The unit still differs in branch layout, stack allocation and the damage display path.
