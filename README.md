# CSC456OS-Paging-based-memory-mngmt-system
A small paging-based memory management system run on Linux.

Please view in raw format to see the correct spacing!


How to run code:

To run the code, you please type "make" in your terminal to run the Makefile that will create an ./a.out file for you to input the commands. As for the commands, the command list is as follows:

 Command list:
 
    - M memorySize frameSize ----- creates and initializes a physical memory space
    
    - A size pid ----- allocate pages to process with pid
    
    - W pid address1 ----- write a value of '1' to a process with pid at the location specified by 'address1'
    
    - D pid ----- deallocate a memory space from a process with pid
    
    - P ----- print out physical address space, free frame list, and process list
    

You can run those commands in order. 
An example input/output would be as follows:

    M 21 1
    A 4 4 
    1
    D 4
    1
    P
    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20



Description of program design:

I decided to make a global vector for the physical memory because it's an improved dynamic array structure, and it was easy to change the size and insert elements into, and many elements in my code needed access to the physical memory space. As for the two different lists, I created a singly linked list for the freeFrameList and inserted new nodes at the end of the linked list to keep the frames in order. A doubly linked list was not needed for this because there was no need to traverse back through the list for any reason. However, a doubly linked list was used to implement the processList, since we need to be able to go back and delete any process in the list at any memory location, not just from the end of the list. I also created a page table for each process, so that I could keep track of mapping the pages to frames correctly. 
