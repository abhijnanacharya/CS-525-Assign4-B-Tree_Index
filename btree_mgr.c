

// Function to get the number of nodes in the B+-tree
extern RC getNumNodes(BTreeHandle *tree, int *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_NULL_ARGUMENT;
    }
    
    *result = tree->mgmtData->numNodes; 
    return RC_OK;
}

// Function to get the number of entries in the B+-tree
extern RC getNumEntries(BTreeHandle *tree, int *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_NULL_ARGUMENT;
    }
    
    *result = tree->mgmtData->numEntries; 
    return RC_OK;
}

// Function to get the data type of the keys used in the B+-tree
extern RC getKeyType(BTreeHandle *tree, DataType *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_NULL_ARGUMENT;
    }
    
    *result = tree->keyType;
    return RC_OK;
}

