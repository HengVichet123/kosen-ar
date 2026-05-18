// ============================================================================================================== 
//									Model Collision Detection
// 
// In this prgoram, 2 models will be display onto an image.
// You can control either one of the two. You can selected the model you wish to control
// by click ">" key or "<" key.
// You can control the selected model by changing its position, rotation angle, and size.
// Both models have height and radius.  When you change the size of the model, its height and radius
// will be scaled accordingly. You can also adjust the height and radius as you want (with key inputs A, W, S, D)
// When the two model collided, the notify text will be display. 
// ============================================================================================================== 



// ============================================================================================================== 
//Libararies
// ============================================================================================================== 

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/gsub_lite.h>
#include <GL/glut.h>
#include "GL/controller.h"	
#include "GL/GLMetaseq.h"	// MQO loader 
#include <iostream>
#include <cstring> 
#include <opencv2/opencv.hpp>
#include <time.h>
#include <vector>
#ifdef _DEBUG
#pragma comment(lib,"libARd.lib")
#pragma comment(lib,"libARgsubd.lib")
#pragma comment(lib,"libARvideod.lib")
#pragma comment(lib,"libARgsub_lited.lib")
#pragma comment(linker,"/NODEFAULTLIB:libcmtd.lib")
#else
#pragma comment(lib,"libAR.lib")
#pragma comment(lib,"libARgsub.lib")
#pragma comment(lib,"libARvideo.lib")
#pragma comment(lib,"libARgsub_lite.lib")
#pragma comment(linker,"/NODEFAULTLIB:libcmt.lib")
#endif



// ============================================================================================================== 
//GLOBAL VARIABLES AND DATA STRUCTURE
// ============================================================================================================== 

//-----------------------------------------------------------------------------------
//Camera configuration.
 //-----------------------------------------------------------------------------------

#ifdef _WIN32
char			vconf[] = "Data\\WDM_camera_flipV.xml";
#else
char			vconf[] = "";
#endif
int             xsize, ysize;
int             thresh = 100;
char            cparam_name[] = "Data/camera_para.dat";
ARParam         cparam;

int             count = 0;
int             count2 = 0;
int             count3 = 0;
#define PROJECTILE_DELAY 20 // Delay in seconds between projectiles

//-----------------------------------------------------------------------------------
//Patterns and markers
//-----------------------------------------------------------------------------------

char            patt_name[] = "Data/patt/patt.hiro";
int             patt_id;
double          patt_width = 80.0;
double          patt_center[2] = { 0.0, 0.0 };
double          patt_trans[3][4];




//-----------------------------------------------------------------------------------
//Image Files
//-----------------------------------------------------------------------------------

#define NUM_IMAGE 4
char imageData[NUM_IMAGE][100] = { "Data/images/test2.jpg", "Data/images/myhall.jpg", "Data/image/hallway.jpg" };

//-----------------------------------------------------------------------------------
//Character structure
//-----------------------------------------------------------------------------------

typedef struct {
    MQO_MODEL	model;
    GLfloat		rot, vrot;
    GLfloat		x, y, z;
    GLfloat     velX, velY;
    GLfloat     dirX, dirY;

    GLfloat     shootingDirX, shootingDirY;
    GLfloat		k;       //model scale factor
    GLfloat		radius;
    GLfloat     ori_radius;
    GLfloat     height;
    GLfloat     ori_height;
    GLfloat     speed;
    double      trans[3][4];
    int			pattId;
    int			patt_found;
    bool        active;
    bool		exist;
    bool        isAttack;
    GLfloat     health;
    GLfloat     maxHealth;
    bool        isShooting;
    bool        hasBeenShot;
    int        isGoal;
    int         enemyIndex;
    GLfloat     enemySpeed;
    bool     isBomb;
    GLfloat  startX, startY;
    int         enemyType;
    int         swammerCount;
} CHARACTER_DATA;
// Character data
#define MAX_MODELS 30
#define ENEMY_MAX_MODELS 100
#define PROJECTILES_MAX_MODELS 5
#define BOMB_MAX_MODELS 1
static CHARACTER_DATA	gCharacter[MAX_MODELS]; //Characters originally display on the scene
static CHARACTER_DATA	gEnemy[MAX_MODELS]; //Enemies characters
bool    LaserHit = FALSE;


#define MAX_PROJECTILES_LOTS 12 // Maximum number of active projectiles
CHARACTER_DATA Right_projectiles[PROJECTILES_MAX_MODELS][MAX_PROJECTILES_LOTS]; // Array to store projectiles
CHARACTER_DATA Left_projectiles[PROJECTILES_MAX_MODELS][MAX_PROJECTILES_LOTS]; // Array to store projectiles
CHARACTER_DATA Lazer_projectiles; // Array to store projectiles

CHARACTER_DATA bomb[BOMB_MAX_MODELS]; // Array to store projectiles

int projectileType = 0;
bool showHUD = false;
bool showHitbox = false;
float LazerLength = 1000.0;
GLfloat gCharacter_radius = 50.0;
GLfloat gCharacter_height = 50.0;

GLfloat gEarth_radius = 100.0;
GLfloat gEarth_height = 100.0;

GLfloat gEnemy_radius = 50.0;
GLfloat gEnemy_height = 200.0;

GLfloat projectile_radius = 10.0;
GLfloat projectile_height = 50.0;





//Gamming variables
//For Gameplay
int score = 0;
int killCount = 0;
const int KILL_GOAL = 100;
bool gameOver = false;
bool gameWon  = false;
int   second = 20;
int difficulty = 3.0;

//For Characters
#define ROTATION_SPEED 0.2 // Earth rotation degrees per iteration
int modelCount = 6;

//For Enemies
int enemyCount = 0;
GLfloat enemySpeed[ENEMY_MAX_MODELS] = { 1.0, 0.25, 0.25, 0.125, 0.025 };
static GLfloat  radius[ENEMY_MAX_MODELS] = { 50.0, 50.0, 50.0, 50.0, 100.0 };
static GLfloat  height[ENEMY_MAX_MODELS] = { 100.0, 100.0, 100.0, 150.0, 200.0 };
static GLfloat  gHealth[ENEMY_MAX_MODELS] = { 40.0, 100.0, 100.0, 1000.0, 5000.0 }; //Enemy health


//For Projectiles
int shootIndex = 0;
int lastShootIndex = 9;
int currentProjectile = 0;
int projectileCount = 0;
int projectileCounter = 0;
int currentProjectileIndex = 0;
float shootingDamage = 1.0;
float bombDamage = 1500.0;
#define BOMB_EXPLOSION_RADIUS 50.0 // Example radius for bomb explosion
bool isBomb = false;



//Model files
#define NUM_MODEL	35//number of type of 3D models
bool isLoading = false;
bool modelsLoaded = false;

static char gModelDisplay[MAX_MODELS][100] = { "Data/mqoFile/jet.mqo", "Data/mqoFile/earth.mqo",   "Data/mqoFile/portal.mqo" };
static char gEnemyDisplay[ENEMY_MAX_MODELS][100] = { "Data/mqoFile/alien0.mqo", "Data/mqoFile/alien1.mqo", "Data/mqoFile/alien2.mqo", "Data/mqoFile/alien3.mqo", "Data/mqoFile/alien4.mqo" };
static char gProjectileDisplay[PROJECTILES_MAX_MODELS][100] = { "Data/mqoFile/bullets.mqo" };
static char gBombDisplay[BOMB_MAX_MODELS][100] = { "Data/mqoFile/explosion.mqo" };
static char gLazerDiisplay[BOMB_MAX_MODELS][100] = { "Data/mqoFile/lazer.mqo" };



//Model Controlling variable
float modelSpeed = 4.0;
float rotSpeed = 2.5;


static CONTROLLER_DATA	g_cntl = { KEY_UP, KEY_UP, KEY_UP, KEY_UP, KEY_UP, KEY_UP, KEY_UP };
void GetControllerData(CONTROLLER_DATA* cntl)
{
    *cntl = g_cntl;
}





// ============================================================================
//	Motions
// ============================================================================
typedef struct {
    MQO_SEQUENCE sequence;
    int n_frame;
} AnimationData;

// The number of animations you have
#define NUM_ANIMATIONS 1

int SelectedAnimation = 0;
AnimationData animations[NUM_ANIMATIONS];
AnimationData animations2[NUM_ANIMATIONS];

int currentAnimation = -1;  // -1 means no animation, 0, 1, and 2 for respective animations
int currentAnimation2 = -1;  // -1 means no animation, 0, 1, and 2 for respective animations

static int currentFrame = 0;
static int currentFrame2 = 0;

int delayTime = 0;
int delayTime2 = 0;


//-----------------------------------------------------------------------------------
//Functions
//-----------------------------------------------------------------------------------

static void   init(void);
static void   cleanup(void);
static void   keyEvent(unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw(void);
void loadAnimation();
static void CalcState(void);
void checkData2();
void onMouseClick(int button, int state, int x, int y);


//-----------------------------------------------------------------------------------
//Main Function
//-----------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    init();
    mqoInit();


    //Initialize data
    gCharacter[0].x = -200.0;
    gCharacter[0].y = 0.0;
    gCharacter[0].z = 0.0;
    gCharacter[0].k = 1.0;


    gCharacter[0].rot = 0.0;
    gCharacter[0].vrot = 0.0;


    gCharacter[0].ori_height = gCharacter_height;
    gCharacter[0].ori_radius = gCharacter_radius;
    gCharacter[0].height = gCharacter_height;
    gCharacter[0].radius = gCharacter_radius;
    gCharacter[0].isShooting = FALSE;
    gCharacter[0].hasBeenShot = FALSE;

    gCharacter[0].model = mqoCreateModel(gModelDisplay[0], 1.0);




    //Initialize data 2
    gCharacter[1].x = 0.0;
    gCharacter[1].y = 0.0;
    gCharacter[1].z = 0.0;
    gCharacter[1].k = 1.0;


    gCharacter[1].rot = 0.0;
    gCharacter[1].vrot = 0.0;

    gCharacter[1].height = gEarth_height;
    gCharacter[1].radius = gEarth_radius;
    gCharacter[1].health = 100.0;
    gCharacter[1].isShooting = FALSE;
    gCharacter[1].hasBeenShot = FALSE;

    gCharacter[1].model = mqoCreateModel(gModelDisplay[1], 1.0);

    //Initialize data 3
    gCharacter[2].x = -300.0;
    gCharacter[2].y = 0.0;
    gCharacter[2].z = 0.0;
    gCharacter[2].k = 1.0;
    gCharacter[2].velX = 1.0;
    gCharacter[2].velY = 1.0;


    gCharacter[2].rot = 100.0;
    gCharacter[2].vrot = 0.0;

    gCharacter[2].height = gCharacter_height;
    gCharacter[2].radius = gCharacter_radius;

    gCharacter[2].model = mqoCreateModel(gModelDisplay[2], 1.0);



    //Initialize data
    gCharacter[3].x = 300.0;
    gCharacter[3].y = 300.0;
    gCharacter[3].z = 0.0;
    gCharacter[3].k = 1.0;


    gCharacter[3].rot = 300.0;
    gCharacter[3].vrot = 0.0;



    gCharacter[3].height = gCharacter_height;
    gCharacter[3].radius = gCharacter_radius;


    gCharacter[3].model = mqoCreateModel(gModelDisplay[2], 1.0);

    //Initialize data
    gCharacter[4].x = 100.0;
    gCharacter[4].y = 300.0;
    gCharacter[4].z = 0.0;
    gCharacter[4].k = 1.0;


    gCharacter[4].rot = 0.0;
    gCharacter[4].vrot = 10.0;



    gCharacter[4].height = gCharacter_height;
    gCharacter[4].radius = gCharacter_radius;

    gCharacter[4].model = mqoCreateModel(gModelDisplay[2], 1.0);



    for (int i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        Right_projectiles[0][i].x = gCharacter[0].x;
        Right_projectiles[0][i].y = gCharacter[0].y;
        Right_projectiles[0][i].z = gCharacter[0].z;
        Right_projectiles[0][i].k = 1.0;


        Right_projectiles[0][i].height = projectile_height;
        Right_projectiles[0][i].radius = projectile_radius;
        Right_projectiles[0][i].hasBeenShot = FALSE;

        Right_projectiles[0][i].model = mqoCreateModel(gProjectileDisplay[0], 1.0);
    }

    for (int i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        Left_projectiles[0][i].x = gCharacter[0].x;
        Left_projectiles[0][i].y = gCharacter[0].y;
        Left_projectiles[0][i].z = gCharacter[0].z;
        Left_projectiles[0][i].k = 1.0;


        Left_projectiles[0][i].height = projectile_height;
        Left_projectiles[0][i].radius = projectile_radius;
        Left_projectiles[0][i].hasBeenShot = FALSE;

        Left_projectiles[0][i].model = mqoCreateModel(gProjectileDisplay[0], 1.0);
    }

    Lazer_projectiles.x = gCharacter[0].x;
    Lazer_projectiles.y = gCharacter[0].y;
    Lazer_projectiles.z = gCharacter[0].z;
    Lazer_projectiles.k = 1.0;


    Lazer_projectiles.height = projectile_height;
    Lazer_projectiles.radius = projectile_radius;

    Lazer_projectiles.model = mqoCreateModel(gLazerDiisplay[0], 1.0);


    //Initialize data 4
   /* gBackground[0].x = 0.0;
    gBackground[0].y = 0.0;
    gBackground[0].z = 0.0;
    gBackground[0].k = 1.0;
    gBackground[0].w = 400.0;
    gBackground[0].h = 20.0;


    gBackground[0].rot = 0.0;
    gBackground[0].vrot = 0.0;

    gBackground[0].ori_height = gBall_height;
    gBackground[0].ori_radius = 200.0;
    gBackground[0].height = gBall_height;
    gBackground[0].radius = 200.0;

    gBackground[0].model = mqoCreateModel(gModelDisplay[3], 1.0);*/

    loadAnimation();
    glutDisplayFunc(draw);
    argMainLoop(NULL, keyEvent, mainLoop);
    glutMainLoop();


    return (0);
}


//---------------------------------------------------------------------------------- -
//KeyEvent Function
//-----------------------------------------------------------------------------------


static void  keyEvent(unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if (key == 0x1b) {
        printf("*** %f (frame/sec)\n", (double)count / arUtilTimer());
        cleanup();
        exit(0);
    }
}


//---------------------------------------------------------------------------------- -
//Mainloop Function
//-----------------------------------------------------------------------------------

static void mainLoop(void)
{
    ARMarkerInfo* marker_info;
    int             marker_num;
    int             i, j, k, l;
    // Load the image


    cv::Mat frame;


    frame = cv::imread(imageData[0]); // Load the image file


    if (frame.cols != xsize || frame.rows != ysize)
    {
        cv::resize(frame, frame, cv::Size(xsize, ysize));
        //printf("Frame resized to: %d x %d\n", frame.cols, frame.rows);
    }


    if (frame.empty()) {  // Check for invalid input
        std::cerr << "Could not open or find the image" << std::endl;
        return;
    }



    cv::Mat frameWithAlpha;



    if (frame.empty()) {  // Check for invalid input
        std::cerr << "Could not open or find the image" << std::endl;
        return;
    }

    cv::cvtColor(frame, frameWithAlpha, cv::COLOR_BGR2BGRA);

    ARUint8* dataPtr = frameWithAlpha.data;

    if (dataPtr == nullptr) {
        std::cerr << "dataPtr is null" << std::endl;
        return;
    }

    if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) {
        cleanup();
        exit(0);
    }




    for (i = 0; i < modelCount; i++) {
        gCharacter[i].exist = TRUE;
    }


    for (l = 0; l < modelCount; l++) {
        if (gCharacter[l].exist == TRUE) {
            gCharacter[l].patt_found = TRUE;
        }
        else {
            gCharacter[l].patt_found = FALSE;

        }
        //printf("patt_found of %d is %d\n", l, gCharacter[l].patt_found);
        //printf("gModelDisplay[%d] = %s\n", l, gModelDisplay[l]);

    }

    //gBackground[0].exist = TRUE;

    for (l = 0; l < enemyCount; l++) {
        if (gEnemy[l].exist == TRUE) {
            gEnemy[l].patt_found = TRUE;
        }
        else {
            gEnemy[l].patt_found = FALSE;

        }


    }


    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        Right_projectiles[currentProjectile][i].exist = TRUE;
        Left_projectiles[currentProjectile][i].exist = TRUE;

    }


    for (l = 0; l < MAX_PROJECTILES_LOTS; l++) {
        if (Right_projectiles[currentProjectile][l].exist == TRUE) {
            Right_projectiles[currentProjectile][l].patt_found = TRUE;
        }
        else {
            Right_projectiles[currentProjectile][l].patt_found = FALSE;

        }

    }


    for (l = 0; l < MAX_PROJECTILES_LOTS; l++) {
        if (Left_projectiles[currentProjectile][l].exist == TRUE) {
            Left_projectiles[currentProjectile][l].patt_found = TRUE;
        }
        else {
            Left_projectiles[currentProjectile][l].patt_found = FALSE;

        }

    }


    for (i = 0; i < modelCount; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (patt_id == marker_info[j].id) {
                //printf("Pattern ID matches with a detected marker!\n");
                if (k == -1) {
                    k = j;
                }
                else if (marker_info[k].cf < marker_info[j].cf) {
                    k = j;
                }
            }
            if (k != -1) {
                arGetTransMat(&marker_info[k], patt_center, patt_width, gCharacter[i].trans);



            }
        }
    }



    for (i = 0; i < enemyCount; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (patt_id == marker_info[j].id) {
                //printf("Pattern ID matches with a detected marker!\n");
                if (k == -1) {
                    k = j;
                }
                else if (marker_info[k].cf < marker_info[j].cf) {
                    k = j;
                }
            }
            if (k != -1) {
                arGetTransMat(&marker_info[k], patt_center, patt_width, gEnemy[i].trans);



            }
        }
    }

    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (patt_id == marker_info[j].id) {
                //printf("Pattern ID matches with a detected marker!\n");
                if (k == -1) {
                    k = j;
                }
                else if (marker_info[k].cf < marker_info[j].cf) {
                    k = j;
                }
            }
            if (k != -1) {
                arGetTransMat(&marker_info[k], patt_center, patt_width, Right_projectiles[currentProjectile][i].trans);



            }
        }
    }

    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (patt_id == marker_info[j].id) {
                //printf("Pattern ID matches with a detected marker!\n");
                if (k == -1) {
                    k = j;
                }
                else if (marker_info[k].cf < marker_info[j].cf) {
                    k = j;
                }
            }
            if (k != -1) {
                arGetTransMat(&marker_info[k], patt_center, patt_width, Left_projectiles[currentProjectile][i].trans);



            }
        }
    }

    Lazer_projectiles.exist = (projectileType == 1) ? TRUE : FALSE;
    if (Lazer_projectiles.exist) {
        Lazer_projectiles.patt_found = TRUE;
    } else {
        Lazer_projectiles.patt_found = FALSE;
    }
    k = -1;
    for (j = 0; j < marker_num; j++) {
        if (patt_id == marker_info[j].id) {
            //printf("Pattern ID matches with a detected marker!\n");
            if (k == -1) {
                k = j;
            }
            else if (marker_info[k].cf < marker_info[j].cf) {
                k = j;
            }
        }
        if (k != -1) {
            arGetTransMat(&marker_info[k], patt_center, patt_width, bomb[0].trans);
            arGetTransMat(&marker_info[k], patt_center, patt_width, Lazer_projectiles.trans);



        }
    }



    if (!frameWithAlpha.empty()) {
        // Text for Earth Health
        std::string text = "EARTH HEALTH";
        cv::Point textPos(150, 50);
        double fontScale = 0.5;
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        int thickness = 2;
        cv::Scalar blueColor(255, 0, 0); // Blue color in BGR
        cv::putText(frameWithAlpha, text, textPos, fontFace, fontScale, blueColor, thickness);

        // Ensure health is not negative
        if (gCharacter[1].health < 0.0) {
            gCharacter[1].health = 0.0;
        };

        // Drawing the health bar
        int maxHealth = 100;
        cv::Scalar healthBarColor(0, 255, 0); // Green color in BGR for health bar
        int healthBarWidth = 200; // Total width of the health bar at full health
        int healthBarHeight = 20;

        // Calculate current width based on Earth's health
        int currentHealthBarWidth = (gCharacter[1].health / (float)maxHealth) * healthBarWidth;

        // Position of the health bar
        cv::Point healthBarPos(150, 70); // Position below the text
        cv::Point healthBarEndPos = healthBarPos + cv::Point(currentHealthBarWidth, healthBarHeight);

        // Draw the health bar
        cv::rectangle(frameWithAlpha, healthBarPos, healthBarEndPos, healthBarColor, cv::FILLED);

        // Optionally, draw a border for the health bar
        cv::rectangle(frameWithAlpha, healthBarPos, healthBarPos + cv::Point(healthBarWidth, healthBarHeight), cv::Scalar(255, 255, 255), 1);

        // Text for Score
        std::string scoreText = "Score: " + std::to_string(score);
        cv::Point scoreTextPos(150, 100); // Position the score text below the health bar
        cv::Scalar whiteColor(255, 255, 255); // White color in BGR
        cv::putText(frameWithAlpha, scoreText, scoreTextPos, fontFace, fontScale, whiteColor, thickness);
    }




    glutSetWindowTitle("!");
    argDrawMode2D(); // Switch to 2D drawing for background image.
    argDispImage(dataPtr, 0, 0); // Draw the background image
    draw();
    glutMouseFunc(onMouseClick);
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);
}




//-----------------------------------------------------------------------------------
 //INIT FUNCTION  
 //-----------------------------------------------------------------------------------
static void init(void)
{
    ARParam  wparam;

    // Load the image to get its dimensions
    cv::Mat img = cv::imread(imageData[0]);
    if (img.empty()) {
        printf("Error: Couldn't load the image file!\n");
        exit(0);
    }

    xsize = img.cols;
    ysize = img.rows;
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);



    /* set the initial camera parameters */
    if (arParamLoad(cparam_name, 1, &wparam) < 0) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize(&wparam, xsize, ysize, &cparam);
    arInitCparam(&cparam);
    printf("*** Camera Parameter ***\n");
    arParamDisp(&cparam);

    if ((patt_id = arLoadPatt(patt_name)) < 0) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit(&cparam, 1.0, 0, 0, 0, 0);
}



//-----------------------------------------------------------------------------------
 //	CLEANUP FUNCTION  
 //-----------------------------------------------------------------------------------
static void cleanup(void)
{
    int i, j;
    arVideoCapStop();
    arVideoClose();
    argCleanup();
    for (i = 0; i < modelCount; i++) {
        mqoDeleteModel(gCharacter[i].model);
    }

    for (i = 0; i < enemyCount; i++) {
        mqoDeleteModel(gEnemy[i].model);

    }
    for (j = 0; j < PROJECTILES_MAX_MODELS; j++) {
        for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
            mqoDeleteModel(Right_projectiles[j][i].model);
            mqoDeleteModel(Left_projectiles[j][i].model);


        }
    }
    mqoDeleteModel(Lazer_projectiles.model);
}






void loadAnimation() {
    char animationFolders[NUM_ANIMATIONS][100] = {
        "Sequence/earthDefender/explosion/bomb%d.mqo",

    };
    int frameCounts[NUM_ANIMATIONS] = {
        6 // n_frame for animation1

    };

    for (int i = 0; i < NUM_ANIMATIONS; ++i) {
        animations[i].sequence = mqoCreateSequence(animationFolders[i], frameCounts[i], 1.0);
        animations[i].n_frame = frameCounts[i];
        if (animations[i].sequence.n_frame <= 0) {
            printf("Error loading sequence: %s\n", animationFolders[i]);
            exit(-1);
        }
    }
}


void ActivateSwarmer(int queenIndex) {
    int i;
    // Check if the enemy is the queen
    int childCount = 5 + (rand() % 6); // Random number between 5 and 10

    for (int i = 0; i < childCount; i++) {
        // Create and initialize the child enemy
        gEnemy[enemyCount].model = mqoCreateModel(gEnemyDisplay[4], 1.0);
        gEnemy[enemyCount].x = gEnemy[queenIndex].x;
        gEnemy[enemyCount].y = gEnemy[queenIndex].y;
        gEnemy[enemyCount].z = gEnemy[queenIndex].z;
        gEnemy[enemyCount].rot = gEnemy[queenIndex].rot;
        gEnemy[enemyCount].radius = gEnemy[queenIndex].radius * 0.7;
        gEnemy[enemyCount].height = gEnemy[queenIndex].height * 0.7;
        gEnemy[enemyCount].health    = 1.0;
        gEnemy[enemyCount].maxHealth = 1.0;
        gEnemy[enemyCount].enemyIndex = queenIndex;

        gEnemy[enemyCount].enemySpeed = 0.15;

        gEnemy[enemyCount].k = 0.5; // Child is smaller than the queen
        gEnemy[enemyCount].exist = TRUE;
        // Set other properties for the child as needed (e.g., health, speed, enemyType, etc.)
        enemyCount++;
    }
 }


void addEnemyModel(int enemyIndex, int portalIndex) {


    if ((gEnemy[enemyCount].model = mqoCreateModel(gEnemyDisplay[enemyIndex], 1.0)) == NULL) {
        printf("Failed to load model.\n");
        return;
    }
    gEnemy[enemyCount].rot = gCharacter[portalIndex].rot;
    gEnemy[enemyCount].x = gCharacter[portalIndex].x;
    gEnemy[enemyCount].y = gCharacter[portalIndex].y;
    gEnemy[enemyCount].z = gCharacter[portalIndex].z;
    gEnemy[enemyCount].radius = radius[enemyIndex];
    gEnemy[enemyCount].height = height[enemyIndex];
    gEnemy[enemyCount].health = gHealth[enemyIndex];
    gEnemy[enemyCount].maxHealth = gHealth[enemyIndex];
    gEnemy[enemyCount].enemyIndex = enemyIndex;
    gEnemy[enemyCount].enemySpeed = enemySpeed[enemyIndex];

    gEnemy[enemyCount].k = 1.0;
    gEnemy[enemyCount].exist = TRUE;
    gEnemy[enemyCount].enemyType = enemyIndex;

    printf("Model %s loaded.\n", gEnemyDisplay[enemyIndex]);
    if (gEnemy[enemyCount].enemyIndex == 4) {
        int queenIndex = enemyCount;

        enemyCount++;

        ActivateSwarmer(queenIndex);
    }
    else {
        enemyCount++;

    }

}

void ActivateTeleporter(int i) {
    // Calculate the current distance between gCharacter[1] and gEnemy[i]
    GLfloat dx = gCharacter[1].x - gEnemy[i].x;
    GLfloat dy = gCharacter[1].y - gEnemy[i].y;
    GLfloat distance = sqrt(dx * dx + dy * dy); // This is the radius of the circle
 
    // Generate a random angle for teleportation
    float angle = (float)rand() / RAND_MAX * 2 * M_PI; // Random angle between 0 and 2π
    GLfloat repulsionDistance = distance + 10.0; // Increase by 10 units, adjust as needed

    // Calculate new position based on the angle and radius (distance)
    gEnemy[i].x = gCharacter[1].x + repulsionDistance * cos(angle);
    gEnemy[i].y = gCharacter[1].y + repulsionDistance * sin(angle);
    
    
    GLfloat newDx = gCharacter[1].x - gEnemy[i].x;
    GLfloat newDy = gCharacter[1].y - gEnemy[i].y;
    gEnemy[i].rot = atan2(newDy, newDx) * 180 / M_PI+90; // Convert to degrees
 
    // If working in 3D, you may want to adjust the z-coordinate as well
    // gEnemy[i].z = gCharacter[1].z + ...; // Adjust as needed

    // Other teleportation logic (if any), like effects, sound, etc.
}
void ActivateSpliter(int i) {
    for (int n = 0; n < 2; n++) {  // Loop twice to create two new enemies
        if ((gEnemy[enemyCount].model = mqoCreateModel(gEnemyDisplay[2], 1.0)) == NULL) {
            printf("Failed to load model.\n");
            return;
        }

        // Spawn at a random portal so children come out of a door
        int portalIdx = rand() % 3 + 2;
        enemyCount++;

        gEnemy[enemyCount].x = gCharacter[portalIdx].x;
        gEnemy[enemyCount].y = gCharacter[portalIdx].y;
        gEnemy[enemyCount].z = gCharacter[portalIdx].z;

        // Set other properties
        gEnemy[enemyCount].rot = gCharacter[portalIdx].rot;
        gEnemy[enemyCount].radius = gEnemy[i].radius;
        gEnemy[enemyCount].height = gEnemy[i].height;
        gEnemy[enemyCount].health    = gEnemy[i].maxHealth / 2.0;
        gEnemy[enemyCount].maxHealth = gEnemy[i].maxHealth / 2.0;
        gEnemy[enemyCount].enemyIndex = gEnemy[i].enemyIndex;
        gEnemy[enemyCount].enemySpeed = gEnemy[i].enemySpeed * 1.2;
        gEnemy[enemyCount].k = gEnemy[i].k * 0.7;
        gEnemy[enemyCount].exist = TRUE;
        gEnemy[enemyCount].enemyType = 2;

        printf("Model %s loaded.\n", gEnemyDisplay[2]);
    }
}

 

void addBomb() {


    if ((bomb[0].model = mqoCreateModel(gBombDisplay[0], 1.0)) == NULL) {
        printf("Failed to load model.\n");
        return;
    }
    bomb[0].rot = gCharacter[0].rot;
    bomb[0].x = gCharacter[0].x + 20.0;
    bomb[0].y = gCharacter[0].y + 20.0;
    bomb[0].z = gCharacter[0].z;
    bomb[0].radius = 10.0;
    bomb[0].dirX = sin(gCharacter[0].rot * M_PI / 180);;
    bomb[0].dirY = -cos(gCharacter[0].rot * M_PI / 180);


    bomb[0].k = 1.0;
    bomb[0].exist = TRUE;
    bomb[0].active = 1;

    bomb[0].patt_found = TRUE;


}

void DeleteSelectModel(int SelectModel) {
    // Delete the current model

    int i;
    if (SelectModel < enemyCount) {
        for (i = SelectModel; i < enemyCount - 1; i++) {
            gEnemy[i] = gEnemy[i + 1];
        }
    }
    else {
        printf("Invalid Select Index\n");
    }
    // Clear the last index value
    gEnemy[enemyCount - 1].rot = NULL;
    gEnemy[enemyCount - 1].x = NULL;
    gEnemy[enemyCount - 1].y = NULL;
    gEnemy[enemyCount - 1].z = NULL;
    gEnemy[enemyCount - 1].k = NULL;
    gEnemy[enemyCount - 1].radius = NULL;
    gEnemy[enemyCount - 1].health = NULL;
    gEnemy[enemyCount - 1].height = NULL;
    gEnemy[enemyCount - 1].exist = NULL;

    enemyCount--;
    printf("SelectModel after deletion %d\n", SelectModel);
    printf("Current model deleted.\n");
}




void onMouseClick(int button, int state, int x, int y) {

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        addBomb();
        bomb[0].isBomb = TRUE;
        printf("right mouse clicked!\n");
    }
}


bool CheckRightShootingCollision(int projectileIndex, int model) {
    // Calculate the distance
    GLfloat dx = Right_projectiles[currentProjectile][projectileIndex].x - gEnemy[model].x;
    GLfloat dy = Right_projectiles[currentProjectile][projectileIndex].y - gEnemy[model].y;
    GLfloat distance = sqrt(dx * dx + dy * dy);

    GLfloat tRadius = Right_projectiles[currentProjectile][projectileIndex].radius + gEnemy[model].radius;

    bool touch = (distance <= tRadius);

    return touch;
}

bool CheckLeftShootingCollision(int projectileIndex, int model) {
    // Calculate the distance
    GLfloat dx = Left_projectiles[currentProjectile][projectileIndex].x - gEnemy[model].x;
    GLfloat dy = Left_projectiles[currentProjectile][projectileIndex].y - gEnemy[model].y;
    GLfloat distance = sqrt(dx * dx + dy * dy);

    GLfloat tRadius = Left_projectiles[currentProjectile][projectileIndex].radius + gEnemy[model].radius;

    bool touch = (distance <= tRadius);

    return touch;
}

bool CheckEnemyHitByLaser(int i) {
    // Direction vector of the laser
    float dirX = sin(gCharacter[0].rot * M_PI / 180);
    float dirY = -cos(gCharacter[0].rot * M_PI / 180);

    // Calculate the perpendicular distance from the enemy to the laser line

    float distance = (dirY * gEnemy[i].x + dirX * gEnemy[i].y + (gCharacter[0].y * dirY - gCharacter[0].x * dirX))
        / sqrt(dirX * dirX + dirY * dirY);

    if (distance <= gEnemy[i].radius) {
        printf("HIT HIT HIT \n");
         return true;
    }
    else {
        LazerLength = 1000.0;

        return false;
    };  // Assuming each enemy has a 'radius' property


}


void CheckEnemyDie(int i) {
    if (gEnemy[i].health <= 0)
    {
        DeleteSelectModel(i);
        score += 10;
        killCount++;

    }
}



void ExplodeBomb() {


    // Check and apply damage to enemies within the explosion radius
    for (int j = 0; j < enemyCount; j++) {
        GLfloat dx = bomb[0].x - gEnemy[j].x;
        GLfloat dy = bomb[0].y - gEnemy[j].y;
        GLfloat distance = sqrt(dx * dx + dy * dy);
        if (distance <= 1000.0) {
            gEnemy[j].health -= bombDamage;
            CheckEnemyDie(j);

            printf("Bomb activated\n");


            // bombDamage is the damage caused by the bomb
            // Additional logic for enemy defeat
        }
    }

    // Optional: Add explosion effect here

    // Deactivate the bomb
     // Deactivate the bomb after explosion
    bomb[0].active = 0;
    bomb[0].isBomb = FALSE;
}

void UpdateBombs() {
    if (bomb[0].active) {
        // Update bomb position
        bomb[0].x += 20.0 * bomb[0].dirX;
        bomb[0].y += 20.0 * bomb[0].dirY;

        for (int i = 0; i < enemyCount; i++) {
            GLfloat enemyRadius = gEnemy[i].radius;
            GLfloat bombRadius = bomb[0].radius;
            GLfloat dx = bomb[0].x - gEnemy[i].x;
            GLfloat dy = bomb[0].y - gEnemy[i].y;
            GLfloat distance = sqrt(dx * dx + dy * dy);
            if (distance <= bombRadius + enemyRadius) {
                printf("Target hit\n");
                printf("Distance = %f\n", distance);
                printf("enemyRadius = %f\n", enemyRadius);
                printf("bombRadius = %f\n", bombRadius);

                currentAnimation = 0;
                printf("--------------------------\n");
                ExplodeBomb();
                break;
            }
        }

    }

}



float distance(int model1, int model2) {
    float dx = gCharacter[model1].x - gCharacter[model2].x;
    float dy = gCharacter[model1].y - gCharacter[model2].y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance;
}



void MoveEnemyToCenter(int i, GLfloat enemySpeed) {
    GLfloat dx = gCharacter[1].x - gEnemy[i].x;
    GLfloat dy = gCharacter[1].y - gEnemy[i].y;
    GLfloat distance = sqrt(dx * dx + dy * dy);

    // Normalize the direction
    GLfloat dirX = dx / distance;
    GLfloat dirY = dy / distance;

    if (distance < gEnemy[i].radius + gCharacter[1].radius) {
        gCharacter[1].health -= 10.0;
        printf("ENEMY ENTER EARTH!!!\n");
        DeleteSelectModel(i);
        return; // Stop the function, so the enemy's position won't be updated further
    }
    else {
        // Move the enemy towards the center
        gEnemy[i].x += dirX * enemySpeed;
        gEnemy[i].y += dirY * enemySpeed;
    }




}

void ResetProjectiles(int i) {
    float angle_rad = gCharacter[0].rot * M_PI / 180;
    float offsetDistance = 25.0; // Distance from the character to the projectile

    // Calculate right offset
    float rightOffsetX = cos(angle_rad) * offsetDistance;
    float rightOffsetY = sin(angle_rad) * offsetDistance;

    // Calculate left offset (which is just the negative of the right offset)
    float leftOffsetX = -rightOffsetX;
    float leftOffsetY = -rightOffsetY;
    Right_projectiles[currentProjectile][i].x = gCharacter[0].x + rightOffsetX;
    Right_projectiles[currentProjectile][i].y = gCharacter[0].y + rightOffsetY;
    Right_projectiles[currentProjectile][i].rot = gCharacter[0].rot;
    Right_projectiles[currentProjectile][i].dirX = sin(angle_rad);
    Right_projectiles[currentProjectile][i].dirY = -cos(angle_rad);
    Right_projectiles[currentProjectile][i].active = 0;
    Right_projectiles[currentProjectile][i].exist = 0;
    Right_projectiles[currentProjectile][i].k = 0.0;


    Left_projectiles[currentProjectile][i].x = gCharacter[0].x + leftOffsetX;
    Left_projectiles[currentProjectile][i].y = gCharacter[0].y + leftOffsetY;
    Left_projectiles[currentProjectile][i].rot = gCharacter[0].rot;
    Left_projectiles[currentProjectile][i].dirX = sin(angle_rad);
    Left_projectiles[currentProjectile][i].dirY = -cos(angle_rad);
    Left_projectiles[currentProjectile][i].active = 0;
    Left_projectiles[currentProjectile][i].exist = 0;
    Left_projectiles[currentProjectile][i].k = 0.0;

}



void UpdateLaser() {
    LazerLength = 1000.0; // Default length if no enemy is hit

    // Direction vector of the laser
    float dirX = sin(gCharacter[0].rot * M_PI / 180);
    float dirY = -cos(gCharacter[0].rot * M_PI / 180);

    for (int i = 0; i < enemyCount; i++) {
        // Calculate the vector from the character to the enemy
        float toEnemyX = gEnemy[i].x - gCharacter[0].x;
        float toEnemyY = gEnemy[i].y - gCharacter[0].y;

        // Project this vector onto the laser direction vector
        float projection = (toEnemyX * dirX + toEnemyY * dirY) /
            (dirX * dirX + dirY * dirY);

        // Find the closest point on the laser line to the enemy
        float closestX = gCharacter[0].x + projection * dirX;
        float closestY = gCharacter[0].y + projection * dirY;

        // Calculate the distance from this point to the enemy
        float distanceToLine = sqrt(pow(closestX - gEnemy[i].x, 2) + pow(closestY - gEnemy[i].y, 2));
        // Check if the enemy is within the effective radius of the laser line
        if (distanceToLine <= gEnemy[i].radius && projection >= 0) {
            float directDistance = sqrt(pow(gEnemy[i].x - gCharacter[0].x, 2) + pow(gEnemy[i].y - gCharacter[0].y, 2));
            LazerLength = directDistance; // Update the laser length to the distance to the hit enemy


            gEnemy[i].health -= 10.0; // Apply damage
            printf("Laser hit enemy\n");
            if (gEnemy[i].enemyType == 3) {
                ActivateTeleporter(i);
            }
            CheckEnemyDie(i);
           
        }

    }
}
void UpdateLazerProjectiles() {
    float angle_rad = gCharacter[0].rot * M_PI / 180;
    Lazer_projectiles.x = gCharacter[0].x;
    Lazer_projectiles.y = gCharacter[0].y;
    Lazer_projectiles.rot = gCharacter[0].rot;
    Lazer_projectiles.dirX = sin(angle_rad);
    Lazer_projectiles.dirY = -cos(angle_rad);
}

void UpdateRightDamage() {
    int i, j;
    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (!Right_projectiles[currentProjectile][i].active) continue;
        for (j = 0; j < enemyCount; j++) {
            if (CheckRightShootingCollision(i, j)) {
                gEnemy[j].health -= shootingDamage * 20.0;

                if (gEnemy[j].health <= 0) {
                    DeleteSelectModel(j);
                    score += 10;
                    killCount++;
                    shootingDamage += 0.01;

                }
                ResetProjectiles(i);


            }
        }
    }
}


void UpdateLeftDamage() {
    int i, j;
    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (!Left_projectiles[currentProjectile][i].active) continue;
        for (j = 0; j < enemyCount; j++) {
            if (CheckLeftShootingCollision(i, j)) {
                gEnemy[j].health -= shootingDamage * 20.0;

                if (gEnemy[j].health <= 0) {
                    DeleteSelectModel(j);
                    score += 10;
                    killCount++;
                    shootingDamage += 0.01;

                }
                ResetProjectiles(i);

            }
        }
    }
}

void UpdateRLProjectiles() {
    float angle_rad = gCharacter[0].rot * M_PI / 180;
    float offsetDistance = 25.0; // Distance from the character to the projectile

    // Calculate right offset
    float rightOffsetX = cos(angle_rad) * offsetDistance;
    float rightOffsetY = sin(angle_rad) * offsetDistance;

    // Calculate left offset (which is just the negative of the right offset)
    float leftOffsetX = -rightOffsetX;
    float leftOffsetY = -rightOffsetY;
    // Reset the position of the current projectile
    if (Right_projectiles[currentProjectile][currentProjectileIndex].active == 0) {

        Right_projectiles[currentProjectile][currentProjectileIndex].x = gCharacter[0].x + rightOffsetX;
        Right_projectiles[currentProjectile][currentProjectileIndex].y = gCharacter[0].y + rightOffsetY;
        Right_projectiles[currentProjectile][currentProjectileIndex].rot = gCharacter[0].rot;
        Right_projectiles[currentProjectile][currentProjectileIndex].dirX = sin(angle_rad);
        Right_projectiles[currentProjectile][currentProjectileIndex].dirY = -cos(angle_rad);
        Right_projectiles[currentProjectile][currentProjectileIndex].k = 1.0;

        Right_projectiles[currentProjectile][currentProjectileIndex].active = 1;
        Right_projectiles[currentProjectile][currentProjectileIndex].exist = 1;

    }

    if (Left_projectiles[currentProjectile][currentProjectileIndex].active == 0) {

        Left_projectiles[currentProjectile][currentProjectileIndex].x = gCharacter[0].x + leftOffsetX;
        Left_projectiles[currentProjectile][currentProjectileIndex].y = gCharacter[0].y + leftOffsetY;
        Left_projectiles[currentProjectile][currentProjectileIndex].rot = gCharacter[0].rot;
        Left_projectiles[currentProjectile][currentProjectileIndex].dirX = sin(angle_rad);
        Left_projectiles[currentProjectile][currentProjectileIndex].dirY = -cos(angle_rad);

        Left_projectiles[currentProjectile][currentProjectileIndex].active = 1;
        Left_projectiles[currentProjectile][currentProjectileIndex].exist = 1;
        Left_projectiles[currentProjectile][currentProjectileIndex].k = 1.0;

    }


    // Update the position of all active projectiles
    for (int i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (Right_projectiles[currentProjectile][i].active) {

            Right_projectiles[currentProjectile][i].x += 10.0 * Right_projectiles[currentProjectile][i].dirX;
            Right_projectiles[currentProjectile][i].y += 10.0 * Right_projectiles[currentProjectile][i].dirY;
        }
    }
    // Update the position of all active projectiles
    for (int i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (Left_projectiles[currentProjectile][i].active) {

            Left_projectiles[currentProjectile][i].x += 10.0 * Left_projectiles[currentProjectile][i].dirX;
            Left_projectiles[currentProjectile][i].y += 10.0 * Left_projectiles[currentProjectile][i].dirY;
        }
    }

    // Increment the counter and update the current projectile index
    projectileCounter += 2;
    if (projectileCounter >= MAX_PROJECTILES_LOTS) {
        projectileCounter = 0;
        currentProjectileIndex = (currentProjectileIndex + 1) % MAX_PROJECTILES_LOTS;
        Right_projectiles[currentProjectile][currentProjectileIndex].active = 0; // Reset the next projectile
        Left_projectiles[currentProjectile][currentProjectileIndex].active = 0; // Reset the next projectile

    }
}



static void CalcState(void) {
    if (gameOver || gameWon) return;

    static GLfloat v1 = 0;		// Speed in direction of travel
    static GLfloat v2 = 0;		// Speed in direction of travel
    static const GLfloat MAX_SCALE = 10.0;
    static const GLfloat MIN_SCALE = 0.1;
    static const GLfloat MAX_HEIGHT = 1000.0;
    static const GLfloat MIN_HEIGHT = 10.0;
    static const GLfloat MAX_RADIUS = 1000.0;
    static const GLfloat MIN_RADIUS = 4.0;

    CONTROLLER_DATA control;	// Controller (keyboard)
    GetControllerData(&control);    // Get controller data



    //Player 1

    //-----------------------------------------------------------------------------------
    // Acceleration and deceleration
    //-----------------------------------------------------------------------------------

    if (control.up == KEY_DOWN) {
        v1 = modelSpeed;
    }
    else if (control.down == KEY_DOWN) {
        v1 = -modelSpeed;
    }
    else {
        if (v1 > 0) { v1--; }
        else if (v1 < 0) { v1++; }
    }
    gCharacter[0].x += v1 * sin(gCharacter[0].rot * M_PI / 180);
    gCharacter[0].y += -v1 * cos(gCharacter[0].rot * M_PI / 180);

    if (control.left == KEY_DOWN) { gCharacter[0].rot += rotSpeed; }
    if (control.right == KEY_DOWN) { gCharacter[0].rot -= rotSpeed; }




    if (control.Z == KEY_DOWN) {
        gCharacter[0].x = 0.0;
        gCharacter[0].y = 0.0;
        gCharacter[0].z = 0.0;
    }


    if (control.C == KEY_DOWN) {
        gCharacter[0].k *= 10;
    }



    count++;

    if (score < 100) {
        difficulty = 3.0;
    }
    else if (100 <= score < 200) {
        difficulty = 2.5;
    }
    else if (200 <= score < 500) {
        difficulty = 2.0;
    }
    else if (500 < score < 1000) {
        difficulty = 1.0;
    }
    else if (1000 < score < 2000) {
        difficulty = 0.5;
    }
    else {
        difficulty = 0.2;
    }

    for (int i = 0; i < enemyCount; i++) {
        if (gEnemy[i].enemyType == 4 && gEnemy[i].k == 1.0) {
            gEnemy[i].swammerCount++;
            if (gEnemy[i].swammerCount >= 10 * second) {
                ActivateSwarmer(i);
                gEnemy[i].swammerCount = 0;
            }
        }
    }


    if (enemyCount < 1) {
        if (count > difficulty * second) {
            int rand1 = rand() % 3 + 2;//for portals
            int rand2 = rand() % 5;//for enemies type
            addEnemyModel(rand2, rand1);

            count = 0;

        }
    }
    for (int i = 0; i < enemyCount; i++) {
        MoveEnemyToCenter(i, gEnemy[i].enemySpeed);
    }

    if (projectileType == 0) {
        UpdateRLProjectiles();
        UpdateRightDamage();
        UpdateLeftDamage();
    }

    UpdateLazerProjectiles();
    if (projectileType == 1) {
        UpdateLaser();
    }

    if (gCharacter[1].health <= 0) {
        gCharacter[1].health = 0;
        gameOver = true;
    }
    if (killCount >= KILL_GOAL) {
        gameWon = true;
    }

    if (bomb[0].isBomb) {
        UpdateBombs();
    }
 


}

void drawEnemyHealthBar(int model) {

    glTranslatef(gEnemy[model].x, gEnemy[model].y, gEnemy[model].z);
    glRotatef(gEnemy[model].rot, 0.0, 0.0, 1.0);
    glTranslatef(-gEnemy[model].x, -gEnemy[model].y, -gEnemy[model].z);

    GLfloat health     = gEnemy[model].health;
    GLfloat fullHealth = gEnemy[model].maxHealth;
    if (fullHealth <= 0.0f) return;

    // Calculate the dimensions of the health bar based on the health percentage
    GLfloat barWidth = (health / fullHealth) * 50.0; // 200.0 is an example width
    GLfloat barHeight = 5.0; // Adjust the height as needed

    // Set the position of the health bar at the center
    GLfloat x = gEnemy[model].x - barWidth / 2.0;
    GLfloat y = gEnemy[model].y + 1.3;
    GLfloat z = gEnemy[model].z + gEnemy[model].height;
    // Rotate the health bar back 90 degrees vertically

     // Draw the gray background as a rectangle
    glBegin(GL_QUADS);
    glVertex3f(x, y, z); // Top-left
    glVertex3f(x + 50.0, y, z); // Top-right
    glVertex3f(x + 50.0, y + barHeight, z); // Bottom-right
    glVertex3f(x, y + barHeight, z); // Bottom-left
    glEnd();

    // Set the color for the red foreground
    glColor3f(1.0f, 0.0f, 0.0f); // Red color (RGB: 1, 0, 0)

    // Draw the red foreground as a rectangle based on the health percentage
    glBegin(GL_QUADS);
    glVertex3f(x, y, z); // Top-left
    glVertex3f(x + barWidth, y, z); // Top-right
    glVertex3f(x + barWidth, y + barHeight, z); // Bottom-right
    glVertex3f(x, y + barHeight, z); // Bottom-left
    glEnd();

}




//draw cylinder with radius and height
void drawCylinderAround(int model) {
    // Draw the cylinder around the character model
    GLfloat height = gCharacter[model].height; // height of the cylinder
    GLfloat radius = gCharacter[model].radius;
    float x = gCharacter[model].x;
    float y = gCharacter[model].y;
    float z = gCharacter[model].z;

    //printf("draw with radius = %f\n", radius);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Color with alpha (transparency)

    // Drawing the bottom circle (filled)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, 0.0, z); // vertices of the bottom circle
    }
    glEnd();

    // Drawing the top circle (outline only)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, height, z); // vertices of the top circle
    }
    glEnd();



    // Drawing the 4 lines (front, back, left, right)
    glBegin(GL_LINES);
    // front
    glVertex3f(0.0f, height, radius);
    glVertex3f(0.0f, 0.0, radius);
    // back
    glVertex3f(0.0f, height, -radius);
    glVertex3f(0.0f, 0.0 / 2.0f, -radius);
    // left
    glVertex3f(-radius, height, 0.0f);
    glVertex3f(-radius, 0.0 / 2.0f, 0.0f);
    // right
    glVertex3f(radius, height, 0.0f);
    glVertex3f(radius, 0.0 / 2.0f, 0.0f);
    glEnd();

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

}


//draw cylinder with radius and height
void drawCylinderAroundEnemy(int model) {
    // Draw the cylinder around the character model
    GLfloat height = gEnemy[model].height; // height of the cylinder
    GLfloat radius = gEnemy[model].radius;
    float x = gEnemy[model].x;
    float y = gEnemy[model].y;
    float z = gEnemy[model].z;

    //printf("draw with radius = %f\n", radius);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Color with alpha (transparency)

    // Drawing the bottom circle (filled)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, 0.0, z); // vertices of the bottom circle
    }
    glEnd();

    // Drawing the top circle (outline only)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, height, z); // vertices of the top circle
    }
    glEnd();



    // Drawing the 4 lines (front, back, left, right)
    glBegin(GL_LINES);
    // front
    glVertex3f(0.0f, height, radius);
    glVertex3f(0.0f, 0.0, radius);
    // back
    glVertex3f(0.0f, height, -radius);
    glVertex3f(0.0f, 0.0 / 2.0f, -radius);
    // left
    glVertex3f(-radius, height, 0.0f);
    glVertex3f(-radius, 0.0 / 2.0f, 0.0f);
    // right
    glVertex3f(radius, height, 0.0f);
    glVertex3f(radius, 0.0 / 2.0f, 0.0f);
    glEnd();

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

}


//draw cylinder with radius and height
void drawCylinderAroundRightProjectile(int model) {
    // Draw the cylinder around the character model
    GLfloat height = Right_projectiles[currentProjectile][model].height; // height of the cylinder
    GLfloat radius = Right_projectiles[currentProjectile][model].radius;
    float x = Right_projectiles[currentProjectile][model].x;
    float y = Right_projectiles[currentProjectile][model].y;
    float z = Right_projectiles[currentProjectile][model].z;

    //printf("draw with radius = %f\n", radius);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Color with alpha (transparency)

    // Drawing the bottom circle (filled)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, 0.0, z); // vertices of the bottom circle
    }
    glEnd();

    // Drawing the top circle (outline only)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, height, z); // vertices of the top circle
    }
    glEnd();



    // Drawing the 4 lines (front, back, left, right)
    glBegin(GL_LINES);
    // front
    glVertex3f(0.0f, height, radius);
    glVertex3f(0.0f, 0.0, radius);
    // back
    glVertex3f(0.0f, height, -radius);
    glVertex3f(0.0f, 0.0 / 2.0f, -radius);
    // left
    glVertex3f(-radius, height, 0.0f);
    glVertex3f(-radius, 0.0 / 2.0f, 0.0f);
    // right
    glVertex3f(radius, height, 0.0f);
    glVertex3f(radius, 0.0 / 2.0f, 0.0f);
    glEnd();

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

}

void drawCylinderAroundLeftProjectile(int model) {
    // Draw the cylinder around the character model
    GLfloat height = Left_projectiles[currentProjectile][model].height; // height of the cylinder
    GLfloat radius = Left_projectiles[currentProjectile][model].radius;
    float x = Left_projectiles[currentProjectile][model].x;
    float y = Left_projectiles[currentProjectile][model].y;
    float z = Left_projectiles[currentProjectile][model].z;

    //printf("draw with radius = %f\n", radius);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Color with alpha (transparency)

    // Drawing the bottom circle (filled)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, 0.0, z); // vertices of the bottom circle
    }
    glEnd();

    // Drawing the top circle (outline only)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, height, z); // vertices of the top circle
    }
    glEnd();



    // Drawing the 4 lines (front, back, left, right)
    glBegin(GL_LINES);
    // front
    glVertex3f(0.0f, height, radius);
    glVertex3f(0.0f, 0.0, radius);
    // back
    glVertex3f(0.0f, height, -radius);
    glVertex3f(0.0f, 0.0 / 2.0f, -radius);
    // left
    glVertex3f(-radius, height, 0.0f);
    glVertex3f(-radius, 0.0 / 2.0f, 0.0f);
    // right
    glVertex3f(radius, height, 0.0f);
    glVertex3f(radius, 0.0 / 2.0f, 0.0f);
    glEnd();

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

}


//draw cylinder with radius and height
void drawCylinderAroundBomb(int model) {
    // Draw the cylinder around the character model
    GLfloat height = bomb[0].height; // height of the cylinder
    GLfloat radius = bomb[0].radius;
    float x = bomb[0].x;
    float y = bomb[0].y;
    float z = bomb[0].z;

    //printf("draw with radius = %f\n", radius);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Color with alpha (transparency)

    // Drawing the bottom circle (filled)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, 0.0, z); // vertices of the bottom circle
    }
    glEnd();

    // Drawing the top circle (outline only)
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j <= 360; j += 10) {
        GLfloat angle = j * (M_PI / 180.0f);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);
        glVertex3f(x, height, z); // vertices of the top circle
    }
    glEnd();



    // Drawing the 4 lines (front, back, left, right)
    glBegin(GL_LINES);
    // front
    glVertex3f(0.0f, height, radius);
    glVertex3f(0.0f, 0.0, radius);
    // back
    glVertex3f(0.0f, height, -radius);
    glVertex3f(0.0f, 0.0 / 2.0f, -radius);
    // left
    glVertex3f(-radius, height, 0.0f);
    glVertex3f(-radius, 0.0 / 2.0f, 0.0f);
    // right
    glVertex3f(radius, height, 0.0f);
    glVertex3f(radius, 0.0 / 2.0f, 0.0f);
    glEnd();

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

}
//---------------------------------------------------------------------------------- -
//Drawing Function
//-----------------------------------------------------------------------------------

void drawGlowEffect(float x, float y, float z, float baseRadius) {
    GLUquadric* quadric = gluNewQuadric();

    glPushMatrix();
    glTranslatef(x, y, z);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // White core
    glColor4f(1.0, 1.0, 1.0, 1.0); // White core
    gluSphere(quadric, baseRadius * 1.2, 32, 32);

    // First blue outer layer
    glColor4f(0.5, 0.5, 1.0, 0.4); // Light blue outer glow
    gluSphere(quadric, baseRadius * 1.5, 32, 32);

    // Second blue outer layer
    glColor4f(0.4, 0.4, 0.9, 0.4); // More transparent blue
    gluSphere(quadric, baseRadius * 1.8, 32, 32);

    // "Shatter-like" effect
    glColor4f(0.4, 0.4, 0.9, 0.3); // Slightly transparent for shatter effect
    glBegin(GL_LINES);
    for (int i = 0; i < 20; i++) { // Number of "shatter" lines
        float angle = 2.0 * M_PI * i / 20;
        glVertex3f(0, 0, 0); // Center of the sphere
        glVertex3f(baseRadius * 2 * cos(angle), baseRadius * 2 * sin(angle), 0);
    }
    glEnd();

    gluDeleteQuadric(quadric);
    glDisable(GL_BLEND);
    glPopMatrix();
}
void drawLaserBeam(float radius, float length) {
    GLUquadric* quadric = gluNewQuadric();

    // Laser cylinder
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0, 1.0, 1.0, 1.0); // White core
    gluCylinder(quadric, radius * 0.6, radius * 0.6, length, 32, 1);
    glColor4f(0.5, 0.5, 1.0, 0.4); // Light blue outer glow
    gluCylinder(quadric, radius, radius, length, 32, 1);
    gluDeleteQuadric(quadric);
    glDisable(GL_BLEND);

    // Draw the glow effect at the end of the laser
     if (LazerLength<1000.0) {
        drawGlowEffect(0, 0, length, radius * 1.2);
    }
}




static void resetGame(void) {
    // Clear all enemies
    for (int i = 0; i < enemyCount; i++)
        mqoDeleteModel(gEnemy[i].model);
    enemyCount = 0;

    // Reset projectiles
    for (int i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        Right_projectiles[currentProjectile][i].active = 0;
        Right_projectiles[currentProjectile][i].exist  = 0;
        Right_projectiles[currentProjectile][i].k      = 0.0;
        Left_projectiles[currentProjectile][i].active  = 0;
        Left_projectiles[currentProjectile][i].exist   = 0;
        Left_projectiles[currentProjectile][i].k       = 0.0;
    }

    // Reset bomb
    bomb[0].active   = 0;
    bomb[0].isBomb   = FALSE;
    bomb[0].patt_found = FALSE;

    // Reset Earth health and player position
    gCharacter[1].health = 100.0;
    gCharacter[0].x = 0.0; gCharacter[0].y = 0.0; gCharacter[0].z = 0.0;
    gCharacter[0].rot = 0.0;

    // Reset game state
    score         = 0;
    killCount     = 0;
    shootingDamage = 1.0;
    count         = 0;
    gameOver      = false;
    gameWon       = false;
}

static void drawBitmapString(int x, int y, const char *str) {
    glWindowPos2i(x, y);
    for (const char *c = str; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

static void DrawHUD(void) {
    if (!showHUD) return;

    // Semi-transparent background quad (uses ortho coords)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, xsize, 0, ysize, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
    glVertex2i(5,   ysize - 15);
    glVertex2i(330, ysize - 15);
    glVertex2i(330, ysize - 268);
    glVertex2i(5,   ysize - 268);
    glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    // Text lines — glWindowPos2i bypasses transforms, uses pixel coords
    int x = 12, y = ysize - 32, lineH = 18;
    glColor3f(1.0f, 1.0f, 0.0f);
    drawBitmapString(x, y, "=== KEY COMMANDS ===");          y -= lineH;
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapString(x, y, "Arrow UP/DOWN : Move forward/back"); y -= lineH;
    drawBitmapString(x, y, "Arrow L/R     : Rotate");            y -= lineH;
    drawBitmapString(x, y, "Z             : Reset position");    y -= lineH;
    drawBitmapString(x, y, "C             : Scale up x10");      y -= lineH;
    drawBitmapString(x, y, "1             : Weapon - Bullets");  y -= lineH;
    drawBitmapString(x, y, "2             : Weapon - Laser");    y -= lineH;
    drawBitmapString(x, y, "Right-click   : Launch bomb");       y -= lineH;
    drawBitmapString(x, y, "H             : Toggle hitboxes");    y -= lineH;
    drawBitmapString(x, y, "I             : Enemy debug info");  y -= lineH;
    drawBitmapString(x, y, "Shift+D       : Toggle this HUD");   y -= lineH;
    drawBitmapString(x, y, "R             : Restart game");       y -= lineH;
    drawBitmapString(x, y, "ESC           : Quit");              y -= lineH;
    glColor3f(0.0f, 1.0f, 0.5f);
    drawBitmapString(x, y, (projectileType == 0) ? "WEAPON: Bullets" : "WEAPON: Laser");
}

// Always-on status bar: weapon + kill counter
static void DrawAlwaysHUD(void) {
    char buf[64];
    int lineH = 18, x = 10, y = ysize - 20;

    // Background strip
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, xsize, 0, ysize, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2i(0, ysize);       glVertex2i(xsize, ysize);
    glVertex2i(xsize, ysize - lineH * 2 - 8); glVertex2i(0, ysize - lineH * 2 - 8);
    glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    // Weapon label
    glColor3f(0.0f, 1.0f, 0.5f);
    drawBitmapString(x, y, (projectileType == 0) ? "WEAPON: Bullets" : "WEAPON: Laser");

    // Kill counter
    snprintf(buf, sizeof(buf), "Kills: %d / %d", killCount, KILL_GOAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    // right-align the kills string
    int strW = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)buf);
    drawBitmapString(xsize - strW - 10, y, buf);
}

static void drawCenteredLarge(const char *str, int y, float r, float g, float b) {
    int w = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)str);
    int x = (xsize - w) / 2;
    glColor3f(r, g, b);
    glWindowPos2i(x, y);
    for (const char *c = str; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
}

static void drawCenteredSmall(const char *str, int y, float r, float g, float b) {
    int w = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)str);
    int x = (xsize - w) / 2;
    glColor3f(r, g, b);
    drawBitmapString(x, y, str);
}

static void DrawGameOverScreen(void) {
    if (!gameOver) return;

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, xsize, 0, ysize, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.72f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0); glVertex2i(xsize, 0);
    glVertex2i(xsize, ysize); glVertex2i(0, ysize);
    glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    int cy = ysize / 2;
    drawCenteredLarge("GAME OVER",         cy + 40, 1.0f, 0.15f, 0.15f);
    drawCenteredSmall("Earth has fallen!", cy + 10, 1.0f, 1.0f, 1.0f);
    drawCenteredSmall("Press R to restart", cy - 20, 0.9f, 0.9f, 0.3f);
}

static void DrawGameWonScreen(void) {
    if (!gameWon) return;

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, xsize, 0, ysize, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.1f, 0.0f, 0.72f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0); glVertex2i(xsize, 0);
    glVertex2i(xsize, ysize); glVertex2i(0, ysize);
    glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    char buf[64];
    int cy = ysize / 2;
    drawCenteredLarge("YOU WIN!",           cy + 40, 0.2f, 1.0f, 0.3f);
    snprintf(buf, sizeof(buf), "%d enemies destroyed!", killCount);
    drawCenteredSmall(buf,                  cy + 10, 1.0f, 1.0f, 1.0f);
    drawCenteredSmall("Press R to restart", cy - 20, 0.9f, 0.9f, 0.3f);
}

static void draw(void)
{
    int i;
    int animationDelay = 1;

    double    gl_para[16];
    argDrawMode3D();
    argDraw3dCamera(0, 0);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    CalcState();


    /* load the camera transformation matrix */
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(gl_para);
    glClear(GL_DEPTH_BUFFER_BIT);			// Zバッファの初期化


    // 光源の設定（視点に対して固定）
    GLfloat material_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat material_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat material_shininess = 2.0;







    // Drawing models

    for (i = modelCount - 1; i >= 0; i--) {
        if (gCharacter[i].patt_found) {
            argConvGlpara(gCharacter[i].trans, gl_para);

            glLoadMatrixd(gl_para);


            glPushMatrix();

            glTranslatef(gCharacter[i].x, gCharacter[i].y, gCharacter[i].z);	// モデルの位置

            glRotatef(gCharacter[i].rot, 0.0, 0.0, 1.0);	// モデルの向き
            glRotatef(gCharacter[i].vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
            glRotatef(90.0, 1.0, 0.0, 0.0);

            if (showHitbox) drawCylinderAround(i);

            glScalef(gCharacter[i].k, gCharacter[i].k, gCharacter[i].k);

            if (i == 1) {
                gCharacter[1].rot += ROTATION_SPEED;
                if (gCharacter[1].rot >= 360.0) {
                    gCharacter[1].rot -= 360.0; // Reset rotation to keep it within 0-360 degrees
                }
            }



            mqoCallModel(gCharacter[i].model);


        }
    }


    for (i = 0; i < enemyCount; i++) {
        if (gEnemy[i].patt_found) {
            argConvGlpara(gEnemy[i].trans, gl_para);

            glLoadMatrixd(gl_para);


            glPushMatrix();

            glTranslatef(gEnemy[i].x, gEnemy[i].y, gEnemy[i].z);	// モデルの位置

            glRotatef(gEnemy[i].rot, 0.0, 0.0, 1.0);	// モデルの向き
            glRotatef(gEnemy[i].vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
            glRotatef(90.0, 1.0, 0.0, 0.0);

            if (showHitbox) drawCylinderAroundEnemy(i);

            glScalef(gEnemy[i].k, gEnemy[i].k, gEnemy[i].k);

            mqoCallModel(gEnemy[i].model);

            if (gEnemy[i].maxHealth > 1.0f) {
                glPushMatrix();
                argConvGlpara(gEnemy[i].trans, gl_para);
                glLoadMatrixd(gl_para);
                drawEnemyHealthBar(i);
                glPopMatrix();
            }









        }
    }



    if (bomb[0].patt_found) {
        argConvGlpara(bomb[0].trans, gl_para);

        glLoadMatrixd(gl_para);


        glPushMatrix();

        glTranslatef(bomb[0].x, bomb[0].y, bomb[0].z);	// モデルの位置

        glRotatef(bomb[0].rot, 0.0, 0.0, 1.0);	// モデルの向き
        glRotatef(bomb[0].vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
        glRotatef(90.0, 1.0, 0.0, 0.0);

        if (showHitbox) drawCylinderAroundBomb(0);


        glScalef(bomb[0].k, bomb[0].k, bomb[0].k);

        if (currentAnimation != -1) {
            // printf("currentAnimation %d\n", currentAnimation);
            // printf("CurrentFrame is %d\n", currentFrame);
           //  printf("delayTime is %d\n", delayTime);


            mqoCallSequence(animations[currentAnimation].sequence, currentFrame);
            //printf("Print delayTime %d\n", delayTime);
            //printf("Print current frame %d\n", currentFrame);



            delayTime++;
            if (delayTime >= animationDelay) {
                // Update the frame every other frame
                currentFrame = (currentFrame + 1) % animations[currentAnimation].n_frame;
                delayTime = 0;

            }
            if (currentFrame == 0 && delayTime == 0) {
                currentAnimation = -1;
                bomb[0].patt_found = FALSE;
                bomb[0].exist = FALSE;


            }


        }
        else {
            mqoCallModel(bomb[0].model);
        }

    }


    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (Right_projectiles[currentProjectile][i].patt_found) {
            argConvGlpara(Right_projectiles[currentProjectile][i].trans, gl_para);

            glLoadMatrixd(gl_para);


            glPushMatrix();

            glTranslatef(Right_projectiles[currentProjectile][i].x, Right_projectiles[currentProjectile][i].y, Right_projectiles[currentProjectile][i].z);	// モデルの位置

            glRotatef(Right_projectiles[currentProjectile][i].rot, 0.0, 0.0, 1.0);	// モデルの向き
            glRotatef(Right_projectiles[currentProjectile][i].vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
            glRotatef(90.0, 1.0, 0.0, 0.0);



            glScalef(Right_projectiles[currentProjectile][i].k, Right_projectiles[currentProjectile][i].k, Right_projectiles[currentProjectile][i].k);

            mqoCallModel(Right_projectiles[currentProjectile][i].model);
        }
    }


    for (i = 0; i < MAX_PROJECTILES_LOTS; i++) {
        if (Left_projectiles[currentProjectile][i].patt_found) {
            argConvGlpara(Left_projectiles[currentProjectile][i].trans, gl_para);

            glLoadMatrixd(gl_para);


            glPushMatrix();

            glTranslatef(Left_projectiles[currentProjectile][i].x, Left_projectiles[currentProjectile][i].y, Left_projectiles[currentProjectile][i].z);	// モデルの位置

            glRotatef(Left_projectiles[currentProjectile][i].rot, 0.0, 0.0, 1.0);	// モデルの向き
            glRotatef(Left_projectiles[currentProjectile][i].vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
            glRotatef(90.0, 1.0, 0.0, 0.0);



            glScalef(Left_projectiles[currentProjectile][i].k, Left_projectiles[currentProjectile][i].k, Left_projectiles[currentProjectile][i].k);

            mqoCallModel(Left_projectiles[currentProjectile][i].model);
        }
    }


    if (Lazer_projectiles.patt_found) {
        argConvGlpara(Lazer_projectiles.trans, gl_para);

        glLoadMatrixd(gl_para);


        glPushMatrix();

        glTranslatef(Lazer_projectiles.x, Lazer_projectiles.y, Lazer_projectiles.z);	// モデルの位置

        glRotatef(Lazer_projectiles.rot, 0.0, 0.0, 1.0);	// モデルの向き
        glRotatef(Lazer_projectiles.vrot, 1.0, 0.0, 0.0);	// モデルの向き (vertical rotation)
        glRotatef(90.0, 1.0, 0.0, 0.0);



        glScalef(Lazer_projectiles.k, Lazer_projectiles.k, Lazer_projectiles.k);

        drawLaserBeam(5.0, LazerLength);
    }

    // printf("backgroundCount = %d\n", backgroundCount);


    glDisable(GL_DEPTH_TEST);
    DrawAlwaysHUD();
    DrawHUD();
    DrawGameOverScreen();
    DrawGameWonScreen();
    argSwapBuffers(); // Swap buffers to display the drawn frame.



}


void checkData2() {
    int i;
    for (int i = 0; i < enemyCount; i++) {

        printf("--------------------------------------------\n");
        printf("%d\t%-10.2f\t%s\t\n",
            i,
            gEnemy[i].health,
            gEnemy[i].model);

        printf("---------------------------------------------\n");

    }
}



//-----------------------------------------------------------------------------------
//KEYBOARD KEYS CONTROLLER
//-----------------------------------------------------------------------------------


void KeyDown(unsigned char key, int x, int y)
{

    switch (key) {
    case 'i':
        checkData2();
        break;
    case 'r':   resetGame(); break;
    case '1':   projectileType = 0; break;  // Bullets
    case 'h':   showHitbox = !showHitbox; break;
    case '2':                               // Laser — clear any bullets still in flight
        for (int _i = 0; _i < MAX_PROJECTILES_LOTS; _i++) {
            Right_projectiles[currentProjectile][_i].active = 0;
            Right_projectiles[currentProjectile][_i].exist  = 0;
            Right_projectiles[currentProjectile][_i].k      = 0.0;
            Left_projectiles[currentProjectile][_i].active  = 0;
            Left_projectiles[currentProjectile][_i].exist   = 0;
            Left_projectiles[currentProjectile][_i].k       = 0.0;
        }
        projectileType = 1;
        break;
    case KEY_A:		g_cntl.A = KEY_DOWN;	break;
    case KEY_B:		g_cntl.B = KEY_DOWN;	break;

    case KEY_S:		g_cntl.S = KEY_DOWN;	break;
    case KEY_D:		g_cntl.D = KEY_DOWN;	break;
    case KEY_W:		g_cntl.W = KEY_DOWN;	break;
    case KEY_X:		g_cntl.X = KEY_DOWN;	break;
    case KEY_Z:		g_cntl.Z = KEY_DOWN;	break;
    case KEY_C:		g_cntl.C = KEY_DOWN;	break;
    case KEY_N:		g_cntl.N = KEY_DOWN;	break;
    case KEY_P:		g_cntl.P = KEY_DOWN;	break;
    case KEY_SPACE:	g_cntl.SPACE = KEY_DOWN;	break;
    case KEY_AA:		g_cntl.AA = KEY_DOWN;	break;
    case KEY_WW:		g_cntl.WW = KEY_DOWN;	break;
    case KEY_SS:		g_cntl.SS = KEY_DOWN;	break;
    case KEY_DD:		g_cntl.DD = KEY_DOWN; showHUD = !showHUD; break;


    default:	break;
    }
}
void KeyUp(unsigned char key, int x, int y)
{
    switch (key) {
    case KEY_ESC:	exit(0);				break;	// ESC
    case KEY_A:     g_cntl.A = KEY_UP;		break;
    case KEY_B:     g_cntl.B = KEY_UP;		break;

    case KEY_S:		g_cntl.S = KEY_UP;		break;
    case KEY_D:		g_cntl.D = KEY_UP;		break;
    case KEY_W:		g_cntl.W = KEY_UP;		break;
    case KEY_X:		g_cntl.X = KEY_UP;		break;
    case KEY_Z:		g_cntl.Z = KEY_UP;		break;
    case KEY_C:		g_cntl.C = KEY_UP;		break;
    case KEY_O:		g_cntl.O = KEY_UP;		break;
    case KEY_N:		g_cntl.N = KEY_UP;		break;
    case KEY_P:		g_cntl.P = KEY_UP;		break;
    case KEY_SPACE:	g_cntl.SPACE = KEY_UP;	break;
    case KEY_AA:		g_cntl.AA = KEY_UP;	break;
    case KEY_WW:		g_cntl.WW = KEY_UP;	break;
    case KEY_SS:		g_cntl.SS = KEY_UP;	break;
    case KEY_DD:		g_cntl.DD = KEY_UP;	break;

    default:	break;
    }
}
void SpecialKeyDown(int key, int x, int y)
{
    switch (key) {

    case GLUT_KEY_UP:		g_cntl.up = KEY_DOWN;	break;
    case GLUT_KEY_DOWN:		g_cntl.down = KEY_DOWN;	break;
    case GLUT_KEY_LEFT:		g_cntl.left = KEY_DOWN;	break;
    case GLUT_KEY_RIGHT:	g_cntl.right = KEY_DOWN;	break;
    default:	break;
    }
}



void SpecialKeyUp(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:		g_cntl.up = KEY_UP;	break;
    case GLUT_KEY_DOWN:		g_cntl.down = KEY_UP;	break;
    case GLUT_KEY_LEFT:		g_cntl.left = KEY_UP;	break;
    case GLUT_KEY_RIGHT:	g_cntl.right = KEY_UP;	break;
    default:	break;
    }
}

