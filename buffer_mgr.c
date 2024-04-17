#include "buffer_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "dberror.h"
#include "storage_mgr.h"
#include <math.h>
#include "buffer_mgr_helper.h"

int totalPageRead = 0; // stores the total number of pages read from the buffer pool

int totalPageWritten = 0; // stores the total number of pages written into the buffer pool

int clockRefBit = 0; // Reference bit variable used in CLOCK pin page strategy

int lruBit = 0; // determines least recently added page into the buffer pool

int bmBufferSize = 0; // size of the buffer pool - MAX page frames can be stored in BufferPool

bool globalObj; // global object init checker flag

extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{
    bm->pageFile = (char *)pageFileName;
    bm->strategy = strategy;
    bm->numPages = numPages;

    // Reserver memory space = number of pages x space required for one page
    Frame *buffer = malloc(sizeof(Frame) * numPages);

    // Buffersize is the total number of pages in memory or the buffer pool.
    bmBufferSize = numPages;
    int i = 0;
    while (i < bmBufferSize)
    {
        addPageInController(buffer, i);
        i++;
    }
    globalObj = true;
    printf("\n");
    printf("**********************************************************************************\n");
    printf("*************************BUFFER SUCCESSFULLY INITIALIZED**************************\n");
    printf("**********************************************************************************\n");
    printf("\n"); \
    bm->mgmtData = buffer; \
    totalPageWritten = clockRefBit = 0;\
    return RC_OK;
}

// FIFO (First In First Out) Page Replacement Strategy Implementations
extern RC FIFO_Helper(BM_BufferPool *const bm, Frame *page)
{
    printf("\nFIFO Started \n");
    Frame *frame = (Frame *)bm->mgmtData;

    int i = 0;
    int noOfPagesRead = totalPageRead % bmBufferSize;

    // Interating through all the page frames in the buffer pool
    for (;i < bmBufferSize;)
    {
        if (frame[noOfPagesRead].fixCount != 0)
        {
            // If the current page frame is being used by some client, we move on to the next location
            noOfPagesRead++;
            if(noOfPagesRead % bmBufferSize == 0)
                noOfPagesRead = 0;
            else    
                noOfPagesRead= noOfPagesRead;
            // If page in memory has been modified (dirtyBit = 1), then write page to disk
        }
        else
        {
            if (frame[noOfPagesRead].dirtyFlag == 1)
            {
                SM_FileHandle fh;
                openPageFile(bm->pageFile, &fh);
                writeBlock(frame[noOfPagesRead].pageNum, &fh, frame[noOfPagesRead].pageData);

                // Increase the writeCount which records the number of writes done by the buffer manager.
                totalPageWritten++;
            }

            // Setting page frame's content to new page's content
            frame[noOfPagesRead].fixCount = page->fixCount;
            frame[noOfPagesRead].pageData = page->pageData;
            frame[noOfPagesRead].dirtyFlag = page->dirtyFlag;
            frame[noOfPagesRead].pageNum = page->pageNum;
            break;
        }
        i++;
    }
    printf("\nFIFO DONE \n");
    return RC_OK;
}

// The shutdownBufferPool function writes all the pages to the disk and destroys the buffer pool object.
extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
    // object not initialized
    if (!globalObj)
        return RC_ERROR;

    Frame *frame = (Frame *)bm->mgmtData;

    forceFlushPool(bm);

    int i = 0;
    while (i < bmBufferSize)
    {
        if (frame[i].fixCount != 0)
        {
            return RC_PINNED_PAGES_IN_BUFFER;
        }
        i++;
    }
    free(frame);
    bm->mgmtData = NULL;
    return RC_OK;
}

// The forceFlushPool function writes all the pages to the disk where dirtyFlag is set and fix count is 0
extern RC forceFlushPool(BM_BufferPool *const bm)
{

    // object not initialized
    if (!globalObj)
        return RC_ERROR;

    Frame *frame = (Frame *)bm->mgmtData;

    int eachpage = 0;
    while (eachpage < bmBufferSize)
    {
        if (frame[eachpage].fixCount == 0 && frame[eachpage].dirtyFlag == 1)
        {
            SM_FileHandle fh;
            openPageFile(bm->pageFile, &fh);
            writeBlock(frame[eachpage].pageNum, &fh, frame[eachpage].pageData);
            frame[eachpage].dirtyFlag = 0;
            totalPageWritten++;
        }
        eachpage++;
    }
    return RC_OK;
}

// he markDirty function marks the specified page as dirty.
extern RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Frame *frame = (Frame *)bm->mgmtData;

    int pageIter = 0;
    // Iterating through all the pages in the buffer pool
    while (pageIter < bmBufferSize)
    {
        // If the current page is the page to be marked dirty, then set dirtyBit = 1 (page has been modified) for that page
        if (frame[pageIter].pageNum == page->pageNum)
        {
            frame[pageIter].dirtyFlag = 1;
            return RC_OK;
        }
        pageIter++;
    }
    return RC_ERROR;
}

// The forcePage function writes the specified page to the disk
extern RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Frame *frame = (Frame *)bm->mgmtData;
    int pageIter = 0;

    while (pageIter < bmBufferSize)
    {
        if (frame[pageIter].pageNum == page->pageNum)
        {
            SM_FileHandle fh;
            openPageFile(bm->pageFile, &fh);
            writeBlock(frame[pageIter].pageNum, &fh, frame[pageIter].pageData);
            frame[pageIter].dirtyFlag = 0;
            totalPageWritten++;
        }
        pageIter++;
    }
    return RC_OK;
}

// function that implements Clock Page Replacement Strategy.
extern RC CLOCK_Helper(BM_BufferPool *const bm, Frame *page)
{
    Frame *pageFrameObj = (Frame *)bm->mgmtData;

    while (true)
    {
        clockRefBit = (clockRefBit % bmBufferSize == 0) ? 0 : clockRefBit;

        if (pageFrameObj[clockRefBit].hitCount == 0)
        {
            // If page in memory has been modified (dirtyFlag = 1), then write page to disk
            if (pageFrameObj[clockRefBit].dirtyFlag == 1)
            {
                SM_FileHandle fileHandle;
                openPageFile(bm->pageFile, &fileHandle);
                writeBlock(pageFrameObj[clockRefBit].pageNum, &fileHandle, pageFrameObj[clockRefBit].pageData);

                // Increase the totalPageWritten which records the number of writes done by the buffer manager.
                totalPageWritten++;
            }

            // Setting page frame's content to new page's content
            addReplacememtPageContentsToFrame(pageFrameObj, clockRefBit, page);
            clockRefBit++;
            break;
        }
        else
        {
            // Incrementing clockRefBit so that we can check the next page frame location.
            // CAREFUL:: Setting hitCount = 0 to prevent infinite loop
            pageFrameObj[clockRefBit++].hitCount = 0;
        }
    }
    return RC_OK;
}

// function returns an array of PageNumbers (of size numPages)
extern PageNumber *getFrameContents(BM_BufferPool *const bm)
{
    Frame *pageFrameObj = (Frame *)bm->mgmtData;
    PageNumber *pageNumberArr = malloc(sizeof(PageNumber) * bmBufferSize);

    int eachPage;
    for (eachPage = 0; eachPage < bmBufferSize; eachPage++)
    {
        pageNumberArr[eachPage] = (pageFrameObj[eachPage].pageNum != -1) ? pageFrameObj[eachPage].pageNum : NO_PAGE;
    }
    return pageNumberArr;
}

// function returns an array of bools (of size numPages)
extern bool *getDirtyFlags(BM_BufferPool *const bm)
{
    Frame *pageFrameObj = (Frame *)bm->mgmtData;
    bool *boolArr = malloc(sizeof(bool) * bmBufferSize);

    int i=0;
    for (;i < bmBufferSize; i++)
    {
        if(pageFrameObj[i].dirtyFlag ==1)
            boolArr[i]=true;
        else    
            boolArr[i]=false;
    }
    return boolArr;
}

// function returns an array of ints (of size numPages)
extern int *getFixCounts(BM_BufferPool *const bm)
{
    Frame *pageFrameObj = (Frame *)bm->mgmtData;
    int *fixCountArr = malloc(sizeof(int) * bmBufferSize);

    int eachPage=0;
    for (; eachPage < bmBufferSize; eachPage++)
    {
        fixCountArr[eachPage] = (pageFrameObj[eachPage].fixCount != -1) ? pageFrameObj[eachPage].fixCount : 0;
    }
    return fixCountArr;
}

// The getNumReadIO function returns the number of pages that have been read from disk
extern int getNumReadIO(BM_BufferPool *const bm)
{
    return (totalPageRead + 1);
}

// The getNumWriteIO method returns the number of pages written to the page file
extern int getNumWriteIO(BM_BufferPool *const bm)
{
    return totalPageWritten;
}

// After the client is done using the page, it is set free
extern RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Frame *frame = (Frame *)bm->mgmtData;
    int i = 0;
    for(;i<bmBufferSize;i++){
        // If the buffer manager page number matches with the page that needs to be unpinned,
        // then decrease fixCount to set it free
        if (frame[i].pageNum == page->pageNum)
        {
            frame[i].fixCount-=1;
            break;
        }
    }
    return RC_OK;
}

// LRU (Least Recently Used) Page Replacement Strategy Implementations
extern RC LRU_Helper(BM_BufferPool *const bm, Frame *page)
{
    Frame *frame = (Frame *)bm->mgmtData;
    int minHitIdex, minHitCount;

    // Traverse all frames in the buffer pool to find a page frame with fixCount = 0
    int index = 0;

    while (index < bmBufferSize)
    {
        if (frame[index].fixCount == 0)
        {
            minHitCount = frame[index].hitCount;
            minHitIdex = index;
            break;
        }
        index++;
    }

    // Traverse all frames in the buffer pool to find a page frame with minimum hitCount (LRU) page
    index = minHitIdex + 1;
    while (index < bmBufferSize)
    {
        if (frame[index].hitCount < minHitCount)
        {
            minHitCount = frame[index].hitCount;
            minHitIdex = index;
        }
        index++;
    }

    // If page in memory has been modified (dirtyBit = 1), then write page to disk
    if (frame[minHitIdex].dirtyFlag == 1)
    {
        SM_FileHandle fileHandle;
        openPageFile(bm->pageFile, &fileHandle);
        writeBlock(frame[minHitIdex].pageNum, &fileHandle, frame[minHitIdex].pageData);
        totalPageWritten++;
    }

    addReplacememtPageContentsToFrame(frame, minHitIdex, page);
    return RC_OK;
}

// Checking if buffer pool is empty and this is the first page to be pinned
RC occupyFirstSpaceInBufferPool(BM_BufferPool *const bm, BM_PageHandle *page, const PageNumber pageNum)
{

    Frame *frame = (Frame *)bm->mgmtData;
    if (frame[0].pageNum == -1)
    {
        SM_FileHandle fileHandle;
        openPageFile(bm->pageFile, &fileHandle);
        frame[0].pageData = (SM_PageHandle)malloc(PAGE_SIZE);
        ensureCapacity(pageNum, &fileHandle);
        readBlock(pageNum, &fileHandle, frame[0].pageData);
        frame[0].fixCount++;
        frame[0].pageNum = pageNum;
        frame[0].hitCount = totalPageRead = lruBit = 0;

        page->pageNum = pageNum;
        page->data = frame[0].pageData;

        return RC_OK;
    }

    return RC_ERROR;
}

// set hit counts of various algos as per strategy
void setHitCounts(ReplacementStrategy strategy, Frame *frame, int counter)
{
    switch (strategy)
    {
    case RS_LRU:
        frame[counter].hitCount = lruBit;
        break;
    case RS_CLOCK:
        frame[counter].hitCount = 1;
        break;

    case RS_LRU_K:
    case RS_LFU:
        printf("The given strategy [%d] is not implemented !!!", strategy);
        break;
    case RS_FIFO:
    default:
        break;
    }
}

// If the Buffer Pool is Full, we need to replace an existing page using page replacement strategy
RC replacePageInBufferPool(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

    Frame *newPage = (Frame *)malloc(sizeof(Frame));

    // Reading page from disk and initializing page frame's contents in the buffer pool
    SM_FileHandle fileHandle;
    newPage->pageData = (SM_PageHandle)malloc(PAGE_SIZE);

    openPageFile(bm->pageFile, &fileHandle);
    readBlock(pageNum, &fileHandle, newPage->pageData);
    newPage->pageNum = pageNum;
    newPage->dirtyFlag = 0;
    newPage->fixCount = 1;

    totalPageRead++;
    lruBit++;

    switch (bm->strategy)
    {
    case RS_LRU:
        newPage->hitCount = lruBit;
        break;
    case RS_CLOCK:
        newPage->hitCount = 1;
        break;
    case RS_LRU_K:
    case RS_LFU:
        printf("The given strategy [%d] is not implemented !!!", bm->strategy);
        break;
    case RS_FIFO:
    default:
        break;
    }

    page->pageNum = pageNum;
    page->data = newPage->pageData;

    switch (bm->strategy)
    {
    case RS_FIFO:

        return FIFO_Helper(bm, newPage);

    case RS_LRU:
        return LRU_Helper(bm, newPage);

    case RS_CLOCK:
        return CLOCK_Helper(bm, newPage);

    default:
        printf("The given strategy [%d] is not implemented !!!", bm->strategy);
        return RC_ERROR;
    }
}

// The pinPage function pins the page with page number "pageNum".
extern RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
    // object not initialized
    if (globalObj == false)
        return RC_ERROR;

    // Checking if buffer pool is empty and this is the first page to be pinned
    if (occupyFirstSpaceInBufferPool(bm, page, pageNum) == RC_OK)
        return RC_OK;

    bool isBufferFull = true;
    Frame *frame = (Frame *)bm->mgmtData;
    int counter = 0;

    for (; counter < bmBufferSize; counter++)
    {
        if (frame[counter].pageNum != -1)
        {
            if (frame[counter].pageNum == pageNum) // if the Page is already Present In Buffer Pool
            {

                // increment fixcount and lruBit to indicate one more client is using the page now
                frame[counter].fixCount++;
                lruBit++;
                clockRefBit++;
                isBufferFull = false;
                setHitCounts(bm->strategy, frame, counter);
                page->pageNum = pageNum;
                page->data = frame[counter].pageData;
                break;
            }
        }
        else
        {

            SM_FileHandle fileHandle;
            openPageFile(bm->pageFile, &fileHandle);
            frame[counter].pageData = (SM_PageHandle)malloc(PAGE_SIZE);
            readBlock(pageNum, &fileHandle, frame[counter].pageData);

            frame[counter].fixCount = 1;
            frame[counter].pageNum = pageNum;
            lruBit++;
            totalPageRead++;

            setHitCounts(bm->strategy, frame, counter);

            page->pageNum = pageNum;
            page->data = frame[counter].pageData;

            isBufferFull = false;
            break;
        }
    }

    if (isBufferFull == true)
    {
        return replacePageInBufferPool(bm, page, pageNum);
    }
    return RC_OK;
}
