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
