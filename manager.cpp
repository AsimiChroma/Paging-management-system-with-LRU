
#include <vector>
#include <iostream>
#include "manager.h"

using namespace std;

ProcessList processList;
FreeList freeFrameList;
StackList stackList;
vector<int> g_physMem;
int pageSize;


void delMemFrame(int frame)
{
    for (int i = frame*pageSize; i < frame*pageSize+pageSize; i++)  // set i to frame * pageSize and keep deleting each frame until specified
    {
        g_physMem[i] = 0;
    }
}

void printMemory()
{
    cout << "Global Memory: " << endl;
    for (int i = 0; i < g_physMem.size(); i++)
    {
        cout << g_physMem[i] << endl;
    }
}

void memMngr(int memSize, int frameSize) // memSize = # of frames in physical memory
{
    g_physMem.resize(memSize*frameSize, 0); 
    freeFrameList.init(memSize);
    pageSize = frameSize;   // set the pageSize here
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
            // --- LRU Implementation --- //
   			// Find the victim pid and pagenum so we can store it before deleting it
   			int victimPage = stackList.victimPage(pid, i);
   			int victimPid = stackList.victimPid(pid, i);
   			stackList.removePid(pid, i);
   			processList.updatePgTable(victimPid, victimPage, -1, false);	// set page table to be false b/c we delete it
   			int frame = processList.pageLookup(victimPid, victimPage);
            delMemFrame(frame);   // delete the value inside that frame in the physical memory to free up phys memory
   			freeFrameList.updateFrame(frame, true);
   		}
        stackList.insert(pid, i);   // insert pagenum onto the top of the stack
        int free = freeFrameList.findFrame();

		processList.updatePgTable(pid, i, free, true);	// now new framenum can be inserted to top of stack
		freeFrameList.updateFrame(free, false);    // new framenum inserted here and free is set to false
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
        if (processList.checkValid(pid, logi_addr))
        {
             // remove least recently used   
   			stackList.removePid(pid, logi_addr);
            
            stackList.insert(pid, logi_addr);   // insert onto the top of the stack
        }
        else 
        {
            // apply LRU
            int victimPage = stackList.victimPage(pid, logi_addr);
   			int victimPid = stackList.victimPid(pid, logi_addr);

            // remove least recently used   
   			stackList.removePid(pid, logi_addr);
   			processList.updatePgTable(victimPid, victimPage, -1, false);	// set page table to be false b/c we delete it
   			int frame = processList.pageLookup(victimPid, victimPage);
            delMemFrame(frame);   // delete the value inside that frame in the physical memory to free up phys memory
   			freeFrameList.updateFrame(frame, true);
            
            // insert newly used
            stackList.insert(pid, logi_addr);   // insert onto the top of the stack
		    processList.updatePgTable(pid, logi_addr, freeFrameList.findFrame(), true);	// now new framenum can be inserted to top of stack
		    freeFrameList.updateFrame(freeFrameList.findFrame(), false);    // new framenum inserted here and free is set to false             
        }
        int frameNum = processList.pageLookup(pid, logi_addr);
        g_physMem[frameNum*pageSize] = 1;    // writes '1' to the frame number - assume first value in page
        cout << frameNum << endl;
        printMemory();
        return 1;
    }
    else
    {
        return -1;
    }
}

int read(int pid, int logi_addr)
{
    if (processList.findPid(pid) && processList.findPgSize(pid) > logi_addr && logi_addr >= 0)
    {
        if (processList.checkValid(pid, logi_addr))
        { 
   			stackList.removePid(pid, logi_addr);
            
            stackList.insert(pid, logi_addr);   // insert onto the top of the stack
        }
        else 
        {
            // apply LRU
            int victimPage = stackList.victimPage(pid, logi_addr);
   			int victimPid = stackList.victimPid(pid, logi_addr);

            // remove least recently used   
   			stackList.removePid(pid, logi_addr);
   			processList.updatePgTable(victimPid, victimPage, -1, false);	// set page table to be false b/c we delete it
   			int frame = processList.pageLookup(victimPid, victimPage);
            delMemFrame(frame);   // delete the value inside that frame in the physical memory to free up phys memory
   			freeFrameList.updateFrame(frame, true);
            
            // insert newly used
            stackList.insert(pid, logi_addr);   // insert onto the top of the stack
		    processList.updatePgTable(pid, logi_addr, freeFrameList.findFrame(), true);	// now new framenum can be inserted to top of stack
		    freeFrameList.updateFrame(freeFrameList.findFrame(), false);    // new framenum inserted here and free is set to false        
        }
        int frameNum = processList.pageLookup(pid, logi_addr);
        // cout << "framenum" << frameNum << endl;
        // cout << g_physMem[frameNum*pageSize] << endl;
        return g_physMem[frameNum*pageSize]; // reads and returns the value of that frame - assume first value in page
                        // remove least recently used  
    }
    else
    {
        return -1;
    }     
}


void printMem(void)
{
    freeFrameList.print();
    processList.print();
    stackList.print();
    // printMemory();
    //cout << g_physMem[1] << endl;
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
    cout << "Free Frame List: " << endl;
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
    cout << "Process List: " << endl;
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
        node->validFlag.resize(pageSize, 1);    // new page marked as valid in page table so value set to 1 for true
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

 bool ProcessList::checkValid (int pid, int pgNum) 
 {
    ProcessNode* ptr = head;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
        {
           return ptr->validFlag[pgNum];
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
    head = node;    // head is the most recently used
}

// Print the LRU stack
void StackList::print() 
{
    StackNode* ptr = head;
    cout << "Stack: " << endl;
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
	
	return -1;
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
	
	return -1;
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
