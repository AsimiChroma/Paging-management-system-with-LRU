#include <vector>
#include <iostream>
#include "manager.h"

using namespace std;

ProcessList processList;
FreeList freeFrameList;
StackList stackList;
vector<int> g_physMem;



void memMngr(int memSize, int frameSize) // memSize = # of frames in physical memory
{
    g_physMem.resize(memSize*frameSize, 0); 
    freeFrameList.init(memSize);
    //cout << memSize << "MEMSIZE" << endl;
    //cout << frameSize << "FRAMESIZE" << endl;
    //cout << g_physMem.size() << "PHYSMEMSIZE" << endl;
    //freeFrameList.print();
}

int allocate(int allocSize, int pid)    // allocSize = pagesize
{
   processList.insert(pid, allocSize);
   for (int i = 0; i < allocSize; i++)
   {
   		if (freeFrameList.findFrame() == -1)
   		{
   			// Find the victim pid and pagenum so we can store it before deleting it
   			int victimPage = stackList.victimPage(pid, i);
   			int victimPid = stackList.victimPid(pid, i);
   			stackList.removePid(pid, i);
   			processList.updatePgTable(victimPid, victimPage, -1, false);	// set page table to be false b/c we delete it
   			int frame = processList.pageLoookup(victimPid, victimPage);
   			freeFrameList.updateFrame(frame, true);
   		}
		processList.updatePgTable(pid, i, freeFrameList.findFrame(), true);	// now new framenum can be inserted to top of stack
		freeFrameList.updateFrame(freeFrameList.findFrame(), false);
   }
   return 1;
}

int deallocate(int pid)
{
    if (processList.findPid(pid)) // if pid is found then return 1, otherwise return -1
    {
        for (int i = 0; i < processList.findPgSize(pid); i++)
        {
            // i is the page number; returns the frame number associated with page number
            freeFrameList.updateFrame(processList.pageLookup(pid, i), true);
        }
        processList.removePid(pid);
        return 1;
    }
    else
    {
        return -1;
    }       
}

int write(int pid, int logi_addr)   // logical address = page number in this case
{
    // write a value of '1' at the memory location specified by the page number of a pid
    // page number converted to frame number before writing value '1'
        // page num --> page table --> frame number --> write value '1'
    // if succesful (if pid or size of pid exists), return 1, if unsuccesful return -1

    if (processList.findPid(pid) && processList.findPgSize(pid) > logi_addr && logi_addr >= 0)
    {
        int frameNum = processList.pageLookup(pid, logi_addr);
        g_physMem[frameNum] = 1;    // writes '1' to the frame number

        return 1;
    }
    else
    {
        return -1;
    }
}

int read(int pid, int logi_addr)
{
        int frameNum = processList.pageLookup(pid, logi_addr);

        return g_physMem[frameNum]; // reads and returns the value of that frame
}


void printMem(void)
{
    freeFrameList.print();
    processList.print();
}


// FreeList class functions
int FreeList::findFrame() 
{
    FreeNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->freeFrame)
        {
            return ptr->frameNum;
        }
        ptr = ptr->next;
    }    
    // If there are no free frames then return -1
    return -1;
}

void FreeList::updateFrame(int frameNum, bool freeFrame)
{
    FreeNode* ptr = head;
    while (ptr != NULL)
    {
        if (frameNum == ptr->frameNum)
        {
            ptr->freeFrame = freeFrame;
        }
        ptr = ptr->next;
    }    
}


// Probs delete after b/c not using
int FreeList::totalFree()
{
    int counter = 0;
    FreeNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->freeFrame)
        {
            counter++;
        }
        ptr = ptr->next;
    }    
    return counter;
}

void FreeList::print() 
{
    FreeNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->freeFrame)
        {
            cout << ptr->frameNum << " ";
        }
        ptr = ptr->next;
    }
    cout << endl;
}

void FreeList::insert(int frameNum, bool freeFrame) 
{
    FreeNode* node = new FreeNode;
    if (node == NULL)
    {
        cerr << "MEMORY FULL - no insertion allowed" << endl;
    }
    else
    {
        node->frameNum = frameNum;
        node->freeFrame = freeFrame;
        node->next = NULL;
    }

    if (head == NULL)
    {
        head = node;
    }
    else
    {
        FreeNode* ptr = head;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = node;
    }
}

void FreeList::remove() 
{
    while (head != NULL)
    {
        FreeNode* ptr = head;
        head = head->next;
        delete ptr;    
    }
}

FreeList::FreeList() 
{
    head = NULL;
}

FreeList::~FreeList() 
{
    remove();
}

void FreeList::init(int frameSize) 
{
    for (int i = 0; i < frameSize; i++ )
    {
        insert(i, true);    // Initialize every frame value to be true because it's currently free
    }
}

// -------------------------------------------------------------------------------------//


// ProcessList class functions
void ProcessList::print() 
{
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
            cout << ptr->pid << " " << ptr->pageSize << endl;
            ptr = ptr->next;
    }
}

void ProcessList::insert(int pid, int pageSize) 
{
    ProcessNode* node = new ProcessNode;
    if (node == NULL)
    {
        cerr << "MEMORY FULL - no insertion allowed" << endl;
    }
    else
    {
        node->pid = pid;
        node->pageSize = pageSize;
        node->pgTable.resize(pageSize, -1);
        node->next = NULL;
        node->prev = NULL;
    }

    if (head == NULL)
    {
        head = node;
        tail = node;
    }
    else
    {
        ProcessNode* ptr = head;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;    // old tail of current list
        }
        ptr->next = node;   // new tail after adding new node to list
        node->prev = ptr;   // set the previous node
        tail = node;
    }
}

void ProcessList::remove() 
{
    while (head != NULL)
    {
        ProcessNode* ptr = head;
        head = head->next;
        delete ptr;    
    }
    tail = NULL;
}

void ProcessList::removePid(int pid) 
{
    
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if  (pid == ptr->pid)
        {       
            if ( ptr == head && ptr == tail)
            {
                head = NULL;
                tail = NULL;
            }
            else if (ptr == head) // we need to set new head
            {
                (ptr->next)-> prev = ptr->prev;
                head = ptr->next;
            }
            else if (ptr == tail) // we need to set new tail 
            {
                (ptr->prev)->next = ptr->next;
                tail = ptr->prev;
            }  
            else // if not head nor tail
            {
                (ptr->prev)->next = ptr->next;
                (ptr->next)-> prev = ptr->prev;
            }

            delete ptr;
            return;  
        }

        ptr = ptr->next;
    }
}

void ProcessList::updatePgTable(int pid, int pgNum, int frameNum, bool validFlag) 
{
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
            ptr->pgTable[pgNum] = frameNum;
            ptr->validFlag[pgNum] = validFlag; 
        }
        ptr = ptr->next;
    }
}

int ProcessList::findPgSize(int pid)
{
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
           return ptr->pageSize;
        }
        ptr = ptr->next;
    }
    return -1;
}

int ProcessList::pageLookup(int pid, int pgNum) 
{
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
           return ptr->pgTable[pgNum];
        }
        ptr = ptr->next;
    }
    return -1;
}

bool ProcessList::findPid(int pid)
{
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
           return true;
        }
        ptr = ptr->next;
    }
    return false;
}

ProcessList::ProcessList() 
{
    head = NULL;
    tail = NULL;
}

ProcessList::~ProcessList() 
{
    remove();
}


// -------------------------------------------------------------------------------------//



// LRU Stack list functions
void StackList::insert(int pid, int pageNum) 
{
	StackNode* node = new StackNode;
    if (node == NULL)
    {
        cerr << "MEMORY FULL - no insertion allowed" << endl;
    }
    else
    {
        node->pid = pid;
        node->pageNum = pageNum;
        node->next = head;
        node->prev = NULL;
    }

    if (head != NULL)
    {
       head->prev = node;
    }
    else 
    {
       tail = node;
    }
    node = head;
}

// Print the LRU stack
void StackList::print() 
{
    StackNode* ptr = head;
    while (ptr != NULL)
    {
            cout << ptr->pid << " " << ptr->pageNum << endl;
            ptr = ptr->next;
    }
}


void StackList::remove() 
{
    while (head != NULL)
    {
        StackNode* ptr = head;
        head = head->next;
        delete ptr;    
    }
    tail = NULL;
}


void StackList::removePid(int pid, int pageNum) 
{
	StackNode* ptr = head;
	while (ptr != NULL)
	{
	    if  (pid == ptr->pid && pageNum == ptr->pageNum || ptr == tail)	// if the pid and pageNum are the same then we want to delete that node, or if the ptr is at the tail, we want to delete the tail node b/c that's the least recently used node
	    {       
	        if (ptr == head && ptr == tail)
	        {
	            head = NULL;
	            tail = NULL;
	        }
	        else if (ptr == head) // we need to set new head
	        {
	            (ptr->next)-> prev = ptr->prev;
	            head = ptr->next;
	        }
	        else if (ptr == tail) // we need to set new tail 
	        {
	            (ptr->prev)->next = ptr->next;
	            tail = ptr->prev;
	        }  
	        else // if not head nor tail
	        {
	            (ptr->prev)->next = ptr->next;
	            (ptr->next)-> prev = ptr->prev;
	        }

	        delete ptr;
	        return;  
	    }

	    ptr = ptr->next;
	}
}

int StackList::victimPid(int pid, int pageNum)
{
	StackNode* ptr = head;
	while (ptr != NULL)
	{
	    if  (pid == ptr->pid && pageNum == ptr->pageNum || ptr == tail)	// if the pid and pageNum are the same then we want to delete that node, or if the ptr is at the tail, we want to delete the tail node b/c that's the least recently used node
	    {
	        return ptr->pid;  
	    }

	    ptr = ptr->next;
	}
	
	return -1
}


int StackList::victimPage(int pid, int pageNum)
{
	StackNode* ptr = head;
	while (ptr != NULL)
	{
	    if  (pid == ptr->pid && pageNum == ptr->pageNum || ptr == tail)	// if the pid and pageNum are the same then we want to delete that node, or if the ptr is at the tail, we want to delete the tail node b/c that's the least recently used node
	    {
	        return ptr->pageNum;  
	    }

	    ptr = ptr->next;
	}
	
	return -1
}
    

StackList::StackList() 
{
    head = NULL;
    tail = NULL;
}

StackList::~StackList() 
{
    remove();
}

