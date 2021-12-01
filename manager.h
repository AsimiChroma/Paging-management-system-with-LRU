#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
using namespace std;

extern vector<int> g_physMem; // global physical memory array
extern int pageSize;

//----- Functions -----//


void delMemFrame(int frame);    // Deletes contents inside each specified frame


void memMngr(int memSize, int frameSize);   // Initialize memory space
int allocate(int allocSize, int pid);   // Allocating memory space to process
int deallocate(int pid);    // Deallocates memory space from process with process ID pid
int read(int pid, int logi_addr);   // Used to read value from memory location specified by page number 'logi_addr'
int write(int pid, int logi_addr);  // Used to write a value to memory space specified by parameter 'logi_addr' of a process with pid
void printMem(void);    // Prints out physical memory space -- freeFrameList and processList
void printMemory();

//----- Lists -----//


// Singly linked list for freeFrameList
class FreeNode {
public: 
    // pointer to next node
    FreeNode* next;

    // frame number
    int frameNum;

    // free or not free frame
    bool freeFrame;
};

class FreeList {
public:
    FreeNode* head; // points to first node in list (currently NULL)

    // Find the next free frame available (randomly) returns the frame number that's true
    int findFrame();

    // Return number of free frames in physical memory
    int totalFree();

    // Update frame to be true of false (free or not free)
    void updateFrame(int frameNum, bool freeFrame);

    // Print
    void print();   // printing free frames

    // Insert
    void insert(int frameNum, bool freeFrame);  // inserts next node at end of list

    // Delete
    void remove();  // destructor calls this each time

    // Constructor to build list
    FreeList();
    // Destructor to delete list
    ~FreeList();

    void init(int frameSize);
};

extern FreeList freeFrameList;  // global variable to hold linked list



// Doubly linked list for processList
class ProcessNode {
public:

    ProcessNode* next;  // points to next node in list
    ProcessNode* prev;  // points to previous node in list

    // Active process from PID
    int pid;

    // Size in terms of number of pages
    int pageSize;
    
    // Victim table vector that stores true for valid and false for invalid
    vector<bool> validFlag;

    // Page table vector
    vector<int> pgTable;    
};

class ProcessList {
public:

    ProcessNode* head; // points to first node in list
    ProcessNode* tail;  // points to last node in list

    // Print
    void print();   // printing free frames with sizes

    // Insert
    void insert(int pid, int pageSize);  // inserts next node at end of list

    // Delete list
    void remove();  // destructor calls this each time

    // Delete pid
    void removePid(int pid);

    // Update pgTable
    void updatePgTable(int pid, int pgNum, int frameNum, bool validFlag);   // insert new values into pgTable

    // Finds page size of process
    int findPgSize(int pid);

    // Page lookup to return a frame number
    int pageLookup(int pid, int pgNum);

    // Find pid
    bool findPid(int pid);

    // Valid or invalid
    bool checkValid (int pid, int pgNum);

    // Constructor
    ProcessList();

    // Destructor
    ~ProcessList();
};

extern ProcessList processList; // global variable for processList


// Doubly linked list for LRU stack
class StackNode {
public:

    StackNode* next;  // points to next node in list
    StackNode* prev;  // points to previous node in list

    // Active process from PID
    int pid;

    // Page number
    int pageNum;   
};

class StackList {
public:

    StackNode* head; // points to first node in list
    StackNode* tail;  // points to last node in list
    
    // Print
    void print();   // printing the LRU stack
    
    // Insert
    void insert(int pid, int pageNum);  // inserts next node at head of list b/c stack
    
    // Remove
    void remove();  // destructor calls this each time
    
    // Delete pid and pageNum from stack (so it can be moved to top of stack after)
    void removePid(int pid, int pageNum);
    
    // Find victim pid
    int victimPid(int pid, int pageNum);
    
    // Find victim page
    int victimPage(int pid, int pageNum);
    
    // Constructor
    StackList();

    // Destructor
    ~StackList();

};

extern StackList stackList; // global variable for stackList

#endif
