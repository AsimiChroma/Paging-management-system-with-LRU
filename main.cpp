// page table as an array?

// Paging-based memory management system
// Implement functions:
//      - initializing memory space: void memoryManager(int memSize, int frameSize)
//              - frame size fixed to 1
//              - int frameSize specifies frame size of the created memory space in bytes
//              - int memSize specifies size of the created memory space = # of frames in physical memory
//              - free frames will be allocated to processes
//              - create and intialize a freeFrameList that maintains and keeps track of free frames
//      - allocating memory space to process: int allocate(int allocSize, int pid) // allocating frames for each each page process
//              - allocates a c hunk of memory space of 'allocSize' bytes to a process w/process ID pid
//              - default size of 1 byte
//              - use freeFrameList to find available free frames then change used frames to False
//              - use random mapping method to mapp btwn pages and free frames and create page table to record and maintain allocated pages
//              - return value = 1 if a requested memory space has been succesfully allocated, else value = -1 if there isn't enough free frames
//              - assume page tables not stored in physical memory space (array of free frames)
//              - update freeFrameList and processList (add pid) after allocating free frames to a process (remove the free frames)
//      - deallocating memory space to process: int deallocate(int pid)
//              - deallocates memory space from process with process ID pid
//              - similar to allcoate(); freeFrameList and processList must also be updated after deallocating a memory space from process
//              - return value = 1 if successful, value = -1 if unsuccessful
//      - reading data to process memory space: int read(int pid, int logical_address)
//              - similar to write(); only difference is this function is used to read a value from the memory location specified by the page number 'logical_address'
//      - writing data to process memory space: int write(int pid, int logical_address)
//              -  used to write a value of '1' to the memory space (memory address specified by parameter 'logical_address') of a process with pid
//              - logical_address is just a page number (page number must be converted to corresponding frame number in order to write value)
//              - return value = 1 if successful, else value = -1 if unsuccessful
//      - printing out the memory contents of a process: void printMemory(void)
//              - prints out physical memory space
//              - freeFrameList ---- prints out " 1 2 6 " if 1, 2, and 6 are free frames
//              - processList ---- prints out "1 7" if value 1 is pid and 7 is process size 
//
// Define 2 lists: 1. freeFrameList and processList
//      - freeFrameList keeps track of free frames: simply a list of the frame numbers of free frames
//      - processList keeps track of active processes (that are loaded into memory) and their sizes in terms of the number of allocated pages (size 3 = 3 pages, size 4 = 4 pages, etc)


#include "manager.h"
#include "string_handler.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;


    // Command list:
    //  - M memorySize frameSize ----- creates and initializes a physical memory space
    //  - A size pid ----- allocate pages to process with pid
    //  - W pid address1 ----- write a value of '1' to a process with pid at the location specified by 'address1'
    //  - D pid ----- deallocate a memory space from a process with pid
    //  - P ----- print out physical address space, free frame list, and process list

int main()
{
    string command;
    vector<string> cmd;
    char cmdLetter;
    int param1;
    int param2;

    while (true)
    {
       
        // Get user input
        getline(cin, command);

        cmd = split(command);
        // cout << cmd[0] << endl;
        // cout << cmd[1] << endl;
        // cout << cmd[2] << endl;

        if (cmd.size() >= 1)
        {
            if (cmd[0].size() != 1)
            {
                cout << "Please try again -- check your inputs" << endl;
                continue;
            }
            else
            {
                cmdLetter = cmd[0][0];
            }
        }
//cout << cmdLetter << endl;
        if (cmd.size() >= 2)
        {
            param1 = stoi(cmd[1]);
        }

//cout << param1 << endl;
        
        if (cmd.size() == 3)
        {
            param2 = stoi(cmd[2]);
        }

//cout << param2 << endl;


        if(cmdLetter == 'M')
            memMngr(param1, param2);
        else if(cmdLetter == 'A')
            cout << allocate(param1, param2) << endl;
        else if(cmdLetter == 'W')
            cout << write(param1, param2) << endl;
        else if(cmdLetter == 'R')
            cout << read(param1, param2) << endl;
        else if(cmdLetter == 'D')
            cout << deallocate(param1) << endl;
        else if (cmdLetter == 'P')
            printMem();
        else
            cout << "Please try again -- check your inputs" << endl;

    }

    return 0;
}
