#ifndef PROCESS_C
#define PROCESS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

int DoActions(KEY_Elist *KeyEL, Elist *lx_el)
{
    Elist_Elem *i; // esi@1
    KEYPTR *KeyPtr; // edx@2
    int v5; // [sp+18h] [bp-10h]@1

    v5 = -1;
    for ( i = lx_el->Next; (Elist *)i != lx_el; i = i->Next )
    {
        KeyPtr = (KEYPTR *)i->UserData;
        if ( KeyPtr->Type == 3 )                // holds an Elist*
        {
            v5 = process(KeyEL, KeyPtr->u_Ptr.List);	// recursive here!     KeyPtr->u_Ptr.List (Elist*)
        }
        else
        {
            if ( KeyPtr->Type == 1 )            // Holds a function pointer: int(*)(void)
            {
                Arg_Trav = i;
                Arg_End = (Elist_Elem *)lx_el;
                //((void (__cdecl *)(_DWORD))KeyPtr->u_Ptr.Func)(KeyEL);
                ((int (*)(KEY_Elist*))KeyPtr->u_Ptr.Func)(KeyEL);			///  **** need to check this ***
                i = Arg_Trav;
            }
        }
    }
    return v5;
}

int process(KEY_Elist *KeyEL, Elist *a2)
{
    Elist_Elem *v2;
	KEY_Elem *kelem;
	signed int v4;
	signed int v5;
	int		v7;
	int		v9;
	int		v10;
	int		v11;
	int		v12;
	
    v10 = -3;
    v11 = 0;
    do
    {
        if ( PushBackChar )
        {
            v12 = PushBackChar;
            CharRegister = PushBackChar;
            PushBackChar = 0;
        }
        else
        {
            v12 = KeyEL->RdFunc();
            CharRegister = v12;
        }
//--------------------------------------------
        v2 = a2->Next;
        v9 = 0;
        if ( (Elist *)v2 != a2 )
        {
            while ( 1 )
            {
                kelem = v2->UserData;
                v4 = kelem->lx_KeyType;
                if ( kelem->lx_KeyType == 2 )
                {
                    if ( v12 < kelem->lx_CRstart || v12 > kelem->lx_CRend )
                        goto LABEL_26;
                }
                else
                {
                    if ( v4 <= 2 )
                    {
                        if ( v4 != 1 || kelem->lx_CRstart != v12 )
                            goto LABEL_26;
                        v5 = DoActions(KeyEL, kelem->lx_elist);
                        if ( kelem->lx_keyno )
						{
							++v11;
							if ( GlobalReturnValue == -1 )
							{
							    if ( kelem->lx_keyno == -1 )
							        v10 = v5;
							    else
							        v10 = kelem->lx_keyno;
							}
							else
							{
								v10 = GlobalReturnValue;
								GlobalReturnValue = -1;
							}
						}
						goto LABEL_25;
                    }
                    if ( v4 != 3 )
                        goto LABEL_26;
                }
                v5 = DoActions(KeyEL, kelem->lx_elist);
                if ( kelem->lx_keyno )
                {
                    ++v11;
                    if ( GlobalReturnValue == -1 )
					{
						if ( kelem->lx_keyno == -1 )
							v10 = v5;
						else
							v10 = kelem->lx_keyno;
					}
					else
					{
						v10 = GlobalReturnValue;
						GlobalReturnValue = -1;
					}
                }
LABEL_25:
                ++v9;
LABEL_26:
                v2 = v2->Next;
                if ( (Elist *)v2 == a2 )
                    break;
                if ( v9 )
                    goto LABEL_33;
            }
        }
        if ( !v9 )
        {
            v7 = v11 + 1;
            if ( (Elist *)v2 != a2 )
                v7 = v11;
            v11 = v7;
        }
LABEL_33:
        ;
    }
    while ( !v11 );
    return v10;
}

Elist *ReadKeyList(Elist *ListHhead, FILE *stream, KEYFUNC *a2, KEYFUNC *KfuncArr)
{
    KEY_Elem *lxelem; // esi@2
    KEYPTR *v5; // edi@6

    int (*v6)(KEY_Elist*); // eax@6
    void *v9; // eax@10
    Elist *v11; // esi@12
    Elist *v14; // [sp+0h] [bp-28h]@2

    while ( 1 )
    {
        switch ( Lex(stream) + 1 )
        {
            default:
				//printf("ReadKeyList case:default\n");
                continue;
            case 4:
				//printf("ReadKeyList case:4, New element added. Lex_KeyType = %d, Lex_CommRStart = %c, Lex_CommREnd = %c \n", Lex_KeyType, Lex_CommRStart, Lex_CommREnd);
				//lxelem				= (KEY_Elem *)mmalloc(20u);
				lxelem				= (KEY_Elem *)mmalloc(sizeof(KEY_Elem));
                lxelem->lx_KeyType	= Lex_KeyType;
                lxelem->lx_CRstart	= Lex_CommRStart;
                lxelem->lx_CRend	= Lex_CommREnd;
                lxelem->lx_keyno	= 0;
                lxelem->lx_elist	= elist_Create();

                elist_Add(ListHhead, 0, lxelem);
                break;
            case 9:
				//printf("ReadKeyList case:9, , Lex_CommRStart = %c (%4d)\n", Lex_CommRStart, Lex_CommRStart);
                //*((int *)ListHhead->Prev->UserData + 4) = Lex_CommRStart;
				((KEY_Elem *)ListHhead->Prev->UserData)->lx_keyno = Lex_CommRStart;	// This is the KeyNo that starts the sequence
                continue;
            case 5:                             // function name is returned.
                if ( !a2 )
                {
                    fwrite("Function not allowed in key file\n", 1u, 0x21u, stderr);
                    exit(1);
                }
				//printf("ReadKeyList case:5, FN name = %s\n",Lex_CommFunctionName);
                v5 = (KEYPTR *)mmalloc(sizeof(KEYPTR));
                v5->Type = 1;  // signify that first field is a function pointer
                v6 = Function_Bind(Lex_CommFunctionName, a2);// a2 and KfuncArr are both set to global FTable[] by caller
                v5->u_Ptr.Func = v6;
                if ( !v6 )
                {
                    v6 = Function_Bind(Lex_CommFunctionName, KfuncArr);
                    v5->u_Ptr.Func = v6;
                    if ( !v6 )
                    {
                        fprintf(stderr, "Unknown function '%s'\n", Lex_CommFunctionName);
                        exit(1);
                    }
                }
				v14 = ((KEY_Elem *)ListHhead->Prev->UserData)->lx_elist;		// Add to the sub-elist
                elist_Add(v14, 0, v5);
                break;
            case 6:                             // Lex returned 5. String Value
				//printf("ReadKeyList case:6, Lex_CommString = %s\n",Lex_CommString);
                v5 = (KEYPTR *)mmalloc(sizeof(KEYPTR));
                v5->Type = 2;
                v5->u_Ptr.Name = mstrcpy(Lex_CommString, 0);// string pointer type
				v14 = ((KEY_Elem *)ListHhead->Prev->UserData)->lx_elist;		// Add to the sub-elist
                elist_Add(v14, 0, v5);
                break;
            case 7:                             // Lex returned 6. Numeric Value
				//printf("ReadKeyList case:7, Lex_CommDigit = %d\n",Lex_CommDigit);
                v5 = (KEYPTR *)mmalloc(sizeof(KEYPTR));
                v5->Type = 4;
                v5->u_Ptr.Count = Lex_CommDigit; // int type
				v14 = ((KEY_Elem *)ListHhead->Prev->UserData)->lx_elist;		// Add to the sub-elist
                elist_Add(v14, 0, v5);
                break;
            case 2:                             // Lex returned 1.  "{" was detected, which means a sub-array of keys.
				//printf("ReadKeyList case:2, Adding a new sub-list\n");
                v11 = elist_Create();
                v5 = (KEYPTR *)mmalloc(sizeof(KEYPTR));
                v5->Type = 3;					// Elist* type
                v5->u_Ptr.List = ReadKeyList(v11, stream, a2, KfuncArr);		// *** recursion here ***
				v14 = ((KEY_Elem *)ListHhead->Prev->UserData)->lx_elist;		// Add to the sub-elist
                elist_Add(v14, 0, v5);
                break;
            case 0:
            case 3:					// '}' found. end of sub-list
                return ListHhead;
        }
    }
}


#endif
