

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__


#define KEY_DOWN	1
#define KEY_UP		0

#define KEY_ESC		27
#define KEY_A		'a'
#define KEY_B		'b'
#define KEY_S		's'
#define KEY_D		'd'
#define KEY_W		'w'
#define KEY_X		'x'
#define KEY_Z		'z'
#define KEY_C		'c'
#define KEY_O		'o'
#define KEY_P		'p'
#define KEY_N		'n'
#define KEY_SPACE	' '
#define KEY_AA		'A'
#define KEY_WW		'W'
#define KEY_SS		'S'
#define KEY_DD		'D'

typedef struct {
	int	up;
	int down;
	int left;
	int right;
	int A;
	int B;
	int S;
	int D;
	int W;
	int X;
	int Z;
	int C;
	int O;
	int P;
	int N;
	int SPACE;
	int AA;
	int WW;
	int SS;
	int DD;
} CONTROLLER_DATA;

void GetControllerData( CONTROLLER_DATA *cntl );
void KeyDown( unsigned char key, int x, int y );
void KeyUp( unsigned char key, int x, int y );
void SpecialKeyDown(int key, int x, int y);
void SpecialKeyUp(int key, int x, int y);

#endif