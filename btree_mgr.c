








//creating a new B+ Tree. 
//initializePage initializes the TreeManager structure which stores additional information of our B+ Tree.
RC initializePage(SM_FileHandle *fhandle, DataType keyType, int n) {
    SM_PageHandle pageData = (SM_PageHandle)malloc(PAGE_SIZE);
    if (pageData == NULL) {
        return RC_MEMORY_ALLOCATION_ERROR;
    }

    memcpy(pageData, &keyType, sizeof(DataType));
    memcpy(pageData + sizeof(DataType), &n, sizeof(int));
    RC rc = writeBlock(0, fhandle, pageData);
    free(pageData);
    return rc;
}

RC createBtree(char *idxId, DataType keyType, int n) {
    if (idxId == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RC rc;
    SM_FileHandle fhandle;

    if ((rc = createPageFile(idxId)) != RC_OK) {
        return rc;
    }

    if ((rc = openPageFile(idxId, &fhandle)) != RC_OK) {
        return rc;
    }

    rc = initializePage(&fhandle, keyType, n);
    closePageFile(&fhandle);
    return rc;
}

//The below function opens an existing B+ Tree which is stored on the file specified by "idxId" parameter.
//We retrieve our TreeManager and initialize the Buffer Pool.
// Open or initialize a B-tree structure with a given index ID

RC openBtree(BTreeHandle **tree, char *idxId) {
    if (idxId == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    BTreeHandle *tempTree = (BTreeHandle *)malloc(sizeof(BTreeHandle));
    if (!tempTree) {
        return RC_MEMORY_ALLOCATION_ERROR;
    }
    *tree = tempTree;

    BM_BufferPool *bm = MAKE_POOL();
    RC status = initBufferPool(bm, idxId, 10, RS_CLOCK, NULL);
    if (status != RC_OK) {
        free(tempTree);
        return status;
    }

    BM_PageHandle *page = MAKE_PAGE_HANDLE();
    status = pinPage(bm, page, 0);
    if (status != RC_OK) {
        shutdownBufferPool(bm);
        free(tempTree);
        return status;
    }

    int keyTypeVal;
    memcpy(&keyTypeVal, page->data, sizeof(int));

    tempTree->keyType = (DataType)keyTypeVal;

// Cast page->data to an int pointer and increment the pointer
    int *dataPtr = (int *)page->data;
    dataPtr++;

// Use the dereferenced pointer to directly access maxKeys
    int maxKeys = *dataPtr;

// Decrement the pointer to restore original position
    dataPtr--;
    page->data = (char *)dataPtr;
    

    RM_bTree_mgmtData *managementData = (RM_bTree_mgmtData *)malloc(sizeof(RM_bTree_mgmtData));
    if (!managementData) {
        shutdownBufferPool(bm);
        free(page);
        free(tempTree);
        return RC_MEMORY_ALLOCATION_ERROR;
    }

    managementData->numEntries = 0;
    managementData->maxKeyNum = maxKeys;
    managementData->bp = bm;
    tempTree->mgmtData = managementData;

    free(page);
    return RC_OK;
}


// Closes a B-tree structure and releases associated resources
RC closeBtree(BTreeHandle *tree) {
    if (!tree) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
    RC rc = shutdownBufferPool(bTreeMgmt->bp);
    if (rc == RC_OK) {
        free(bTreeMgmt);
        free(tree);
        if (root) { // Safely handle global variable
            free(root);
            root = NULL;
        }
    }

    return rc;
}


//This function deletes the page file having the specified file name "idxId" in the parameter. It uses Storage Manager for this purpose.
RC deleteBtree(char *idxId) {
    RC rc = RC_OK; // Initialize return code to success

    if (idxId == NULL) {
        rc = RC_IM_KEY_NOT_FOUND;
    } else {
        
        RC (*operation)(char*) = destroyPageFile;
        rc = operation(idxId);

    }

    return rc;
}

