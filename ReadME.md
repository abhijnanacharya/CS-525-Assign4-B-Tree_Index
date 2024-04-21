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
