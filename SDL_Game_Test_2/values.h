#ifndef values_h
#define values_h

#define DEV_MODE 0

#define RENDER_SCALE 1

#define TILESIZE 0x20

#define WINDOW_SIZE_TILES_WIDTH 	0x20
#define WINDOW_SIZE_TILES_HEIGHT 	0x18

#define PLAYER_WIDTH 		31
#define PLAYER_HEIGHT 		63
#define RECOVER_TIME        60

#define JUMP_VEL			0x0a
#define GRAV				0x01p-1 /* 1/16 */ // not used??
#define FALL_VEL_FAST 		0x0.6p1
#define FALL_VEL_SLOW 		0x0.2cp1
#define ACC_GROUND 			0x0.4p1
#define ACC_AIR 			0x0.1p1
#define DE_ACC_GROUND 		0x0.4p1
#define DE_ACC_AIR 			0x0.02p1
#define MAX_VEL_X 			0x04
#define MAX_VEL_Y 			0x0A



#endif /* values_h */
