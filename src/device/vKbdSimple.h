#ifndef __V_KBD_SIMPLE_H__
#define __V_KBD_SIMPLE_H__

#include "device/hKbd_lrudab.h"
#include "device/vKbd.h"
#include<stdint.h>
#include<stdio.h>

class vKbdSimple : public vKbd {
public:
    vKbdSimple(void *scr);
    ~vKbdSimple();
    static bool loop(void *driverHandlerPtr, char *key, bool repaint, bool visible = true);
    static void setPosX(void *driverHandlerPtr, int pos) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        drv->currPosX = pos;
    }
    static void setPosY(void *driverHandlerPtr, int pos) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        drv->currPosY = pos;
    }
    static void reset(void *driverHandlerPtr) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        drv->cursorState = false; drv->bigLetter = false;
    }
    static uint8_t getKbdHeight(void *driverHandlerPtr) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        return drv->kbdHeight;
    }
    static void setKbdWidth(void *driverHandlerPtr, unsigned int width) {
    }
    static void setBigLetter(void *driverHandlerPtr, bool big_letter) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        drv->bigLetter = big_letter;
    }
    static bool getBigLetter(void *driverHandlerPtr) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
        return drv->bigLetter;
    }
    static void setKbdRowLow(void *driverHandlerPtr, char *kbdRow) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
    }
    static void setKbdRowHi(void *driverHandlerPtr, char *kbdRow) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
    }
    static void restoreKbd(void *driverHandlerPtr) {
        vKbdSimple *drv = (vKbdSimple *)driverHandlerPtr;
    }
private:
    void *screen = NULL;
    void *cursorBlinkTimer = NULL;
    bool cursorState = false;
    bool bigLetter = false;
    bool lastBigLetter = false;
    int8_t cursorPosX = 0;
    int8_t cursorPosY = 0;

    hKbd_lrudab hKbd;

    int currPosX = -1;
    int currPosY = -1;
    int lastPosX = -1;
    int lastPosY = -1;
    bool lastVisible = false;

    int kbdHeight = 27;
};

#endif // VKBD_H
