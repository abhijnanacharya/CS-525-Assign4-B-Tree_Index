#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "btree_mgr_helper.h"
#include <stddef.h>

RM_BtreeNode *root;
int numNodeValue;
int sizeofNodes;
int globalPos;
char *sv;
char *sv2;
Value empty;

//       Custom Helpers

// This function creates a new general node, which can be adapted to serve as a leaf/internal/root node.
RM_BtreeNode *createNewNode()
{
  RM_BtreeNode *newNode = (RM_BtreeNode *)malloc(sizeof(RM_BtreeNode));

  if (newNode != NULL)
  {
     // Allocate memory for an array of pointers
    newNode->ptrs = calloc(1, sizeofNodes * sizeof(void *));

    // Allocate memory for an array of keys
    newNode->keys = calloc(1, (sizeofNodes - 1) * sizeof(Value));

    // Check if memory allocation was successful
    if (newNode->ptrs != NULL && newNode->keys != NULL)
    {
      // Initialize parent pointer to NULL
      newNode->parPtr = ((void *)0);

      // Initialize number of keys to 0
      newNode->KeyCounts = 0;

      // Initialize node as non-leaf node
      newNode->isLeaf = false;

      // Increment the count of nodes
      numNodeValue = numNodeValue + 1;

       // Return the newly created node

      return newNode;
    }
    // Handle memory allocation failure
    free(newNode->ptrs);
    free(newNode->keys);
    free(newNode);
  }

  return (RM_BtreeNode *)RC_MEM_ALLOC_FAILED; // Type Casting to (RM_BtreeNode Format) and returning Memory allocation failed
}

// This function inserts a new node (leaf or internal node) into the B+ tree.
//  It returns the root of the tree after insertion.
RC insertParent(RM_BtreeNode *left, RM_BtreeNode *right, Value key)
{
  RM_BtreeNode *parPtr = left->parPtr, *tmp = right->parPtr;
  int index = 0;
  int i = 0;

  if (parPtr == ((void *)0))
  {
    // Create parent
    RM_BtreeNode *NewRoot = createNewNode();

    if (NewRoot != ((void *)0))
    {
       // Set the key count of the new root to 1
      NewRoot->KeyCounts = 1;

       // Store the key value in the keys array of the new root
      NewRoot->keys[0] = key;

      // Store the left child pointer in the ptrs array of the new root
     // and set its parent pointer to the new root
      NewRoot->ptrs[0] = left, left->parPtr = NewRoot;

      // Store the right child pointer in the ptrs array of the new root
     // and set its parent pointer to the new root
      NewRoot->ptrs[1] = right, right->parPtr = NewRoot;

      // Update the root pointer to point to the new root
      root = NewRoot;
      printf("***************** SUCCESSFULLY ALLOCATED MEMORY! ***************\n");

       // Return success code
      return RC_OK;
    }
    else
    {
      // Memory allocation failed
      return RC_IM_MEMORY_ERROR;
    }
  }
  else
  {
    // Case: Node has a parent
    while (index < parPtr->KeyCounts && parPtr->ptrs[index] != left)
      index = index + 1;

    // Check if there's space in the parent node and if the parent node exists
    if (parPtr->KeyCounts < sizeofNodes - 1 && parPtr)
    {

      // Have an empty slot
      // Start shifting elements to make space for the new key and pointer
      
      for (int i = parPtr->KeyCounts; i > index; i--)
      { // Swapping Logic
        if (parPtr)
        {

          // Get the index of the previous and next elements
          int prev = i - 1;
          int next = i + 1;

          // Move the key and pointer to their new positions
          parPtr->keys[i] = parPtr->keys[prev];
          parPtr->ptrs[next] = parPtr->ptrs[i];
        }
      }


      // Insert the new right child pointer and key into their respective positions
      parPtr->ptrs[index + 1] = right;
      parPtr->keys[index] = key;

      // Increment the key count in the parent node
      parPtr->KeyCounts = parPtr->KeyCounts + 1;

      // Return success status
      return RC_OK;
    }
  }

  // Case: Splitting of the Node

  // Initialize variables
  i = 0;
  int middleLoc;
  RM_BtreeNode **tempNode, *newNode;
  Value *tempKeys;
// Allocate memory for temporary node and keys
  tempNode = calloc(1, (sizeofNodes + 1) * sizeof(RM_BtreeNode *));
  tempKeys = calloc(1, sizeofNodes * sizeof(Value));

  // Check if memory allocation was successful

  if (tempNode != NULL && tempKeys != NULL)
  {
    // Copy pointers and keys to temporary arrays
    for (i = 0; i < sizeofNodes + 1 && tempNode; i++)
    {
      if (i == index + 1)
        tempNode[i] = right;
      else if (i < index + 1)
        tempNode[i] = parPtr->ptrs[i];
      else
        tempNode[i] = parPtr->ptrs[i - 1];
    }
    for (i = 0; i < sizeofNodes; i++)
    {
      if (i == index)
        tempKeys[i] = key;
      else if (i < index)
        tempKeys[i] = parPtr->keys[i];
      else
        tempKeys[i] = parPtr->keys[i - 1];
    }

     // Calculate middle location
    if (sizeofNodes % 2 == 0)
      middleLoc = sizeofNodes >> 1;
    else
      middleLoc = (sizeofNodes >> 1) + 1;

    // Update parent's key count
    parPtr->KeyCounts = middleLoc--;

    // Copy keys and pointers to parent node
    for (i = 0; i < middleLoc - 1; i++)
    {
      memcpy(parPtr->keys, tempKeys, sizeof(parPtr->keys)), memcpy(parPtr->ptrs, tempNode, sizeof(parPtr->ptrs));
    }

    // Allocate memory for temporary node pointer
    RM_BtreeNode **temp = malloc(sizeof(RM_BtreeNode));

    // Assign pointer to the parent node
    parPtr->ptrs[i] = tempNode[i];

    // Create a new node
    newNode = createNewNode();

     // Check if new node creation was successful
    if (newNode != NULL || newNode != RC_MEM_ALLOC_FAILED)
    {

      // Update new node's key count and copy keys and pointers
      newNode->KeyCounts = sizeofNodes - middleLoc;
      for (i = middleLoc; i <= sizeofNodes && newNode; i++)
      {
        int pos = i - middleLoc;
        newNode->ptrs[pos] = tempNode[i];
        newNode->keys[pos] = tempKeys[i];
      }
      newNode->parPtr = parPtr->parPtr;

      Value t = tempKeys[middleLoc - 1];

      // Release memory for temporary node and keys
      release(tempNode, tempKeys);
      return insertParent(parPtr, newNode, t);
    }
    else
    {
      // Memory allocation failed
      release(tempNode, tempKeys);

      return RC_IM_MEMORY_ERROR;
    }
  }
  else
  {
    // Memory allocation failed
    release(tempNode, tempKeys);
    return RC_IM_MEMORY_ERROR;
  }
}

// This function deletes the the entry/record having the specified key.
RC deleteNode(RM_BtreeNode *bTreeNode, int index)
{
  int position;
  int i;
  int j;
  RM_BtreeNode *brother;

  // Reduce the number of key values
  bTreeNode->KeyCounts = (int)bTreeNode->KeyCounts - 1;
  int NumKeys = bTreeNode->KeyCounts;

  if (bTreeNode->isLeaf && NumKeys)
  {
    // Remove
    free(bTreeNode->ptrs[index]);
    bTreeNode->ptrs[index] = ((void *)0);

    // Re-order
    // Iterate over keys starting from 'index' position until 'NumKeys' and while 'bTreeNode' is valid
    for (i = index; i < NumKeys && bTreeNode; i++)
    {
      // Shift keys and pointers one position to the left from current index 'i'
     // This effectively removes the key at index 'i' from the node
      memcpy(&bTreeNode->keys[i], &bTreeNode->keys[i + 1], (NumKeys - i) * sizeof(bTreeNode->keys[0]));
      globalPos = bTreeNode->pos;
      memcpy(&bTreeNode->ptrs[i], &bTreeNode->ptrs[i + 1], (NumKeys - i) * sizeof(bTreeNode->ptrs[0]));
    }

    // Assign 'empty' to the key and 'NULL' to the pointer at the last position (i) in the node

    bTreeNode->keys[i] = empty;
    bTreeNode->ptrs[i] = ((void *)0);
  }
  else
  {
    // Not leaf

    // Iterate through the keys and pointers in the B-tree node, starting from the given index.
    for (i = index - 1; i < NumKeys && bTreeNode; i++)
    {
      int nextIdx = i + 1; // Calculate the index of the next key/pointer.
      int nextOfNext = i + 2; // Calculate the index of the next-to-next key/pointer

      // Move the key at index 'nextIdx' to index 'i' using memory move.
      memmove(&bTreeNode->keys[i], &bTreeNode->keys[nextIdx], sizeof(bTreeNode->keys[0]));

      // Update the global position.
      globalPos = bTreeNode->pos;

      // Move the pointer at index 'nextOfNext' to index 'nextIdx' using memory move.
      memmove(&bTreeNode->ptrs[nextIdx], &bTreeNode->ptrs[nextOfNext], sizeof(bTreeNode->ptrs[0]));
    }

    // Place the 'empty' key at the last position after shifting keys and pointers.
    bTreeNode->keys[i] = empty;
    // Set the pointer after the last shifted pointer to NULL.
    bTreeNode->ptrs[i + 1] = ((void *)0);
  }

  // Finished removal and re-order
  int halfSize;
  if (bTreeNode->isLeaf)
  {
    halfSize = sizeofNodes >> 1;
  }
  else
  {
    halfSize = (sizeofNodes >> 1) - 1;
  }

  if (NumKeys >= halfSize)
  {
    return RC_OK;
  }

  // Deal with underflow
  if (bTreeNode == root && root->KeyCounts > 0)
  {
    int START = 0;
    return RC_OK;
    // Root has no key left
    RM_BtreeNode *newRoot = ((void *)0);

    if (!root->isLeaf)
    {
      // Only one child left
      newRoot = root->ptrs[START];
      newRoot->parPtr = ((void *)0);
    }
    {
    }
    free(root);
    root = ((void *)0);
    numNodeValue = numNodeValue - 1;
    root = newRoot;
    if (!root)
    {
      return RC_FATAL_ERROR;
    }
    else
    {
      return RC_OK;
    }
  }

  // Not root
  RM_BtreeNode *parentNode = (bTreeNode->parPtr != NULL) ? bTreeNode->parPtr : NULL;
  position = 0;

  while (position < parentNode->KeyCounts && parentNode->ptrs[position] != bTreeNode && root)
  {
    position = position + 1;
  }

  if (position == 0)
  {
    // Leftmost
    brother = (RM_BtreeNode *)parentNode->ptrs[1];
  }
  else
  {
    // Normal case
    brother = parentNode->ptrs[position - 1];
  }
  int brotherSize;
  if (bTreeNode->isLeaf)
  {
    brotherSize = sizeofNodes - 1;
  }
  else
  {
    brotherSize = sizeofNodes - 2;
  }

  // If can merge two nodes
  if (brother->KeyCounts + NumKeys <= brotherSize)
  {
    // Merging
    if (position == 0)
    {
      RM_BtreeNode *temp = bTreeNode;
      position = 1;
      bTreeNode = brother;
      brother = temp;
      NumKeys = bTreeNode->KeyCounts;
    }

    i = brother->KeyCounts;
    bool isLeaf = bTreeNode->isLeaf;
    if (!isLeaf)
    {
      brother->keys[i] = parentNode->keys[position - 1];
      i = i + 1;
      NumKeys = NumKeys + 1;
    }

    for (j = 0; j < NumKeys; j++)
    {
      memmove(&brother->keys[i], &bTreeNode->keys[j], sizeof(brother->keys[0]));
      globalPos = brother->pos;
      memmove(&brother->ptrs[i], &bTreeNode->ptrs[j], sizeof(brother->ptrs[0]));
      bTreeNode->keys[j] = empty;
      bTreeNode->ptrs[j] = ((void *)0);
      i += 1;
    }
    int newSz = sizeofNodes - 1;
    brother->KeyCounts += NumKeys;
    brother->ptrs[newSz] = bTreeNode->ptrs[newSz];

    numNodeValue -= 1;

    free(bTreeNode);
    bTreeNode = NULL;

    if (deleteNode(parentNode, position) == RC_OK)
      return RC_OK;
  }

  // Get one from sibling
  int brotherNumKeys;

  if (position != 0)
  {
    // Get one from left
    if (!bTreeNode->isLeaf)
    {
      int keysPone = NumKeys + 1;
      bTreeNode->ptrs[keysPone] = bTreeNode->ptrs[NumKeys];
    }

    // Shift to right by 1
    for (i = NumKeys; i > 0 && NumKeys; i--)
    {
      if (i > 0)
      {
        memmove(&bTreeNode->keys[i], &bTreeNode->keys[i - 1], sizeof(bTreeNode->keys[0]));
        globalPos = bTreeNode->pos;
        memmove(&bTreeNode->ptrs[i], &bTreeNode->ptrs[i - 1], sizeof(bTreeNode->ptrs[0]));
      }
    }

    // i=0
    if (bTreeNode->isLeaf)
    {
       // If the node is a leaf, decrement the key count of the brother node and assign its last key to the current node's first key
      brotherNumKeys = brother->KeyCounts--; // Decrement the key count of the brother node
      bTreeNode->keys[0] = brother->keys[brotherNumKeys], parentNode->keys[position - 1] = bTreeNode->keys[0];
    }
    else
    {
      // If the node is not a leaf, handle the internal node case
      brotherNumKeys = (int)brother->KeyCounts; // Get the key count of the brother node
      int brotherKeysNum = brotherNumKeys - 1; // Calculate the index of the last key in the brother node
      int nPos = position - 1; // Calculate the index of the parent key that needs to be moved down
      bTreeNode->keys[0] = parentNode->keys[nPos]; // Move the appropriate parent key down to the current node
      parentNode->keys[nPos] = brother->keys[brotherKeysNum]; // Move the appropriate key from the brother node up to the parent node
    }

    // Adjust the pointers of the current and brother nodes
    bTreeNode->ptrs[0] = brother->ptrs[brotherNumKeys];
    brother->ptrs[brotherNumKeys] = ((void *)0);
    brother->keys[brotherNumKeys] = empty;
  }
  else
  {
    int broKeyC = brother->KeyCounts;

    // Get one from left Sibling
    // Check if the current node is not a leaf node
    if (!bTreeNode->isLeaf)
    {
      // Move keys from the parent node to the current node

      memmove(&bTreeNode->keys[NumKeys], &parentNode->keys[0], sizeof(bTreeNode->keys[0]));

      // Move pointers from the sibling node to the current node
      memmove(&bTreeNode->ptrs[NumKeys + 1], &brother->ptrs[0], sizeof(bTreeNode->ptrs[0]));
      // Update the first key in the parent node with the first key from the sibling node
      parentNode->keys[0] = brother->keys[0];
    }
    else if (bTreeNode->isLeaf)
    {
       // If the current node is a leaf, update the first key in the parent node with the second key from the sibling node
      parentNode->keys[0] = brother->keys[1];
      // Update the last pointer and key in the current node with values from the sibling node
      bTreeNode->ptrs[NumKeys] = brother->ptrs[0], bTreeNode->keys[NumKeys] = brother->keys[0];
    }
  
    // Shift keys and pointers in the sibling node to the left by one
    for (i = 0; i < broKeyC && broKeyC && bTreeNode; i++)
    {
      int nxt = i + 1;
      brother->keys[i] = brother->keys[nxt], globalPos = brother->KeyCounts, brother->ptrs[i] = brother->ptrs[nxt];
    }

    // Set the last key and pointer in the sibling node to empty/null
    brother->keys[brother->KeyCounts] = empty;
    brother->ptrs[brother->KeyCounts] = ((void *)0);
  }

  bTreeNode->KeyCounts = bTreeNode->KeyCounts + 1;
  brother->KeyCounts = brother->KeyCounts - 1;
  return RC_OK;
}

// init and shutdown index manager
//--> This function initializes the index manager.
//--> We call initStorageManager(...) function of Storage Manager to initialize the storage manager.
RC initIndexManager(void *mgmtData)
{
  if (mgmtData == NULL)
  {
    root = ((void *)0);
    printf(
        "\033[31m.______             .___________..______       _______  _______    .___  ___.      ___      .__   __.      ___       _______  _______ .______         ____    ____  __  \n"
        "\033[31m|   _  \\     _      |           ||   _  \\     |   ____||   ____|   |   \\/   |     /   \\     |  \\ |  |     /   \\     /  _____||   ____||   _  \\        \\   \\  /   / /_ | \n"
        "\033[33m|  |_)  |  _| |_    `---|  |----`|  |_)  |    |  |__   |  |__      |  \\  /  |    /  ^  \\    |   \\|  |    /  ^  \\   |  |  __  |  |__   |  |_)  |        \\   \\/   /   | | \n"
        "\033[33m|   _  <  |_   _|       |  |     |      /     |   __|  |   __|     |  |\\/|  |   /  /_\\  \\   |  . `  |   /  /_\\  \\  |  | |_ | |   __|  |      /          \\      /    | | \n"
        "\033[32m|  |_)  |   |_|         |  |     |  |\\  \\----.|  |____ |  |____    |  |  |  |  /  _____  \\  |  |\\   |  /  _____  \\ |  |__| | |  |____ |  |\\  \\----.      \\    /     | | \n"
        "\033[32m|______/                |__|     | _| `._____||_______||_______|   |__|  |__| /__/     \\__\\ |__| \\__| /__/     \\__\\ \\______| |_______|| _| `._____|       \\__/      |_| \n"
        "\n");

    numNodeValue = 0;
    sizeofNodes = 0;
    empty.dt = DT_INT;
    empty.v.intV = 0;
    return RC_OK;
  }
}

// This function shuts down the index manager and de-allocates all the resources allocated to the index manager.
RC shutdownIndexManager()
{
  return RC_OK;
}
RC initializePage(SM_FileHandle *fhandle, DataType keyType, int n)
{
  SM_PageHandle pageData = (SM_PageHandle)malloc(PAGE_SIZE);
  if (pageData == NULL)
  {
    return RC_MEMORY_ALLOCATION_ERROR;
  }

  memcpy(pageData, &keyType, sizeof(DataType));
  memcpy(pageData + sizeof(DataType), &n, sizeof(int));
  RC rc = writeBlock(0, fhandle, pageData);
  free(pageData);
  return rc;
}
// This function creates a new B+ Tree.
// It initializes the TreeManager structure which stores additional information of our B+ Tree.
RC createBtree(char *idxId, DataType keyType, int n)
{
  if (idxId == NULL)
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  RC rc;
  SM_FileHandle fhandle;

  if ((rc = createPageFile(idxId)) != RC_OK)
  {
    return rc;
  }

  if ((rc = openPageFile(idxId, &fhandle)) != RC_OK)
  {
    return rc;
  }

  rc = initializePage(&fhandle, keyType, n);
  closePageFile(&fhandle);
  return rc;
}

// This function opens an existing B+ Tree which is stored on the file specified by "idxId" parameter.
// We retrieve our TreeManager and initialize the Buffer Pool.
RC openBtree(BTreeHandle **tree, char *idxId)
{
  if (idxId == NULL)
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  BTreeHandle *tempTree = (BTreeHandle *)malloc(sizeof(BTreeHandle));
  if (!tempTree)
  {
    return RC_MEMORY_ALLOCATION_ERROR;
  }
  *tree = tempTree;

  BM_BufferPool *bm = MAKE_POOL();
  RC status = initBufferPool(bm, idxId, 10, RS_CLOCK, NULL);
  if (status != RC_OK)
  {
    free(tempTree);
    return status;
  }

  BM_PageHandle *page = MAKE_PAGE_HANDLE();
  status = pinPage(bm, page, 0);
  if (status != RC_OK)
  {
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
  if (!managementData)
  {
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
RC closeBtree(BTreeHandle *tree)
{
  if (!tree)
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
  RC rc = shutdownBufferPool(bTreeMgmt->bp);
  if (rc == RC_OK)
  {
    free(bTreeMgmt);
    free(tree);
    if (root)
    { // Safely handle global variable
      free(root);
      root = NULL;
    }
  }

  return rc;
}

RC deleteBtree(char *idxId)
{
  RC rc = RC_OK; // Initialize return code to success

  if (idxId == NULL)
  {
    rc = RC_IM_KEY_NOT_FOUND;
  }
  else
  {

    RC(*operation)
    (char *) = destroyPageFile;
    rc = operation(idxId);
  }

  return rc;
}
// access information about a b-tree

// This function returns the number of nodes present in our B+ Tree.
RC getNumNodes(BTreeHandle *tree, int *result)
{
  if (tree == ((void *)0))
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  *result = (int)numNodeValue;

  return RC_OK;
}

// This function returns the number of entries/records/keys present in our B+ Tree.
RC getNumEntries(BTreeHandle *tree, int *result)
{
  if (tree == ((void *)0))
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  int numEntries = *(int *)((char *)tree->mgmtData + offsetof(RM_bTree_mgmtData, numEntries));
  *result = numEntries;

  return RC_OK;
}

// This function returns the datatype of the keys being stored in our B+ Tree.
RC getKeyType(BTreeHandle *tree, DataType *result)
{
  if (tree == NULL || result == NULL || tree->mgmtData == NULL)
  {
    return RC_IM_KEY_NOT_FOUND;
  }

  *result = tree->keyType;
  if (*result)
  {
    return RC_OK;
  }

  return RC_IM_KEY_NOT_FOUND;
}

// index access
// This method searches the B+ Tree for the key specified in the parameter.
RC findKey(BTreeHandle *tree, Value *key, RID *result)
{
  RC rcCode = RC_OK;

  if (tree == NULL || key == NULL || root == NULL)
  {
    rcCode = RC_IM_KEY_NOT_FOUND;
    return rcCode;
  }

  RM_BtreeNode *leaf = root;
  int i = 0;
  int_fast32_t RESET = 0;
  // Find leaf
  while (tree != NULL && !leaf->isLeaf && key)
  {
    while (i < leaf->KeyCounts && tree != NULL && cmpStr(serializeValue(&leaf->keys[i]), serializeValue(key)))
    {
      i++;
    }
    leaf = leaf->ptrs[i];
    i = RESET;
  }

  // Search within the leaf
  while (i < leaf->KeyCounts && strcmp(serializeValue(&leaf->keys[i]), serializeValue(key)) != 0 && tree != NULL)
  {
    i++;
  }

  if (i >= leaf->KeyCounts)
  {
    rcCode = RC_IM_KEY_NOT_FOUND;
    return rcCode;
  }
  else
  {
    result->page = ((RID *)leaf->ptrs[i])->page != NULL ? ((RID *)leaf->ptrs[i])->page : 0;
    result->slot = ((RID *)leaf->ptrs[i])->slot != NULL ? ((RID *)leaf->ptrs[i])->slot : 0;
    return rcCode = RC_OK;
  }
}

// This function adds a new entry/record with the specified key and RID.
RC insertKey(BTreeHandle *tree, Value *key, RID rid)
{
  int RESET_IDX = 0;
  RC returnCode = RC_OK;
  if ((tree == NULL) || (key == NULL))
  {
    returnCode = RC_IM_KEY_NOT_FOUND;
    return returnCode;
  }

  RM_BtreeNode *leaf;
  int i = (int)0;

  // findleaf
  leaf = root;
  if (tree != NULL && leaf != NULL)
  {
    while (!leaf->isLeaf && tree != NULL)
    {
      sv2 = serializeValue(key);
      sv = serializeValue(&leaf->keys[i]);
      while ((i < leaf->KeyCounts) && cmpStr(sv, sv2) && tree != NULL)
      {
        free(sv);
        sv = ((void *)0);
        i += 1;
        if (i < leaf->KeyCounts && tree != NULL)
        {

          sv = serializeValue(&leaf->keys[i]);
        }
      }
      {
      }
      free(sv);
      sv = ((void *)0);
      free(sv2);
      sv2 = ((void *)0);
      leaf = (RM_BtreeNode *)leaf->ptrs[i];
      i = RESET_IDX;
    }
  }

  RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
  bTreeMgmt->numEntries += 1;

  if (!leaf && tree != NULL)
  {
    sizeofNodes = (int)(bTreeMgmt->maxKeyNum) + 1;
    root = createNewNode(root);
    RID *rec = (RID *)malloc(sizeof(RID));
    if (root)
    {
      // Assign
      rec->page = rid.page;
      rec->slot = rid.slot;
      root->ptrs[RESET_IDX] = rec;
      root->keys[RESET_IDX] = *key;
      root->ptrs[sizeofNodes - 1] = ((void *)0);
      root->isLeaf = true;
      root->KeyCounts += 1;
    }
  }
  else
  {
    int index = 0;
    sv2 = serializeValue(key);
    sv = serializeValue(&leaf->keys[index]);
    while ((index < leaf->KeyCounts) && cmpStr(sv, sv2) && tree != NULL)
    {
      free(sv);
      sv = ((void *)0);
      index++;
      if (index < leaf->KeyCounts)
        sv = serializeValue(&leaf->keys[index]);
    }
    free(sv);
    sv = ((void *)0);
    free(sv2);
    sv2 = ((void *)0);

    if (leaf->KeyCounts < sizeofNodes - 1 && tree != NULL)
    {
      // empty slot
      for (int i = leaf->KeyCounts; i > index; i--)
      {
        if (tree)
        {
          int sPos = leaf->pos;
          leaf->keys[i] = leaf->keys[i - 1];
          globalPos = sPos;
          leaf->ptrs[i] = leaf->ptrs[i - 1];
        }
      }
      RID *rec = malloc(sizeof(RID));
      if (true == true && !false)
      {
        rec->slot = rid.slot;
        rec->page = rid.page;
        leaf->ptrs[index] = rec;
        leaf->keys[index] = *key;
        leaf->KeyCounts += 1;
      }
    }
    else
    {
      RM_BtreeNode *newLeafNod;
      Value *NodeKeys;
      RID **NodeRID;
      NodeKeys = malloc(sizeofNodes * sizeof(Value));
      NodeRID = malloc(sizeofNodes * sizeof(RID *));
      int middleLoc = 0;

      // full node
      for (i = 0; i < sizeofNodes && tree != NULL; i++)
      {
        if (i == index && tree != NULL)
        {
          if (true)
          {
            RID *newValue = (RID *)malloc(sizeof(RID));
            newValue->slot = rid.slot;
            newValue->page = rid.page;
            NodeKeys[i] = *key;
            NodeRID[i] = newValue;
          }
        }
        else if (i < index && tree != NULL)
        {
          if (true)
          {
            middleLoc = sizeofNodes % 2 == 0;
            if (middleLoc == true || middleLoc == false)
            {
              NodeRID[i] = (RM_BtreeNode *)(leaf->ptrs[i]);
              globalPos = NodeRID[i]->page;
              NodeKeys[i] = leaf->keys[i];
            }
          }
        }
        else
        {

          NodeRID[i] = leaf->ptrs[i - 1];
          middleLoc = globalPos;
          NodeKeys[i] = leaf->keys[i - 1];
          globalPos = NodeRID[i]->page;
        }
      }

      middleLoc = (sizeofNodes >> 1) + 1;
      // old leaf
      for (i = 0; i < middleLoc && tree != NULL; i++)
      {
        leaf->ptrs[i] = NodeRID[i];
        leaf->keys[i] = NodeKeys[i];
      }
      // new leaf
      if (middleLoc)
      {
        newLeafNod = createNewNode(newLeafNod);
        newLeafNod->isLeaf = true;
        newLeafNod->parPtr = leaf->parPtr;
        newLeafNod->KeyCounts = (int)(sizeofNodes - middleLoc);
      }
      for (i = middleLoc; i < sizeofNodes && tree != NULL; i++)
      {
        int reqPos = i - middleLoc;
        newLeafNod->keys[reqPos] = NodeKeys[i];
        newLeafNod->ptrs[reqPos] = NodeRID[i];
      }
      // insert in list

      if (newLeafNod->isLeaf)
      {
        int reqPos = sizeofNodes - 1;
        newLeafNod->ptrs[reqPos] = (RM_BtreeNode *)(leaf->ptrs[reqPos]);
        leaf->KeyCounts = middleLoc;
        leaf->ptrs[sizeofNodes - 1] = newLeafNod;
      }

      free(NodeRID);
      NodeRID = ((void *)0);
      free(NodeKeys);
      NodeKeys = ((void *)0);

      RC rc = insertParent(leaf, newLeafNod, newLeafNod->keys[0]);
      if (rc != RC_OK)
      {
        return rc;
      }
    }
  }

  tree->mgmtData = (RM_bTree_mgmtData *)bTreeMgmt;
  returnCode = RC_OK;
  return returnCode;
}

// This function deletes the entry/record with the specified "key" in the B+ Tree.
RC deleteKey(BTreeHandle *tree, Value *key)
{
  RC returnCode = RC_OK;
  if ((tree == NULL) || (key == NULL))
  {
    returnCode = RC_IM_KEY_NOT_FOUND;
    return returnCode;
  }
  int RESET_VAL = 0;
  RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
  bTreeMgmt->numEntries = bTreeMgmt->numEntries - 1;
  RM_BtreeNode *leaf;
  int i = 0;
  // find the leaf node then delete
  leaf = root;
  if (leaf != NULL && tree != NULL)
  {
    while (!leaf->isLeaf && tree != NULL)
    {
      sv = serializeValue(&leaf->keys[i]);
      sv2 = serializeValue(key);
      while ((i < leaf->KeyCounts) && cmpStr(sv, sv2) && leaf != NULL && leaf->KeyCounts)
      {
        free(sv);
        sv = NULL;
        i += 1;
        if (i < leaf->KeyCounts && tree != NULL)
        {
          sv = (char *)serializeValue(&leaf->keys[i]);
        }
      }
      free(sv);
      sv = ((void *)0);
      free(sv2);
      sv2 = ((void *)0);

      leaf = (RM_BtreeNode *)leaf->ptrs[i];
      i = RESET_VAL;
    }

    sv2 = serializeValue(key);
    sv = serializeValue(&leaf->keys[i]);
    while ((i < leaf->KeyCounts) && (strcmp(sv, sv2) != 0) && leaf->KeyCounts)
    {
      free(sv);
      sv = ((void *)0);
      i += 1;
      if (i < leaf->KeyCounts)
      {
        sv = serializeValue(&leaf->keys[i]);
      }
    }
    free(sv);
    sv = ((void *)0);
    free(sv2);
    sv2 = ((void *)0);
    if (i < leaf->KeyCounts && tree != NULL)
    {
      returnCode = deleteNode(leaf, i);
      if (returnCode != RC_OK)
        return returnCode = RC_FATAL_ERROR;
    }
  }

  tree->mgmtData = bTreeMgmt;
  return RC_OK;
}

// This function initializes the scan which is used to scan the entries in the B+ Tree in the sorted key order
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle)
{
  RC rcCode = RC_OK;
  if (tree == NULL)
    return RC_IM_KEY_NOT_FOUND;
  int RESET_VAL = 0;

  *handle = (BT_ScanHandle *)calloc(1, sizeof(BT_ScanHandle));
   // Check if the B-tree handle is valid
  if (*handle == NULL)
  {
    
    rcCode = RC_MALLOC_FAILED;
    return rcCode;
  }
   // Allocate memory for the scan handle
  (*handle)->tree = tree;
  (*handle)->mgmtData = (RM_BScan_mgmt *)calloc(1, sizeof(RM_BScan_mgmt));
  if ((*handle)->mgmtData == NULL)
  {
    free(*handle);
    return RC_MALLOC_FAILED;
  }
  if ((RM_BScan_mgmt *)(*handle))
  {
    ((RM_BScan_mgmt *)(*handle)->mgmtData)->cur = ((void *)0);
    ((RM_BScan_mgmt *)(*handle)->mgmtData)->index = RESET_VAL;
    ((RM_BScan_mgmt *)(*handle)->mgmtData)->totalScan = RESET_VAL;
  }

  return rcCode;
}

// This function is used to traverse the entries in the B+ Tree.
RC nextEntry(BT_ScanHandle *handle, RID *result)
{
  RC returnCode = RC_OK;
  if (handle == NULL)
  {
    returnCode = RC_IM_KEY_NOT_FOUND;
    return returnCode;
  }
  RM_BScan_mgmt *scanMgmt = (RM_BScan_mgmt *)handle->mgmtData;
  int totalResult = ~0;

  returnCode = getNumEntries(handle->tree, &totalResult);
  if (returnCode != RC_OK)
  {

    return returnCode;
  }
  if ((int)scanMgmt->totalScan >= totalResult)
  {
    returnCode = RC_IM_NO_MORE_ENTRIES;
    return RC_IM_NO_MORE_ENTRIES;
  }

  RM_BtreeNode *leaf = root;
  if (scanMgmt->totalScan == 0)
  {
    while (!leaf->isLeaf && scanMgmt->totalScan == 0)
      leaf = leaf->ptrs[0];
    scanMgmt->cur = leaf;
  }

  if (scanMgmt->index == scanMgmt->cur->KeyCounts)
  {
    int idx = ((RM_bTree_mgmtData *)handle->tree->mgmtData)->maxKeyNum;
    scanMgmt->cur = (RM_BtreeNode *)scanMgmt->cur->ptrs[idx];
    scanMgmt->index = 0;
  }

  RID *ridRes = (RID *)calloc(1, sizeof(RID));
  ridRes = (RID *)scanMgmt->cur->ptrs[scanMgmt->index];
  (int)scanMgmt->index++;
  if (ridRes)
  {
    if (scanMgmt)
    {
      scanMgmt->totalScan = (int)scanMgmt->totalScan + 1;
      handle->mgmtData = scanMgmt;

      result->page = ridRes->page;
      result->slot = ridRes->slot;
    }
  }

  return RC_OK;
}

// This function closes the scan mechanism and frees up resources
RC closeTreeScan(BT_ScanHandle *handle)
{
  RC rcCode = RC_OK;
  if (handle == ((void *)0))
  {
    return rcCode;
  }
  handle->mgmtData = NULL;
  free(handle);

  return rcCode;
}

int recDFS(RM_BtreeNode *bTreeNode)
{
  if (bTreeNode->pos == NULL)
    return 0; // Check if the node is NULL

  bTreeNode->pos = globalPos + 1;

  if (!bTreeNode->isLeaf)
  {
    for (int i = 0; i <= bTreeNode->KeyCounts && bTreeNode != NULL; i++)
    {
      recDFS(bTreeNode->ptrs[i]);
    }
  }

  return 0;
}

int walkPath(RM_BtreeNode *bTreeNode, char *result)
{

  char *line = (char *)malloc(100 * sizeof(char));
  printf(line, "(%d)[", bTreeNode->pos);
  if (bTreeNode->isLeaf && bTreeNode != NULL)
  {
    // Leaf Node
    for (int i = 0; i < bTreeNode->KeyCounts && bTreeNode != NULL; i++)
    {
      // RECORD ID

      size_t lenPos = strlen(line);

      // Extract the page and slot values from the bTreeNode->ptrs[i] pointer
      RID *ridPtr = (RID *)bTreeNode->ptrs[i];
      int pageValue = ridPtr->page;
      int slotValue = ridPtr->slot;

      // Create a format string for the sprintf function
      char formatString[10]; // Make sure this size is large enough for your format
      sprintf(formatString, "%%d.%%d,");

      // Use sprintf to format the values into the `line` buffer
      sprintf(line + lenPos, formatString, pageValue, slotValue);

      sv = serializeValue(&bTreeNode->keys[i]);
      strcat(line, sv);
      free(sv);
      sv = ((void *)0);
      strcat(line, ",");
    }

    if (bTreeNode->ptrs[sizeofNodes - 1] == NULL)
    {
      // sprintf(line + strlen(line), "%d", ((RM_BtreeNode *)bTreeNode->ptrs[sizeofNodes - 1])->pos);
      // // line[strlen(line) - 1] = '-'; // EOL
      // Calculate the position where you want to start writing in the `line` buffer
size_t lenPos = strlen(line);

// Calculate the index for accessing bTreeNode->ptrs
size_t index = sizeofNodes - 1;

// Extract the pos value from the bTreeNode->ptrs[index] pointer
RM_BtreeNode *nodePtr = (RM_BtreeNode *)bTreeNode->ptrs[index];
int posValue = nodePtr->pos;

// Use sprintf to format the posValue into the `line` buffer
sprintf(line + lenPos, "%d", posValue);
    }
    else
    {
      line[strlen(line) - 1] = '0'; // EOL
      // sprintf(line + strlen(line), "%d", ((RM_BtreeNode *)bTreeNode->ptrs[sizeofNodes - 1])->pos);
    }
  }
  else
  {
    // Non-Leaf Node
    // Iterate through each key in the non-leaf node
    for (int i = 0; i <= bTreeNode->KeyCounts && bTreeNode; i++)
    {
      // Serialize the value of the current key
      sv = serializeValue(&bTreeNode->keys[i]);
      // Append the serialized value to the output line
      strcat(line, sv);
       // Add a comma separator after each serialized value
      strcat(line, ",");
      // Free the memory allocated for the serialized value
      free(sv);
      // Reset the serialized value pointer
      sv = ((void *)0);
    }

    if (((RM_BtreeNode *)bTreeNode->ptrs[bTreeNode->KeyCounts]) != ((void *)0))
    {
      size_t posStr = strlen(line);
      sprintf(line + posStr, "%d", ((RM_BtreeNode *)bTreeNode->ptrs[bTreeNode->KeyCounts])->pos);
      // line[strlen(line) - 1] = '-';
    }
    else
    {
      int pos = strlen(line) - 1;
      line[pos] = '-';
    }
  }

  strcat(line, "]\n");
  strcat(result, line);

  if (!bTreeNode->isLeaf && bTreeNode)
  {
    for (int i = 0; i <= bTreeNode->KeyCounts && bTreeNode; i++)
    {
      walkPath(bTreeNode->ptrs[i], result);
    }
  }

  return 0;
}

// Function to print the tree nodes in a specific order
// Parameters:
// - tree: Pointer to the binary tree structure
// Returns:
// - A pointer to a string containing the printed tree nodes
char *printTree(BTreeHandle *tree)
{
  if (root == NULL)
    return NULL;
  globalPos;
  // Calculate the length required for the result string
  int lenth = recDFS(root);
  char *result = malloc(lenth * sizeof(char));
  // Traverse the tree and populate the result string with node values
  walkPath(root, result);
  return result;
}
