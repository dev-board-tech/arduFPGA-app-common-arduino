#ifndef __V_KBD_LINE_SIMPLE_H__
#define __V_KBD_LINE_SIMPLE_H__

#include "device/hKbd_lrudab.h"
#include "device/vKbd.h"
#include<stdint.h>
#include<stdio.h>

class vKbdLineSimple : public vKbd {
public:
    vKbdLineSimple(void *scr);
    ~vKbdLineSimple();
    static bool loop(void *driverHandlerPtr, char *key, bool repaint, bool visible = true);
    static void setPosX(void *driverHandlerPtr, int pos) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->currPosX = pos;
    }
    static void setPosY(void *driverHandlerPtr, int pos) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->currPosY = pos;
    }
    static void reset(void *driverHandlerPtr) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->cursorState = false; drv->bigLetter = false;
    }
    static uint8_t getKbdHeight(void *driverHandlerPtr) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        return drv->kbdHeight;
    }
    static void setKbdWidth(void *driverHandlerPtr, unsigned int width) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->kbdWidth = width;
    }
    static void setBigLetter(void *driverHandlerPtr, bool big_letter) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->bigLetter = big_letter;
    }
    static bool getBigLetter(void *driverHandlerPtr) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        return drv->bigLetter;
    }
    static void setKbdRowLow(void *driverHandlerPtr, char *kbdRow) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->kbdRowLow = kbdRow;
    }
    static void setKbdRowHi(void *driverHandlerPtr, char *kbdRow) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->kbdRowHi = kbdRow;
    }
    static void restoreKbd(void *driverHandlerPtr) {
        vKbdLineSimple *drv = (vKbdLineSimple *)driverHandlerPtr;
        drv->kbdRowLow = (char *)"1234567890abcdefghijklmnoprstuvwxyz\n";
        drv->kbdRowHi =  (char *)"1234567890ABCDEFGHIJKLMNOPRSTUVWXYZ\n";
    }
private:
    void *screen = NULL;
    void *cursorBlinkTimer = NULL;
    bool cursorState = false;
    bool bigLetter = false;
    bool lastBigLetter = false;
    int8_t cursorPosX = 0;

    hKbd_lrudab hKbd;

    int currPosX = -1;
    int currPosY = -1;
    int lastPosX = -1;
    int lastPosY = -1;
    bool lastVisible = false;
    unsigned int kbdWidth = 128;
    unsigned int lastKbdWidth = -1;

    char *kbdRowLow = (char *)"1234567890abcdefghijklmnoprstuvwxyz\n";
    char *kbdRowHi =  (char *)"1234567890ABCDEFGHIJKLMNOPRSTUVWXYZ\n";

    int kbdHeight = 9;
};

#endif // VKBD_H
