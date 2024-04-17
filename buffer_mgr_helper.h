#ifndef BUFFER_MGR_HELPER_H
#define BUFFER_MGR_HELPER_H

#include "buffer_mgr.h"

extern void addPageInController(Frame *buffer, int i);
extern void addReplacememtPageContentsToFrame(Frame *frame, int minHitIndex, Frame *page);

#endif