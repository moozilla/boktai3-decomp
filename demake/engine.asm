; Boktai DMG. Room assets live in the MBC5 switch window. Normal gameplay
; writes VRAM only during VBlank. OAM is built in WRAM then copied by DMA.
boot:
    di
    ld sp,0xdfff
    ld a,1
    ld [0xffff],a
    call lcd_off
    ld hl,0xc000
    ld bc,768
    call clear
    ld hl,dma_code
    ld de,0xff80
    ld bc,10
    call copy
    ld hl,obj_markers
    ld de,0x8700
    ld bc,marker_bytes
    call copy
    ld a,0xe4
    ldh [0x47],a
    ld a,0xd0
    ldh [0x48],a
    ld a,0xe0
    ldh [0x49],a
    ld a,7
    ldh [0x4b],a
    ld a,0x80
    ldh [0x26],a
    ld a,0x77
    ldh [0x24],a
    ld a,0xff
    ldh [0x25],a
    call music_init
    ld a,3
    ld [energy],a
    ld [health],a
    ld a,1
    ld [bank],a
    ld [0x2000],a
    ld a,[header+4]
    ld [player_x],a
    ld a,[header+5]
    ld [player_z],a
    ld a,1
    ld [shadow_choice],a
    call title_screen

loop:
    ld a,[mode]
    and a
    jp nz,static_tick
    call music_tick
    call input
    ld a,[pressed]
    bit 2,a
    jr z,.pause
    call open_meter
    jp loop
.pause:
    bit 3,a
    jr z,.menu_set
    ld a,[menu]
    xor 1
    ld [menu],a
.menu_set:
    ld a,[menu]
    and a
    jr nz,.render
    ld hl,tick
    inc [hl]
    call move_player
    call interactions
    ld a,[need_meter]
    and a
    jr z,.door
    xor a
    ld [need_meter],a
    call open_meter
    jp loop
.door:
    ld a,[next_bank]
    and a
    jr z,.stay
    ld [bank],a
    call load_room
    jp loop
.stay:
    call update_shot
    call update_ghoul
    call sk_update
    call damage
    ld a,[next_bank]
    and a
    jr z,.render
    call load_room
    jp loop
.render:
    call draw
    call limit_oam
    call sort_oam
    call prepare_map
    call prepare_pose
    call vblank
    ld a,[camera_x]
    ldh [0x43],a
    ld a,[camera_y]
    ldh [0x42],a
    ld a,[menu]
    and a
    ld a,136
    jr z,.window
    ld a,48
.window:
    ldh [0x4a],a
    call 0xff80
    call flush_pose
    call flush_map
    call hud
    jp loop

vblank:
    ; With IME clear, a VBlank request wakes HALT without taking the vector.
    ; NOP is harmless if the request arrives between clearing IF and HALT.
    xor a
    ldh [0x0f],a
    halt
    nop
    ret

lcd_off:
    ldh a,[0x40]
    bit 7,a
    ret z
    call vblank
    xor a
    ldh [0x40],a
    ret

dma_code:
    ld a,0xc0
    ldh [0x46],a
    ld a,40
.spin:
    dec a
    jr nz,.spin
    ret

copy:
    ld a,[hl+]
    ld [de],a
    inc de
    dec bc
    ld a,b
    or c
    jr nz,copy
    ret
clear:
    xor a
    ld [hl+],a
    dec bc
    ld a,b
    or c
    jr nz,clear
    ret

room_vram:
    ld a,[bank]
    ld [0x2000],a
    ld hl,patterns
    ld de,0x8800
    ld bc,0x1800
    call copy
    ret
load_room:
    call lcd_off
    call room_vram
    ; Patterns and both 32x32 maps occupy consecutive VRAM addresses.
    xor a
    ld [next_bank],a
    ld [menu],a
    ld [projectile],a
    ld [enemy_phase],a
    ld [point_index],a
    ld [walking],a
    ; draw clears the previous room's emitted slots as well.
    ld a,4
    ld [facing],a
    ld a,1
    ld [rng],a
    ld a,[header+6]
    and a
    jr z,.no_enemy
    ld a,[route]
    ld [enemy_x],a
    ld a,[route+1]
    ld [enemy_z],a
    ld a,[route+2]
    ld [enemy_timer],a
    jr .spawn
.no_enemy:
    ld a,3
    ld [enemy_phase],a
.spawn:
    ld a,[player_x]
    ld b,a
    ld a,[player_z]
    ld c,a
    call surface
    ld [player_h],a
    ld a,1
    ld [camera_snap],a
    call reset_poses
    call sk_init
show_field:
    call draw
    call limit_oam
    call sort_oam
    call initial_map
    xor a
    ld [camera_snap],a
    ld a,[camera_x]
    ldh [0x43],a
    ld a,[camera_y]
    ldh [0x42],a
    ld a,136
    ldh [0x4a],a
    call 0xff80
    call hud
    ld a,0xe7
    ldh [0x40],a
    ret

; JOYP groups are active low. Allow the hardware matrix to settle.
input:
    ld a,0x20
    ldh [0],a
    call read_matrix
    ld b,a
    ld a,[direction]
    cpl
    and b
    ld [pressed_dir],a
    ld a,b
    ld [direction],a
    ld a,0x10
    ldh [0],a
    call read_matrix
    ld b,a
    ld a,[buttons]
    cpl
    and b
    ld [pressed],a
    ld a,b
    ld [buttons],a
    ld a,0x30
    ldh [0],a
    ret
read_matrix:
    ldh a,[0]
    ldh a,[0]
    ldh a,[0]
    ldh a,[0]
    cpl
    and 15
    ret

; B = world X, C = world Z in eighths of a cell. Carry means solid.
; The decoded cell's high nibble is slope shape, low nibble is level.
surface:
    ld a,c
    rrca
    rrca
    rrca
    and 31
    ld e,a
    ld a,[header+3]
    cp e
    jr z,.solid
    jr c,.solid
    ld a,b
    rrca
    rrca
    rrca
    and 31
    ld d,a
    ld a,[header+2]
    cp d
    jr z,.solid
    jr c,.solid
    ld a,e
    add a
    ld l,a
    ld h,0x42
    ld a,[hl+]
    ld h,[hl]
    ld l,a
    ld e,d
    ld d,0
    add hl,de
    ld a,[hl]
    cp 255
    jr z,.solid
    ld e,a
    and 15
    add a
    add a
    add a
    ld d,a
    ld a,e
    and 0xf0
    jr z,.flat
    cp 0x10
    ld a,c
    jr z,.slope
    ld a,b
.slope:
    and 7
    ld e,a
    ld a,d
    sub e
    and a
    ret
.flat:
    ld a,d
    and a
    ret
.solid:
    scf
    ret

move_player:
    xor a
    ld [walking],a
    ld a,[direction]
    and a
    ret z
    add a
    add a
    ld e,a
    ld d,0
    ld hl,directions
    add hl,de
    ld a,[hl+]
    ld [dx],a
    ld a,[hl+]
    ld [dz],a
    ld a,[hl+]
    ld [facing],a
    ld a,[hl]
    ld [mirror],a
    ld a,[tick]
    and 1
    ret nz
    ; Test the two components separately. Sliding along the wall also
    ; prevents a diagonal step from cutting across a blocked corner.
    ld a,[dx]
    and a
    jr z,.z
    ld hl,player_x
    add [hl]
    ld b,a
    ld a,[player_z]
    ld c,a
    call try_step
.z:
    ld a,[dz]
    and a
    ret z
    ld hl,player_z
    add [hl]
    ld c,a
    ld a,[player_x]
    ld b,a
    jp try_step

try_step:
    call surface
    ret c
    ld e,a
    ld hl,player_h
    sub [hl]
    inc a
    cp 3
    ret nc
    ld [hl],e
    ld a,b
    ld [player_x],a
    ld a,c
    ld [player_z],a
    ld a,1
    ld [walking],a
    ret

; Each eight-byte record stores kind, an inclusive XZ box, and 3 operands.
; Door uses A. Fruit is collected by stepping on it. B stores sunlight.
interactions:
    xor a
    ld [used_a],a
    ld a,[header+7]
    and a
    jp z,.fire
    ld b,a
    ld hl,events
.scan:
    push bc
    push hl
    ld a,[hl+]
    ld d,a
    ld a,[player_x]
    cp [hl]
    jp c,.next
    inc hl
    ld a,[player_z]
    cp [hl]
    jp c,.next
    inc hl
    ld a,[player_x]
    ld e,[hl]
    inc e
    cp e
    jp nc,.next
    inc hl
    ld a,[player_z]
    ld e,[hl]
    inc e
    cp e
    jp nc,.next
    inc hl
    ld a,d
    cp 2
    jr z,.fruit
    cp 3
    jr z,.light
    ld a,[pressed]
    bit 0,a
    jp z,.next
    ld a,[hl+]
    ld [next_bank],a
    ld a,[hl+]
    ld [player_x],a
    ld a,[hl]
    ld [player_z],a
    ld a,1
    ld [used_a],a
    pop hl
    pop bc
    ret
.fruit:
    ld a,[hl]
    call fruit_bit
    and [hl]
    jp nz,.next
    ld a,c
    or [hl]
    ld [hl],a
    ld a,[energy]
    add 3
    cp 7
    jr c,.store
    ld a,6
.store:
    ld [energy],a
    call chime
    jr .next
.light:
    ld a,[buttons]
    bit 1,a
    jp z,.next
    ld a,[sun_source]
    and a
    jr nz,.exposure
    ld a,1
    ld [need_meter],a
    jr .next
.exposure:
    ld a,[sun_level]
    and a
    jp z,.next
    cp 1
    ld e,31
    jr z,.rate
    cp 2
    ld e,15
    jr z,.rate
    ld e,7
.rate:
    ld a,[tick]
    and e
    jp nz,.next
    ld hl,energy
    ld a,[hl]
    cp 6
    jp nc,.next
    inc [hl]
    call chime
.next:
    pop hl
    ld de,8
    add hl,de
    pop bc
    dec b
    jp nz,.scan
.fire:
    ld a,[used_a]
    and a
    ret nz
    ld a,[pressed]
    bit 0,a
    ret z
    ld a,[projectile]
    and a
    ret nz
    ld hl,energy
    ld a,[hl]
    and a
    ret z
    dec [hl]
    ld a,[player_x]
    ld [shot_x],a
    ld a,[player_z]
    ld [shot_z],a
    ld a,[dx]
    ld [shot_dx],a
    ld a,[dz]
    ld b,a
    ld a,[dx]
    or b
    jr nz,.dir
    ld a,1
    ld [shot_dx],a
    ld b,1
.dir:
    ld a,b
    ld [shot_dz],a
    ld a,28
    ld [projectile],a
    ld a,12
    ld [sfx_pulse],a
    ld a,0x80
    ldh [0x16],a
    ld a,0x92
    ldh [0x17],a
    ld a,0x70
    ldh [0x18],a
    ld a,0x85
    ldh [0x19],a
    ret

; A = bit number. Return A and C = bit mask and HL = room pickup byte.
fruit_bit:
    ld c,1
    and a
    jr z,.found
.shift:
    sla c
    dec a
    jr nz,.shift
.found:
    ld hl,pickups
    ld a,[bank]
    dec a
    ld e,a
    ld d,0
    add hl,de
    ld a,c
    ret

update_shot:
    ld hl,projectile
    ld a,[hl]
    and a
    ret z
    dec [hl]
    ld hl,shot_x
    ld a,[shot_dx]
    add [hl]
    ld [hl],a
    ld b,a
    ld hl,shot_z
    ld a,[shot_dz]
    add [hl]
    ld [hl],a
    ld c,a
    call surface
    jr c,.finish
    ld a,[enemy_phase]
    cp 2
    ret nc
    ld a,[enemy_x]
    sub b
    add 4
    cp 9
    ret nc
    ld a,[enemy_z]
    sub c
    add 4
    cp 9
    ret nc
    ld a,2
    ld [enemy_phase],a
    xor a
    ld [enemy_timer],a
    ld [death_flag],a
    call hit_sound
.finish:
    xor a
    ld [projectile],a
    ret

; Ghoul_ResumeDelayedChase waits through counter 60. Patrol selection follows
; Ghoul_SelectRandomRoutePoint: sample modulo count, bump a repeated index.
update_ghoul:
    ld a,[enemy_phase]
    cp 3
    ret z
    cp 2
    jr z,.dying
    and a
    jr nz,.walking
    ld hl,enemy_timer
    ld a,[hl]
    and a
    jr z,.choose
    dec [hl]
    ret
.choose:
    ld a,[header+6]
    ld c,a
    ld a,[rng]
    srl a
    jr nc,.random
    xor 0xb8
.random:
    ld [rng],a
.modulo:
    cp c
    jr c,.selected
    sub c
    jr .modulo
.selected:
    ld hl,point_index
    cp [hl]
    jr nz,.target
    inc a
    cp c
    jr c,.target
    xor a
.target:
    ld [hl],a
    add a
    add a
    ld e,a
    ld d,0
    ld hl,route
    add hl,de
    ld a,[hl+]
    ld [goal_x],a
    ld a,[hl+]
    ld [goal_z],a
    ld a,[hl]
    ld [route_wait],a
    ld a,1
    ld [enemy_phase],a
    ret
.dying:
    ld hl,enemy_timer
    inc [hl]
    ld a,[hl]
    cp death_mark
    jr nz,.ending
    ld a,0x10
    ld [death_flag],a
    ret
.ending:
    cp death_end
    ret c
    ld a,3
    ld [enemy_phase],a
    ret
.walking:
    ld a,[tick]
    and 3
    ret nz
    ld a,[enemy_x]
    ld b,a
    ld a,[enemy_z]
    ld c,a
    ld hl,goal_x
    ld a,b
    cp [hl]
    jr z,.along_z
    inc b
    jr c,.x_ready
    dec b
    dec b
.x_ready:
    call surface
    jr c,.restore_x
    ld a,b
    ld [enemy_x],a
    ret
.restore_x:
    ld a,[enemy_x]
    ld b,a
.along_z:
    ld a,c
    ld hl,goal_z
    cp [hl]
    jr z,.arrive
    inc c
    jr c,.z_ready
    dec c
    dec c
.z_ready:
    call surface
    jr c,.arrive
    ld a,c
    ld [enemy_z],a
    ret
.arrive:
    xor a
    ld [enemy_phase],a
    ld a,[route_wait]
    ld [enemy_timer],a
    ret

damage:
    ld hl,grace
    ld a,[hl]
    and a
    jr z,.check
    dec [hl]
    ret
.check:
    ld a,[enemy_phase]
    cp 2
    ret nc
    ld a,[player_x]
    ld hl,enemy_x
    sub [hl]
    add 2
    cp 5
    ret nc
    ld a,[player_z]
    ld hl,enemy_z
    sub [hl]
    add 2
    cp 5
    ret nc
player_hit:
    call hit_sound
    ld a,90
    ld [grace],a
    ld hl,health
    dec [hl]
    ret nz
    ld [hl],3
    ld a,[bank]
    ld [next_bank],a
    ld a,[header+4]
    ld [player_x],a
    ld b,a
    ld a,[header+5]
    ld [player_z],a
    ld c,a
    call surface
    ld [player_h],a
    ret

chime:
    ld a,12
    ld [sfx_pulse],a
    ld a,0x80
    ldh [0x16],a
    ld a,0x83
    ldh [0x17],a
    ld a,0x87
    ldh [0x18],a
    ld a,0x87
    ldh [0x19],a
    ret
hit_sound:
    ld a,12
    ld [sfx_noise],a
    ld a,0x93
    ldh [0x21],a
    ld a,0x35
    ldh [0x22],a
    ld a,0x80
    ldh [0x23],a
    ret

draw:
    ; Hide exactly the sprites emitted last frame, including their unused
    ; scanlines. Every metasprite below uses at most two slots per scanline.
    ld a,[oam_count]
    and a
    jr z,.cleared
    ld b,a
    ld hl,0xc000
.clear:
    xor a
    ld [hl+],a
    dec b
    jr nz,.clear
.cleared:
    xor a
    ld [oam_count],a
    ld a,[player_x]
    ld b,a
    ld a,[player_z]
    ld c,a
    call project
    call camera
    ld a,[menu]
    and a
    ret nz
    ld a,[grace]
    and 4
    jr nz,.enemy
    ld a,[facing]
    add a
    ld b,a
    ld a,[direction]
    and a
    jr z,.player_pose
    ld a,[tick]
    and 8
    jr z,.player_pose
    inc b
.player_pose:
    ld a,b
    ld [want_pose],a
    ld a,[live_tile]
    ld [pose],a
    ld a,[mirror]
    ld [attr],a
    call actor_oam
.enemy:
    ld a,[enemy_phase]
    cp 3
    jr z,.markers
    cp 2
    jr nz,.normal
    ld a,[death_flag]
    and a
    jr z,.fallen
    ld a,[enemy_timer]
    and 4
    jr nz,.markers
.fallen:
    ld a,13
    jr .ghoul
.normal:
    ld a,[tick]
    and 16
    ld a,10
    jr z,.ghoul
    ld a,11
.ghoul:
    ld [want_pose+1],a
    ld a,[live_tile+1]
    ld [pose],a
    ld a,0x10
    ld [attr],a
    ld a,[enemy_x]
    ld b,a
    ld a,[enemy_z]
    ld c,a
    call project
    call actor_oam
.markers:
    call sk_draw
    ld a,[header+7]
    and a
    jr z,.shot
    ld b,a
    ld hl,events
.mark:
    push bc
    push hl
    ld a,[hl]
    cp 1
    jr z,.next
    ld de,5
    add hl,de
    cp 3
    jr z,.sun
    push hl
    ld a,[hl]
    call fruit_bit
    and [hl]
    pop hl
    jr nz,.next
    ld a,112
    jr .location
.sun:
    ld a,114
.location:
    inc hl
    ld b,[hl]
    inc hl
    ld c,[hl]
    push af
    call project
    pop af
    call marker_oam
.next:
    pop hl
    ld de,8
    add hl,de
    pop bc
    dec b
    jr nz,.mark
.shot:
    ld a,[projectile]
    and a
    ret z
    ld a,[shot_x]
    ld b,a
    ld a,[shot_z]
    ld c,a
    call project
    ld hl,world_y
    ld a,[hl]
    sub 9
    ld [hl+],a
    ld a,[hl]
    sbc 0
    ld [hl],a
    ld a,116
    jp marker_oam

actor_oam:
    call screen_xy
    ret c
    ld a,[oam_count]
    ld l,a
    ld h,0xc0
    ld a,e
    sub 16
    ld e,a
    ld a,[pose]
    ld b,a
    ld a,[attr]
    ld c,a
    call row_oam
    ld a,e
    add 16
    ld e,a
    call row_oam
    ld a,l
    ld [oam_count],a
    ret
row_oam:
    push de
    ld a,d
    sub 4
    bit 5,c
    jr z,.start
    add 16
.start:
    ld d,a
    ld a,3
.loop:
    push af
    ld a,e
    ld [hl+],a
    ld a,d
    ld [hl+],a
    ld a,b
    ld [hl+],a
    inc b
    inc b
    ld a,c
    ld [hl+],a
    ld a,d
    add 8
    bit 5,c
    jr z,.advance
    sub 16
.advance:
    ld d,a
    pop af
    dec a
    jr nz,.loop
    pop de
    ret
marker_oam:
    ld [pose],a
    call screen_xy
    ret c
    ld a,[oam_count]
    ld l,a
    ld h,0xc0
    ld a,e
    ld [hl+],a
    ld a,d
    add 4
    ld [hl+],a
    ld a,[pose]
    ld [hl+],a
    xor a
    ld [hl+],a
    ld a,l
    ld [oam_count],a
    ret

; Put smaller X first, the DMG overlap order. Equal X keeps emission order.
; The compiler bounds the number of active objects in the room descriptor.
sort_oam:
    ld a,[oam_count]
    cp 8
    ret c
    ld c,4
    ld h,0xc0
.record:
    ld l,c
    inc l
    ld b,[hl]
    ld a,l
    sub 4
    ld l,a
    ld a,[hl]
    cp b
    jr c,.next
    jr z,.next
    ; Insert this record into the already sorted prefix. Adjacent records
    ; in one metasprite usually arrive sorted, so most comparisons stop here.
    ld l,c
    ld a,[hl+]
    ld d,a
    ld a,[hl+]
    ld e,a
    push de
    ld a,[hl+]
    ld d,a
    ld e,[hl]
    push de
    ld e,l
    ld d,h
    ld a,l
    sub 4
    ld l,a
.shift:
    ld a,[hl-]
    ld [de],a
    dec e
    ld a,[hl-]
    ld [de],a
    dec e
    ld a,[hl-]
    ld [de],a
    dec e
    ld a,[hl-]
    ld [de],a
    dec e
    ld a,e
    cp 3
    jr z,.insert
    dec l
    dec l
    ld a,[hl]
    inc l
    inc l
    cp b
    jr z,.insert
    jr nc,.shift
.insert:
    pop hl
    ld a,l
    ld [de],a
    dec e
    ld a,h
    ld [de],a
    dec e
    pop hl
    ld a,l
    ld [de],a
    dec e
    ld a,h
    ld [de],a
    ld h,0xc0
.next:
    ld a,c
    add 4
    ld c,a
    ld a,[oam_count]
    cp c
    jr nz,.record
    ret

hud:
    ld hl,0x9c02
    ld a,[energy]
    ld c,a
    ld b,6
    ld a,[header+8]
    ld d,a
    ld a,[header+9]
    ld e,a
    call meter
    ld hl,0x9c0a
    ld a,[health]
    ld c,a
    ld b,3
    ld a,[header+10]
    ld d,a
    ld a,[header+11]
    ld e,a
meter:
    ld a,c
    and a
    ld a,e
    jr z,.put
    dec c
    ld a,d
.put:
    ld [hl+],a
    dec b
    jr nz,meter
    ret
