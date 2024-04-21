# CS-525 Record Manager

## Contributors:

- Abhijnan Acharya A20536263

- Tanushree Halder A20554883

- Roshan Hyalij A20547441

- Rana Feyza Soylu A20465152

---

## FAQ

- ### How to Run

  - 1.  `make`
  - 2. `./test_assign1`
  - 3. `make clean`

- ### Sample Output has been generated and presented in the repository by piping the execution command to a new txt file
  - `./test_assign1> result.txt`
  - When make `make clean ` command is run it also cleans up this txt file

---
## Function createNewNode

This file describes the `createNewNode` function to allocate and initialize a new B-tree node.

**Function Signature**

```c
RM_BtreeNode *createNewNode()
```

**Purpose:** This function allocates memory for a new B-tree node and initializes its components.

**Return:**

- `RM_BtreeNode *`: A pointer to the newly created B-tree node if successful.
- `RC_MEM_ALLOC_FAILED`: Indicates memory allocation failure.

**Details:**

1. Allocates memory for a new B-tree node (`RM_BtreeNode`) using `malloc`.
   
2. Allocates memory for the node's pointers and keys using `calloc`.

3. Initializes the node's fields:
   - `ptrs`: Pointer array for child nodes.
   - `keys`: Array to store keys.
   - `parPtr`: Pointer to the parent node.
   - `KeyCounts`: Number of keys currently in the node.
   - `isLeaf`: Flag indicating if the node is a leaf.

4. Updates `numNodeValue` variable to reflect the creation of a new node.

5. Returns a pointer to the new node if memory allocation and initialization are successful. Otherwise, returns `RC_MEM_ALLOC_FAILED`.

**Note:** If memory allocation for any component fails, appropriate cleanup is performed, and `RC_MEM_ALLOC_FAILED` is returned.


---
## Function insertParent

This file describes the `insertParent` function to insert a new node (leaf or internal node) into the B+ tree, returning the root of the tree after insertion.

**Function Signature**

```c
RC insertParent(RM_BtreeNode *left, RM_BtreeNode *right, Value key)
```

**Purpose:** This function inserts a new node into the B+ tree and returns the root of the tree after insertion.

**Parameters:**

- `left`: Pointer to the left child node.
- `right`: Pointer to the right child node.
- `key`: The key value to be inserted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_MEMORY_ERROR`: Indicates memory allocation failure.

**Details:**

1. Retrieves the parent pointers of the left and right child nodes.

2. If the left child node has no parent:
   - Creates a new root node (`NewRoot`) using the `createNewNode` function.
   - Initializes `NewRoot` with the key and pointers to `left` and `right`.
   - Sets the parent pointers of `left` and `right` to `NewRoot`.
   - Assigns `NewRoot` as the root of the tree.
   - Returns `RC_OK` upon successful allocation and initialization of `NewRoot`. Otherwise, returns `RC_IM_MEMORY_ERROR`.

3. If the left child node has a parent:
   - Finds the appropriate index for insertion of the key.
   - If there's space in the parent node for the new key:
     - Shifts existing keys and pointers to accommodate the new key and pointer.
     - Inserts the new key and pointer at the appropriate position.
     - Increments the key count of the parent node.
     - Returns `RC_OK`.
   - If there's no space in the parent node for the new key:
     - Splits the parent node:
       - Allocates memory for temporary arrays (`tempNode` and `tempKeys`) to hold keys and pointers during the split.
       - Copies existing keys and pointers into the temporary arrays, inserting the new key and pointer at the correct position.
       - Determines the middle location for splitting the node.
       - Adjusts the key count of the parent node.
       - Creates a new node (`newNode`) using the `createNewNode` function.
       - Copies keys and pointers from the temporary arrays into the new node.
       - Assigns the parent pointer of `newNode` to the parent pointer of the original node.
       - Recursively calls `insertParent` to insert the new key and `newNode` into the parent of the original node.
     - Returns `RC_OK` upon successful insertion. Otherwise, returns `RC_IM_MEMORY_ERROR`.

**Note:** If memory allocation fails during any step, the function returns `RC_IM_MEMORY_ERROR`.

---
## Function deleteNode

This file describes the `deleteNode` function to delete the entry/record having the specified key in the B+ tree.

**Function Signature**

```c
RC deleteNode(RM_BtreeNode *bTreeNode, int index)
```

**Purpose:** This function deletes the entry/record having the specified key from the B+ tree node.

**Parameters:**

- `bTreeNode`: Pointer to the B+ tree node from which the entry/record is to be deleted.
- `index`: Index of the entry/record to be deleted within the node.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Decrements the number of key values (`KeyCounts`) in the B+ tree node.

2. If the node is a leaf node and contains keys:
   - Frees the memory occupied by the pointer at the specified index.
   - Reorders the keys and pointers in the node to maintain the structure.
   - Sets the last key and pointer to NULL to signify the removal.

3. If the node is not a leaf node:
   - Reorders the keys and pointers in the node to maintain the structure after deleting the specified index.
   - Sets the last pointer to NULL to signify the removal.

4. If the number of keys in the node falls below a certain threshold:
   - Checks if merging with a sibling node is possible.
   - If merging is not possible, redistributes keys between the node and its sibling.

5. Returns `RC_OK` upon successful deletion and restructuring of the B+ tree node.

**Note:** No explicit memory deallocation is performed for the deleted key or pointer. Memory management for these resources is assumed to be handled elsewhere in the program.


---
## Function initIndexManager

This file describes the `initIndexManager` function to initialize the index manager.

**Function Signature**

```c
RC initIndexManager(void *mgmtData)
```

**Purpose:** This function initializes the index manager.

**Parameters:**

- `mgmtData`: A pointer to management data that can be utilized for passing configuration options or necessary state required for initialization.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. **Parameter Validation**:
   - Check if the `mgmtData` parameter is `NULL`.

2. **Initialization**:
   - If `mgmtData` is `NULL`, proceed with initializing the index manager.
   - Set the `root` pointer to `NULL`.
   - Print a formatted message to indicate the initialization process has started.
   - Set `numNodeValue` and `sizeofNodes` variables to `0`.
   - Initialize the `empty` variable:
     - Set `empty.dt` to `DT_INT`.
     - Set `empty.v.intV` to `0`.

3. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.

---
## Function shutdownIndexManager

This file describes the `shutdownIndexManager` function to shut down the index manager.

**Function Signature**

```c
RC shutdownIndexManager()
```

**Purpose:** This function shuts down the index manager.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. **Shut Down**:
   - Perform any necessary operations to shut down the index manager.
   - Currently, this function does not require any specific shut down operations.

2. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.

---
## Function initializePage

This file describes the `initializePage` function to initialize a page in the file with specified metadata.

**Function Signature**

```c
RC initializePage(SM_FileHandle *fhandle, DataType keyType, int n)
```

**Purpose:** This function initializes a page in the file with specified metadata.

**Parameters:**

- `fhandle`: A pointer to the file handle structure (`SM_FileHandle`) representing the file where the page is to be initialized.
- `keyType`: The data type of the keys to be stored in the page.
- `n`: The number of keys to be stored in the page.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_MEMORY_ALLOCATION_ERROR`: Indicates memory allocation failure.

**Details:**

1. **Memory Allocation**:
   - Allocate memory for a new page (`SM_PageHandle`) using `malloc`.
   - If memory allocation fails (`pageData == NULL`), return `RC_MEMORY_ALLOCATION_ERROR`.

2. **Data Initialization**:
   - Copy the `keyType` into the beginning of the page using `memcpy`.
   - Copy the integer `n` after `keyType` using `memcpy`.

3. **Write to File**:
   - Call the `writeBlock` function to write the initialized page data to the file at block 0.
   - Store the return value of `writeBlock` in the variable `rc`.

4. **Memory Deallocation**:
   - Free the allocated memory for the page using `free(pageData)`.

5. **Return**:
   - Return the value of `rc` to indicate the success or failure of the operation.

---
## Function createBtree

This file describes the `createBtree` function to create a new B+ tree.

**Function Signature**

```c
RC createBtree(char *idxId, DataType keyType, int n)
```

**Purpose:** This function creates a new B+ tree by initializing the necessary structures and metadata.

**Parameters:**

- `idxId`: The identifier for the B+ tree.
- `keyType`: The data type of the keys to be stored in the B+ tree.
- `n`: The degree of the B+ tree.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the index identifier is not found.

**Details:**

1. **Index Identifier Validation**:
   - Check if the `idxId` parameter is `NULL`.
   - If `idxId` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **File Creation**:
   - Declare a variable `rc` of type `RC` to store the return code of file creation operations.
   - Call the `createPageFile` function to create a new page file with the specified index identifier (`idxId`).
   - If the return code of `createPageFile` is not `RC_OK`, return the same return code.

3. **File Opening**:
   - Declare a file handle variable `fhandle` of type `SM_FileHandle`.
   - Call the `openPageFile` function to open the created page file (`idxId`) and obtain a file handle (`fhandle`).
   - If the return code of `openPageFile` is not `RC_OK`, return the same return code.

4. **Page Initialization**:
   - Call the `initializePage` function to initialize the first page of the B+ tree with metadata such as key type (`keyType`) and degree (`n`).
   - Pass the file handle (`&fhandle`) to `initializePage`.
   - Store the return code of `initializePage` in the variable `rc`.

5. **File Closing**:
   - Call the `closePageFile` function to close the opened page file (`&fhandle`).

6. **Return**:
   - Return the value of `rc` to indicate the success or failure of the operation.

---
## Function openBtree

This file describes the `openBtree` function to open an existing B+ tree stored in the file specified by the `idxId` parameter.

**Function Signature**

```c
RC openBtree(BTreeHandle **tree, char *idxId)
```

**Purpose:** This function opens an existing B+ tree and initializes the necessary structures and metadata.

**Parameters:**

- `tree`: A pointer to a pointer to the `BTreeHandle` structure representing the B+ tree.
- `idxId`: The identifier for the B+ tree stored in the file.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the index identifier is not found.
- `RC_MEMORY_ALLOCATION_ERROR`: Indicates memory allocation failure.

**Details:**

1. **Index Identifier Validation**:
   - Check if the `idxId` parameter is `NULL`.
   - If `idxId` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Temporary Tree Allocation**:
   - Allocate memory for a temporary `BTreeHandle` structure (`tempTree`) using `malloc`.
   - If memory allocation fails (`tempTree == NULL`), return `RC_MEMORY_ALLOCATION_ERROR`.
   - Assign the address of `tempTree` to the `tree` pointer.

3. **Buffer Pool Initialization**:
   - Declare a buffer pool pointer `bm` of type `BM_BufferPool *`.
   - Call the `MAKE_POOL` macro to create a new buffer pool.
   - Call the `initBufferPool` function to initialize the buffer pool (`bm`) with default parameters.
   - If initialization fails (`status != RC_OK`), free `tempTree` and return `status`.

4. **Page Pinning**:
   - Declare a page handle pointer `page` of type `BM_PageHandle *`.
   - Call the `MAKE_PAGE_HANDLE` macro to create a new page handle.
   - Call the `pinPage` function to pin the first page of the file to the buffer pool.
   - If pinning fails (`status != RC_OK`), shutdown the buffer pool, free `tempTree`, and return `status`.

5. **Metadata Extraction**:
   - Extract the key type and maximum keys from the pinned page data.
   - Update the `tempTree` structure with the extracted metadata.

6. **Management Data Allocation**:
   - Allocate memory for the `RM_bTree_mgmtData` structure (`managementData`) using `malloc`.
   - If memory allocation fails (`managementData == NULL`), shutdown the buffer pool, free `page`, free `tempTree`, and return `RC_MEMORY_ALLOCATION_ERROR`.

7. **Management Data Initialization**:
   - Initialize the fields of `managementData`.
   - Assign the buffer pool (`bm`) to the management data.
   - Assign the management data to the `tempTree`.

8. **Resource Cleanup**:
   - Free the page handle (`page`).

9. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.
  

---

## Function closeBtree

This file describes the `closeBtree` function to close a B-tree structure and release associated resources.

**Function Signature**

```c
RC closeBtree(BTreeHandle *tree)
```

**Purpose:** This function closes a B-tree structure and releases associated resources.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B-tree to be closed.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle is not found.

**Details:**

1. **Tree Handle Validation**:
   - Check if the `tree` parameter is `NULL`.
   - If `tree` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Management Data Extraction**:
   - Extract the pointer to the B-tree management data (`bTreeMgmt`) from the `tree`.

3. **Buffer Pool Shutdown**:
   - Call the `shutdownBufferPool` function to shut down the buffer pool associated with the B-tree management data (`bTreeMgmt`).
   - Store the return code of `shutdownBufferPool` in the variable `rc`.

4. **Resource Deallocation**:
   - If the buffer pool shutdown is successful (`rc == RC_OK`):
     - Free the allocated memory for the B-tree management data (`bTreeMgmt`).
     - Free the allocated memory for the B-tree handle (`tree`).
     - Safely handle the global variable `root`:
       - If `root` is not `NULL`, free `root` and set it to `NULL`.

5. **Return**:
   - Return the value of `rc` to indicate the success or failure of the operation.

  ---
## Function deleteBtree

This file describes the `deleteBtree` function to delete a B-tree stored in the file specified by the `idxId` parameter.

**Function Signature**

```c
RC deleteBtree(char *idxId)
```

**Purpose:** This function deletes a B-tree stored in the file specified by the `idxId` parameter.

**Parameters:**

- `idxId`: The identifier for the B-tree stored in the file.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the index identifier is not found.

**Details:**

1. **Index Identifier Validation**:
   - Check if the `idxId` parameter is `NULL`.
   - If `idxId` is `NULL`, set the return code `rc` to `RC_IM_KEY_NOT_FOUND`.

2. **File Deletion Operation**:
   - Declare a function pointer `operation` to a function that takes a `char *` parameter and returns an `RC` value.
   - Initialize `operation` to point to the `destroyPageFile` function.
   - Call the function pointed by `operation` with the `idxId` parameter and store the return code in `rc`.

3. **Return**:
   - Return the value of `rc` to indicate the success or failure of the file deletion operation.
---

## Function getNumNodes

This file describes the `getNumNodes` function to return the number of nodes present in the B+ tree associated with the given tree handle.

**Function Signature**

```c
RC getNumNodes(BTreeHandle *tree, int *result)
```

**Purpose:** This function returns the number of nodes present in the B+ tree.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `result`: A pointer to an integer where the result (number of nodes) will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle is not found.

**Details:**

1. **Tree Handle Validation**:
   - Check if the `tree` parameter is `NULL`.
   - If `tree` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Result Assignment**:
   - Assign the value of `numNodeValue` to the memory location pointed by `result`.

3. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.
---

## Function getNumEntries

This file describes the `getNumEntries` function to return the number of entries/records/keys present in the B+ tree associated with the given tree handle.

**Function Signature**

```c
RC getNumEntries(BTreeHandle *tree, int *result)
```

**Purpose:** This function returns the number of entries/records/keys present in the B+ tree.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `result`: A pointer to an integer where the result (number of entries) will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle is not found.

**Details:**

1. **Tree Handle Validation**:
   - Check if the `tree` parameter is `NULL`.
   - If `tree` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Number of Entries Retrieval**:
   - Extract the number of entries from the management data associated with the B+ tree.
   - Calculate the offset of `numEntries` within the `RM_bTree_mgmtData` structure.
   - Dereference the pointer to `mgmtData` in `tree` and add the offset to access the value of `numEntries`.
   - Store the retrieved number of entries in the memory location pointed by `result`.

3. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.
---
## Function getKeyType

This file describes the `getKeyType` function to return the data type of the keys being stored in the B+ tree associated with the given tree handle.

**Function Signature**

```c
RC getKeyType(BTreeHandle *tree, DataType *result)
```

**Purpose:** This function returns the data type of the keys being stored in the B+ tree.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `result`: A pointer to a `DataType` variable where the result (key type) will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle or the result pointer is `NULL`.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `tree` parameter, the `result` pointer, or the `mgmtData` pointer in `tree` is `NULL`.
   - If any of these pointers are `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Key Type Retrieval**:
   - Assign the value of `keyType` from the B+ tree handle (`tree->keyType`) to the memory location pointed by `result`.

3. **Return**:
   - If the key type is successfully retrieved (`result` is not `NULL`), return `RC_OK`.
   - Otherwise, return `RC_IM_KEY_NOT_FOUND`.

---
## Function findKey

This file describes the `findKey` function to search for a key in the B+ tree associated with the given tree handle.

**Function Signature**

```c
RC findKey(BTreeHandle *tree, Value *key, RID *result)
```

**Purpose:** This function searches the B+ tree for the specified key and retrieves the corresponding Record ID (RID).

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `key`: A pointer to the `Value` structure representing the key to search for.
- `result`: A pointer to the `RID` structure where the result (page number and slot number) will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle, the key, or the root node is `NULL`.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `tree`, `key`, or `root` pointer is `NULL`.
   - If any of these pointers are `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Leaf Node Search**:
   - Initialize a variable `leaf` to point to the root node of the B+ tree.
   - Initialize a variable `i` to 0 for iteration.
   - Use a loop to traverse down the tree until reaching a leaf node or encountering a `NULL` key.
   - Inside the loop, iterate through the keys of the current node (`leaf`) and compare them with the search key.
   - Update the `leaf` pointer to the appropriate child node based on the comparison.

3. **Key Search within Leaf**:
   - Iterate through the keys of the leaf node to find the matching key.
   - If the key is found, retrieve the corresponding page number and slot number from the leaf node.
   - If the key is not found, return `RC_IM_KEY_NOT_FOUND`.

4. **Result Assignment**:
   - Assign the page number and slot number to the `result` structure.

5. **Return**:
   - Return `RC_OK` to indicate successful execution of the function.

---

## Function insertKey

This file describes the `insertKey` function to add a new entry/record with the specified key and RID to the B+ tree.

**Function Signature**

```c
RC insertKey(BTreeHandle *tree, Value *key, RID rid)
```

**Purpose:** This function inserts a new key-value pair into the B+ tree.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `key`: A pointer to the `Value` structure representing the key to insert.
- `rid`: The Record ID (RID) associated with the key to be inserted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle or the key is `NULL`.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `tree` or `key` pointer is `NULL`.
   - If either pointer is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Leaf Node Search**:
   - Initialize variables and traverse down the tree to find the appropriate leaf node for insertion.

3. **Node Splitting**:
   - If the leaf node is full, split it into two nodes.
   - Distribute the keys and pointers between the old and new leaf nodes.
   - Update the parent node if necessary.

4. **Insertion**:
   - Insert the key-value pair into the leaf node.

5. **Parent Node Update**:
   - If a node split occurred, update the parent node to accommodate the new child nodes.

6. **Return**:
   - Return `RC_OK` to indicate successful insertion.
---

## Function deleteKey

This file describes the `deleteKey` function to delete the entry/record with the specified key in the B+ tree.

**Function Signature**

```c
RC deleteKey(BTreeHandle *tree, Value *key)
```

**Purpose:** This function deletes the entry/record with the specified key in the B+ tree.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `key`: A pointer to the `Value` structure representing the key to be deleted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle or the key is `NULL`.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `tree` or `key` pointer is `NULL`.
   - If either pointer is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Leaf Node Search**:
   - Traverse down the tree to find the appropriate leaf node containing the key to be deleted.

3. **Key Deletion**:
   - Search for the key in the leaf node.
   - If found, delete the key and associated record from the leaf node.

4. **Parent Node Update**:
   - If necessary, update the parent node to maintain the B+ tree properties.

5. **Return**:
   - Return `RC_OK` to indicate successful deletion.
---

## Function openTreeScan

This file describes the `openTreeScan` function to initialize the scan used for scanning the entries in the B+ tree in sorted key order.

**Function Signature**

```c
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle)
```

**Purpose:** This function initializes the scan used to scan the entries in the B+ tree in sorted key order.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B+ tree.
- `handle`: A pointer to a pointer to the `BT_ScanHandle` structure to store the scan handle.

**Return:**

- `RC_OK`: Indicates successful initialization of the scan.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the B-tree handle is `NULL`.
- `RC_MALLOC_FAILED`: Indicates memory allocation failure.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `tree` pointer is `NULL`.
   - If `tree` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Allocate Memory**:
   - Allocate memory for the `BT_ScanHandle` structure and initialize it to zero using `calloc`.
   - If memory allocation fails, return `RC_MALLOC_FAILED`.

3. **Initialize Scan Handle**:
   - Set the `tree` pointer of the scan handle to the provided `tree`.
   - Allocate memory for the `RM_BScan_mgmt` structure within the scan handle and initialize it to zero using `calloc`.
   - If memory allocation fails, free the previously allocated memory and return `RC_MALLOC_FAILED`.
  
4. **Initialize Scan Management Data**:
   - If the scan management data structure is successfully allocated:
     - Set the current pointer (`cur`) to `NULL`.
     - Set the index to zero.
     - Set the total scan count to zero.

5. **Return**:
   - Return `RC_OK` to indicate successful initialization of the scan handle.
  
---
## Function nextEntry

This file describes the `nextEntry` function to traverse the entries in the B+ Tree.

**Function Signature**

```c
RC nextEntry(BT_ScanHandle *handle, RID *result)
```

**Purpose:** This function is used to traverse the entries in the B+ Tree.

**Parameters:**

- `handle`: A pointer to the `BT_ScanHandle` structure representing the scan handle.
- `result`: A pointer to the `RID` structure to store the result.

**Return:**

- `RC_OK`: Indicates successful traversal of the next entry.
- `RC_IM_KEY_NOT_FOUND`: Indicates that the scan handle is `NULL`.
- `RC_IM_NO_MORE_ENTRIES`: Indicates that there are no more entries to traverse.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `handle` pointer is `NULL`.
   - If `handle` is `NULL`, return `RC_IM_KEY_NOT_FOUND`.

2. **Scan Management Data Retrieval**:
   - Retrieve the scan management data structure from the scan handle.

3. **Get Total Number of Entries**:
   - Call the `getNumEntries` function to get the total number of entries in the B+ Tree.
   - If the return code is not `RC_OK`, return the same error code.

4. **Check for No More Entries**:
   - If the total number of scanned entries exceeds or equals the total number of entries in the B+ Tree, return `RC_IM_NO_MORE_ENTRIES`.

5. **Traverse to Leaf Node**:
   - If the total scan count is zero, traverse to the leaf node containing the first entry.
   
6. **Update Scan Management Data**:
   - If the current index in the leaf node exceeds the number of keys, move to the next leaf node.
   
7. **Retrieve RID**:
   - Allocate memory for the `RID` structure.
   - Retrieve the RID from the current leaf node.
   
8. **Update Scan Management and Result**:
   - Update the scan management data with the incremented total scan count.
   - Update the scan handle's management data.
   - Store the result in the provided `RID` structure.

9. **Return**:
   - Return `RC_OK` to indicate successful traversal of the next entry.
 
---

## Function closeTreeScan

This file describes the `closeTreeScan` function to close the scan mechanism and free up resources.

**Function Signature**

```c
RC closeTreeScan(BT_ScanHandle *handle)
```

**Purpose:** This function closes the scan mechanism and frees up resources.

**Parameters:**

- `handle`: A pointer to the `BT_ScanHandle` structure representing the scan handle to be closed.

**Return:**

- `RC_OK`: Indicates successful closure of the scan mechanism.

**Details:**

1. **Input Parameter Validation**:
   - Check if the `handle` pointer is `NULL`.
   - If `handle` is `NULL`, return `RC_OK` indicating successful closure.

2. **Release Resources**:
   - Set the `mgmtData` pointer of the scan handle to `NULL` to release any associated resources.
   - Free the memory allocated for the scan handle.

3. **Return**:
   - Return `RC_OK` to indicate successful closure of the scan mechanism.

---
## Function recDFS

This file describes the `recDFS` function to perform a recursive depth-first search (DFS) traversal of the B-tree nodes.

**Function Signature**

```c
int recDFS(RM_BtreeNode *bTreeNode)
```

**Purpose:** This function performs a recursive depth-first search (DFS) traversal of the B-tree nodes.

**Parameters:**

- `bTreeNode`: A pointer to the `RM_BtreeNode` representing the current node in the traversal.

**Return:**

- `int`: Returns 0 to indicate successful traversal.

**Details:**

1. **Node Validation**:
   - Check if the `bTreeNode` pointer is `NULL`.
   - If `bTreeNode` is `NULL`, return 0 to terminate the traversal.

2. **Update Node Position**:
   - Update the position of the current node (`bTreeNode->pos`) to `globalPos + 1`.

3. **Depth-First Search (DFS) Traversal**:
   - If the current node is not a leaf node, recursively traverse its child nodes:
     - Iterate through each child node and recursively call `recDFS` on each child node.

4. **Return**:
   - Return 0 to indicate successful traversal.
---

## Function walkPath

This file describes the `walkPath` function to traverse the path of the B-tree nodes and construct a string representation of the keys.

**Function Signature**

```c
int walkPath(RM_BtreeNode *bTreeNode, char *result)
```

**Purpose:** This function traverses the path of the B-tree nodes and constructs a string representation of the keys.

**Parameters:**

- `bTreeNode`: A pointer to the `RM_BtreeNode` representing the current node in the traversal.
- `result`: A pointer to a character array (`char *`) where the string representation of the keys will be stored.

**Return:**

- `int`: Returns 0 to indicate successful traversal.

**Details:**

1. **Memory Allocation**:
   - Allocate memory for the `line` character array with a size of 100 characters.

2. **Leaf Node Handling**:
   - If the current node is a leaf node and not `NULL`, iterate through its keys:
     - Serialize each key and concatenate it to the `line`.
     - Free the memory allocated for the serialized value.
     - If the last pointer of the leaf node is `NULL`, append '-' to indicate the end of the line.

3. **Non-Leaf Node Handling**:
   - If the current node is not a leaf node and not `NULL`, iterate through its keys:
     - Serialize each key and concatenate it to the `line`.
     - If the last child pointer of the node is `NULL`, append '-' to indicate the end of the line.

4. **Concatenate Line to Result**:
   - Concatenate the `line` to the `result` string.

5. **Recursive Traversal**:
   - If the current node is not a leaf node and not `NULL`, recursively call `walkPath` for each child node.

6. **Return**:
   - Return 0 to indicate successful traversal.
---

## Function printTree

This file describes the `printTree` function to print the entire B-tree structure.

**Function Signature**

```c
char *printTree(BTreeHandle *tree)
```

**Purpose:** This function prints the entire B-tree structure.

**Parameters:**

- `tree`: A pointer to the `BTreeHandle` structure representing the B-tree.

**Return:**

- `char *`: A pointer to a character array containing the string representation of the B-tree structure.

**Details:**

1. **Root Node Check**:
   - Check if the `root` node of the B-tree is `NULL`. If so, return `NULL`.

2. **Recursive Depth-First Search (DFS)**:
   - Perform a depth-first search traversal of the B-tree starting from the root node.
   - Invoke the `recDFS` function to calculate the length of the traversal path and store it in `length`.

3. **Allocate Memory**:
   - Allocate memory for the `result` character array with a size of `length` multiplied by 1000 characters.

4. **Walk Path and Build String Representation**:
   - Invoke the `walkPath` function to traverse the B-tree and construct the string representation of its keys.
   - Store the result in the `result` character array.

5. **Return Result**:
   - Return the `result` character array containing the string representation of the B-tree structure.
