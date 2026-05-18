// Group Moving / Environment Designer
// Ported from Windows (progress11-GroupMovingWorked.cpp) to Linux.
// Static-image AR: cv::imread() replaces live camera feed, same approach as EarthDefender.

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
#include <dirent.h>

// ============================================================================
// Type definitions
// ============================================================================
static CONTROLLER_DATA g_cntl = {
    KEY_UP,KEY_UP,KEY_UP,KEY_UP, // up down left right
    KEY_UP,KEY_UP,KEY_UP,KEY_UP, // A B S D
    KEY_UP,KEY_UP,KEY_UP,KEY_UP, // W X Z C
    KEY_UP,KEY_UP,KEY_UP,KEY_UP, // O P N SPACE
    KEY_UP,KEY_UP,KEY_UP,KEY_UP  // AA WW SS DD
};

#define MAX_MODELS 50

typedef enum { mdNull=0, mdDomain=1, mdDoor=2, mdFurniture=3, mdInitPos=4 } ModelType;

typedef struct {
    MQO_MODEL   model;
    GLfloat     rot, vrot;
    GLfloat     x, y, z;
    GLfloat     k;
    GLfloat     height, ori_height;
    GLfloat     radius, ori_radius;
    GLfloat     offset_x, offset_y, offset_z;
    int         pattId;
    int         patt_found;
    double      trans[3][4];
    int         exist;
    ModelType   modelType;
    int         doorGoTo;
} CHARACTER_DATA;

typedef struct {
    int     group_start;
    int     group_end;
    int     num_models;
    GLfloat center_x, center_y, center_z;
    GLfloat rot;
} GROUP_INFO;

// ============================================================================
// Global variables
// ============================================================================

#define NUM_MODEL 92

int CurrentModel       = 0;
int ifGroup[MAX_MODELS];
int modelCount         = 1;
int loadModelCount     = 0;

static const int    gWindowed      = TRUE;
static const int    gFullWidth     = 800;
static const int    gFullHeight    = 600;
static const int    gFullDepth     = 32;
static const int    gFullRefresh   = 0;
static const double gViewScaleFactor  = 1.0;
static const double gViewDistanceMin  = 0.1;
static const double gViewDistanceMax  = 10000.0;

int groupModelCount[100] = { 0 };
int current_group_num    = 0;
int totalModel;
bool groupSelected       = false;
int SelectGroup          = 0;

// Camera / pattern
static char gCparamName[] = "Data/camera_para.dat";
static char gPattName[]   = "Data/patt/patt.hiro";
static char gImagePath[]  = "Data/images/test2.jpg";

// Model file paths — [100] to fit "Data/mqoFile/<name>.mqo"
static char gModelDisplay[MAX_MODELS][100]     = { "Data/mqoFile/street.mqo" };
static char gLoadModelDisplay[MAX_MODELS][100] = { "Data/mqoFile/street.mqo" };

static char ModelName[NUM_MODEL][50] = {
    "Bishop",       "BlackBishop",    "BlackChessKing", "BlackChessPawn",
    "BlackChessQueen","BlackChessRook","BlackHair",      "BlackKnight",
    "ChessKing",    "ChessPawn",      "ChessQueen",     "ChessRook",
    "GoldKnight",   "Knight",         "MaskKnight",     "MaskKnight1",
    "MaskKnight2",  "MaskKnight3",    "MaskKnight4",    "Robot",
    "TableClock",   "Tele",           "ball",           "balls",
    "barricade",    "bed",            "bench",          "bicycle",
    "bl_ninja",     "books",          "car",            "chair",
    "character",    "character1",     "character2",     "character3",
    "character4",   "character5",     "character6",     "character7",
    "clock",        "cone",           "cube",           "cubes",
    "cylinder",     "desk",           "domain",         "door",
    "drawer",       "falling",        "floor",          "goalNet",
    "human",        "iamRun",         "invisible",      "jeep",
    "jeep2",        "jeep3",          "keyboard",       "mario",
    "monitor",      "mouse",          "mug",            "myHuman",
    "mymotion0",    "mystickman",     "newSticker",     "ninja",
    "r8",           "raptor",         "raptor1",        "roundTable",
    "rug",          "runner0",        "runner1",        "sacchi",
    "sofa",         "stickMan",       "sticker",        "stickerMan",
    "street",       "table",          "tachikoma",      "theCone",
    "theRed",       "theStick",       "tree",           "tree2",
    "tunnel",       "tv",             "wall",           "window"
};
static char gModelName[NUM_MODEL][100] = {
    "Data/mqoFile/Bishop.mqo",       "Data/mqoFile/BlackBishop.mqo",
    "Data/mqoFile/BlackChessKing.mqo","Data/mqoFile/BlackChessPawn.mqo",
    "Data/mqoFile/BlackChessQueen.mqo","Data/mqoFile/BlackChessRook.mqo",
    "Data/mqoFile/BlackHair.mqo",    "Data/mqoFile/BlackKnight.mqo",
    "Data/mqoFile/ChessKing.mqo",    "Data/mqoFile/ChessPawn.mqo",
    "Data/mqoFile/ChessQueen.mqo",   "Data/mqoFile/ChessRook.mqo",
    "Data/mqoFile/GoldKnight.mqo",   "Data/mqoFile/Knight.mqo",
    "Data/mqoFile/MaskKnight.mqo",   "Data/mqoFile/MaskKnight1.mqo",
    "Data/mqoFile/MaskKnight2.mqo",  "Data/mqoFile/MaskKnight3.mqo",
    "Data/mqoFile/MaskKnight4.mqo",  "Data/mqoFile/Robot.mqo",
    "Data/mqoFile/TableClock.mqo",   "Data/mqoFile/Tele.mqo",
    "Data/mqoFile/ball.mqo",         "Data/mqoFile/balls.mqo",
    "Data/mqoFile/barricade.mqo",    "Data/mqoFile/bed.mqo",
    "Data/mqoFile/bench.mqo",        "Data/mqoFile/bicycle.mqo",
    "Data/mqoFile/bl_ninja.mqo",     "Data/mqoFile/books.mqo",
    "Data/mqoFile/car.mqo",          "Data/mqoFile/chair.mqo",
    "Data/mqoFile/character.mqo",    "Data/mqoFile/character1.mqo",
    "Data/mqoFile/character2.mqo",   "Data/mqoFile/character3.mqo",
    "Data/mqoFile/character4.mqo",   "Data/mqoFile/character5.mqo",
    "Data/mqoFile/character6.mqo",   "Data/mqoFile/character7.mqo",
    "Data/mqoFile/clock.mqo",        "Data/mqoFile/cone.mqo",
    "Data/mqoFile/cube.mqo",         "Data/mqoFile/cubes.mqo",
    "Data/mqoFile/cylinder.mqo",     "Data/mqoFile/desk.mqo",
    "Data/mqoFile/domain.mqo",       "Data/mqoFile/door.mqo",
    "Data/mqoFile/drawer.mqo",       "Data/mqoFile/falling.mqo",
    "Data/mqoFile/floor.mqo",        "Data/mqoFile/goalNet.mqo",
    "Data/mqoFile/human.mqo",        "Data/mqoFile/iamRun.mqo",
    "Data/mqoFile/invisible.mqo",    "Data/mqoFile/jeep.mqo",
    "Data/mqoFile/jeep2.mqo",        "Data/mqoFile/jeep3.mqo",
    "Data/mqoFile/keyboard.mqo",     "Data/mqoFile/mario.mqo",
    "Data/mqoFile/monitor.mqo",      "Data/mqoFile/mouse.mqo",
    "Data/mqoFile/mug.mqo",          "Data/mqoFile/myHuman.mqo",
    "Data/mqoFile/mymotion0.mqo",    "Data/mqoFile/mystickman.mqo",
    "Data/mqoFile/newSticker.mqo",   "Data/mqoFile/ninja.mqo",
    "Data/mqoFile/r8.mqo",           "Data/mqoFile/raptor.mqo",
    "Data/mqoFile/raptor1.mqo",      "Data/mqoFile/roundTable.mqo",
    "Data/mqoFile/rug.mqo",          "Data/mqoFile/runner0.mqo",
    "Data/mqoFile/runner1.mqo",      "Data/mqoFile/sacchi.mqo",
    "Data/mqoFile/sofa.mqo",         "Data/mqoFile/stickMan.mqo",
    "Data/mqoFile/sticker.mqo",      "Data/mqoFile/stickerMan.mqo",
    "Data/mqoFile/street.mqo",       "Data/mqoFile/table.mqo",
    "Data/mqoFile/tachikoma.mqo",    "Data/mqoFile/theCone.mqo",
    "Data/mqoFile/theRed.mqo",       "Data/mqoFile/theStick.mqo",
    "Data/mqoFile/tree.mqo",         "Data/mqoFile/tree2.mqo",
    "Data/mqoFile/tunnel.mqo",       "Data/mqoFile/tv.mqo",
    "Data/mqoFile/wall.mqo",         "Data/mqoFile/window.mqo"
};

static double gPatt_width     = 80.0;
static double gPatt_centre[2] = { 0.0, 0.0 };

static ARUint8*                  gARTImage    = NULL;
static int                       gARTThresh   = 100;
static ARParam                   gARTCparam;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;

static cv::Mat frameRGBA;   // global so gARTImage stays valid between Idle and Display
static bool    showHelp = false;
static bool    showCylinder = false;
static bool    showModelList = false;
static int     spawnPicker = 0;
static bool    inputMode = false;
static char    inputBuffer[64] = "";
static char    inputFeedback[80] = "";

typedef enum { actionSpawn = 0, actionSave, actionLoad } InputAction;
static InputAction inputAction = actionSpawn;
static char    saveFileList[32][64];
static int     saveFileCount = 0;

#define DEFAULT_RADIUS 80.0f
#define DEFAULT_HEIGHT 150.0f

static CHARACTER_DATA gCharacter[100];
static CHARACTER_DATA gLoadCharacter[100];
static int            gLight_on        = TRUE;
int SelectModel    = 0;
int SelectMultiModel = 0;

GROUP_INFO Group_Info[10];
GLfloat sum_x = 0, sum_y = 0, sum_z = 0;

static char gSavedModel[MAX_MODELS][100];

// Forward declarations
void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void SpecialKeyDown(int key, int x, int y);
void SpecialKeyUp(int key, int x, int y);

// ============================================================================
// Camera setup — uses OpenCV to read image size (no live camera)
// ============================================================================
static int SetupCamera(const char* cparam_name, const char* image_path, ARParam* cparam)
{
    ARParam wparam;

    cv::Mat img = cv::imread(image_path);
    if (img.empty()) {
        printf("Error: could not load image %s\n", image_path);
        return FALSE;
    }
    int xsize = img.cols;
    int ysize = img.rows;
    printf("Image size: %d x %d\n", xsize, ysize);

    if (arParamLoad(cparam_name, 1, &wparam) < 0) {
        printf("Camera parameter load error!\n");
        return FALSE;
    }
    arParamChangeSize(&wparam, xsize, ysize, cparam);
    arInitCparam(cparam);
    printf("*** Camera Parameter ***\n");
    arParamDisp(cparam);

    return TRUE;
}

// ============================================================================
// Marker setup
// ============================================================================
static int SetupMarker(const char* patt_name, int* patt_id)
{
    if ((*patt_id = arLoadPatt(patt_name)) < 0) {
        printf("Pattern load error: %s\n", patt_name);
        return FALSE;
    }
    return TRUE;
}

// ============================================================================
// Window setup
// ============================================================================
static int SetupGraphicsWin(const char* title, int w, int h)
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow(title);

    if ((gArglSettings = arglSetupForCurrentContext()) == NULL) return FALSE;
    // Use GL_DRAW_PIXELS mode — avoids texture issues on some Linux/NVIDIA drivers
    arglDrawModeSet(gArglSettings, AR_DRAW_BY_GL_DRAW_PIXELS);
    arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_FULL_IMAGE);
    return TRUE;
}

// ============================================================================
// Quit
// ============================================================================
static void Quit(void)
{
    arglCleanup(gArglSettings);
    for (int i = 0; i < modelCount; i++)     mqoDeleteModel(gCharacter[i].model);
    for (int i = 0; i < loadModelCount; i++) mqoDeleteModel(gLoadCharacter[i].model);
    mqoCleanup();
}

// ============================================================================
// Helpers
// ============================================================================
void CopyString(const char* src, char* dst, int maxSize)
{
    std::strncpy(dst, src, maxSize - 1);
    dst[maxSize - 1] = '\0';
}

#define MAX_FILENAME_LENGTH 256

static void saveData(const char* name)
{
    char posFn[MAX_FILENAME_LENGTH + 30];
    char strFn[MAX_FILENAME_LENGTH + 30];
    snprintf(posFn, sizeof(posFn), "Data/SavedData/%s_positions.txt", name);
    snprintf(strFn, sizeof(strFn), "Data/SavedData/%s_stringData.txt", name);

    FILE* pf = fopen(posFn, "w");
    if (!pf) { snprintf(inputFeedback, sizeof(inputFeedback), "Error: cannot open %s", posFn); return; }
    for (int i = 0; i < modelCount; i++) {
        fprintf(pf, "%d %f %f %f %f %f %f %f %f %f %f %d\n",
            i,
            gCharacter[i].x, gCharacter[i].y, gCharacter[i].z,
            gCharacter[i].rot, gCharacter[i].vrot,
            gCharacter[i].height, gCharacter[i].ori_height,
            gCharacter[i].radius, gCharacter[i].ori_radius,
            gCharacter[i].k,
            (int)gCharacter[i].modelType);
    }
    fclose(pf);

    FILE* sf = fopen(strFn, "w");
    if (!sf) { snprintf(inputFeedback, sizeof(inputFeedback), "Error: cannot open %s", strFn); return; }
    for (int i = 0; i < modelCount; i++) fprintf(sf, "%s\n", gModelDisplay[i]);
    fclose(sf);

    snprintf(inputFeedback, sizeof(inputFeedback), "Saved %d models -> %s", modelCount, name);
}

static void scanSaveFiles()
{
    saveFileCount = 0;
    DIR* d = opendir("Data/SavedData");
    if (!d) return;
    struct dirent* ent;
    while ((ent = readdir(d)) != NULL && saveFileCount < 32) {
        const char* n = ent->d_name;
        int len = strlen(n);
        if (len > 14 && strcmp(n + len - 14, "_positions.txt") == 0) {
            int nl = len - 14;
            if (nl < 64) {
                strncpy(saveFileList[saveFileCount], n, nl);
                saveFileList[saveFileCount][nl] = '\0';
                saveFileCount++;
            }
        }
    }
    closedir(d);
}

static void loadData(const char* name)
{
    char posFn[MAX_FILENAME_LENGTH + 30];
    char strFn[MAX_FILENAME_LENGTH + 30];
    snprintf(posFn, sizeof(posFn), "Data/SavedData/%s_positions.txt", name);
    snprintf(strFn, sizeof(strFn), "Data/SavedData/%s_stringData.txt", name);

    FILE* pf = fopen(posFn, "r");
    FILE* sf = fopen(strFn, "r");
    if (!pf) { snprintf(inputFeedback, sizeof(inputFeedback), "Cannot open: %s", name); return; }
    if (!sf) { snprintf(inputFeedback, sizeof(inputFeedback), "Cannot open string file: %s", name); fclose(pf); return; }

    int idx, mtype, count = 0;
    float x, y, z, rot, vrot, h, oh, r, ori_r, k;
    char tempStr[100];

    while (fscanf(pf, "%d %f %f %f %f %f %f %f %f %f %f %d",
            &idx, &x, &y, &z, &rot, &vrot, &h, &oh, &r, &ori_r, &k, &mtype) == 12) {
        if (!fgets(tempStr, sizeof(tempStr), sf)) break;
        int len = strlen(tempStr);
        while (len > 0 && (tempStr[len-1]=='\n'||tempStr[len-1]=='\r')) tempStr[--len]='\0';
        // fix old save format: "Data/name.mqo" → "Data/mqoFile/name.mqo"
        if (strncmp(tempStr, "Data/", 5) == 0 && strstr(tempStr, "mqoFile/") == NULL) {
            char tmp[100];
            snprintf(tmp, sizeof(tmp), "Data/mqoFile/%s", tempStr + 5);
            strncpy(tempStr, tmp, sizeof(tempStr));
        }

        if (modelCount >= MAX_MODELS) break;
        MQO_MODEL m = mqoCreateModel(tempStr, 1.0);
        if (!m) { printf("Failed to load model: %s\n", tempStr); continue; }

        strncpy(gModelDisplay[modelCount], tempStr, 100);
        gCharacter[modelCount].model      = m;
        gCharacter[modelCount].x          = x;
        gCharacter[modelCount].y          = y;
        gCharacter[modelCount].z          = z;
        gCharacter[modelCount].rot        = rot;
        gCharacter[modelCount].vrot       = vrot;
        gCharacter[modelCount].height     = h;
        gCharacter[modelCount].ori_height = oh;
        gCharacter[modelCount].radius     = r;
        gCharacter[modelCount].ori_radius = ori_r;
        gCharacter[modelCount].k          = k;
        gCharacter[modelCount].modelType  = (ModelType)mtype;
        gCharacter[modelCount].exist      = TRUE;
        gCharacter[modelCount].offset_x   = 0;
        gCharacter[modelCount].offset_y   = 0;
        gCharacter[modelCount].offset_z   = 0;
        modelCount++;
        count++;
    }
    fclose(pf); fclose(sf);

    // keep SelectModel valid
    if (SelectModel < 0 && modelCount > 0) SelectModel = 0;
    if (SelectModel >= modelCount)          SelectModel = modelCount - 1;

    if (count > 0)
        snprintf(inputFeedback, sizeof(inputFeedback), "Loaded +%d models from '%s' (total %d)", count, name, modelCount);
    else
        snprintf(inputFeedback, sizeof(inputFeedback), "ERROR: no models loaded from '%s'", name);
}

static void drawHUD(cv::Mat& img);
static void drawModelList(cv::Mat& img);
static void drawSaveList(cv::Mat& img);
static const char* modelTypeName(ModelType t);

// ============================================================================
// Idle — static image replaces arVideoGetImage()
// ============================================================================
static void Idle(void)
{
    static int ms_prev;
    int ms = glutGet(GLUT_ELAPSED_TIME);
    float s_elapsed = (float)(ms - ms_prev) * 0.001f;
    if (s_elapsed < 0.01f) return;
    ms_prev = ms;

    cv::Mat frame = cv::imread(gImagePath);
    if (frame.empty()) { printf("Error loading image\n"); return; }
    cv::cvtColor(frame, frameRGBA, cv::COLOR_BGR2BGRA);

    // Always-visible status bar
    {
        cv::rectangle(frameRGBA, cv::Point(0,0), cv::Point(frameRGBA.cols, 22),
                      cv::Scalar(0,0,0,200), cv::FILLED);
        if (inputMode) {
            char line[128];
            const char* prompt = (inputAction == actionSave) ? "Save as:"
                               : (inputAction == actionLoad) ? "Load file:"
                               :                               "Spawn model:";
            snprintf(line, sizeof(line), "%s %s|   (Enter=confirm  Esc=cancel)", prompt, inputBuffer);
            cv::Scalar promptCol = (inputAction == actionSave) ? cv::Scalar(80,200,255,255)
                                 : (inputAction == actionLoad) ? cv::Scalar(255,200,80,255)
                                 :                               cv::Scalar(80,255,80,255);
            cv::putText(frameRGBA, line, cv::Point(6, 15),
                        cv::FONT_HERSHEY_SIMPLEX, 0.42, promptCol, 1);
        } else {
            char status[160];
            const char* typeName = (SelectModel >= 0 && SelectModel < modelCount)
                ? modelTypeName(gCharacter[SelectModel].modelType) : "none";
            snprintf(status, sizeof(status), "[%d] i=add  m=models  type:%s (y)  t=cyl  h=help  %s",
                     modelCount, typeName, inputFeedback);
            cv::putText(frameRGBA, status, cv::Point(6, 15),
                        cv::FONT_HERSHEY_SIMPLEX, 0.42, cv::Scalar(80,220,255,255), 1);
        }
    }

    if (showHelp)                              drawHUD(frameRGBA);
    if (showModelList)                         drawModelList(frameRGBA);
    if (inputMode && inputAction == actionLoad) drawSaveList(frameRGBA);
    if (false) {  // old help block — replaced by drawHUD above
        const cv::Scalar bg(0, 0, 0, 180);
        const cv::Scalar fg(255, 255, 255, 255);
        const double fs = 0.45;
        const int    th = 1;
        const int    lh = 22;  // line height px
        int x0 = 10, y0 = 24;
        // semi-transparent dark panel
        cv::rectangle(frameRGBA, cv::Point(x0-4, y0-18),
                      cv::Point(x0+300, y0 + lh*13), bg, cv::FILLED);
        static const char* lines[] = {
            "H          Toggle this help",
            "I          Spawn model (type name)",
            "> / <      Cycle selection",
            "W/S        Move forward/back",
            "A/D        Move left/right",
            "Arrow keys Rotate",
            "C / X      Scale up / down",
            "P / O      Move up / down",
            "Z          Reset position",
            "F          Delete selected model",
            "0          Save layout",
            "9          Load layout",
            "ESC        Quit"
        };
        for (int n = 0; n < 13; n++)
            cv::putText(frameRGBA, lines[n],
                        cv::Point(x0, y0 + n*lh),
                        cv::FONT_HERSHEY_SIMPLEX, fs, fg, th);
    }

    gARTImage = frameRGBA.data;

    ARMarkerInfo* marker_info;
    int marker_num;
    if (arDetectMarker(gARTImage, gARTThresh, &marker_info, &marker_num) < 0) {
        exit(0);
    }

    // All placed models use the Hiro marker for their coordinate frame
    gCharacter[0].patt_found = TRUE;
    int i, j, k;
    for (i = 0; i < modelCount; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (marker_info[j].id == gCharacter[i].pattId) {
                if (k == -1 || marker_info[j].cf > marker_info[k].cf) k = j;
            }
        }
        if (k != -1) arGetTransMat(&marker_info[k], gPatt_centre, gPatt_width, gCharacter[i].trans);
    }
    for (int l = 1; l < modelCount; l++) {
        if (gCharacter[l].exist == 1) gCharacter[l].patt_found = TRUE;
    }
    for (i = 0; i < loadModelCount; i++) {
        k = -1;
        for (j = 0; j < marker_num; j++) {
            if (marker_info[j].id == gLoadCharacter[i].pattId) {
                if (k == -1 || marker_info[j].cf > marker_info[k].cf) k = j;
            }
        }
        if (k != -1) arGetTransMat(&marker_info[k], gPatt_centre, gPatt_width, gLoadCharacter[i].trans);
    }
    for (int l = 0; l < loadModelCount; l++) {
        if (gLoadCharacter[l].exist == 1) gLoadCharacter[l].patt_found = TRUE;
    }

    glutPostRedisplay();
}

static void Visibility(int visible)
{
    glutIdleFunc(visible == GLUT_VISIBLE ? Idle : NULL);
}

static void Reshape(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

static void SetLight(GLenum light)
{
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

void GetControllerData(CONTROLLER_DATA* cntl) { *cntl = g_cntl; }

// ============================================================================
// Type helpers
// ============================================================================
static const char* modelTypeName(ModelType t) {
    switch (t) {
    case mdDomain:    return "Domain";
    case mdDoor:      return "Door";
    case mdFurniture: return "Furniture";
    case mdInitPos:   return "InitPos";
    default:          return "Null";
    }
}

static void spawnByName(const char* name) {
    if (modelCount >= MAX_MODELS) {
        snprintf(inputFeedback, sizeof(inputFeedback), "Too many models!");
        return;
    }
    for (int i = 0; i < NUM_MODEL; i++) {
        if (strcmp(name, ModelName[i]) == 0) {
            CopyString(gModelName[i], gModelDisplay[modelCount], 100);
            if ((gCharacter[modelCount].model = mqoCreateModel(gModelName[i], 1.0)) == NULL) {
                snprintf(inputFeedback, sizeof(inputFeedback), "Failed to load: %s", name);
                return;
            }
            gCharacter[modelCount].x          = 0.0f;
            gCharacter[modelCount].y          = 0.0f;
            gCharacter[modelCount].z          = 0.0f;
            gCharacter[modelCount].rot        = 0.0f;
            gCharacter[modelCount].vrot       = 0.0f;
            gCharacter[modelCount].k          = 2.0f;
            gCharacter[modelCount].height     = DEFAULT_HEIGHT;
            gCharacter[modelCount].ori_height = DEFAULT_HEIGHT;
            gCharacter[modelCount].radius     = DEFAULT_RADIUS;
            gCharacter[modelCount].ori_radius = DEFAULT_RADIUS;
            gCharacter[modelCount].modelType  = mdDomain;
            gCharacter[modelCount].exist      = TRUE;
            gCharacter[modelCount].patt_found = TRUE;
            SelectModel = modelCount;
            snprintf(inputFeedback, sizeof(inputFeedback), "Spawned: %s", name);
            modelCount++;
            CurrentModel++;
            ifGroup[CurrentModel] = 0;
            return;
        }
    }
    snprintf(inputFeedback, sizeof(inputFeedback), "Unknown: \"%s\"  (check h for list)", name);
}

static void cycleModelType(CHARACTER_DATA& c) {
    switch (c.modelType) {
    case mdNull:      c.modelType = mdDomain;    break;
    case mdDomain:    c.modelType = mdDoor;      c.doorGoTo = 0; break;
    case mdDoor:      c.modelType = mdFurniture; break;
    case mdFurniture: c.modelType = mdInitPos;   break;
    case mdInitPos:   c.modelType = mdNull;      break;
    }
}

// ============================================================================
// Collision cylinder + axes
// ============================================================================
static void drawCollisionCylinder(CHARACTER_DATA& c, float r, float g, float b) {
    const float pi = 3.14159f;
    GLfloat cx = c.x, cy = c.y, cz = c.z;
    GLfloat radius = c.radius;
    GLfloat top    = cz + fabsf(c.height);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < 36; j++) { float a = j*(2*pi/36); glVertex3f(cx+radius*cosf(a), cy+radius*sinf(a), cz); }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < 36; j++) { float a = j*(2*pi/36); glVertex3f(cx+radius*cosf(a), cy+radius*sinf(a), top); }
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(cx+radius, cy, cz); glVertex3f(cx+radius, cy, top);
    glVertex3f(cx-radius, cy, cz); glVertex3f(cx-radius, cy, top);
    glVertex3f(cx, cy+radius, cz); glVertex3f(cx, cy+radius, top);
    glVertex3f(cx, cy-radius, cz); glVertex3f(cx, cy-radius, top);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
}

static void drawText3D(float x, float y, float z, const char* txt) {
    glRasterPos3f(x, y, z);
    for (const char* c = txt; *c; c++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

static void drawModelAxes(CHARACTER_DATA& c) {
    GLfloat x = c.x, y = c.y, z = c.z;
    GLfloat len = c.radius * 1.5f;
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1,0,0); glVertex3f(x,y,z); glVertex3f(x+len,y,z);
    glColor3f(0,1,0); glVertex3f(x,y,z); glVertex3f(x,y+len,z);
    glColor3f(0,0,1); glVertex3f(x,y,z); glVertex3f(x,y,z+len);
    glEnd();
    glLineWidth(1.0f);
    glColor3f(1,0,0); drawText3D(x+len, y, z, "X");
    glColor3f(0,1,0); drawText3D(x, y+len, z, "Y");
    glColor3f(0,0,1); drawText3D(x, y, z+len, "Z");
    glEnable(GL_DEPTH_TEST);
}

// ============================================================================
// HUD
// ============================================================================
static void drawHUD(cv::Mat& img) {
    const double fs  = 0.42;
    const int    th  = 1;
    const int    lh  = 20;
    const int    pad = 14;

    struct Row { const char* key; const char* desc; };

    char typeBuf[64], doorBuf[64], spawnBuf[64], selBuf[80];
    CHARACTER_DATA& sel = gCharacter[SelectModel];
    snprintf(typeBuf,  sizeof(typeBuf),  "Y   cycle type  [ %s ]", modelTypeName(sel.modelType));
    if (sel.modelType == mdDoor)
        snprintf(doorBuf, sizeof(doorBuf), "1/2/3  door -> room %d", sel.doorGoTo);
    else
        snprintf(doorBuf, sizeof(doorBuf), "1/2/3  set door dest");
    snprintf(spawnBuf, sizeof(spawnBuf), "[ ]  pick: %s", ModelName[spawnPicker]);
    const char* slash = strrchr(gModelDisplay[SelectModel], '/');
    snprintf(selBuf, sizeof(selBuf), "[ %s ]", slash ? slash+1 : gModelDisplay[SelectModel]);

    Row rows[] = {
        { "> / <",   "Select model"     },
        { "Arrows",  "Move XY"          },
        { "P / O",   "Move up / down"   },
        { "A / D",   "Tilt"             },
        { "C / X",   "Scale + / -"      },
        { "D / A",   "Radius + / -"     },
        { "W / S",   "Height + / -"     },
        { spawnBuf,  ""                 },
        { "O",       "Spawn model"      },
        { typeBuf,   ""                 },
        { doorBuf,   ""                 },
        { "t",       "Toggle cylinder"  },
        { "Z",       "Reset position"   },
        { "F",       "Delete model"     },
        { "0",       "Save"             },
        { "9",       "Load"             },
    };
    int nRows = 16;

    int colKey  = 80, colDesc = 170, panelW = colKey + colDesc + pad*2;
    int titleH  = lh + 4, subH = lh;
    int panelH  = titleH + subH + nRows*lh + pad*2;
    int rx = img.cols/2 - panelW/2, ry = img.rows/2 - panelH/2;

    cv::Mat overlay = img.clone();
    cv::rectangle(overlay, cv::Point(rx,ry), cv::Point(rx+panelW,ry+panelH),
                  cv::Scalar(20,20,20,255), cv::FILLED);
    cv::rectangle(overlay, cv::Point(rx,ry), cv::Point(rx+panelW,ry+panelH),
                  cv::Scalar(200,200,200,255), 1);
    cv::addWeighted(overlay, 0.55, img, 0.45, 0, img);

    int tx = rx+pad, ty = ry+pad+lh-4;
    cv::putText(img, "ENV DESIGNER", cv::Point(tx,ty),
                cv::FONT_HERSHEY_SIMPLEX, 0.52, cv::Scalar(0,220,220,255), 1);
    int rowY = ty + titleH;
    cv::putText(img, selBuf, cv::Point(tx, rowY),
                cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(255,220,80,255), th);
    rowY += subH;
    cv::line(img, cv::Point(rx+4,rowY-6), cv::Point(rx+panelW-4,rowY-6),
             cv::Scalar(140,140,140,255), 1);
    for (int i = 0; i < nRows; i++) {
        int y = rowY + i*lh;
        cv::putText(img, rows[i].key,  cv::Point(tx,       y), cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(80,200,255,255), th);
        cv::putText(img, rows[i].desc, cv::Point(tx+colKey,y), cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(220,220,220,255), th);
    }
}

static void drawSaveList(cv::Mat& img) {
    const double fs  = 0.40;
    const int    th  = 1;
    const int    lh  = 18;
    const int    pad = 10;

    int n = saveFileCount;
    if (n == 0) {
        // nothing saved yet — just show a small message
        int pw = 260, ph = lh + pad*2;
        int rx = img.cols/2 - pw/2, ry = img.rows/2 - ph/2;
        cv::Mat ov = img.clone();
        cv::rectangle(ov, cv::Point(rx,ry), cv::Point(rx+pw,ry+ph), cv::Scalar(20,20,20,255), cv::FILLED);
        cv::addWeighted(ov, 0.65, img, 0.35, 0, img);
        cv::putText(img, "No save files found", cv::Point(rx+pad, ry+pad+lh-2),
                    cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(200,100,100,255), th);
        return;
    }

    int pw = 220, ph = lh + pad*2 + n*lh;
    int rx = img.cols/2 - pw/2, ry = img.rows/2 - ph/2;
    cv::Mat ov = img.clone();
    cv::rectangle(ov, cv::Point(rx,ry), cv::Point(rx+pw,ry+ph), cv::Scalar(15,15,15,255), cv::FILLED);
    cv::rectangle(ov, cv::Point(rx,ry), cv::Point(rx+pw,ry+ph), cv::Scalar(180,180,100,255), 1);
    cv::addWeighted(ov, 0.65, img, 0.35, 0, img);

    int ty = ry + pad + lh - 2;
    cv::putText(img, "SAVED FILES", cv::Point(rx+pad, ty),
                cv::FONT_HERSHEY_SIMPLEX, 0.42, cv::Scalar(255,200,80,255), 1);
    cv::line(img, cv::Point(rx+4, ty+4), cv::Point(rx+pw-4, ty+4), cv::Scalar(120,120,80,255), 1);

    for (int i = 0; i < n; i++) {
        cv::putText(img, saveFileList[i], cv::Point(rx+pad, ty + lh + i*lh),
                    cv::FONT_HERSHEY_SIMPLEX, fs, cv::Scalar(210,210,210,255), th);
    }
}

static void drawModelList(cv::Mat& img) {
    const int COLS   = 4;
    const int ROWS   = (NUM_MODEL + COLS - 1) / COLS;
    const double fs  = 0.38;
    const int    th  = 1;
    const int    lh  = 17;
    const int    pad = 10;
    const int    colW = 115;

    int panelW = COLS * colW + pad * 2;
    int panelH = lh + pad * 2 + ROWS * lh;
    int rx = img.cols / 2 - panelW / 2;
    int ry = img.rows / 2 - panelH / 2;

    cv::Mat overlay = img.clone();
    cv::rectangle(overlay, cv::Point(rx, ry), cv::Point(rx+panelW, ry+panelH),
                  cv::Scalar(15,15,15,255), cv::FILLED);
    cv::rectangle(overlay, cv::Point(rx, ry), cv::Point(rx+panelW, ry+panelH),
                  cv::Scalar(180,180,180,255), 1);
    cv::addWeighted(overlay, 0.60, img, 0.40, 0, img);

    int ty = ry + pad + lh - 2;
    cv::putText(img, "MODELS  (type name with  i )", cv::Point(rx+pad, ty),
                cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(0,220,220,255), 1);
    cv::line(img, cv::Point(rx+4, ty+4), cv::Point(rx+panelW-4, ty+4),
             cv::Scalar(120,120,120,255), 1);

    for (int i = 0; i < NUM_MODEL; i++) {
        int col = i / ROWS;
        int row = i % ROWS;
        int x = rx + pad + col * colW;
        int y = ty + lh + row * lh + 2;
        cv::Scalar color = (i == spawnPicker)
            ? cv::Scalar(80, 255, 80, 255)
            : cv::Scalar(210, 210, 210, 255);
        cv::putText(img, ModelName[i], cv::Point(x, y),
                    cv::FONT_HERSHEY_SIMPLEX, fs, color, th);
    }
}

// ============================================================================
// CalcState
// ============================================================================
static void CalcState(void)
{
    CONTROLLER_DATA ctrl;
    static GLfloat v = 0, h = 0;

    if (!gCharacter[SelectModel].patt_found) return;
    GetControllerData(&ctrl);

    if      (ctrl.W == KEY_DOWN) v =  3;
    else if (ctrl.S == KEY_DOWN) v = -3;
    else { if (v > 0) v--; else if (v < 0) v++; }

    if      (ctrl.A == KEY_DOWN) h = -3;
    else if (ctrl.D == KEY_DOWN) h =  3;
    else { if (h > 0) h--; else if (h < 0) h++; }

    gCharacter[SelectModel].x += v;
    gCharacter[SelectModel].y += h;

    if      (ctrl.C  == KEY_DOWN) gCharacter[SelectModel].k *= 1.1f;
    else if (ctrl.X  == KEY_DOWN) { gCharacter[SelectModel].k *= 0.9f; if (gCharacter[SelectModel].k < 0.1f) gCharacter[SelectModel].k = 0.1f; }

    if (ctrl.right == KEY_DOWN) gCharacter[SelectModel].rot  -= 2.1f;
    if (ctrl.left  == KEY_DOWN) gCharacter[SelectModel].rot  += 2.1f;
    if (ctrl.up    == KEY_DOWN) gCharacter[SelectModel].vrot += 3.0f;
    if (ctrl.down  == KEY_DOWN) gCharacter[SelectModel].vrot -= 3.0f;
    if (ctrl.P     == KEY_DOWN) gCharacter[SelectModel].z    += 3.0f;
    if (ctrl.O     == KEY_DOWN) gCharacter[SelectModel].z    -= 3.0f;

    if (ctrl.DD    == KEY_DOWN) gCharacter[SelectModel].radius     = std::min(gCharacter[SelectModel].radius     + 5.0f, 1000.0f);
    if (ctrl.AA    == KEY_DOWN) gCharacter[SelectModel].radius     = std::max(gCharacter[SelectModel].radius     - 5.0f,    4.0f);
    if (ctrl.WW    == KEY_DOWN) gCharacter[SelectModel].height     = std::min(gCharacter[SelectModel].height     + 5.0f, 1000.0f);
    if (ctrl.SS    == KEY_DOWN) gCharacter[SelectModel].height     = std::max(gCharacter[SelectModel].height     - 5.0f,   10.0f);
    gCharacter[SelectModel].ori_radius = gCharacter[SelectModel].radius;
    gCharacter[SelectModel].ori_height = gCharacter[SelectModel].height;

    if (ctrl.Z == KEY_DOWN) { gCharacter[SelectModel].rot = 0; gCharacter[SelectModel].x = 0; gCharacter[SelectModel].y = 0; gCharacter[SelectModel].z = 0; }
}

static void CalcGroupState(void)
{
    CONTROLLER_DATA ctrl;
    static GLfloat v = 0, h = 0;
    GetControllerData(&ctrl);

    if (ctrl.C == KEY_DOWN) for (int i = Group_Info[SelectGroup].group_start; i <= Group_Info[SelectGroup].group_end; i++) gLoadCharacter[i].k *= 1.1f;
    if (ctrl.X == KEY_DOWN) for (int i = Group_Info[SelectGroup].group_start; i <= Group_Info[SelectGroup].group_end; i++) gLoadCharacter[i].k *= 0.9f;

    if (ctrl.right == KEY_DOWN) Group_Info[SelectGroup].rot -= 3.0f;
    if (ctrl.left  == KEY_DOWN) Group_Info[SelectGroup].rot += 3.0f;

    Group_Info[SelectGroup].center_x += v;
    Group_Info[SelectGroup].center_y += h;

    if      (ctrl.W == KEY_DOWN) v =  3;
    else if (ctrl.S == KEY_DOWN) v = -3;
    else { if (v > 0) v--; else if (v < 0) v++; }

    if      (ctrl.A == KEY_DOWN) h = -3;
    else if (ctrl.D == KEY_DOWN) h =  3;
    else { if (h > 0) h--; else if (h < 0) h++; }

    for (int i = Group_Info[SelectGroup].group_start; i <= Group_Info[SelectGroup].group_end; i++) {
        gLoadCharacter[i].x = gLoadCharacter[i].offset_x + Group_Info[SelectGroup].center_x;
        gLoadCharacter[i].y = gLoadCharacter[i].offset_y + Group_Info[SelectGroup].center_y;
        gLoadCharacter[i].z = gLoadCharacter[i].offset_z + Group_Info[SelectGroup].center_z;
    }
}

// ============================================================================
// Display
// ============================================================================
static void Display(void)
{
    GLdouble p[16], m[16];
    int i;

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw background image
    arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);

    if (groupSelected) CalcGroupState();
    else               CalcState();

    arglCameraFrustumRH(&gARTCparam, gViewDistanceMin, gViewDistanceMax, p);
    glMatrixMode(GL_PROJECTION); glLoadMatrixd(p);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix(); SetLight(GL_LIGHT0); glPopMatrix();

    // Draw individual models
    for (i = 0; i < modelCount; i++) {
        if (!gCharacter[i].patt_found) continue;
        glPushMatrix();
        arglCameraViewRH(gCharacter[i].trans, m, gViewScaleFactor);
        glLoadMatrixd(m);
        glTranslatef(gCharacter[i].x, gCharacter[i].y, gCharacter[i].z);
        glScalef(gCharacter[i].k, gCharacter[i].k, gCharacter[i].k);
        glPushMatrix();
        glRotatef(gCharacter[i].rot,  0.0f, 0.0f, 1.0f);
        glRotatef(gCharacter[i].vrot, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f,              1.0f, 0.0f, 0.0f);
        if (!groupSelected && i == SelectModel) glDisable(GL_LIGHTING);
        else                                    glEnable(GL_LIGHTING);
        mqoCallModel(gCharacter[i].model);
        glPopMatrix();
        glPopMatrix();

        if (!groupSelected && i == SelectModel && showCylinder) {
            glPushMatrix();
            arglCameraViewRH(gCharacter[i].trans, m, gViewScaleFactor);
            glLoadMatrixd(m);
            drawCollisionCylinder(gCharacter[i], 1.0f, 1.0f, 0.0f);
            drawModelAxes(gCharacter[i]);
            glPopMatrix();
        }
    }

    // Draw loaded group models
    for (i = 0; i < loadModelCount; i++) {
        if (!gLoadCharacter[i].patt_found) continue;
        glPushMatrix();
        arglCameraViewRH(gLoadCharacter[i].trans, m, gViewScaleFactor);
        glLoadMatrixd(m);
        glTranslatef(gLoadCharacter[i].x, gLoadCharacter[i].y, gLoadCharacter[i].z);
        glScalef(gLoadCharacter[i].k, gLoadCharacter[i].k, gLoadCharacter[i].k);
        bool inGroup = (i >= Group_Info[SelectGroup].group_start && i <= Group_Info[SelectGroup].group_end);
        if (inGroup && groupSelected) glDisable(GL_LIGHTING);
        else                          glEnable(GL_LIGHTING);
        glPushMatrix();
        glRotatef(gLoadCharacter[i].rot,  0.0f, 0.0f, 1.0f);
        glRotatef(gLoadCharacter[i].vrot, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f,                  1.0f, 0.0f, 0.0f);
        mqoCallModel(gLoadCharacter[i].model);
        glPopMatrix();
        glPopMatrix();
    }

    glDisable(GL_DEPTH_TEST);
    glutSwapBuffers();
}

// ============================================================================
// Key callbacks
// ============================================================================
int SelectIndex = 0;

void KeyDown(unsigned char key, int x, int y)
{
    // Input mode: capture text for model name
    if (inputMode) {
        if (key == 13 || key == '\r') {          // Enter — confirm
            inputMode = false;
            if (inputBuffer[0] != '\0') {
                if      (inputAction == actionSpawn) spawnByName(inputBuffer);
                else if (inputAction == actionSave)  saveData(inputBuffer);
                else if (inputAction == actionLoad)  loadData(inputBuffer);
            }
            inputBuffer[0] = '\0';
        } else if (key == 27) {                  // Esc — cancel
            inputMode = false;
            inputBuffer[0] = '\0';
            snprintf(inputFeedback, sizeof(inputFeedback), "Cancelled");
        } else if (key == 127 || key == 8) {     // Backspace / Delete
            int len = strlen(inputBuffer);
            if (len > 0) inputBuffer[len-1] = '\0';
        } else if (key >= 32 && key < 127) {     // printable character
            int len = strlen(inputBuffer);
            if (len < (int)sizeof(inputBuffer)-1) {
                inputBuffer[len]   = key;
                inputBuffer[len+1] = '\0';
            }
        }
        return;
    }

    switch (key) {
    case '[':
        spawnPicker = (spawnPicker - 1 + NUM_MODEL) % NUM_MODEL;
        break;
    case ']':
        spawnPicker = (spawnPicker + 1) % NUM_MODEL;
        break;
    case 'i':  // open input box for spawning
        inputAction    = actionSpawn;
        inputMode      = true;
        inputBuffer[0] = '\0';
        snprintf(inputFeedback, sizeof(inputFeedback), "");
        break;
    case 'u':  // quick-spawn from picker
        spawnByName(ModelName[spawnPicker]);
        break;
    case 'y':
        if (SelectModel >= 0 && SelectModel < modelCount)
            cycleModelType(gCharacter[SelectModel]);
        break;
    case '1': if (SelectModel>=0 && gCharacter[SelectModel].modelType==mdDoor) gCharacter[SelectModel].doorGoTo=0; break;
    case '2': if (SelectModel>=0 && gCharacter[SelectModel].modelType==mdDoor) gCharacter[SelectModel].doorGoTo=1; break;
    case '3': if (SelectModel>=0 && gCharacter[SelectModel].modelType==mdDoor) gCharacter[SelectModel].doorGoTo=2; break;

    case '>':
        SelectIndex++;
        if (SelectIndex >= CurrentModel + 1) SelectIndex = 0;
        if (ifGroup[SelectIndex] == 0) {
            groupSelected = false;
            if (++SelectModel >= modelCount) SelectModel = 0;
        } else {
            groupSelected = true;
            if (++SelectGroup == current_group_num + 1) SelectGroup = 1;
        }
        printf("SelectIndex=%d groupSelected=%d SelectModel=%d SelectGroup=%d\n",
               SelectIndex, groupSelected, SelectModel, SelectGroup);
        break;

    case '<':
        SelectIndex--;
        if (SelectIndex < 0) SelectIndex = CurrentModel;
        if (ifGroup[SelectIndex] == 0) {
            groupSelected = false;
            if (--SelectModel < 0) SelectModel = modelCount - 1;
        } else {
            groupSelected = true;
            if (--SelectGroup <= 0) SelectGroup = current_group_num;
        }
        printf("SelectIndex=%d groupSelected=%d SelectModel=%d SelectGroup=%d\n",
               SelectIndex, groupSelected, SelectModel, SelectGroup);
        break;

    case 'f':
        if (modelCount > 0 && SelectModel < modelCount) {
            for (int i = SelectModel; i < modelCount - 1; i++) {
                gCharacter[i] = gCharacter[i + 1];
                CopyString(gModelDisplay[i + 1], gModelDisplay[i], 100);
            }
            modelCount--;
            if (SelectModel >= modelCount) SelectModel = modelCount - 1;
            printf("Model deleted.\n");
        }
        break;

    case 'h': showHelp      = !showHelp;      break;
    case 'm': showModelList = !showModelList; break;
    case 't': showCylinder  = !showCylinder;  break;
    case '0':
        inputAction = actionSave;
        inputMode   = true;
        inputBuffer[0] = '\0';
        snprintf(inputFeedback, sizeof(inputFeedback), "");
        break;
    case '9':
        scanSaveFiles();
        inputAction = actionLoad;
        inputMode   = true;
        inputBuffer[0] = '\0';
        snprintf(inputFeedback, sizeof(inputFeedback), "");
        break;
    case KEY_ESC: exit(0); break;
    case KEY_A: g_cntl.A = KEY_DOWN; break;
    case KEY_S: g_cntl.S = KEY_DOWN; break;
    case KEY_D: g_cntl.D = KEY_DOWN; break;
    case KEY_W: g_cntl.W = KEY_DOWN; break;
    case KEY_X: g_cntl.X = KEY_DOWN; break;
    case KEY_Z: g_cntl.Z = KEY_DOWN; break;
    case KEY_C: g_cntl.C = KEY_DOWN; break;
    case KEY_O:  g_cntl.O  = KEY_DOWN; break;
    case KEY_P:  g_cntl.P  = KEY_DOWN; break;
    case KEY_AA: g_cntl.AA = KEY_DOWN; break;
    case KEY_DD: g_cntl.DD = KEY_DOWN; break;
    case KEY_WW: g_cntl.WW = KEY_DOWN; break;
    case KEY_SS: g_cntl.SS = KEY_DOWN; break;
    default: break;
    }
}

void KeyUp(unsigned char key, int x, int y)
{
    switch (key) {
    case KEY_A:  g_cntl.A  = KEY_UP; break;
    case KEY_S:  g_cntl.S  = KEY_UP; break;
    case KEY_D:  g_cntl.D  = KEY_UP; break;
    case KEY_W:  g_cntl.W  = KEY_UP; break;
    case KEY_X:  g_cntl.X  = KEY_UP; break;
    case KEY_Z:  g_cntl.Z  = KEY_UP; break;
    case KEY_C:  g_cntl.C  = KEY_UP; break;
    case KEY_O:  g_cntl.O  = KEY_UP; break;
    case KEY_P:  g_cntl.P  = KEY_UP; break;
    case KEY_AA: g_cntl.AA = KEY_UP;  break;
    case KEY_DD: g_cntl.DD = KEY_UP;  break;
    case KEY_WW: g_cntl.WW = KEY_UP;  break;
    case KEY_SS: g_cntl.SS = KEY_UP;  break;
    default: break;
    }
}

void SpecialKeyDown(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:    g_cntl.up    = KEY_DOWN; break;
    case GLUT_KEY_DOWN:  g_cntl.down  = KEY_DOWN; break;
    case GLUT_KEY_LEFT:  g_cntl.left  = KEY_DOWN; break;
    case GLUT_KEY_RIGHT: g_cntl.right = KEY_DOWN; break;
    default: break;
    }
}

void SpecialKeyUp(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:    g_cntl.up    = KEY_UP; break;
    case GLUT_KEY_DOWN:  g_cntl.down  = KEY_UP; break;
    case GLUT_KEY_LEFT:  g_cntl.left  = KEY_UP; break;
    case GLUT_KEY_RIGHT: g_cntl.right = KEY_UP; break;
    default: break;
    }
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    if (!SetupCamera(gCparamName, gImagePath, &gARTCparam)) return 1;
    if (!SetupMarker(gPattName, &gCharacter[0].pattId))     return 1;

    if (!SetupGraphicsWin("EnvDesigner on ARToolKit", arImXsize, arImYsize)) return 1;

    mqoInit();

    for (int i = 0; i < modelCount; i++) {
        if ((gCharacter[i].model = mqoCreateModel(gModelDisplay[i], 1.0)) == NULL) {
            printf("Failed to load model: %s\n", gModelDisplay[i]); return 1;
        }
        gCharacter[i].rot        = 0.0f;
        gCharacter[i].x          = 0.0f;
        gCharacter[i].y          = 0.0f;
        gCharacter[i].z          = 0.0f;
        gCharacter[i].k          = 2.0f;
        gCharacter[i].height     = DEFAULT_HEIGHT;
        gCharacter[i].ori_height = DEFAULT_HEIGHT;
        gCharacter[i].radius     = DEFAULT_RADIUS;
        gCharacter[i].ori_radius = DEFAULT_RADIUS;
        gCharacter[i].modelType  = mdDomain;
    }

    atexit(Quit);
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutVisibilityFunc(Visibility);
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);

    glutMainLoop();
    return 0;
}
