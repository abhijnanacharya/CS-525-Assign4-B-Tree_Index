#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

// Include return codes and methods for logging errors
#include "dberror.h"
#include "storage_mgr.h"
// Include bool DT
#include "dt.h"

// Replacement Strategies
typedef enum ReplacementStrategy
{
    RS_FIFO = 0,
    RS_LRU = 1,
    RS_CLOCK = 2,
    RS_LFU = 3,
    RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1

typedef struct BM_BufferPool
{
    char *pageFile;
    int numPages;
    ReplacementStrategy strategy;
    void *mgmtData; // use this one to store the bookkeeping info your buffer
                    // manager needs for a buffer pool
} BM_BufferPool;

typedef struct BM_PageHandle
{
    PageNumber pageNum;
    char *data;
} BM_PageHandle;

// Page Frame Reference to store value
typedef struct Frame
{
    SM_PageHandle pageData; // Data of page
    PageNumber pageNum;     // Each Page Number in Page Frame
    int dirtyFlag;          // determines if page is Dirty
    int fixCount;           // gives count of current users accessing the page
    int hitCount;           // pointer to the previous frame in the list
} Frame;

typedef struct Statlist
{
    Frame *fpt;            // frame pointer
    struct Statlist *next; // pointer to the next node in the status list
} Statlist;

typedef struct Buffer
{                           // buffer pool structure
    int numFrames;          // number of frames in the frame list
    int readCount;          // count of read operations
    int writeCount;         // count of write operations
    void *stratData;        // strategy-specific data
    Frame *head;            // pointer to the first frame in the frame list
    Frame *tail;            // pointer to the last frame in the frame list
    Frame *pointer;         // pointer for special purposes; initialized as the head
    Statlist *statListHead; // pointer to the head of the status list
} Buffer;

// convenience macros
#define MAKE_POOL() \
    ((BM_BufferPool *)malloc(sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE() \
    ((BM_PageHandle *)malloc(sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
           const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents(BM_BufferPool *const bm);
bool *getDirtyFlags(BM_BufferPool *const bm);
int *getFixCounts(BM_BufferPool *const bm);
int getNumReadIO(BM_BufferPool *const bm);
int getNumWriteIO(BM_BufferPool *const bm);

#endif
