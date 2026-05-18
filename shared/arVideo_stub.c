/* Stub video functions - video capture not used (static image mode) */
#include <AR/config.h>
#include <AR/ar.h>

int  arVideoDispOption(void)  { return 0; }
int  arVideoOpen(char *config){ return 0; }
int  arVideoClose(void)       { return 0; }
int  arVideoCapStart(void)    { return 0; }
int  arVideoCapStop(void)     { return 0; }
int  arVideoCapNext(void)     { return 0; }
ARUint8 *arVideoGetImage(void){ return 0; }
int  arVideoInqSize(int *x, int *y){ return -1; }
