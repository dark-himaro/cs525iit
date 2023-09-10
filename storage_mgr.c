// including library files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
// including storage manager and dberror header files
#include "storage_mgr.h"
#include "dberror.h"

// Implementing the code for storage manager, we need global file pointer
// Declaring global file pointer
FILE *file_strmgr;

// Implementing the function initStorageManager
void initStorageManager(void)
{
    printf("Hurray...The storage manager is initialised successfully!");
}

// Implementing the function createPageFile
// This function createPageFile is used to create new page
RC createPageFile(char *file_name)
{

    if (file_name != NULL)
    {
        // Here we use w+ mode to make write and read simultaneously on file_name
        file_strmgr = fopen(file_name, "w+");

        // Here we are allocating the memory for the page with the specific size PAGE_SIZE
        SM_PageHandle new_page = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));

        // writing into the page using fwrite function
        //  This is the syntax of fwrite(Buffer, element size,Element count, file stream) so this is set up accordingly
        fwrite(new_page, 1, PAGE_SIZE, file_strmgr);

        // close the file pointer as it finished writing on the page using fwrite
        int isClosed = fclose(file_strmgr);
        if(isClosed == 0)
        {
        printf("\nClosing the file in the memory successfully!");
        // Deallocating the memory of created new page new_page using inbuilt free function
        free(new_page);
        printf("\nDeallocated the memory successfully");
        // returning the error code
        return RC_OK;
        }
        else{
            printf("File not closed");
        }
    }
    else
    {
        // returning the error code when it returns the null value for file_name
        return RC_FILE_NOT_FOUND;
    }
}

// implementing the closePageFile function
RC closePageFile(SM_FileHandle *file_handle)
{
    // if file_handle is null which checks if the file handle is valid or not
    if (file_handle == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // if the file doesnot exist in the memory then it releases the error code as specified
    if (file_handle->mgmtInfo == NULL)
    {
        // returning the corresponding error code
        return RC_FILE_NOT_FOUND;
    }
    // storing the result of the inbuilt fclose function in the variable fclose_result
    int fclose_result = fclose(file_handle->mgmtInfo);
    // checking if the file is closed successfully or not and printing accordingly
    if (fclose_result == 0)
    {
        // if the file is closed then making the pointer to NULL
        file_strmgr = NULL;
        printf("\nThe file in closePageFile function is successfully closed");
        // returning the corresponding error code
        return RC_OK;
    }
    else
    {
        // returning the error code for the failing condition
        return RC_FILE_NOT_FOUND;
    }
}
// implementing the function destroyPageFile
RC destroyPageFile(char *file_name)
{
    // Opening the file in the read mode using the file pointers
    file_strmgr = fopen(file_name, "r");
    // It is used to check if the file exists or not, if it returns null then it shows the error code
    if (file_strmgr == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        // We are closing the opened file
        fclose(file_strmgr);
        // we are deleting the file_name using unlink function, here unlink is used to permanently delete the file
        unlink(file_name);
        // pointing the file pointer to NULL
        file_strmgr = NULL;
        printf("\nThe opened page is destroyed!\n");
        return RC_OK;
    }
}
// Implementing the openPageFile function
RC openPageFile(char *file_name, SM_FileHandle *file_handle)
{
    // opening the file using fopen function
    file_strmgr = fopen(file_name, "r+");
    if (file_strmgr == NULL)
    {
        // if the required file is not found and it returns NULL then we return the error code
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        // here we use the SEEK_END attribute in fseek to move the file pointer to the end of the file
        fseek(file_strmgr, 0, SEEK_END);
        // initialise and declare the endvalue as endval and initialise with 0
        int endval = ftell(file_strmgr);
        // total_pg is used for storing the total number of pages in the file
        int total_pg = 0;
        // endval = ;
        // Calculating the total number of pages
        if ((endval % PAGE_SIZE) != 0)
        {
            total_pg = (endval / PAGE_SIZE) + 1;
        }
        else
        {
            total_pg = (endval / PAGE_SIZE);
        }
        // initialising the needed attributes
        int att1;
        // finding the total number of pages
        file_handle->totalNumPages = total_pg;
        att1 = file_handle->totalNumPages;
        printf("\nTotal number of pages in the file is %d", att1);

        // curPagepos shows the position of the current page
        file_handle->curPagePos = 0;
        printf("\nThe current page is %d\n", file_handle->curPagePos);
        // It is used to store the file name
        file_handle->fileName = file_name;
        // It is used to store the filepointer
        file_handle->mgmtInfo = file_strmgr;
        // returning the error code as ok as everything is correct
        return RC_OK;
    }
}

// Implementing readBlock function, which is reading blocks from disk
RC readBlock(int page_number, SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    file_strmgr = fopen(file_handle->fileName, "r");
    if (file_strmgr == NULL)
    {
        printf("\nThe file does not exist in this readBlock function");
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        printf("\nThe file has been opened successfully!");
        if ((file_handle->totalNumPages) > page_number && page_number >= 0)
        {
            printf("\nThis is a existing page that you are reading!");
        }
        else
        {
            // printing the total number of pages
            printf("\nTotal number of pages is %d", file_handle->totalNumPages);
            // printing the respective page number
            printf("\n The page number is %d", page_number);
            // Here print that the page does not exist
            printf("\nThe page does not exist!");
            // return the corresponding error code
            return RC_READ_NON_EXISTING_PAGE;
        }
        int starting_page = (page_number * PAGE_SIZE);
        // Using the fseek and the seek set to point the start of the page
        int fseek_pointer = fseek(file_strmgr, starting_page, SEEK_SET);
        // checking the return value if it is 0 then it is pointing to the start of the page
        if (fseek_pointer == 0)
        {
            printf("\nPointer is at the start of the page");
        }
        else
        {
            // printing the corresponding error and exiting from the function
            printf("\nThe pointer is not at the starting of the page");
            exit(1);
        }

        printf("\nThe position of the pointer before reading the file is at the position %d", starting_page);
        int pointer_position, current_position;
        fread(page_handle, sizeof(char), PAGE_SIZE, file_strmgr);
        pointer_position = ftell(file_strmgr);

        printf("\n The position of the pointer after reading the file is at the position %d", pointer_position);

        if ((pointer_position % PAGE_SIZE) != 0)
        {
            current_position = pointer_position / PAGE_SIZE;
        }
        else
        {
            current_position = (pointer_position / PAGE_SIZE) - 1;
        }
        // changing current position in the SM_FileHandle using the attribute curPagePos
        file_handle->curPagePos = current_position;
        // capturing the current position of the filehandle after execution of readBlock function
        int curr_pos = file_handle->curPagePos;
        printf("\nAfter readblock the current position is %d", curr_pos);
        // capturing the totalnumber of pages after the execution of readBlock using total_num variable
        int total_num = file_handle->totalNumPages;
        printf("\nAfter executing the readblock function the total number of pages is %d", total_num);
        // closing the file pointer
        int f_pr = fclose(file_strmgr);
        // checking the value of the file pointer
        if (f_pr != 0)
        {
            printf("\nThe file is not closed, please recheck the operation and make the changes");
            exit(1);
        }
        else
        {
            printf("\nHurray! The file is closed!");
            return RC_OK;
        }
    }
}

// Implementing the readFirstBlock function
RC readFirstBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    // checking the file_handle pointer returns null or not and proceeding further
    if (file_handle == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }
    // here readFB_position is used to find the readFirstBlock position
    int readFB_position = file_handle->curPagePos;
    // printinf the corresponding position before reading the first block
    printf("\nBefore reading the first block, the position of the block is %d", readFB_position);
    int initial_page = 0;
    return readBlock(initial_page, file_handle, page_handle);
}

// Implementing the getBlockPos function
int getBlockPos(SM_FileHandle *file_handle)
{
    // checking the file handle is null or not and printing the corresponding error message
    if (file_handle == NULL)
    {
        printf("\nThe initialisation of File handler is not done successfully");
        return 0;
    }
    // Executing the loop if filehandle is not NULL and find the block position
    if (file_handle != NULL)
    {
        int block_pos = file_handle->curPagePos;
        // printing the current block position
        printf("\ncurrent block position is %d", block_pos);
        // returning the block position of the current file
        return block_pos;
    }
}

RC readCurrentBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    // capturing the current position of the file
    int current_pos;
    // getting the current position of the file using the getBlockPos function from the functions written
    current_pos = getBlockPos(file_handle);

    // printing the current position of the page after reading the current block
    printf("\nAfter reading the current block, the position of the block is %d", file_handle->curPagePos);
    // returning the readBlock function with its arguments
    return readBlock(current_pos, file_handle, page_handle);
}

// Implementing the readLastBlock function
RC readLastBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    // Total number of pages-1 gives the last page number in the file
    int last_page;
    last_page = file_handle->totalNumPages - 1;
    // returning the readBlock function with its required arguments
    return readBlock(last_page, file_handle, page_handle);
}

/*Implementing the readPreviousBlock function*/
RC readPreviousBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    // storing the current position of the file
    int current_pos = getBlockPos(file_handle);
    // previous page is current page -1
    int prev_pg = current_pos - 1;

    // setting the current page to previous page
    file_handle->curPagePos = prev_pg;

    printf("\nThe block position after reading is %d", file_handle->curPagePos);
    // returning the readBlock function
    return readBlock(prev_pg, file_handle, page_handle);
}

// Implementing the readNextBlock function
RC readNextBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    // Here we read the page from the next block
    // capturing the current position of the page
    int current_pos,next_pg;

    // calling getBlockPos
    current_pos = getBlockPos(file_handle);
    // next page is current_pos+1
    next_pg = current_pos + 1;
    // changing the current position to next page
    file_handle->curPagePos = next_pg;
    // capturing the current position
    current_pos = file_handle->curPagePos;
    printf("\nAfter readNext block the current position of the block is %d", current_pos);
    // closing the file stream
    fclose(file_strmgr);
    // returning the readBlock function
    return readBlock(next_pg, file_handle, page_handle);
}

RC writeBlock(int page_number, SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    if (file_strmgr == NULL)
    {
        printf("\nFile that you are looking for does not exist");
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        if (page_number >= 0 && page_number < file_handle->totalNumPages)
        {
            if (file_handle != NULL)
            {
                FILE *file_pointer = file_handle->mgmtInfo;
                if (file_pointer != NULL)
                {
                    int fseek_result = fseek(file_pointer, page_number * PAGE_SIZE, SEEK_SET);
                    if (fseek_result == 0)
                    {
                        size_t fwrite_result = fwrite(page_handle, sizeof(char), PAGE_SIZE, file_pointer);
                        if (fwrite_result == PAGE_SIZE)
                        {
                            int current_pos;

                            file_handle->curPagePos = page_number;
                            current_pos = file_handle->curPagePos;
                            printf("\nCurrent position after the write Block is %d", current_pos);
                            int att1 = file_handle->totalNumPages;
                            printf("\nTotal number of pages inside writeBlock are %d", att1);
                            return RC_OK;
                        }
                        else
                        {
                            return RC_WRITE_FAILED;
                        }
                    }
                    else
                    {
                        printf("\nThe fseek function did not function properly");
                        return RC_WRITE_FAILED;
                    }
                }
                else
                {
                    printf("\nThis is NOT a valid file pointer");
                    return RC_FILE_NOT_FOUND;
                }
            }
            else
            {
                printf("\nThis is not a valid File handle");
                return RC_FILE_HANDLE_NOT_INIT;
            }
        }
        else
        {
            printf("\nSize of page exceeded");
            return RC_WRITE_FAILED;
        }
    }
}

RC ensureCapacity(int total_pages, SM_FileHandle *file_handle)
{
    //opening the file in append mode
    file_strmgr = fopen(file_handle->fileName, "a");

    //checking if the file exists
    if (file_strmgr == NULL)
    {
        printf("\nThe file does not exist in ensureCapacity block");
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        if (total_pages <= file_handle->totalNumPages)
        {
            return RC_OK;
        }
        else
        {
            while ((file_handle->totalNumPages) < total_pages)
            {
                appendEmptyBlock(file_handle);
            }
        }
    }
    int curr_pos = file_handle->curPagePos;
    printf("\nThe current position of the block is %d", curr_pos);
    int total_pg = file_handle->totalNumPages;
    printf("\nTotal number of pages after ensureCapacity function is %d", total_pg);
    // closing the file stream using fclose function
    fclose(file_strmgr);
    return RC_OK;
    // Ended the implementation of ensureCapacity function
}

RC appendEmptyBlock(SM_FileHandle *file_handle)
{
    if (file_strmgr == NULL)
    {
        printf("\n File is not existing in this function");
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        // Using calloc to allocate the memory to the page in the appendEmpty Block function
        char *page_empty = (char *)calloc(PAGE_SIZE, sizeof(char));
        int fseek_result = fseek(file_handle->mgmtInfo, 0, SEEK_END);
        if (fseek_result != 0)
        {
            free(page_empty);
            return RC_WRITE_FAILED;
        }
        else
        {
            fwrite(page_empty, PAGE_SIZE, 1, file_handle->mgmtInfo);

            // Keeping a track of total number of pages and updating them
            file_handle->totalNumPages++;
            file_handle->curPagePos++;
            int total_pg = file_handle->totalNumPages;
            printf("\nThe total number of pages after appendEmpty block is %d", total_pg);
            int curr_pos = file_handle->curPagePos;
            printf("\nThe current position of file handle pointer after appendEmpty block is %d", curr_pos);
        }
        free(page_empty);
        return RC_OK;
    }
}

RC writeCurrentBlock(SM_FileHandle *file_handle, SM_PageHandle page_handle)
{
    if (file_handle != NULL)
    {
        int curr_pos = file_handle->curPagePos;
        RC writecb_result = writeBlock(curr_pos, file_handle, page_handle);
        return writecb_result;
    }
    else
    {
        printf("\nThis is not an existing file");
        return RC_FILE_NOT_FOUND;
    }
}


