#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


extern void addPageInController(Frame *buffer, int i)
{
  buffer[i].pageData = NULL;
  buffer[i].pageNum = -1;
  buffer[i].dirtyFlag = 0;
  buffer[i].fixCount = 0;
  buffer[i].hitCount = 0;
}

extern void addReplacememtPageContentsToFrame(Frame *frame, int minHitIndex, Frame *page)
{
    frame[minHitIndex].hitCount = page->hitCount;
    frame[minHitIndex].fixCount = page->fixCount;
    frame[minHitIndex].pageNum = page->pageNum;
    frame[minHitIndex].pageData = page->pageData;
    frame[minHitIndex].dirtyFlag = page->dirtyFlag;
}