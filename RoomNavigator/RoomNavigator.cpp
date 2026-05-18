// ============================================================================
//   Room Navigator — Linux port of Progress1206.cpp
//   Multi-room AR environment: walk a main character through rooms via doors.
//   Static-image AR using cv::imread(), same pattern as EarthDefender.
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include "GL/controller.h"
#include "GL/GLMetaseq.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>
#include <time.h>
#include <vector>

// ============================================================================
// Type definitions
// ============================================================================

static CONTROLLER_DATA g_cntl = {
    KEY_UP, KEY_UP, KEY_UP, KEY_UP,
    KEY_UP, KEY_UP, KEY_UP, KEY_UP,
    KEY_UP, KEY_UP, KEY_UP, KEY_UP,
    KEY_UP, KEY_UP, KEY_UP, KEY_UP,
    KEY_UP, KEY_UP, KEY_UP, KEY_UP
};

#define MAX_MODELS 100

typedef enum {
    isNull,
    isDomain,
    isDoor,
    isFurniture,
    isInitPos
} ModelType;
ModelType addModelType;

typedef struct {
    int doorID;
    int numDoor;
    int goTo;
} DoorData;

typedef struct {
    int furnitureID;
} FurnitureData;

typedef struct {
    int domainID;
} DomainData;

typedef struct {
    MQO_MODEL   model;
    GLfloat     rot;
    GLfloat     ori_rot;
    GLfloat     vrot, ori_vrot;
    GLfloat     x, y, z;
    GLfloat     ori_x, ori_y, ori_z;
    GLfloat     k, ori_k;
    GLfloat     radius, ori_radius;
    GLfloat     height, ori_height;
    GLfloat     offset_x, offset_y, offset_z;

    double      trans[3][4];
    int         pattId;
    int         patt_found;
    bool        exist;
    bool        showMe;

    ModelType   modelType;
    union {
        DomainData    domainData;
        DoorData      doorData;
        FurnitureData furnitureData;
    };

    bool        inContact;
    int         isAttack;
    float       health;
    bool        isDead;

    GLfloat     zStandOn;
    bool        isJumping;
} CHARACTER_DATA;

// ============================================================================
// Global variables
// ============================================================================

// Window / view
static const int    gWindowed         = TRUE;
static const int    gFullWidth        = 800;
static const int    gFullHeight       = 600;
static const double gViewScaleFactor  = 1.0;
static const double gViewDistanceMin  = 0.1;
static const double gViewDistanceMax  = 10000.0;

// Camera / pattern
static char cparam_name[] = "Data/camera_para.dat";
static char patt_name[]   = "Data/patt/patt.hiro";
int         xsize, ysize;
int         thresh = 100;
int         count  = 0;
ARParam     cparam;
int         patt_id;
double      patt_width     = 80.0;
double      patt_center[2] = { 0.0, 0.0 };
double      patt_trans[3][4];

// AR rendering
static ARUint8*                  gARTImage    = NULL;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;
static cv::Mat                   frameRGBA;    // global — keeps gARTImage valid

// Models
#define NUM_MODEL 37

static char gMainModelDisplay[100] = { "Data/mqoFile/stickMan.mqo" };
static char gModelDisplay[MAX_MODELS][100];

static char ModelName[NUM_MODEL][100] = {
    "goalNet", "Barricade", "RedMan", "roundTable", "bench",
    "cubes", "tunnel", "cone", "ball", "car2",
    "car3", "character", "invisible", "BlackHair", "GoldKnight",
    "sofar", "clock", "car", "table", "mouse",
    "monitor", "mug", "drawer", "bed", "door",
    "window", "TV", "human", "bicycle", "tree",
    "desk", "rug", "keyboard", "wall", "floor",
    "chair", "street"
};
static char gModelName[NUM_MODEL][100] = {
    "Data/mqoFile/goalNet.mqo",    "Data/mqoFile/barricade.mqo",
    "Data/mqoFile/theRed.mqo",     "Data/mqoFile/roundTable.mqo",
    "Data/mqoFile/bench.mqo",      "Data/mqoFile/cubes.mqo",
    "Data/mqoFile/tunnel.mqo",     "Data/mqoFile/cone.mqo",
    "Data/mqoFile/ball.mqo",       "Data/mqoFile/jeep2.mqo",
    "Data/mqoFile/jeep3.mqo",      "Data/mqoFile/character.mqo",
    "Data/mqoFile/invisible.mqo",  "Data/mqoFile/BlackHair.mqo",
    "Data/mqoFile/GoldKnight.mqo", "Data/mqoFile/sofa.mqo",
    "Data/mqoFile/TableClock.mqo", "Data/mqoFile/jeep.mqo",
    "Data/mqoFile/table.mqo",      "Data/mqoFile/mouse.mqo",
    "Data/mqoFile/monitor.mqo",    "Data/mqoFile/mug.mqo",
    "Data/mqoFile/drawer.mqo",     "Data/mqoFile/bed.mqo",
    "Data/mqoFile/door.mqo",       "Data/mqoFile/window.mqo",
    "Data/mqoFile/Tele.mqo",       "Data/mqoFile/human.mqo",
    "Data/mqoFile/bicycle.mqo",    "Data/mqoFile/tree.mqo",
    "Data/mqoFile/desk.mqo",       "Data/mqoFile/rug.mqo",
    "Data/mqoFile/keyboard.mqo",   "Data/mqoFile/wall.mqo",
    "Data/mqoFile/floor.mqo",      "Data/mqoFile/chair.mqo",
    "Data/mqoFile/street.mqo"
};

// Characters
static CHARACTER_DATA gCharacter[MAX_MODELS];
static CHARACTER_DATA gMainCharacter;
GLfloat gCharacter_radius = 10.0;
GLfloat gModel_radius     = 80.0;
GLfloat gCharacter_height = 40.0;
GLfloat gModel_height     = 150.0;

// Scene state
int modelCount     = 0;
int SelectModel    = -1;
bool ControlMode   = false;
bool showHUD       = false;
bool contact       = false;
float modelSpeed   = 3.0;
float rotSpeed     = 5.0;
bool modelsLoaded  = false;
bool isLoading     = false;
bool printed       = false;

// Groups / environments
#define MAX_GROUP_NUM           50
#define MAX_FILENAME_LENGTH     100
#define MAX_CHARACTERS_IN_GROUP 20
#define NUM_ENV                 100
#define MAX_FILENAME_LEN        100

typedef struct {
    CHARACTER_DATA envChar[MAX_CHARACTERS_IN_GROUP];
    int numChar;
} ENV_MODELS;

typedef struct Environment {
    int envIndex;
    char name[MAX_FILENAME_LEN];
    char imageFileName[MAX_FILENAME_LEN];
    char envModelFileName[MAX_FILENAME_LEN];
    struct Environment** childEnvironments;
    struct Environment* parentEnvironment;
    int numChildren;
    int childIndex;
    int numDoor;
    int initPos;
    ENV_MODELS envModel;
} Environment;

Environment  gEnvironment[NUM_ENV];
Environment* currentEnvironment;
int          envIndex = 0;

// Animations
typedef struct {
    MQO_SEQUENCE sequence;
    int n_frame;
} AnimationData;

#define NUM_ANIMATIONS  8  // +2 for push, pull
#define NUM_ANIMATIONS2 1

int SelectedAnimation  = 0;
AnimationData animations[NUM_ANIMATIONS];
AnimationData animations2[NUM_ANIMATIONS2];
int currentAnimation  = -1;
static int currentFrame = 0;
int delayTime  = 0;
int currentAnimation2 = -1;
static int currentFrame2 = -1;
int delayTime2 = 0;

// ============================================================================
// Forward declarations
// ============================================================================
static void init(void);
static void cleanup(void);
static void Idle(void);
static void Display(void);
static void draw(void);
static void CalcMainState(void);
static void CalcState(void);
void loadAnimation(void);
void loadAnimation2(void);
void onMouseClick(int button, int state, int x, int y);
void loadEnvModel(const char filename[], int envIdx);
void saveEnvModel(const char filename[]);
void deleteSelectedModel(void);
void deleteEnvModels(Environment* env, int envIdx);
void createEnvironment(Environment* env, int envIdx, const char imageFileName[],
    const char envModelFileName[],
    Environment** children, int numChildren, Environment* parent);
void transitionToEnvironment(Environment* newEnvironment);
void moveToChild(int index);
void DeleteListModels(void);

// ============================================================================
// Distance / collision helpers
// ============================================================================

static GLfloat XdisBtw(int m1, int m2) { return gCharacter[m1].x - gCharacter[m2].x; }
static GLfloat YdisBtw(int m1, int m2) { return gCharacter[m1].y - gCharacter[m2].y; }
static GLfloat ZdisBtw(int m1, int m2) { return gCharacter[m1].z - gCharacter[m2].z; }
static GLfloat disBtw(int m1, int m2) {
    GLfloat dx = XdisBtw(m1,m2), dy = YdisBtw(m1,m2), dz = ZdisBtw(m1,m2);
    return sqrtf(dx*dx + dy*dy + dz*dz);
}
static GLfloat disBtw2D(int m1, int m2) {
    GLfloat dx = XdisBtw(m1,m2), dy = YdisBtw(m1,m2);
    return sqrtf(dx*dx + dy*dy);
}
static GLfloat totalRadius(int m1, int m2) {
    return gCharacter[m1].radius + gCharacter[m2].radius;
}

static bool CheckMainSquareCollison(int model) {
    GLfloat halfW = gCharacter[model].radius * sqrtf(2.0f) * 0.5f;
    GLfloat halfH = halfW;
    float dx = fabsf(gMainCharacter.x - gCharacter[model].x);
    float dy = fabsf(gMainCharacter.y - gCharacter[model].y);
    return !(dx > halfW || dy > halfH);
}

static bool isMainModelInContact(int model) {
    if (!gCharacter[model].exist || !gCharacter[model].patt_found) return false;
    GLfloat dx = gMainCharacter.x - gCharacter[model].x;
    GLfloat dy = gMainCharacter.y - gCharacter[model].y;
    return sqrtf(dx*dx + dy*dy) <= (gMainCharacter.radius + gCharacter[model].radius);
}

static void MainKnockBack(int model) {
    GLfloat dx = gMainCharacter.x - gCharacter[model].x;
    GLfloat dy = gMainCharacter.y - gCharacter[model].y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.0001f) return;
    GLfloat tRadius = gMainCharacter.radius + gCharacter[model].radius;
    float overlap = tRadius - len + 1.0f;
    gMainCharacter.x -= overlap * (gCharacter[model].x - gMainCharacter.x) / len;
    gMainCharacter.y -= overlap * (gCharacter[model].y - gMainCharacter.y) / len;
}

static void PushAway(int model) {
    GLfloat dx = gCharacter[model].x - gMainCharacter.x;
    GLfloat dy = gCharacter[model].y - gMainCharacter.y;
    GLfloat dist = sqrtf(dx*dx + dy*dy);
    if (dist < 0.0001f) return;
    GLfloat tRadius = gMainCharacter.radius + gCharacter[model].radius;
    GLfloat overlap = tRadius - dist;
    gCharacter[model].x += overlap * (dx / dist);
    gCharacter[model].y += overlap * (dy / dist);
}

static void PullTowards(int model, GLfloat speed) {
    GLfloat dx = gMainCharacter.x - gCharacter[model].x;
    GLfloat dy = gMainCharacter.y - gCharacter[model].y;
    GLfloat dist = sqrtf(dx*dx + dy*dy);
    if (dist < gCharacter[model].radius) return;
    gCharacter[model].x += speed * (dx / dist);
    gCharacter[model].y += speed * (dy / dist);
}

static void MainWalkOnIncline(int model) {
    float pi = 3.14f;
    GLfloat inclineAngle = 30.0f * pi / 180.0f;
    GLfloat radius = gCharacter[model].radius;
    GLfloat squareSide = radius * sqrtf(2.0f);
    GLfloat halfSize = squareSide / 2.0f;
    GLfloat rotDeg = gCharacter[model].rot;
    GLfloat sx = gCharacter[model].x - halfSize * cosf(rotDeg);
    GLfloat dx = fabsf(sx - gMainCharacter.x);
    gMainCharacter.z = dx * tanf(inclineAngle) + 10.0f;
}

static void scaleDomain(int i) {
    gCharacter[i].radius = gCharacter[i].ori_radius * gCharacter[i].k;
    gCharacter[i].height = gCharacter[i].ori_height * gCharacter[i].k;
}

// ============================================================================
// Attack
// ============================================================================
static void DamageModel(int idx, int damage) {
    gCharacter[idx].health -= damage;
    if (gCharacter[idx].health <= 0.0f) {
        currentAnimation2 = 0;
        currentFrame2 = 0;
    }
}

static void AttackNearbyModel(void) {
    for (int i = 0; i < modelCount; i++) {
        if (gCharacter[i].patt_found && isMainModelInContact(i)) {
            DamageModel(i, 50);
            if (gCharacter[i].exist) {
                GLfloat kd = 50.0f;
                GLfloat dx = gMainCharacter.x - gCharacter[i].x;
                GLfloat dy = gMainCharacter.y - gCharacter[i].y;
                GLfloat n = sqrtf(dx*dx + dy*dy);
                if (n > 0.0001f) {
                    gCharacter[i].x -= (dx/n) * kd;
                    gCharacter[i].y -= (dy/n) * kd;
                }
            }
            break;
        }
    }
}

// ============================================================================
// Keyboard / controller
// ============================================================================
void GetControllerData(CONTROLLER_DATA* cntl) { *cntl = g_cntl; }

void KeyDown(unsigned char key, int x, int y) {
    switch (key) {
    case '?':
        ControlMode = !ControlMode;
        if (ControlMode && SelectModel < 0 && modelCount > 0) SelectModel = 0;
        break;
    case 'H':   showHUD = !showHUD; break;
    case '+':   DeleteListModels(); modelsLoaded = false; printed = false; moveToChild(1); break;
    case '=':   DeleteListModels(); modelsLoaded = false; printed = false; moveToChild(2); break;
    case '-':   DeleteListModels(); modelsLoaded = false; printed = false; moveToChild(0); break;
    case 't': {
        bool v = gCharacter[0].exist ? !gCharacter[0].showMe : true;
        for (int i = 0; i < modelCount; i++) gCharacter[i].showMe = v;
        break;
    }
    case '0':   saveEnvModel(currentEnvironment->envModelFileName); break;
    case 127:   if (ControlMode) deleteSelectedModel(); break; // Delete key
    case '9':   printf("Load not implemented in this build\n"); break;
    case '>':
        if (ControlMode && modelCount > 0) {
            SelectModel = (SelectModel + 1) % modelCount;
        } else { modelSpeed = std::min(modelSpeed + 1.0f, 5.0f); }
        break;
    case '<':
        if (ControlMode && modelCount > 0) {
            SelectModel--;
            if (SelectModel < 0) SelectModel = modelCount - 1;
        } else { modelSpeed = std::max(modelSpeed - 1.0f, 0.2f); }
        break;
    case '7':   printf("checkData() — %d models\n", modelCount); break;
    case 'u':
        SelectedAnimation++;
        currentAnimation = (SelectedAnimation < NUM_ANIMATIONS) ? SelectedAnimation : -1;
        break;
    case KEY_A:     g_cntl.A     = KEY_DOWN; break;
    case KEY_S:     g_cntl.S     = KEY_DOWN; break;
    case KEY_D:     g_cntl.D     = KEY_DOWN; break;
    case KEY_W:     g_cntl.W     = KEY_DOWN; break;
    case KEY_X:     g_cntl.X     = KEY_DOWN; break;
    case KEY_Z:     g_cntl.Z     = KEY_DOWN; break;
    case KEY_C:     g_cntl.C     = KEY_DOWN; break;
    case KEY_N:     g_cntl.N     = KEY_DOWN; break;
    case KEY_P:     g_cntl.P     = KEY_DOWN; break;
    case KEY_SPACE: g_cntl.SPACE = KEY_DOWN; break;
    case KEY_AA:    g_cntl.AA    = KEY_DOWN; break;
    case KEY_WW:    g_cntl.WW    = KEY_DOWN; break;
    case KEY_SS:    g_cntl.SS    = KEY_DOWN; break;
    case KEY_DD:    g_cntl.DD    = KEY_DOWN; break;
    default: break;
    }
}

void KeyUp(unsigned char key, int x, int y) {
    switch (key) {
    case KEY_ESC:   exit(0);                 break;
    case KEY_A:     g_cntl.A     = KEY_UP;  break;
    case KEY_S:     g_cntl.S     = KEY_UP;  break;
    case KEY_D:     g_cntl.D     = KEY_UP;  break;
    case KEY_W:     g_cntl.W     = KEY_UP;  break;
    case KEY_X:     g_cntl.X     = KEY_UP;  break;
    case KEY_Z:     g_cntl.Z     = KEY_UP;  break;
    case KEY_C:     g_cntl.C     = KEY_UP;  break;
    case KEY_O:     g_cntl.O     = KEY_UP;  break;
    case KEY_N:     g_cntl.N     = KEY_UP;  break;
    case KEY_P:     g_cntl.P     = KEY_UP;  break;
    case KEY_SPACE: g_cntl.SPACE = KEY_UP;  break;
    case KEY_AA:    g_cntl.AA    = KEY_UP;  break;
    case KEY_WW:    g_cntl.WW    = KEY_UP;  break;
    case KEY_SS:    g_cntl.SS    = KEY_UP;  break;
    case KEY_DD:    g_cntl.DD    = KEY_UP;  break;
    default: break;
    }
}

void SpecialKeyDown(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:    g_cntl.up    = KEY_DOWN; break;
    case GLUT_KEY_DOWN:  g_cntl.down  = KEY_DOWN; break;
    case GLUT_KEY_LEFT:  g_cntl.left  = KEY_DOWN; break;
    case GLUT_KEY_RIGHT: g_cntl.right = KEY_DOWN; break;
    default: break;
    }
}

void SpecialKeyUp(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:    g_cntl.up    = KEY_UP; break;
    case GLUT_KEY_DOWN:  g_cntl.down  = KEY_UP; break;
    case GLUT_KEY_LEFT:  g_cntl.left  = KEY_UP; break;
    case GLUT_KEY_RIGHT: g_cntl.right = KEY_UP; break;
    default: break;
    }
}

void onMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        currentFrame = 0;
        gMainCharacter.isAttack = TRUE;
    }
}

// ============================================================================
// Environment management
// ============================================================================

void createEnvironment(Environment* env, int idx, const char imageFileName[],
    const char envModelFileName[],
    Environment** children, int numChildren, Environment* parent)
{
    strncpy(env->imageFileName,    imageFileName,    MAX_FILENAME_LEN - 1);
    strncpy(env->envModelFileName, envModelFileName, MAX_FILENAME_LEN - 1);
    env->envIndex          = idx;
    env->childEnvironments = children;
    env->numChildren       = numChildren;
    env->parentEnvironment = parent;
}

void deleteEnvModels(Environment* env, int idx) {
    if (env && gEnvironment[idx].envModel.numChar > 0) {
        for (int i = 0; i < modelCount; i++) {
            strncpy(gModelDisplay[i], "", 100);
            if (gCharacter[i].model) {
                mqoDeleteModel(gCharacter[i].model);
                gCharacter[i].model = NULL;
            }
        }
        gEnvironment[idx].envModel.numChar = 0;
    }
    modelCount = 0;
}

void DeleteListModels(void) {
    for (int i = 0; i < modelCount; i++) {
        gCharacter[i].rot = 0;
        gCharacter[i].x = gCharacter[i].y = gCharacter[i].z = 0;
        gCharacter[i].height = gCharacter[i].ori_height = 0;
        gCharacter[i].radius = gCharacter[i].ori_radius = 0;
        gCharacter[i].k = 0;
        gCharacter[i].exist = 0;
        gCharacter[i].modelType = isNull;
        strncpy(gModelDisplay[i], "", 100);
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 4; c++)
                gCharacter[i].trans[r][c] = 0.0;
    }
    modelCount = 0;
}

void transitionToEnvironment(Environment* newEnvironment) {
    if (currentEnvironment)
        deleteEnvModels(currentEnvironment, currentEnvironment->envIndex);
    currentEnvironment = newEnvironment;
    envIndex = currentEnvironment->envIndex;
    printf("Transitioned to environment %d: %s\n", envIndex, currentEnvironment->envModelFileName);
}

void moveToChild(int index) {
    if (index < currentEnvironment->numChildren)
        transitionToEnvironment(currentEnvironment->childEnvironments[index]);
    else
        printf("Invalid child index %d\n", index);
}

void deleteSelectedModel(void) {
    if (SelectModel < 0 || SelectModel >= modelCount) return;
    if (gCharacter[SelectModel].model)
        mqoDeleteModel(gCharacter[SelectModel].model);
    // shift everything above SelectModel down by one
    for (int i = SelectModel; i < modelCount - 1; i++) {
        gCharacter[i] = gCharacter[i + 1];
        strncpy(gModelDisplay[i], gModelDisplay[i + 1], 100);
    }
    modelCount--;
    if (modelCount == 0) SelectModel = -1;
    else if (SelectModel >= modelCount) SelectModel = modelCount - 1;
    printf("Deleted model, %d remaining\n", modelCount);
}

void saveEnvModel(const char filename[]) {
    char posFile[MAX_FILENAME_LENGTH + 15];
    char strFile[MAX_FILENAME_LENGTH + 15];
    sprintf(posFile, "Data/SavedData/%s_positions.txt", filename);
    sprintf(strFile, "Data/SavedData/%s_stringData.txt", filename);

    FILE* pf = fopen(posFile, "w");
    FILE* sf = fopen(strFile, "w");
    if (!pf || !sf) { printf("Cannot open SavedData for writing\n"); return; }

    int mType;
    for (int i = 0; i < modelCount; i++) {
        switch (gCharacter[i].modelType) {
        case isDomain:    mType = 1; break;
        case isDoor:      mType = 2; break;
        case isFurniture: mType = 3; break;
        case isInitPos:   mType = 4; break;
        default:          mType = 0; break;
        }
        fprintf(pf, "%d %f %f %f %f %f %f %f %f %f %f %d\n",
            i,
            gCharacter[i].x, gCharacter[i].y, gCharacter[i].z,
            gCharacter[i].rot, gCharacter[i].vrot,
            gCharacter[i].height, gCharacter[i].ori_height,
            gCharacter[i].radius, gCharacter[i].ori_radius,
            gCharacter[i].k,
            mType);
        fprintf(sf, "%s\n", gModelDisplay[i]);
    }
    fclose(pf);
    fclose(sf);
    printf("Saved %d models to %s\n", modelCount, posFile);
    glutSetWindowTitle("SAVED");
}

void loadEnvModel(const char filename[], int idx) {
    glutSetWindowTitle("LOADING......");
    gEnvironment[idx].envModel.numChar = 0;
    modelCount = 0;

    char posFile[MAX_FILENAME_LENGTH + 15];
    char strFile[MAX_FILENAME_LENGTH + 15];
    sprintf(posFile, "Data/SavedData/%s_positions.txt", filename);
    sprintf(strFile, "Data/SavedData/%s_stringData.txt", filename);

    FILE* pf = fopen(posFile, "r");
    FILE* sf = fopen(strFile, "r");
    if (!pf) { printf("Cannot open %s\n", posFile); return; }
    if (!sf) { printf("Cannot open %s\n", strFile); fclose(pf); return; }

    fseek(pf, 0, SEEK_END);
    if (ftell(pf) == 0) { printf("Positions file empty.\n"); fclose(pf); fclose(sf); return; }
    fseek(pf, 0, SEEK_SET);

    int doorIndex = 0;
    bool foundInit = false;
    int mIdx, mType, ns;
    GLfloat mx, my, mz, mrot, mvrot, mh, moh, mr, mor, mk;

    while ((ns = fscanf(pf, "%d %f %f %f %f %f %f %f %f %f %f %d\n",
        &mIdx, &mx, &my, &mz, &mrot, &mvrot, &mh, &moh, &mr, &mor, &mk, &mType)) != EOF)
    {
        if (ns != 12) { printf("Parse error in positions file.\n"); break; }

        char tmp[100];
        if (fgets(tmp, 100, sf)) {
            int len = strlen(tmp);
            while (len > 0 && (tmp[len-1] == '\n' || tmp[len-1] == '\r')) tmp[--len] = '\0';
            strncpy(gModelDisplay[modelCount], tmp, 100);
        }

        if (modelCount < MAX_MODELS) {
            gCharacter[modelCount].model      = mqoCreateModel(gModelDisplay[modelCount], 1.0);
            gCharacter[modelCount].rot        = mrot;
            gCharacter[modelCount].vrot       = mvrot;
            gCharacter[modelCount].x          = mx;
            gCharacter[modelCount].y          = my;
            gCharacter[modelCount].z          = mz;
            gCharacter[modelCount].height     = mh;
            gCharacter[modelCount].ori_height = moh;
            gCharacter[modelCount].radius     = mor;
            gCharacter[modelCount].ori_radius = mor;
            gCharacter[modelCount].k          = mk;
            gCharacter[modelCount].exist      = 1;
            gCharacter[modelCount].health     = 100.0f;
            gCharacter[modelCount].showMe     = false;
            gCharacter[modelCount].isDead     = false;

            switch (mType) {
            case 1: gCharacter[modelCount].modelType = isDomain;    break;
            case 2: gCharacter[modelCount].modelType = isDoor;      break;
            case 3: gCharacter[modelCount].modelType = isFurniture; break;
            case 4: gCharacter[modelCount].modelType = isInitPos;   break;
            default: gCharacter[modelCount].modelType = isNull;     break;
            }

            if (gCharacter[modelCount].modelType == isDoor) {
                if (doorIndex < gEnvironment[idx].numChildren) {
                    gCharacter[modelCount].doorData.goTo = doorIndex;
                    doorIndex++;
                }
            } else if (gCharacter[modelCount].modelType == isInitPos) {
                gEnvironment[idx].initPos = modelCount;
                foundInit = true;
            }
        }
        modelCount++;
        gEnvironment[idx].envModel.numChar++;
    }

    if (!foundInit) gEnvironment[idx].initPos = -1;
    printf("Loaded %d models for env %d\n", modelCount, idx);
    fclose(pf);
    fclose(sf);
}

// ============================================================================
// Animations
// ============================================================================

void loadAnimation(void) {
    char folders[NUM_ANIMATIONS][100] = {
        "Sequence/myAnimation/running/runner%d.mqo",
        "Sequence/myAnimation/jumping/myjump%d.mqo",
        "Sequence/myAnimation/fighting/fight%d.mqo",
        "Sequence/myAnimation/flipping/flip%d.mqo",
        "Sequence/myAnimation/bowing/bow%d.mqo",
        "Sequence/myAnimation/walking/walk%d.mqo",
        "Sequence/myAnimation/pushing/push%d.mqo",
        "Sequence/myAnimation/pulling/pull%d.mqo"
    };
    int frames[NUM_ANIMATIONS] = { 18, 13, 16, 19, 11, 11, 10, 15 };

    for (int i = 0; i < NUM_ANIMATIONS; i++) {
        animations[i].sequence = mqoCreateSequence(folders[i], frames[i], 1.0);
        animations[i].n_frame  = frames[i];
        if (animations[i].sequence.n_frame <= 0) {
            printf("Error loading sequence: %s\n", folders[i]);
            exit(-1);
        }
    }
}

void loadAnimation2(void) {
    char folders2[NUM_ANIMATIONS2][100] = {
        "Sequence/EnemyAnimation/dying/die%d.mqo"
    };
    int frames2[NUM_ANIMATIONS2] = { 13 };

    for (int i = 0; i < NUM_ANIMATIONS2; i++) {
        animations2[i].sequence = mqoCreateSequence(folders2[i], frames2[i], 1.0);
        animations2[i].n_frame  = frames2[i];
        if (animations2[i].sequence.n_frame <= 0) {
            printf("Error loading sequence: %s\n", folders2[i]);
            exit(-1);
        }
    }
}

// ============================================================================
// State calculation
// ============================================================================

static void CalcMainState(void) {
    static GLfloat v = 0;
    static const GLfloat MAX_SCALE = 10.0f;
    static const GLfloat MIN_SCALE = 0.1f;
    float pi = 3.14f;
    float jumpVelocity = 10.0f;
    float jumpHeight   = 80.0f;
    float gravity_val  = 9.8f;
    static bool isJumping = false;

    CONTROLLER_DATA ctrl;
    GetControllerData(&ctrl);
    gMainCharacter.inContact = false;

    if (ctrl.up == KEY_DOWN)        v =  modelSpeed;
    else if (ctrl.down == KEY_DOWN) v = -modelSpeed;
    else { if (v > 0) v--; else if (v < 0) v++; }

    gMainCharacter.x +=  v * sinf(gMainCharacter.rot * pi / 180.0f);
    gMainCharacter.y += -v * cosf(gMainCharacter.rot * pi / 180.0f);

    if (ctrl.left  == KEY_DOWN) gMainCharacter.rot += rotSpeed;
    if (ctrl.right == KEY_DOWN) gMainCharacter.rot -= rotSpeed;
    if (ctrl.A     == KEY_DOWN) gMainCharacter.vrot += rotSpeed;
    if (ctrl.D     == KEY_DOWN) gMainCharacter.vrot -= rotSpeed;

    if (ctrl.C == KEY_DOWN) gMainCharacter.k = std::min(gMainCharacter.k * 1.1f, MAX_SCALE);
    if (ctrl.X == KEY_DOWN) gMainCharacter.k = std::max(gMainCharacter.k * 0.9f, MIN_SCALE);

    if (ctrl.Z == KEY_DOWN) {
        if (gEnvironment[envIndex].initPos != -1) {
            int ip = gEnvironment[envIndex].initPos;
            gMainCharacter.x = gCharacter[ip].x;
            gMainCharacter.y = gCharacter[ip].y;
            gMainCharacter.z = gCharacter[ip].z;
        } else {
            gMainCharacter.x = gMainCharacter.y = gMainCharacter.z = 0.0f;
        }
        gMainCharacter.rot = gMainCharacter.vrot = 0.0f;
    }

    bool onSomething = false;
    for (int i = 0; i < modelCount; i++) {
        if (!gCharacter[i].patt_found) continue;
        if (!CheckMainSquareCollison(i)) continue;

        contact = true;

        if (gCharacter[i].modelType == isDoor) {
            DeleteListModels();
            modelsLoaded = false;
            isLoading = true;
            moveToChild(gCharacter[i].doorData.goTo);
            return;
        }

        if (gCharacter[i].modelType == isInitPos) continue;

        // JumpOn: if character is high enough, land on top
        if (gMainCharacter.z >= gCharacter[i].height) {
            gMainCharacter.zStandOn = gCharacter[i].height;
            onSomething = true;
        } else {
            // Solid boundary — push character back out
            if (gCharacter[i].modelType == isFurniture) {
                if (ctrl.N == KEY_DOWN) {
                    PushAway(i);
                    currentAnimation = 6;
                } else if (ctrl.P == KEY_DOWN) {
                    PullTowards(i, fabsf(modelSpeed));
                    currentAnimation = 7;
                } else {
                    MainKnockBack(i);
                }
            } else {
                MainKnockBack(i);
            }
        }
    }

    if (!onSomething) gMainCharacter.zStandOn = 0.0f;

    if (isJumping) {
        if (gMainCharacter.isJumping) {
            if (gMainCharacter.z >= gMainCharacter.zStandOn + jumpHeight)
                gMainCharacter.isJumping = false;
            else
                gMainCharacter.z += jumpVelocity;
        } else {
            gMainCharacter.z -= gravity_val;
            if (gMainCharacter.z <= gMainCharacter.zStandOn) {
                gMainCharacter.z = gMainCharacter.zStandOn;
                isJumping = false;
            }
        }
    }
    if (ctrl.SPACE == KEY_DOWN && !isJumping) {
        gMainCharacter.isJumping = true;
        isJumping = true;
    }

    if (ctrl.up == KEY_DOWN || ctrl.down == KEY_DOWN)
        currentAnimation = 0;
}

static void CalcState(void) {
    static GLfloat v = 0;
    static const GLfloat MAX_SCALE = 50.0f;
    static const GLfloat MIN_SCALE = 0.1f;
    static const GLfloat MAX_HEIGHT = 1000.0f;
    static const GLfloat MIN_HEIGHT = 10.0f;
    static const GLfloat MAX_RADIUS = 1000.0f;
    static const GLfloat MIN_RADIUS = 4.0f;
    float pi = 3.14f;

    if (SelectModel < 0 || SelectModel >= modelCount) return;

    CONTROLLER_DATA ctrl;
    GetControllerData(&ctrl);

    if (ctrl.up   == KEY_DOWN) v =  4;
    else if (ctrl.down == KEY_DOWN) v = -4;
    else { if (v > 0) v--; else if (v < 0) v++; }

    gCharacter[SelectModel].x +=  v * sinf(gCharacter[SelectModel].rot * pi / 180.0f);
    gCharacter[SelectModel].y += -v * cosf(gCharacter[SelectModel].rot * pi / 180.0f);

    if (ctrl.right == KEY_DOWN) gCharacter[SelectModel].rot  += 5.0f;
    if (ctrl.left  == KEY_DOWN) gCharacter[SelectModel].rot  -= 5.0f;
    if (ctrl.A     == KEY_DOWN) gCharacter[SelectModel].vrot += 5.0f;
    if (ctrl.D     == KEY_DOWN) gCharacter[SelectModel].vrot -= 5.0f;
    if (ctrl.W     == KEY_DOWN) gCharacter[SelectModel].z    += 4;
    if (ctrl.S     == KEY_DOWN) gCharacter[SelectModel].z    -= 4;

    if (ctrl.C == KEY_DOWN) gCharacter[SelectModel].k = std::min(gCharacter[SelectModel].k * 1.1f, MAX_SCALE);
    if (ctrl.X == KEY_DOWN) gCharacter[SelectModel].k = std::max(gCharacter[SelectModel].k * 0.9f, MIN_SCALE);

    scaleDomain(SelectModel);

    if (ctrl.AA == KEY_DOWN) gCharacter[SelectModel].ori_radius = std::max(gCharacter[SelectModel].ori_radius - 5.0f, MIN_RADIUS);
    if (ctrl.DD == KEY_DOWN) gCharacter[SelectModel].ori_radius = std::min(gCharacter[SelectModel].ori_radius + 5.0f, MAX_RADIUS);
    if (ctrl.WW == KEY_DOWN) gCharacter[SelectModel].ori_height = std::min(gCharacter[SelectModel].ori_height + 5.0f, MAX_HEIGHT);
    if (ctrl.SS == KEY_DOWN) gCharacter[SelectModel].ori_height = std::max(gCharacter[SelectModel].ori_height - 5.0f, MIN_HEIGHT);

    if (ctrl.Z == KEY_DOWN) {
        gCharacter[SelectModel].rot = gCharacter[SelectModel].x =
        gCharacter[SelectModel].y  = gCharacter[SelectModel].z  =
        gCharacter[SelectModel].vrot = 0.0f;
        gCharacter[SelectModel].height = gCharacter[SelectModel].ori_height;
        gCharacter[SelectModel].radius = gCharacter[SelectModel].ori_radius;
    }
}

// ============================================================================
// Drawing helpers
// ============================================================================

static void drawText(float x, float y, float z, const char* text) {
    glRasterPos3f(x, y, z);
    for (const char* c = text; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

static void drawCoordination4Main(void) {
    char buf[100];
    snprintf(buf, sizeof(buf), "(%.1f,%.1f,%.1f)",
        gMainCharacter.x, gMainCharacter.y, gMainCharacter.z);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawText(gMainCharacter.x, gMainCharacter.y, gMainCharacter.z, buf);
}

static void drawDistanceLine2Main(int model) {
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(gMainCharacter.x, gMainCharacter.y, gMainCharacter.z);
    glVertex3f(gCharacter[model].x, gCharacter[model].y, gCharacter[model].z);
    glEnd();
}

static void drawCollisionCylinder(int model, float r, float g, float b) {
    const float pi = 3.14159f;
    GLfloat cx = gCharacter[model].x, cy = gCharacter[model].y, cz = gCharacter[model].z;
    GLfloat radius = gCharacter[model].radius;
    GLfloat top    = cz + fabsf(gCharacter[model].height);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(r, g, b);
    glLineWidth(2.0f);

    // bottom circle
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < 36; j++) {
        float a = j * (2.0f * pi / 36);
        glVertex3f(cx + radius * cosf(a), cy + radius * sinf(a), cz);
    }
    glEnd();

    // top circle
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < 36; j++) {
        float a = j * (2.0f * pi / 36);
        glVertex3f(cx + radius * cosf(a), cy + radius * sinf(a), top);
    }
    glEnd();

    // 4 vertical lines
    glBegin(GL_LINES);
    glVertex3f(cx + radius, cy, cz);   glVertex3f(cx + radius, cy, top);
    glVertex3f(cx - radius, cy, cz);   glVertex3f(cx - radius, cy, top);
    glVertex3f(cx, cy + radius, cz);   glVertex3f(cx, cy + radius, top);
    glVertex3f(cx, cy - radius, cz);   glVertex3f(cx, cy - radius, top);
    glEnd();

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
}

static void drawModelAxes(int model) {
    GLfloat x = gCharacter[model].x, y = gCharacter[model].y, z = gCharacter[model].z;
    GLfloat len = gCharacter[model].radius * 1.5f;
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(x, y, z); glVertex3f(x + len, y, z);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(x, y, z); glVertex3f(x, y + len, z);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(x, y, z); glVertex3f(x, y, z + len);
    glEnd();
    glLineWidth(1.0f);
    glColor3f(1.0f, 0.0f, 0.0f); drawText(x + len, y, z, "X");
    glColor3f(0.0f, 1.0f, 0.0f); drawText(x, y + len, z, "Y");
    glColor3f(0.0f, 0.0f, 1.0f); drawText(x, y, z + len, "Z");
    glEnable(GL_DEPTH_TEST);
}

static void SetLight(GLenum light) {
    GLfloat diffuse[]  = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambient[]  = { 0.3f, 0.3f, 0.3f, 0.1f };
    GLfloat position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(light, GL_DIFFUSE,  diffuse);
    glLightfv(light, GL_SPECULAR, specular);
    glLightfv(light, GL_AMBIENT,  ambient);
    glLightfv(light, GL_POSITION, position);
    glEnable(light);
}

// ============================================================================
// Main draw function
// ============================================================================

static void draw(void) {
    GLdouble gl_para[16];
    int animationDelay = 3;

    if (ControlMode) CalcState();
    else { SelectModel = -1; CalcMainState(); }

    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);

    // --- Main character ---
    if (gMainCharacter.patt_found) {
        arglCameraViewRH(gMainCharacter.trans, gl_para, gViewScaleFactor);
        glLoadMatrixd(gl_para);
        glPushMatrix();
        glTranslatef(gMainCharacter.x, gMainCharacter.y, gMainCharacter.z);
        glRotatef(gMainCharacter.rot,  0.0f, 0.0f, 1.0f);
        glRotatef(gMainCharacter.vrot, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(gMainCharacter.k, gMainCharacter.k, gMainCharacter.k);

        if (gMainCharacter.isAttack) currentAnimation = 2;

        if (currentAnimation != -1) {
            mqoCallSequence(animations[currentAnimation].sequence, currentFrame);
            delayTime++;
            if (delayTime >= animationDelay) {
                currentFrame = (currentFrame + 1) % animations[currentAnimation].n_frame;
                delayTime = 0;
            }
            if (currentAnimation == 2 && currentFrame == 9 && delayTime == 1)
                AttackNearbyModel();
            if (currentFrame == 0 && delayTime == 0) {
                currentAnimation = -1;
                gMainCharacter.isAttack = false;
            }
            if (currentAnimation != 2) gMainCharacter.isAttack = false;
        } else {
            mqoCallModel(gMainCharacter.model);
        }
        glPopMatrix();

        // coordinates overlay
        glPushMatrix();
        arglCameraViewRH(gMainCharacter.trans, gl_para, gViewScaleFactor);
        glLoadMatrixd(gl_para);
        drawCoordination4Main();
        glPopMatrix();
    }

    // --- Scene models ---
    for (int i = 0; i < modelCount; i++) {
        if (!gCharacter[i].patt_found) continue;

        arglCameraViewRH(gCharacter[i].trans, gl_para, gViewScaleFactor);
        glLoadMatrixd(gl_para);
        glPushMatrix();
        glTranslatef(gCharacter[i].x, gCharacter[i].y, gCharacter[i].z);
        glRotatef(gCharacter[i].rot,  0.0f, 0.0f, 1.0f);
        glRotatef(gCharacter[i].vrot, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(gCharacter[i].k, gCharacter[i].k, gCharacter[i].k);

        if (i == 0 && gCharacter[0].health <= 0.0f && !gCharacter[0].isDead) {
            mqoCallSequence(animations2[currentAnimation2].sequence, currentFrame2);
            delayTime2++;
            if (delayTime2 >= animationDelay) {
                currentFrame2 = (currentFrame2 + 1) % animations2[currentAnimation2].n_frame;
                delayTime2 = 0;
            }
            if (currentFrame2 == 0 && delayTime2 == 0) {
                currentAnimation2 = -1;
                gCharacter[0].isDead  = true;
                gCharacter[0].exist   = false;
            }
        } else {
            mqoCallModel(gCharacter[i].model);
        }

        if (gCharacter[i].showMe || (ControlMode && i == SelectModel)) {
            glPushMatrix();
            arglCameraViewRH(gCharacter[i].trans, gl_para, gViewScaleFactor);
            glLoadMatrixd(gl_para);
            drawDistanceLine2Main(i);
            if (ControlMode && i == SelectModel) {
                // bright yellow cylinder + axes for selected model
                drawCollisionCylinder(i, 1.0f, 1.0f, 0.0f);
                drawModelAxes(i);
            } else {
                // dim red cylinder for show-all mode
                drawCollisionCylinder(i, 1.0f, 0.3f, 0.3f);
            }
            glPopMatrix();
        }
    }

    glDisable(GL_DEPTH_TEST);
}

// ============================================================================
// HUD overlay
// ============================================================================

static void drawHUD(cv::Mat& img) {
    const double fs  = 0.42;
    const int    th  = 1;
    const int    lh  = 20;
    const int    pad = 14;

    // key-label pairs: { key string, description }
    struct Row { const char* key; const char* desc; };

    char selBuf[80];
    Row editRows[] = {
        { "> / <",    "Select model"        },
        { "Arrows",   "Move XY"             },
        { "W / S",    "Move up / down"      },
        { "A / D",    "Tilt"                },
        { "C / X",    "Scale  + / -"        },
        { "D / A",    "Radius + / -"        },
        { "W / S",    "Height + / -"        },
        { "Z",        "Reset position"      },
        { "Delete",   "Delete model"        },
        { "0",        "Save"                },
        { "?",        "Exit edit mode"      },
    };
    Row playRows[] = {
        { "Arrows",   "Move"                },
        { "A / D",    "Tilt view"           },
        { "Space",    "Jump"                },
        { "N",        "Push  (on contact)"  },
        { "P",        "Pull  (on contact)"  },
        { "Click",    "Attack"              },
        { "t",        "Show collision box"  },
        { "?",        "Edit mode"           },
        { "ESC",      "Quit"                },
    };

    Row*  rows;
    int   nRows;
    const char* title;

    if (ControlMode) {
        if (SelectModel >= 0 && SelectModel < modelCount) {
            // strip directory prefix for display
            const char* slash = strrchr(gModelDisplay[SelectModel], '/');
            snprintf(selBuf, sizeof(selBuf), "[ %s ]", slash ? slash+1 : gModelDisplay[SelectModel]);
        } else {
            snprintf(selBuf, sizeof(selBuf), "[ no model ]");
        }
        rows  = editRows;
        nRows = 11;
        title = "EDIT MODE";
    } else {
        rows  = playRows;
        nRows = 9;
        title = "PLAY MODE";
    }

    // measure panel width
    int colKey  = 70;   // key column width
    int colDesc = 170;  // desc column width
    int panelW  = colKey + colDesc + pad * 2;
    int titleH  = lh + 4;
    int subH    = ControlMode ? lh : 0;
    int panelH  = titleH + subH + nRows * lh + pad * 2;

    int cx = img.cols / 2;
    int cy = img.rows / 2;
    int rx = cx - panelW / 2;
    int ry = cy - panelH / 2;

    // draw transparent panel using addWeighted
    cv::Mat overlay = img.clone();
    cv::rectangle(overlay, cv::Point(rx, ry), cv::Point(rx+panelW, ry+panelH),
                  cv::Scalar(20, 20, 20, 255), cv::FILLED);
    cv::rectangle(overlay, cv::Point(rx, ry), cv::Point(rx+panelW, ry+panelH),
                  cv::Scalar(200, 200, 200, 255), 1);
    cv::addWeighted(overlay, 0.55, img, 0.45, 0, img);

    // title
    int tx = rx + pad;
    int ty = ry + pad + lh - 4;
    cv::putText(img, title, cv::Point(tx, ty),
                cv::FONT_HERSHEY_SIMPLEX, 0.52,
                ControlMode ? cv::Scalar(0,220,220,255) : cv::Scalar(100,255,100,255), 1);

    // selected model name (edit mode only)
    int rowY = ty + titleH;
    if (ControlMode) {
        cv::putText(img, selBuf, cv::Point(tx, rowY),
                    cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(255,220,80,255), th);
        rowY += lh;
    }

    // divider line
    cv::line(img, cv::Point(rx+4, rowY-6), cv::Point(rx+panelW-4, rowY-6),
             cv::Scalar(140,140,140,255), 1);

    // key rows — key in yellow, description in white
    for (int i = 0; i < nRows; i++) {
        int y = rowY + i * lh;
        cv::putText(img, rows[i].key,  cv::Point(tx,           y),
                    cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(80,200,255,255), th);
        cv::putText(img, rows[i].desc, cv::Point(tx + colKey,  y),
                    cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(220,220,220,255), th);
    }
}

// ============================================================================
// GLUT callbacks
// ============================================================================

static void Idle(void) {
    ARMarkerInfo* marker_info;
    int marker_num;

    if (!currentEnvironment) return;

    cv::Mat frame = cv::imread(currentEnvironment->imageFileName);
    if (frame.empty()) return;
    if (frame.cols != xsize || frame.rows != ysize)
        cv::resize(frame, frame, cv::Size(xsize, ysize));

    cv::cvtColor(frame, frameRGBA, cv::COLOR_BGR2BGRA);
    gARTImage = frameRGBA.data;

    if (arDetectMarker(gARTImage, thresh, &marker_info, &marker_num) < 0) {
        cleanup();
        exit(0);
    }

    // Best marker match for main character
    gMainCharacter.exist      = TRUE;
    gMainCharacter.patt_found = TRUE;
    int best = -1;
    for (int j = 0; j < marker_num; j++) {
        if (patt_id == marker_info[j].id) {
            if (best == -1 || marker_info[best].cf < marker_info[j].cf)
                best = j;
        }
    }
    if (best != -1)
        arGetTransMat(&marker_info[best], patt_center, patt_width, gMainCharacter.trans);

    // Same transform for all scene models
    for (int i = 0; i < modelCount; i++) {
        gCharacter[i].patt_found = gCharacter[i].exist ? TRUE : FALSE;
        if (best != -1)
            arGetTransMat(&marker_info[best], patt_center, patt_width, gCharacter[i].trans);
    }

    // Load environment models once
    if (!modelsLoaded) {
        loadEnvModel(currentEnvironment->envModelFileName, currentEnvironment->envIndex);
        if (gEnvironment[envIndex].initPos > 0) {
            int ip = gEnvironment[envIndex].initPos;
            gMainCharacter.x = gCharacter[ip].x;
            gMainCharacter.y = gCharacter[ip].y;
            gMainCharacter.z = gCharacter[ip].z;
        } else {
            gMainCharacter.x = gMainCharacter.y = gMainCharacter.z = 0.0f;
        }
        modelsLoaded = true;
        isLoading    = false;
    }

    glutSetWindowTitle(currentEnvironment->envModelFileName);

    if (showHUD) drawHUD(frameRGBA);

    glutPostRedisplay();
}

static void Display(void) {
    if (!gARTImage) { glutSwapBuffers(); return; }
    // Draw background image
    arglDispImage(gARTImage, &cparam, 1.0, gArglSettings);

    // Set up 3D projection
    GLdouble gl_frust[16];
    arglCameraFrustumRH(&cparam, gViewDistanceMin, gViewDistanceMax, gl_frust);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(gl_frust);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 3D scene
    draw();

    glutSwapBuffers();
}

// ============================================================================
// Init / cleanup
// ============================================================================

static void init(void) {
    ARParam wparam;

    cv::Mat img = cv::imread("Data/images/parkinglot.jpg");
    if (img.empty()) { printf("Error: cannot load initial image\n"); exit(0); }
    xsize = img.cols;
    ysize = img.rows;
    printf("Image size: %d x %d\n", xsize, ysize);

    if (arParamLoad(cparam_name, 1, &wparam) < 0) {
        printf("Camera parameter load error\n"); exit(0);
    }
    arParamChangeSize(&wparam, xsize, ysize, &cparam);
    arInitCparam(&cparam);
    arParamDisp(&cparam);

    printf("Loading pattern...\n");
    if ((patt_id = arLoadPatt(patt_name)) < 0) {
        printf("Pattern load error\n"); exit(0);
    }
    printf("Pattern loaded, patt_id=%d\n", patt_id);

    // GLUT window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(xsize, ysize);
    printf("Creating GLUT window...\n");
    glutCreateWindow("RoomNavigator");
    printf("Window created.\n");

    // gsub_lite context
    gArglSettings = arglSetupForCurrentContext();
    printf("arglSetup done: %p\n", (void*)gArglSettings);
    if (!gArglSettings) { printf("arglSetup failed!\n"); exit(1); }
    arglDrawModeSet(gArglSettings, AR_DRAW_BY_GL_DRAW_PIXELS);
    arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_FULL_IMAGE);
    printf("init() done.\n");
}

static void cleanup(void) {
    arglCleanup(gArglSettings);
    mqoDeleteModel(gMainCharacter.model);
    for (int i = 0; i < modelCount; i++)
        if (gCharacter[i].model) mqoDeleteModel(gCharacter[i].model);
}

// ============================================================================
// main
// ============================================================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    init();

    mqoInit();

    currentEnvironment = &gEnvironment[0];

    printf("Creating main character model: %s\n", gMainModelDisplay); fflush(stdout);
    gMainCharacter.model      = mqoCreateModel(gMainModelDisplay, 1.0);
    printf("Main character model created.\n"); fflush(stdout);
    gMainCharacter.ori_height = gModel_height;
    gMainCharacter.height     = gModel_height;
    gMainCharacter.ori_radius = gCharacter_radius;
    gMainCharacter.radius     = gCharacter_radius;
    gMainCharacter.k          = 1.0f;
    gMainCharacter.rot        = 0.0f;
    gMainCharacter.zStandOn   = 0.0f;
    gMainCharacter.inContact  = false;
    gMainCharacter.isAttack   = false;

    // Environment graph:
    //   parkinglots -> [lobby]
    //   lobby       -> [parkinglots, room1, room2]
    //   room1       -> [lobby, room2]
    //   room2       -> [lobby, room1]
    Environment* parkinglotChildren[] = { &gEnvironment[1] };
    Environment* lobbyChildren[]      = { &gEnvironment[0], &gEnvironment[2], &gEnvironment[3] };
    Environment* room1Children[]      = { &gEnvironment[1], &gEnvironment[3] };
    Environment* room2Children[]      = { &gEnvironment[1], &gEnvironment[2] };

    createEnvironment(&gEnvironment[0], 0, "Data/images/parkinglot.jpg", "parkinglot", parkinglotChildren, 1, NULL);
    createEnvironment(&gEnvironment[1], 1, "Data/images/frontDesk.jpg",  "frontdesk",  lobbyChildren,      3, NULL);
    createEnvironment(&gEnvironment[2], 2, "Data/images/room1.jpg",      "room1",      room1Children,      2, NULL);
    createEnvironment(&gEnvironment[3], 3, "Data/images/room2.jpg",      "room2",      room2Children,      2, NULL);

    printf("Loading main character model...\n"); fflush(stdout);
    printf("Loading animations...\n"); fflush(stdout);
    loadAnimation();
    printf("loadAnimation done.\n"); fflush(stdout);
    loadAnimation2();
    printf("loadAnimation2 done.\n"); fflush(stdout);

    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);
    glutMouseFunc(onMouseClick);

    glutMainLoop();
    return 0;
}
