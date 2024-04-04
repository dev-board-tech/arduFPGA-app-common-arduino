#if defined (__AVR_MEGA__) && defined(USE_TEXT_EDITOR)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "def.h"
#include "textEditor.h"
#include "util/util.h"


static textEditorPtr getNextLine(textEditor_t *inst, textEditorPtr cursorPos);
static textEditorPtr getStartOfCurrentLine(textEditor_t *inst, textEditorPtr cursorPos);
static textEditorPtr getPreviousLine(textEditor_t *inst, textEditorPtr cursorPos);
static void moveString(textEditor_t *inst, textEditorPtr from, textEditorPtr to);
static int _drawStringWindowed4x6(textEditor_t *inst, box_t *box, textEditorPtr string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState);

void textEditor_Init(textEditor_t *inst) {
    inst->files = (textEditorFile_t **)calloc(1, sizeof(textEditorFile_t *));
    inst->busy = false;
    inst->winX = 0;
    inst->winY = 0;
    inst->cursorPos = 0;
    inst->edited = true;
    inst->cursorState = false;
    inst->box.x_min = 0;
    inst->box.y_min = 0;
    inst->box.x_max = 128;
    inst->box.y_max = 64;
    inst->timer.SetInterval(700);
    inst->lineCnt = 0;
    inst->timer.Start();
}

void textEditorDeInit(textEditor_t *inst) {
    inst->file.close();
    if(inst->files) {
        textEditorFile_t *ptr = inst->files[0];
        for(; ptr != NULL; ptr++) {
            free(ptr);
            ptr = NULL;
        }
        free(inst->files);
        inst->files = NULL;
    }
}

static int filesize(textEditor_t *inst) {
	for(int cnt = 0; cnt < inst->maxFileLen; cnt++) {
		if(((uint8_t *)inst->filePtr)[cnt] == 0) {
			return cnt;
		}
	}
	return inst->maxFileLen - 1;
}

static bool write(textEditor_t *inst, textEditorPtr addr, uint8_t data) {
    ((uint8_t *)inst->filePtr)[addr] = data;
	return true;
}

static void writeChunk(textEditor_t *inst, textEditorPtr addr, uint8_t len, char *buf) {
    memcpy(((uint8_t *)inst->filePtr) + addr, buf, len);
}

static uint8_t read(textEditor_t *inst, textEditorPtr addr) {
    return ((uint8_t *)inst->filePtr)[addr];
}

static void readChunk(textEditor_t *inst, textEditorPtr addr, uint8_t len, char *buf) {
    memcpy(buf, ((uint8_t *)inst->filePtr) + addr, len);
}

static void resetCursorState(textEditor_t *inst) {
    inst->edited = true;
    inst->cursorState = true;
    inst->timer.Start();
}

static void paintWindow(textEditor_t *inst) {
	defaultScreen->drvClear(0);
	//DISPLAY_FUNC_CLEAR(inst->gfxStr.spi, inst->gfxStr.vram, 0);
    inst->gfxStr.setTransparent(true);
    box_t bTmp = inst->box;
    uint8_t lineNrSpace = 0;
    if(!inst->textEditMode) {
        bTmp.x_min += /*getCharWidth4x6()*/4 * 6;
        lineNrSpace = 6;
    }
    _drawStringWindowed4x6(inst, &bTmp, inst->winY, inst->winX + (lineNrSpace * /*getCharWidth4x6()*/4), 0, inst->cursorPos - inst->winY, inst->cursorState);
	//DISPLAY_FUNC_CLEAR(inst->gfxStr.spi, inst->gfxStr.vram, false);
	//DISPLAY_REFRESH(inst->gfxStr.spi, inst->gfxStr.vram);
	//DISPLAY_FUNC_CLEAR(inst->gfxStr.spi, inst->gfxStr.vram, true);
	//DISPLAY_REFRESH(inst->gfxStr.spi, inst->gfxStr.vram);
    if((inst->gfxStr.getEdgeTouch() & inst->gfxStr.EDGE_OUTSIDE_LEFT) && inst->winX + /*getCharWidth4x6()*/4 <= 0) {
        inst->winX += /*getCharWidth4x6()*/4;
        inst->edited = true;
    }
    if(inst->gfxStr.getEdgeTouch() & inst->gfxStr.EDGE_OUTSIDE_RIGHT) {
        inst->winX -= /*getCharWidth4x6()*/4;
        inst->edited = true;
    }
    if((inst->gfxStr.getEdgeTouch() & inst->gfxStr.EDGE_OUTSIDE_UP) || inst->cursorPos <= inst->winY) {
        inst->winY = getPreviousLine(inst, inst->winY);
        inst->lineCnt--;
        inst->edited = true;
    }
    if(inst->gfxStr.getEdgeTouch() & inst->gfxStr.EDGE_OUTSIDE_DOWN) {
        inst->winY = getNextLine(inst, inst->winY);
        inst->lineCnt++;
        inst->edited = true;
    }
    if(!inst->winY) {
        inst->lineCnt = 0;
    }
    inst->gfxStr.setTransparent(false);
    _drawStringWindowed4x6(inst, &bTmp, inst->winY, inst->winX + (lineNrSpace * /*getCharWidth4x6()*/4), 0, inst->cursorPos - inst->winY, inst->cursorState);

    if(!inst->textEditMode) {
        bTmp = inst->box;
        bTmp.x_max = /*getCharWidth4x6()*/4 * 6;
        char mpS[7];
        int t = inst->lineCnt + 1;
        for(uint16_t cnt = 0; cnt < defaultScreen->getY() ; cnt += /*getCharHeight4x6()*/6) {
            char *mpSp = mpS;
            //sprintf(mpS, "%05d|", t);
            if(t < 10000)
                *mpSp++ = '0';
            if(t < 1000)
                *mpSp++ = '0';
            if(t < 100)
                *mpSp++ = '0';
            if(t < 10)
                *mpSp++ = '0';
            util_utoa(t, mpSp);
            mpS[5] = '|';
            mpS[6] = '\0';
            inst->gfxStr.drawStringWindowed4x6(&bTmp, mpS, 0, cnt, -1, 0);
            //drawStringWindowed4x6(&inst->gfxStr, &bTmp, mpS, 0, cnt, -1, false);
            t++;
        }
        char *mpSp = mpS;
        t = filesize(inst);
        if(t < 100000)
			*mpSp++ = '0';
        if(t < 10000)
			*mpSp++ = '0';
        if(t < 1000)
			*mpSp++ = '0';
        if(t < 100)
			*mpSp++ = '0';
        if(t < 10)
			*mpSp++ = '0';
        util_utoa(t, mpSp);
        mpS[6] = '\0';
        bTmp = inst->box;
        defaultScreen->drvDrawRectangleClip(&bTmp, defaultScreen->getX() - (/*getCharWidth4x6()*/ 4 * 6) - 1, defaultScreen->getY() - /*getCharHeight4x6()*/ 6 - 1, (/*getCharWidth4x6()*/ 4 * 5) + 1, /*getCharHeight4x6()*/ 6 + 1, true, inst->gfxStr.getForeColor());
        //ssd1306_draw_rectangle(inst->gfxStr.spi, &bTmp, inst->gfxStr.vram, DISPLAY_FUNC_GET_X() - (getCharWidth4x6() * 6) - 1, DISPLAY_FUNC_GET_Y()  - getCharHeight4x6() - 1, (getCharWidth4x6() * 5) + 1, getCharHeight4x6() + 1, true, inst->gfxStr.foreColor);
        inst->gfxStr.drawStringWindowed4x6(&bTmp, mpS, defaultScreen->getX() - (/*getCharWidth4x6()*/ 4 * 6), defaultScreen->getY()  - /*getCharHeight4x6()*/ 6, -1, 0);
    }
    defaultScreen->drvRefresh();
    //DISPLAY_REFRESH(inst->gfxStr.spi, inst->gfxStr.vram);
    inst->busy = false;
}

void textEditor_Idle(textEditor_t *inst) {
    if(inst->edited) {
        inst->edited = false;
        paintWindow(inst);
    }
    if(inst->timer.Tick()) {
        if(inst->cursorState)
            inst->cursorState = false;
        else
            inst->cursorState = true;
        inst->edited = true;
    }
}

textEditor_Err_e addFile(textEditor_t *inst, char *fileName) {
    textEditorFile_t **ptr = inst->files;
    int fileCnt = 0;
    for(; ptr[fileCnt] != NULL; fileCnt++) {}
    ptr = (textEditorFile_t **)malloc(sizeof(textEditorFile_t *) * (fileCnt + 1));
    if(!ptr)
        return TEXT_EDITOR_ERR_OUT_OF_MEMORY;
    memcpy(ptr, inst->files, sizeof(textEditorFile_t *) * fileCnt);
    //free(inst->files);
    inst->files = ptr;
    ptr[fileCnt] = NULL;
    ptr[fileCnt + 1] = NULL;
    textEditorFile_t *fil = (textEditorFile_t *)calloc(1, sizeof(textEditorFile_t));
    if(!fil)
        return TEXT_EDITOR_ERR_OUT_OF_MEMORY;
    ptr[fileCnt] = fil;
    char *fName = (char *)calloc(1, /*strlen(fileName) + 1*/36);
    if(!fName)
        return TEXT_EDITOR_ERR_OUT_OF_MEMORY;
    //strncpy(fName, fileName, 17);
    memcpy(fName, fileName, 34);
    fil->fileName = fName;
    return TEXT_EDITOR_ERR_OK;
}

textEditor_Err_e changeFile(textEditor_t *inst, uint8_t fileNr) {
    char chunk[16];
    uint32_t bw;
    if(inst->activeFile != (uint8_t)-1 && inst->activeFile != fileNr && inst->files[inst->activeFile]->edited) {
        // Restore the asm file from the temporary file in order to be compatible with PC compilers and be readable.
        inst->file = SD.open(inst->files[inst->activeFile]->fileName, FILE_WRITE);
        if(!inst->file)
            return TEXT_EDITOR_ERR_OPEN_ASM_FILE;
        // Search the end of the text in the temporary file.
        int32_t p = inst->maxFileLen;
        char *pp;
        do {
            p -= sizeof(chunk);
            readChunk(inst, p, sizeof(chunk), chunk);
            pp = chunk + sizeof(chunk);
            for(; pp > chunk; ) {
                pp--;
                if(*pp != '\0')
                    break;
            }
        } while(pp == chunk && p > 0);
        // Copy the content of the temporary file to the open file.
        int32_t end = p + sizeof(chunk);
        for(p = 0; p < end; p+= sizeof(chunk)) {
            readChunk(inst, p, sizeof(chunk), chunk);
            bw = inst->file.write(chunk, strnlen(chunk, sizeof(chunk)));
            if(bw != strnlen(chunk, sizeof(chunk))) {
				inst->file.close();
                return TEXT_EDITOR_ERR_WRITE;
            }
        }
        //f_truncate(&inst->file);
		inst->file.close();
        inst->files[fileNr]->cursorPos = inst->cursorPos;// Save the cursor and window position.
        inst->files[fileNr]->winX= inst->winX;
        inst->files[fileNr]->winY = inst->winY;
        inst->files[fileNr]->lineCnt = inst->lineCnt;
        inst->files[inst->activeFile]->edited = false;
    }
    if(inst->activeFile != (uint8_t)-1 || inst->activeFile != fileNr) {
        // Copy the edited file to a temporary file and make each line "maxTmpLineLen" characters long with the difference filled with space characters.
        // This way we will avoid shifting the right part of the rest of the file to make space for new characters, but the maximum line len will be limited by "maxTmpLineLen".
    	inst->file = SD.open(inst->files[fileNr]->fileName, FILE_READ);
    	if(!inst->file)
            return TEXT_EDITOR_ERR_OPEN_ASM_FILE;
        memset(chunk, 0, sizeof(chunk));
        for(textEditorPtr p = 0; p < inst->maxFileLen; p += sizeof(chunk))
            writeChunk(inst, p, sizeof(chunk), chunk);
        inst->activeFile = fileNr;
        uint32_t br;
        textEditorPtr p = 0;
        do {
            br = inst->file.read(chunk, sizeof(chunk));
            if(!br) {
				inst->file.close();
                return TEXT_EDITOR_ERR_READ;
            }
            if(!br)
                break;
            writeChunk(inst, p, br, chunk);
            p += br;
        } while(p < inst->maxFileLen);
		inst->file.close();
        inst->cursorPos = inst->files[fileNr]->cursorPos; // Restore the cursor and window position.
        inst->winX = inst->files[fileNr]->winX;
        inst->winY = inst->files[fileNr]->winY;
        inst->lineCnt = inst->files[fileNr]->edited;
    }
    return TEXT_EDITOR_ERR_OK;
}

static bool isNewLine(textEditor_t *inst, textEditorPtr ptr) {
    if(read(inst, ptr) == '\n')
        return true;
    return false;
}

static void moveString(textEditor_t *inst, textEditorPtr from, textEditorPtr to) {
    char chunk[16];
    if(from < to) {
	    int8_t l = sizeof(chunk);
	    textEditorPtr sp = inst->maxFileLen - (to - from);
	    textEditorPtr dp = inst->maxFileLen;
	    do {
		    if(sp < from + sizeof(chunk))
				l = sp - from;
		    if(l == 0)
				break;
		    sp -= l; dp -= l;
		    readChunk(inst, sp, l, chunk);
		    writeChunk(inst, dp, l, chunk);
	    }while (1);
		inst->files[inst->activeFile]->edited = true;
	} else if(from > to) {
	    uint8_t l = sizeof(chunk);
	    textEditorPtr sp = from, dp = to;
	    do {
		    if(sp + l > inst->maxFileLen - 1)
				l = inst->maxFileLen - 1 - sp;
		    if(l == 0)
				break;
		    readChunk(inst, sp, l, chunk);
		    writeChunk(inst, dp, l, chunk);
		    sp += l; dp += l;
	    } while(1);
		inst->files[inst->activeFile]->edited = true;
   }
}

void textEditor_edit(textEditor_t *inst, uint8_t c) {
	if(c != 0x07 && c != 0x08 && inst->cursorPos == inst->maxFileLen - 1)
		return;
    if(!inst->busy) {
        inst->busy = true;
        textEditorPtr prevLine;
        switch(c) {
            case 0x08://Backspace 0x08
                if(inst->cursorPos == 0)
                    return;
                inst->cursorPos -= 1;
            case 0x07://Delete 0x07
                moveString(inst, inst->cursorPos + 1, inst->cursorPos);
                resetCursorState(inst);
                break;
            case '\n':// Enter
				prevLine = getStartOfCurrentLine(inst, inst->cursorPos);
				if((read(inst, prevLine) == '\t' && isNewLine(inst, inst->cursorPos)) || read(inst, inst->cursorPos - 1) == '\t' || read(inst, inst->cursorPos) == '\t') {
					moveString(inst, inst->cursorPos, inst->cursorPos + 1);
					write(inst, inst->cursorPos, '\n');
					if(read(inst, inst->cursorPos + 1) == '\t') {
						inst->cursorPos+=2;
						break;
					}
					inst->cursorPos++;
					c = '\t';
				}
            default://The rest
				moveString(inst, inst->cursorPos, inst->cursorPos + 1);
				write(inst, inst->cursorPos, c);
				inst->cursorPos += 1;
				resetCursorState(inst);
				break;
        }
    }
}

static textEditorPtr getNextLine(textEditor_t *inst, textEditorPtr cursorPos) {
    for (textEditorPtr cnt = cursorPos; cnt < inst->maxFileLen; cnt++) {
        if(isNewLine(inst, cnt))
            return cnt + 1;
    }
    for (textEditorPtr cnt = cursorPos; cnt > 0; cnt--) {
        if(isNewLine(inst, cnt))
            return cnt + 1;
    }
    return 0;
}

static textEditorPtr getStartOfCurrentLine(textEditor_t *inst, textEditorPtr cursorPos) {
	textEditorPtr cnt;
	for (cnt = cursorPos - 1; cnt > 0; cnt--) {
		if(isNewLine(inst, cnt))
		return cnt + 1;
	}
	return 0;
}

static textEditorPtr getPreviousLine(textEditor_t *inst, textEditorPtr cursorPos) {
    textEditorPtr cnt;
    for (cnt = cursorPos; cnt > 0; cnt--) {
        if(isNewLine(inst, cnt))
            break;
    }
    if(!cnt)
        return 0;
    if(cnt == cursorPos)
        cnt--;
    for (cnt = cnt - 1; cnt > 0; cnt--) {
        if(isNewLine(inst, cnt))
            return cnt + 1;
    }
    return 0;
}

void textEditor_goLeft(textEditor_t *inst) {
    if(!inst->busy) {
        inst->busy = true;
        if(inst->cursorPos > 0)
            inst->cursorPos--;
        else
            return;
        resetCursorState(inst);
    }
}

void textEditor_goRight(textEditor_t *inst) {
    if(!inst->busy) {
        inst->busy = true;
        if(inst->cursorPos < inst->maxFileLen && read(inst, inst->cursorPos) != 0)
            inst->cursorPos++;
        else
            return;
        resetCursorState(inst);
    }
}

void textEditor_goUp(textEditor_t *inst) {
    if(!inst->busy) {
        inst->busy = true;
        textEditorPtr prevLine = getPreviousLine(inst, inst->cursorPos);
        inst->cursorPos = prevLine;
        resetCursorState(inst);
    }
}

void textEditor_goDown(textEditor_t *inst) {
    if(!inst->busy) {
        inst->busy = true;
        textEditorPtr nextLine = getNextLine(inst, inst->cursorPos);
        inst->cursorPos = nextLine;
        resetCursorState(inst);
    }
}

static int _drawStringWindowed4x6(textEditor_t *inst, box_t *box, textEditorPtr string, int16_t x, int16_t y, int16_t cursorPos, bool cursorState) {
    box_t box__;
    if(box) {
        box__.x_min = box->x_min;
        box__.x_max = box->x_max;
        box__.y_min = box->y_min;
        box__.y_max = box->y_max;
    } else {
        box__.x_min = 0;
        box__.x_max = 128;
        box__.y_min = 0;
        box__.y_max = 64;
    }
    int16_t cX = x;
    int16_t cY = y;
    int16_t cCnt = 0;
    inst->gfxStr.setEdgeTouch(false);
    textEditorPtr str = string;
    do {
        if(cCnt == inst->gfxStr.getMaxLen() && inst->gfxStr.getMaxLen())
            return cCnt;
        if(cursorPos == cCnt) {
            if(cX < box__.x_min)
                inst->gfxStr.setEdgeTouch(inst->gfxStr.getEdgeTouch() | inst->gfxStr.EDGE_OUTSIDE_LEFT);
            if(cY < box__.y_min)
                inst->gfxStr.setEdgeTouch(inst->gfxStr.getEdgeTouch() | inst->gfxStr.EDGE_OUTSIDE_UP);
            if(cX >= box__.x_max)
                inst->gfxStr.setEdgeTouch(inst->gfxStr.getEdgeTouch() | inst->gfxStr.EDGE_OUTSIDE_RIGHT);
            if(cY >= box__.y_max - 6)
                inst->gfxStr.setEdgeTouch(inst->gfxStr.getEdgeTouch() | inst->gfxStr.EDGE_OUTSIDE_DOWN);
        }
        char C, c = C = read(inst, str);
        if(cursorPos == cCnt) {
            if((cursorState) || (cursorState && C == 0))
                C = (uint8_t)(96+32);
        }
        if(!inst->gfxStr.getTransparent()) {
            if(inst->gfxStr.getForeColor() != inst->gfxStr.getInkColor()) {
            	defaultScreen->drvDrawRectangleClip(box, cX, cY, /*getCharWidth4x6()*/ 4, /*getCharHeight4x6()*/ 6, true, inst->gfxStr.getForeColor());
            }
            defaultScreen->drawChar4x6(cX, cY, C, inst->gfxStr.getForeColor(), inst->gfxStr.getInkColor());
		}
        if (c == 0)
            return cCnt - 1;
        switch (c) {
        case '\n':
            if(cY > box__.y_max)
                return cCnt;
            cX = x;
            cY += 6;
            break;
        case 0x09:
            cX = (4 * inst->gfxStr.getTabSpaces()) + ((cX / (4 * inst->gfxStr.getTabSpaces())) * (4 * inst->gfxStr.getTabSpaces()));
            break;
        default:
            cX += 4;
            break;
        }
        str++;
        cCnt++;
    } while (1);
}

#endif
