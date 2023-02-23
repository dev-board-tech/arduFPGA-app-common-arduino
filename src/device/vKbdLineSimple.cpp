#include "vKbdLineSimple.h"
#include "device/screen.h"
#include "sys/sTimer.h"
#include <string.h>

vKbdLineSimple::vKbdLineSimple(void *scr) {
    DriverPtr = this;
    Loop = &vKbdLineSimple::loop;
    SetPosX = &vKbdLineSimple::setPosX;
    SetPosY = &vKbdLineSimple::setPosY;
    Reset = &vKbdLineSimple::reset;
    GetKbdHeight = &vKbdLineSimple::getKbdHeight;
    SetKbdWidth = &vKbdLineSimple::setKbdWidth;
    SetBigLetter = &vKbdLineSimple::setBigLetter;
    GetBigLetter = &vKbdLineSimple::getBigLetter;
    SetKbdRowLow = &vKbdLineSimple::setKbdRowLow;
    SetKbdRowHi = &vKbdLineSimple::setKbdRowHi;
    RestoreKbd = &vKbdLineSimple::restoreKbd;
    screen = scr;
    cursorBlinkTimer = (void *)new sTimer;
    ((sTimer *)cursorBlinkTimer)->SetInterval(500);
    ((sTimer *)cursorBlinkTimer)->Start();
}

vKbdLineSimple::~vKbdLineSimple() {
    delete ((sTimer *)cursorBlinkTimer);
}

bool vKbdLineSimple::loop(void *driverHandlerPtr, char *key, bool repaint, bool visible) {
    vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
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
    if(repaint || kbdChanged || cursorChanged || drv->lastPosX != drv->currPosX || drv->lastPosY != drv->currPosY ||
            drv->lastKbdWidth != drv->kbdWidth || drv->lastBigLetter != drv->bigLetter) {
        drv->lastPosX = drv->currPosX;
        drv->lastPosY = drv->currPosY;
        drv->lastKbdWidth = drv->kbdWidth;
        drv->lastBigLetter = drv->bigLetter;
        char *kbdRow = drv->bigLetter ? drv->kbdRowHi : drv->kbdRowLow;
        if(kbdChanged) {
            ((sTimer *)drv->cursorBlinkTimer)->Start();
            drv->cursorState = true;
            uint8_t keys = drv->hKbd.get();
            if(keys == KBD_L_KEY) {
            }
            if(keys == KBD_A_KEY) {
                *key = kbdRow[drv->cursorPosX];
            } else if(keys == KBD_L_KEY) {
                if(drv->cursorPosX > 0)
                    drv->cursorPosX--;
            } else if(keys == KBD_R_KEY) {
                if(drv->cursorPosX < (int8_t)strlen(kbdRow) - 1)
                    drv->cursorPosX++;
            }
        }
        int y = drv->currPosY;
        ((Screen *)drv->screen)->drvDrawRoundedRectangle(drv->currPosX, y, drv->kbdWidth, 8, 1, false, 1);
        gfxString str;
        box_t box = {drv->currPosX + 2, (int)(drv->kbdWidth - 1 - drv->currPosX), y, y+8};
        int charsPerWidth = (drv->kbdWidth / 4) - 2;
        int strPos = drv->currPosX+3;
        if(strlen(kbdRow) * 4 > drv->kbdWidth - 3) {
            strPos = drv->cursorPosX < charsPerWidth / 2 ? drv->currPosX+3 :
                                                              drv->cursorPosX >= strlen(kbdRow) - (charsPerWidth / 2) ? drv->currPosX+3 - (strlen(kbdRow) - charsPerWidth - 1) * 4:
                                                                                                                        drv->currPosX+3 - ((drv->cursorPosX - (charsPerWidth / 2)) * 4);
        }
        str.drawStringWindowed4x6(&box, kbdRow, strPos, y+=2, drv->cursorPosX, drv->cursorState);
        return true;
    }
    return false;
}
