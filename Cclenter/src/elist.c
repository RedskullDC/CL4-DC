#ifndef ELIST_C
#define ELIST_C

#include "DBdefs.h"
#include "cl4.h"

Elist *elist_Create(void)
{
    Elist *result; // eax@1

//    result			= (Elist *)mmalloc(16u);    // create a blank elist header item
    result			= (Elist *)mmalloc(sizeof(Elist));    // create a blank elist header item
    result->Next	= (Elist_Elem *)result;     // All items point to itself.
    result->Prev	= (Elist_Elem *)result;
    result->Current = (Elist_Elem *)result;
    result->Size	= 0;
    return result;
}

void elist_Add(Elist *head, int BeforeCurrent, void *UserElement)
{
    Elist_Elem *CurrElem;
    Elist_Elem *NewEL;

    CurrElem = head->Current;
//    NewEL = (Elist_Elem *)mmalloc(16u);
    NewEL = (Elist_Elem *)mmalloc(sizeof(Elist_Elem));
    ++head->Size;                               // Bump Number of entries counter
    if ( BeforeCurrent )                        // insert new element before the current element
    {
        NewEL->Next = CurrElem;
        NewEL->Prev = CurrElem->Prev;
        CurrElem->Prev->Next = NewEL;
        CurrElem->Prev = NewEL;
    }
    else                                        // insert *after* the current element
    {
        NewEL->Prev = CurrElem;
        NewEL->Next = CurrElem->Next;
        CurrElem->Next->Prev = NewEL;
        CurrElem->Next = NewEL;
    }
    NewEL->UserData = UserElement;              // insert the User Data structure into the newly created element
    head->Current	= NewEL;					// Update header list current value to point at newly created element
}

void elist_Delete(Elist *Root)
{
    Elist_Elem *CurrElem;
	Elist_Elem *NextElem;
	Elist_Elem *PrevElem;
	
    CurrElem = Root->Current;
    if ( (Elist *)CurrElem != Root )            // Does "current" entry point back at headeritself? If so, list is empty
    {
        --Root->Size;                           // decrement number of entries in list
        NextElem = CurrElem->Next;
        PrevElem = CurrElem->Prev;
        PrevElem->Next = NextElem;              // fix lingages to remove current node
        NextElem->Prev = PrevElem;
        mfree_0(CurrElem);                      // free memory occupied by current node
        
		if ( (Elist *)NextElem != Root || NextElem->Next == NextElem )// if (list is not empty || list has just 1 elem(?))
            Root->Current = NextElem;
        else
            NextElem->UserData = NextElem->Next;// not sure what this does yet
    }
}

int elist_Next(Elist *head)
{
    Elist_Elem *CurrElem;

    CurrElem = head->Current;
    if ( (Elist *)CurrElem != head )            // Does "current" entry point back at headeritself? If so, list is empty
    {
        head->Current = CurrElem->Next;         // spin around the loop
        return 1;								// head-Current points at valid entry
    }
    return 0;									// list is empty!
}

int elist_Traverse(Elist *head, int (*applyfunc)(void *))
{
    Elist_Elem *CurrElem;

    CurrElem = head->Next;
    if ( (Elist *)CurrElem == head )            // List is empty if it points back at itself
		return 0;								// list empty
    else
    {
        while ( !applyfunc(CurrElem->UserData) )// Allows user supplied function to operate on user supplied data ! :o)
        {
            CurrElem = CurrElem->Next;
            if ( (Elist *)CurrElem == head )    // reached end of list if ==
                return 0;						// reached end of list with no entry which satisfied the function!
        }
        head->Current = CurrElem;
        return 1;								// head-Current points at valid entry
    }
}

void elist_Destruct(Elist *head, int (*func)(void *))
{
    elist_Traverse(head, func);					// Apply customs user function against all entries in the list
    head->Current = head->Prev;
    do
        elist_Delete(head);
    while ( (Elist *)head->Current != head );
    mfree_0(head);                              // delete head record itself
}

#endif



