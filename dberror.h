#ifndef DBERROR_H
#define DBERROR_H

#include "stdio.h"

/* module wide constants */
#define PAGE_SIZE 4096

/* return code definitions */
typedef int RC;

#define RC_OK 0
#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4
#define RC_BUFFER_NOT_INIT 5
#define RC_CLOSE_FILE_FAILED 6
#define RC_PAGES_IN_BUFFER 7
#define RC_FAILURE_TO_OPEN_FILE 8
#define RC_FATAL_ERROR 9

#define RC_SEEK_ERROR 199
#define RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE 200
#define RC_RM_EXPR_RESULT_IS_NOT_BOOLEAN 201
#define RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN 202
#define RC_RM_NO_MORE_TUPLES 203
#define RC_RM_NO_PRINT_FOR_DATATYPE 204
#define RC_RM_UNKOWN_DATATYPE 205

#define RC_IM_KEY_NOT_FOUND 300
#define RC_IM_KEY_ALREADY_EXISTS 301
#define RC_IM_N_TO_LAGE 302
#define RC_IM_NO_MORE_ENTRIES 303
#define RC_INVALID_PAGE_VALUE 304
#define RC_MGMT_POINTER_NULL 305
#define RC_FRAME_POINTER_FAILURE 306
#define RC_INVALID_INPUTS 307
#define RC_INVALID_DATATYPE 308

#define RC_MEM_ALLOC_FAILED 400
#define RC_ALLOCATION_FAILED 401
#define RC_ERROR 402

#define RC_RM_NO_TUPLE_WITH_GIVEN_RID 600
#define RC_SCAN_CONDITION_NOT_FOUND 601
#define RC_PINNED_PAGES_IN_BUFFER 607

#define RC_SERIALIZER_ERROR 900
#define RC_TABLE_ERROR 901
#define RC_RECORD_SIZE_FATAL_ERR 902
#define RC_SCHEMA_CREATION_FAILURE 903

#define RC_MEMORY_ALLOCATION_ERROR 950
#define RC_MALLOC_FAILED 951
#define RC_IM_MEMORY_ERROR 952
/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError (RC error);
extern char *errorMessage (RC error);

#define THROW(rc,message) \
		do {			  \
			RC_message=message;	  \
			return rc;		  \
		} while (0)		  \

// check the return code and exit if it is an error
#define CHECK(code)							\
		do {									\
			int rc_internal = (code);						\
			if (rc_internal != RC_OK)						\
			{									\
				char *message = errorMessage(rc_internal);			\
				printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n",__FILE__, __LINE__, __TIME__, message); \
				free(message);							\
				exit(1);							\
			}									\
		} while(0);


#endif
