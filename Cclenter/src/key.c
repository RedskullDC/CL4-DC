#ifndef KEYS_C
#define KEYS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

int key_node(void *ptr)
{
    mfree_0(ptr);
    return 0;
}

void Key_Print(int DispChar, int NumTabs)
{
    int v2; // esi@1

    v2 = NumTabs - 1;
    if ( NumTabs )
    {
        do
        {
            _IO_putc('\t', stdout);
            --v2;
        }
        while ( v2 != -1 );
    }
    if ( DispChar <= 1999 )
    {
        switch ( DispChar )
        {
            case 8:
                printf("'\\b'");	// BELL?
                break;
            case 9:
                printf("'\\t'");	// <TAB>
                break;
            case 27:
                printf("'\\e'");	// <ESC>
                break;
            case 12:
                printf("'\\f'");	// form feed
                break;
            case 10:
                printf("'\\n'");	// NL
                break;
            case 13:
                printf("'\\r'");	// <RET>
                break;
            case 32:
                printf("'\\s'");	// <SPC>
                break;
            default:
                //if ((*__ctype_b_loc())[DispChar] & 0x4000 )	// isprint()
                if (isprint(DispChar))	// isprint()
				    printf("'%c'", DispChar);
                else
				{
					if ( DispChar > 31 )
						printf("%d", DispChar);			// show as keynumber
					else
		 			    printf("'^%c'", DispChar + 64);	// control chars
                }
				break;
        }
    }
	else	// cl4 virtual key. Use name instead
	{
		printf("%s", KeyToName(DispChar));
	}
}

int keys_lgetchar(void)
{
    int v0; // esi@1

    v0 = _IO_getc(stdin);
    if ( Key_Debug )
    {
        Key_Print(v0, 0);
        _IO_putc('\n', stdout);
    }
	//eprint("keys_lgetchar returning %C\n",v0);
    return v0;
}

int keys_getchar(void)
{
    int v0; // esi@1

    v0 = KeyF_GetChar(kt_keys);	// This eventually calls through keys_lgetchar() to _IO_getc()
    if ( Key_Debug )
    {
        Key_Print(v0, 1);
        _IO_putc('\n', stdout);
    }
    return v0;
}

int Key_GetChar(void)
{
    return KeyF_GetChar(kt_macros);
}

int Key_Ready()
{
    return KeyF_Ready(kt_macros);
}

void Key_Construct(void)
{
    char *KeyDefFileName; // esi@1
    char *v1; // eax@1
    const char *v3; // eax@4
    char *MacroDefFileName; // esi@12

    KeyDefFileName = 0;
    v1 = getevar("KEYDEFS");
    if ( v1 && *v1 )
    {
		//printf("Key_Construct : KEYDEFS = %s\n",v1);
        KeyDefFileName = mstrcpy(getdirname(v1), 0);
        if ( Key_Debug )
			printf("KEYDEFS:  [%s]\n", KeyDefFileName);
    }
    else
    {
		//printf("Key_Construct : KEYDEFS = <null>!!\n");
        v1 = getevar("KEYINFO");
        if ( v1 && *v1 )
        {
			//printf("Key_Construct : KEYINFO = %s\n",v1);
            KeyDefFileName = mstrcpy(getdirname(v1), "/", getenv("TERM"), 0);
            if ( Key_Debug )
				printf("KEYINFO:  [%s]\n", KeyDefFileName);
        }
        else
        {
            if ( Key_Debug )
                puts("KEYINFO:  undefined");
        }
    }

	
	//printf("Key_Construct : KeyDefFileName = %s\n",KeyDefFileName);
	//printf("Key_Construct : Calling KeyF_Construct on kt_keys\n");

	kt_keys = KeyF_Construct(KeyDefFileName, keys_lgetchar, FTable);
    MacroDefFileName = 0;
    v1 = getevar("KEYMACRO");
    if ( v1 && *v1 )
    {
        MacroDefFileName = mstrcpy(getdirname(v1), 0);
        if ( Key_Debug )
            printf("KEYMACRO: [%s]\n", MacroDefFileName);
    }
    else
    {
        if ( Key_Debug )
            puts("KEYMACRO: udefined");
    }
	//printf("Key_Construct : Calling KeyF_Construct on kt_macros\n");
    kt_macros = KeyF_Construct(MacroDefFileName, keys_getchar, FTable);
}

// dumpNodes

void key_trav_keys(Elist *head)
{
    Elist_Elem *v1; // eax@1
    KEY_Elem *kelem; // esi@2
    KEYPTR *KeyPtr; // eax@4

    v1 = head->Next;
    head->Current = v1;
    if ( (Elist *)v1 != head )
    {
        do
        {
            kelem = head->Current->UserData;
            if ( kelem->lx_elist )
            {
                kelem->lx_elist->Current = kelem->lx_elist->Next;
                while ( kelem->lx_elist->Current != (Elist_Elem *)kelem->lx_elist )
                {
                    KeyPtr = (KEYPTR *)kelem->lx_elist->Current->UserData;
                    
					if ( KeyPtr->Type == 3 )    // KeyPtr contains a sub-elist
                        key_trav_keys((Elist *)KeyPtr->u_Ptr.List);// iterate over any sub-lists. Nice recursion here.
					else if ( KeyPtr->Type == 2 )// KeyPtr contains a string pointer
						mfree_0(KeyPtr->u_Ptr.Name);

					// KeyPtr-Type == 1 is only an integer, no extra mem allocated
                    elist_Next(kelem->lx_elist);
                }
                elist_Destruct(kelem->lx_elist, key_node);// key_node() just frees the userdata memory
            }
            elist_Next(head);
        }
        while ( (Elist *)head->Current != head );// reached end of the list
    }
    elist_Destruct(head, key_node);
    //--indent_0;	??
}

void Key_Destruct(void)
{
    KEY_Elist *key_el; // esi@1
    KEY_Elist *key_el2; // esi@9

    key_el = kt_keys;
    if ( kt_keys->field_0 )
        key_trav_keys(kt_keys->field_0);
    if ( key_el->field_8 )
        elist_Destruct(key_el->field_8, key_node);
    if ( key_el->field_4 )
        key_trav_keys(key_el->field_4);
    if ( key_el->FileName )
        mfree_0(key_el->FileName);
    mfree_0(key_el);                            // 
                                                // 
                                                // 
    key_el2 = kt_macros;
    if ( kt_macros->field_0 )
        key_trav_keys(kt_macros->field_0);
    if ( key_el2->field_8 )
        elist_Destruct(key_el2->field_8, key_node);
    if ( key_el2->field_4 )
        key_trav_keys(key_el2->field_4);
    if ( key_el2->FileName )
        mfree_0(key_el2->FileName);
    mfree_0(key_el2);
}

int Key_Lookup(int KeyNo)
{
    Elist_Elem	*v1;
    KEY_Elem	*kelem;

    if ( kt_macros && kt_macros->field_0 && (v1 = kt_macros->field_0->Next, (Elist *)v1 != kt_macros->field_0) )
    {
        while ( 1 )
        {
            kelem = (KEY_Elem*)v1->UserData;	// 
            if ( KeyNo == kelem->lx_keyno )
                break;
            v1 = v1->Next;
            if ( (Elist *)v1 == kt_macros->field_0 )	// end of list or list empty
                return -1;
        }
        return kelem->lx_CRstart;				// exit_success!
    }
    else
		return -1;
}

KEY_Elist *KeyF_Construct(char *FileName, int (*ReadFunction)(void), KEYFUNC *KeyFns)
{
    KEY_Elist	*key_el;
    KEY_Elem	*KeyElem;
	Elist		*v6;
	Elist		*KeyElist;
	
    key_el = (KEY_Elist *)mmalloc(20u);
    KeyElist = elist_Create();
	
	//DumpBlock(KeyElist, sizeof(Elist));

	KeyElem = (KEY_Elem *)mmalloc(20u);
    KeyElem->lx_CRstart = 0xFFu;
    KeyElem->lx_CRend	= 0xFFu;
    KeyElem->lx_elist	= elist_Create();
    elist_Add(KeyElist, 0, KeyElem);

	v6 = 0;
    if ( FileName )
	{
		//printf("KeyF_Construct : Calling KeyF_ReadFile(%s) \n", FileName);
        v6 = KeyF_ReadFile(KeyElist, FileName, KeyFns);
	}
    key_el->field_0 = v6;
    key_el->field_8 = KeyElist;
    key_el->field_4 = elist_Create();
    key_el->FileName = FileName;
    key_el->RdFunc = ReadFunction;
    return key_el;
}

Elist* KeyF_ReadFile(Elist *KeyElist, char *FileName, KEYFUNC *a3)
{
    FILE	*stream;
	Elist	*result;
	
    stream = fopen64(FileName, "r");
    if ( stream )
    {
		//printf("KeyF_ReadFile : Calling ReadKeyList() \n");
        ReadKeyList(KeyElist, stream, a3, FTable);
        fclose(stream);
		//printf("KeyF_ReadFile : returned from ReadKeyList() \n");

		//DumpBlock(KeyElist, sizeof(Elist));
		//DumpBlock(KeyElist->Next, sizeof(Elist_Elem));
		//DumpBlock(KeyElist->Next->UserData, sizeof(KEY_Elem));
		//DumpBlock(((KEY_Elem*)KeyElist->Next->UserData)->lx_elist, sizeof(Elist));
		//DumpBlock((((KEY_Elem*)KeyElist->Next->UserData)->lx_elist)->Prev, sizeof(Elist_Elem));
		//DumpBlock( (KEYPTR*)(((KEY_Elem*)KeyElist->Next->UserData)->lx_elist)->Prev->UserData, sizeof(KEYPTR));	// First KEYPTR is always an Elist type!
		//DumpBlock( ((KEYPTR*)(((KEY_Elem*)KeyElist->Next->UserData)->lx_elist)->Prev->UserData)->u_Ptr.List, sizeof(Elist));


        //result = KeyElist	->field_C	->field_4	->field_C	->field_8	->field_4	->Size;
		//**** NEED TO FIX THIS ****//
		//result = 0;
//                  Elist   Elist_Elem   KEY_Elem*	   Elist	Elist_Elem	KEYPTR*		 ????
//        result = KeyElist	->Next		->UserData	->lx_elist	->Prev		->UserData	->field_0;

		result = ((KEYPTR*)(((KEY_Elem*)KeyElist->Next->UserData)->lx_elist)->Prev->UserData)->u_Ptr.List;	// Elist*
		//printf("KeyF_ReadFile : x%08X\n", result);
		//result = 0;
    }
    else
    {
        fwrite("KeyF_ReadFile(): ", 1u, 17u, stderr);
        perror(FileName);
        result = 0;
    }
    return result;
}

int KeyF_GetChar(KEY_Elist *KeyEL)
{
    Elist *v2; // edx@3
    void *v3; // edi@4

    if ( !KeyEL->field_0 )	// ZERO if no keydef file could be read.
        return KeyEL->RdFunc();		// Just calls keys_lgetchar();

    v2 = KeyEL->field_4;
	//printf("KeyF_GetChar : v2->Size =  %d\n",v2->Size);
    if ( v2->Size )
    {
        v2->Current = v2->Next;
        v3 = KeyEL->field_4->Current->UserData;	// UserData is just a character in this case
        elist_Delete(KeyEL->field_4);
    }
    else
    {
        v3 = (void*)process(KeyEL, KeyEL->field_0);	// *** NEED TO CHECK ***
        if ( KeyEL->field_4->Size )
            return KeyF_GetChar(KeyEL);         // Some nice recursion here.
    }
    return (int)v3;	// v3 is actually a character (int)
}


int KeyF_Ready(KEY_Elist *el)
{
    return el->field_4->Size;
}

#endif
