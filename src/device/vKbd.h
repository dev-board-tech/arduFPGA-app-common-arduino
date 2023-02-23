#ifndef __V_KBD_H__
#define __V_KBD_H__

#include <stdint.h>
#include <stdio.h>

class vKbd {
public:
    inline bool loop(char *key, bool repaint, bool visible) {
        return Loop(DriverPtr, key, repaint, visible);
    }
    inline void setPosX(int pos) {
        SetPosX(DriverPtr, pos);
    }
    inline void setPosY(int pos) {
        SetPosY(DriverPtr, pos);
    }
    inline void reset() {
        Reset(DriverPtr);
    }
    inline uint8_t getKbdHeight() {
        return GetKbdHeight(DriverPtr);
    }
    inline void setKbdWidth(unsigned int width) {
        SetKbdWidth(DriverPtr, width);
    }
    inline void setBigLetter(bool big_letter) {
        SetBigLetter(DriverPtr, big_letter);
    }
    inline bool getBigLetter() {
        return GetBigLetter(DriverPtr);
    }
    inline void setKbdLineRowLow(char *kbdRow) {
        SetKbdRowLow(DriverPtr, kbdRow);
    }
    inline void setKbdLineRowHi(char *kbdRow) {
        SetKbdRowHi(DriverPtr, kbdRow);
    }
    inline void restoreKbd(void *driverHandlerPtr) {
        RestoreKbd(driverHandlerPtr);
    }
protected:
    bool (*Loop)(void *driverHandlerPtr, char *key, bool repaint, bool visible);
    void (*SetPosX)(void *driverHandlerPtr, int pos);
    void (*SetPosY)(void *driverHandlerPtr, int pos);
    void (*Reset)(void *driverHandlerPtr);
    uint8_t (*GetKbdHeight)(void *driverHandlerPtr);
    void (*SetKbdWidth)(void *driverHandlerPtr, unsigned int width);
    void (*SetBigLetter)(void *driverHandlerPtr, bool big_letter);
    bool (*GetBigLetter)(void *driverHandlerPtr);
    void *DriverPtr = NULL;
    void (*SetKbdRowLow)(void *driverHandlerPtr, char *kbdRow);
    void (*SetKbdRowHi)(void *driverHandlerPtr, char *kbdRow);
    void (*RestoreKbd)(void *driverHandlerPtr);
};

#endif
