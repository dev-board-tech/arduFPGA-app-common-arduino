#include "vKbdSimple.h"
#include "device/screen.h"
#include "sys/sTimer.h"
#include <string.h>

char kbdRow0[] = "1234567890";
char kbdRow1Low[] = "qwertyuiop";
char kbdRow2Low[] = "asdfghjkl";
char kbdRow3Low[] = "zxcvbnm";
char kbdRow1Hi[] = "QWERTYUIOP";
char kbdRow2Hi[] = "ASDFGHJKL";
char kbdRow3Hi[] = "ZXCVBNM";

vKbdSimple::vKbdSimple(void *scr) {
    DriverPtr = this;
    Loop = &vKbdSimple::loop;
    SetPosX = &vKbdSimple::setPosX;
    SetPosY = &vKbdSimple::setPosY;
    Reset = &vKbdSimple::reset;
    GetKbdHeight = &vKbdSimple::getKbdHeight;
    SetKbdWidth = &vKbdSimple::setKbdWidth;
    SetBigLetter = &vKbdSimple::setBigLetter;
    GetBigLetter = &vKbdSimple::getBigLetter;
    SetKbdRowLow = &vKbdSimple::setKbdRowLow;
    SetKbdRowHi = &vKbdSimple::setKbdRowHi;
    RestoreKbd = &vKbdSimple::restoreKbd;
    screen = scr;
    cursorBlinkTimer = (void *)new sTimer;
    ((sTimer *)cursorBlinkTimer)->SetInterval(500);
    ((sTimer *)cursorBlinkTimer)->Start();
}

vKbdSimple::~vKbdSimple() {
    delete ((sTimer *)cursorBlinkTimer);
}

bool vKbdSimple::loop(void *driverHandlerPtr, char *key, bool repaint, bool visible) {
    vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
    *key = 0;
    if(drv->lastVisible != visible) {
        drv->lastVisible = visible;
        return true;
    }
    if(!visible)
        return false;
    bool kbdChanged = drv->hKbd.getChanged();
    bool cursorChanged = false;
    if(((sTimer *)drv->cursorBlinkTimer)->Tick()) {
        drv->cursorState = !drv->cursorState;
        cursorChanged = true;
    }
    if(repaint || kbdChanged || cursorChanged || drv->lastPosX != drv->currPosX ||
            drv->lastPosY != drv->currPosY || drv->lastBigLetter != drv->bigLetter) {
        drv->lastPosX = drv->currPosX;
        drv->lastPosY = drv->currPosY;
        drv->lastBigLetter = drv->bigLetter;
        if(kbdChanged) {
            ((sTimer *)drv->cursorBlinkTimer)->Start();
            drv->cursorState = true;
            uint8_t keys = drv->hKbd.get();
            if(keys == KBD_L_KEY) {
                if(drv->cursorPosY == 0) {
                    if(drv->cursorPosX > 9)
                        drv->cursorPosX = 9;
                } else if(drv->cursorPosY == 1) {
                    if(drv->cursorPosX > 9)
                        drv->cursorPosX = 9;
                } else if(drv->cursorPosY == 2) {
                    if(drv->cursorPosX > 8)
                        drv->cursorPosX = 8;
                } else if(drv->cursorPosY == 3) {
                    if(drv->cursorPosX > 8)
                        drv->cursorPosX = 8;
                }
            }
            if(keys == KBD_A_KEY) {
                switch(drv->cursorPosY) {
                case 0:
                    *key = kbdRow0[drv->cursorPosX];
                    break;
                case 1:
                    *key = (drv->bigLetter ? kbdRow1Hi : kbdRow1Low)[drv->cursorPosX];
                    break;
                case 2:
                    *key = (drv->bigLetter ? kbdRow2Hi : kbdRow2Low)[drv->cursorPosX];
                    break;
                case 3:
                    if(drv->cursorPosX != 0) {
                        if(drv->cursorPosX < (int8_t)strlen(kbdRow3Low))
                            *key = (drv->bigLetter ? kbdRow3Hi : kbdRow3Low)[drv->cursorPosX];
                        else
                            *key = '\n';
                    } else {
                        drv->bigLetter = !drv->bigLetter;
                    }
                    break;
                }
            } else if(keys == KBD_L_KEY) {
                drv->cursorPosX--;
            } else if(keys == KBD_R_KEY) {
                drv->cursorPosX++;
            } else if(keys == KBD_U_KEY) {
                drv->cursorPosY--;
            } else if(keys == KBD_D_KEY) {
                drv->cursorPosY++;
            }
            if(keys == KBD_R_KEY) {
                if(drv->cursorPosY == 0) {
                    if(drv->cursorPosX > 9)
                        drv->cursorPosX = 9;
                } else if(drv->cursorPosY == 1) {
                    if(drv->cursorPosX > 9)
                        drv->cursorPosX = 9;
                } else if(drv->cursorPosY == 2) {
                    if(drv->cursorPosX > 8)
                        drv->cursorPosX = 8;
                } else if(drv->cursorPosY == 3) {
                    if(drv->cursorPosX > 8)
                        drv->cursorPosX = 8;
                }
            }
        }
        if(drv->cursorPosY < 0)
            drv->cursorPosY = 0;
        if(drv->cursorPosY > 3)
            drv->cursorPosY = 3;
        if(drv->cursorPosX < 0)
            drv->cursorPosX = 0;
        int y = drv->currPosY;
        ((Screen *)drv->screen)->drvDrawRoundedRectangle(drv->currPosX, y, 57, 26, 1, false, 1);
        gfxString str;
        str.setExtraSpace(1);
        str.drawStringWindowed4x6(kbdRow0, drv->currPosX+3, y+=2, drv->cursorPosX < 10 ? drv->cursorPosX : 9,
                                  drv->cursorPosY == 0 ? drv->cursorState : false);
        str.drawStringWindowed4x6(drv->bigLetter ? kbdRow1Hi : kbdRow1Low, drv->currPosX+5, y+=6,
                                  drv->cursorPosX < 10 ? drv->cursorPosX : 9, drv->cursorPosY == 1 ? drv->cursorState : false);
        str.drawStringWindowed4x6(drv->bigLetter ? kbdRow2Hi : kbdRow2Low, drv->currPosX+3, y+=6,
                                  drv->cursorPosX < 9 ? drv->cursorPosX : 8, drv->cursorPosY == 2 ? drv->cursorState : false);
        str.drawStringWindowed4x6(drv->bigLetter ? kbdRow3Hi : kbdRow3Low, drv->currPosX+10, y+=6,
                                  drv->cursorPosX - 1, drv->cursorPosY == 3 && drv->cursorPosX != 0 && drv->cursorPosX < 8 ? drv->cursorState : false);
        str.setExtraSpace(0);
        str.drawStringWindowed4x6((char *)"^", drv->currPosX+3, y, 0,
                                  drv->cursorPosY == 3 && drv->cursorPosX == 0 ? drv->cursorState : false, 0);
        str.drawStringWindowed4x6((char *)"\n", drv->currPosX+48, y, 0,
                                  drv->cursorPosY == 3 && drv->cursorPosX >= 8 ? drv->cursorState : false, 0);
        return true;
    }
    return false;
}
