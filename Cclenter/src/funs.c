#ifndef KEY_FUNCTIONS_C
#define KEY_FUNCTIONS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

Elist* Arg_List()
{
    KEYPTR *keyp; // eax@3

    Arg_Trav = Arg_Trav->Next;
    if ( Arg_Trav == Arg_End )
    {
        fwrite("expected a list argument, got end of list\n", 1, 42, stderr);
        exit(1);
    }
    keyp = (KEYPTR *)Arg_Trav->UserData;
    if ( keyp->Type != 3 )
    {
        fwrite("expected a sub list\n", 1, 20, stderr);
        exit(1);
    }
    return keyp->u_Ptr.List;
}

void Arg_Action(KEYPTR *a1)
{
    KEYPTR *keyp; // edx@3

	Arg_Trav = Arg_Trav->Next;
    if ( Arg_Trav == Arg_End )
    {
        fwrite("No items left in Arg_Action!!!!\n", 1u, 32u, stderr);
    }
    else
    {
        keyp = (KEYPTR *)Arg_Trav->UserData;
        a1->u_Ptr.Func = keyp->u_Ptr.Func;
        *(int *)&a1->Type = *(int *)&keyp->Type;
    }
}

char *Arg_String(void)
{
    KEYPTR *keyp; // eax@2

	Arg_Trav = Arg_Trav->Next;
    if ( Arg_Trav == Arg_End )
        return 0;
    keyp = (KEYPTR *)Arg_Trav->UserData;          // Points at a KEYPTR structure
    if ( keyp->Type != 2 )                        // String type
    {
        fwrite("function wanted string parameter\n", 1u, 0x21u, stderr);
        return 0;
    }
    return keyp->u_Ptr.Name;
}

int Arg_Digit(void)
{
    KEYPTR *keyp; // eax@2

	Arg_Trav = Arg_Trav->Next;
    if ( Arg_Trav == Arg_End )
        return 0;
    keyp = (KEYPTR *)Arg_Trav->UserData;
    if ( keyp->Type != 4 )                        // Numeric type
    {
        fwrite("function wanted digit parameter\n", 1u, 0x20u, stderr);
        return 0;
    }
    return keyp->u_Ptr.Count;
}

//===========================================

int Exit()
{
    return 0;
}

int Display()
{
    printf("%s", Arg_String());
    return 0;
}

int DisplayMe()
{
    char *v0; // edx@1

    v0 = Arg_String();
    if ( v0 )
        printf(v0, CharRegister);
    else
        putchar(CharRegister);
    return 0;
}

int DoFClear()
{
    AStackPointer = 0;
    return 0;
}

int DoFDup()
{
	int result; // eax@2

    if ( AStackPointer <= 38 ) // Must have at least one empty stack slot
    {
        AStack[AStackPointer].u_Ptr = AStack[AStackPointer - 1].u_Ptr;	// union type
        *(int *)&AStack[AStackPointer].Type = *(int *)&AStack[AStackPointer - 1].Type;
        ++AStackPointer;
        result = 0;
    }
    else
    {
        fwrite("stack over/under flow\n", 1u, 0x16u, stderr);
        result = 1;
    }
    return result;
}

int DoFExch()	// swap top two stack entries
{
    int result; // eax@2
	KEYPTR	save;

    if ( AStackPointer > 1 )
    {
		save.u_Ptr = AStack[AStackPointer].u_Ptr;
        *(int *)&save.Type = *(int *)&AStack[AStackPointer].Type;

        AStack[AStackPointer].u_Ptr = AStack[AStackPointer - 1].u_Ptr;
        *(int *)&AStack[AStackPointer].Type = *(int *)&AStack[AStackPointer - 1].Type;

        AStack[AStackPointer - 1].u_Ptr = save.u_Ptr;
        *(int *)&AStack[AStackPointer - 1].Type = *(int *)&save.Type;

		result = 0;
    }
    else
    {
        fwrite("less than two items on stack\n", 1u, 29u, stderr);
        result = 1;
    }
    return result;
}

int DoFPush()
{
    int result; // eax@2

    if ( AStackPointer <= 39 )
    {
        Arg_Action(&AStack[AStackPointer++]);
        return 0;
    }
    else
    {
        fwrite("stack full", 1u, 10u, stderr);
        return 1;
    }
}

int DoFPop()
{
    if ( AStackPointer )
    {
        --AStackPointer;
        return 0;
    }
    else
    {
        fwrite("Stack empty\n", 1, 12, stderr);
        return 1;
    }
}

int DoFAdd()
{
    KEYPTR *v1; // ecx@3
    KEYPTR *v2; // edx@3

    if ( AStackPointer > 1 )	// at least 2 terms on the stack
    {
        v1 = &AStack[AStackPointer - 1];
        v2 = &AStack[AStackPointer - 2];
        if ( v1->Type == 4 && v2->Type == 4 )
        {
            v2->u_Ptr.Count += v1->u_Ptr.Count;
            --AStackPointer;
            return 0;
        }
		fwrite("maths only supported on digits\n", 1, 31, stderr);
    }
    else
    {
		fwrite("less than two items on stack\n", 1, 29, stderr);
    }
    return 1;
}

int DoFDiv()
{
    KEYPTR *v1; // ecx@3
    KEYPTR *v2; // edx@3

    if ( AStackPointer > 1 )	// at least 2 terms on the stack
    {
        v1 = &AStack[AStackPointer - 1];
        v2 = &AStack[AStackPointer - 2];
        if ( v1->Type == 4 && v2->Type == 4 )
        {
            v2->u_Ptr.Count /= v1->u_Ptr.Count;		// no test for divide / zero error here?
            --AStackPointer;
            return 0;
        }
		fwrite("maths only supported on digits\n", 1, 31, stderr);
    }
    else
    {
		fwrite("less than two items on stack\n", 1, 29, stderr);
    }
    return 1;
}

int DoFMul()
{
    KEYPTR *v1; // ecx@3
    KEYPTR *v2; // edx@3

    if ( AStackPointer > 1 )	// at least 2 terms on the stack
    {
        v1 = &AStack[AStackPointer - 1];
        v2 = &AStack[AStackPointer - 2];
        if ( v1->Type == 4 && v2->Type == 4 )
        {
            v2->u_Ptr.Count *= v1->u_Ptr.Count;
            --AStackPointer;
            return 0;
        }
		fwrite("maths only supported on digits\n", 1, 31, stderr);
    }
    else
    {
		fwrite("less than two items on stack\n", 1, 29, stderr);
    }
    return 1;
}

int DoFSub()
{
    KEYPTR *v1; // ecx@3
    KEYPTR *v2; // edx@3

    if ( AStackPointer > 1 )	// at least 2 terms on the stack
    {
        v1 = &AStack[AStackPointer - 1];
        v2 = &AStack[AStackPointer - 2];
        if ( v1->Type == 4 && v2->Type == 4 )
        {
            v2->u_Ptr.Count -= v1->u_Ptr.Count;
            --AStackPointer;
            return 0;
        }
		fwrite("maths only supported on digits\n", 1, 31, stderr);
    }
    else
    {
		fwrite("less than two items on stack\n", 1, 29, stderr);
    }
    return 1;
}

int DoFPushK()
{
    int result; // eax@2

    if ( AStackPointer <= 39 )
    {
        AStack[AStackPointer].Type = 4;	// indicate digit value
        AStack[AStackPointer++].u_Ptr.Count = CharRegister;
        result = 0;	// exit_success
    }
    else
    {
        fwrite("stack full", 1u, 10u, stderr);
        result = 1;
    }
    return result;
}

int DoSDisplayf()
{
    char	*v1;
	KEYPTR	*v3;
	int		v2;
	int		v4;
	
    if ( !AStackPointer )
    {
        fwrite("Stack empty\n", 1, 12, stderr);
        return 1;
    }
    v1 = Arg_String();
    if ( !v1 )
    {
        fwrite("Displayf needs a formatter string\n", 1, 34, stderr);
        return 1;
    }
    v2 = AStackPointer - 1;
    AStackPointer = v2;
    v3 = &AStack[v2];
    v4 = v3->Type;
    if ( v4 != 2 && v4 != 4 )
    {
        fprintf(stderr, "Bad Displayf type %d\n", v3->Type);
        return 1;
    }
    printf(v1, v3->u_Ptr.Count);	// careful, Value could be int or string
    return 0;
}

int DoFPstack()
{
    int i; // esi@1
    KEYPTR *keyp; // eax@2

    for ( i = 0; i < AStackPointer; ++i )
    {
        keyp = &AStack[i];
		switch (keyp->Type)
		{
			case 2:
		        printf("s - %s\n", keyp->u_Ptr.Name);
				break;

			case 4:
		        printf("d - %d\n", keyp->u_Ptr.Count);
				break;
			
			default:
                fprintf(stderr, "Bad Displayf type %d\n", keyp->Type);
                return 1;
		}
    }
    return 0;
}

int DoSUngetkey()
{
    int v1; // edx@3
    KEYPTR *v2; // edx@3

    if ( !AStackPointer )
    {
        fwrite("Stack empty\n", 1, 12, stderr);
        return 1;
    }
    v1 = AStackPointer - 1;
    AStackPointer = v1;
    v2 = &AStack[v1];
    if ( v2->Type != 4 )
    {
        fwrite("can only push back digits\n", 1, 26, stderr);
        return 1;
    }
    if ( PushBackChar )
    {
		fwrite("too many push backs!\n", 1, 21, stderr);
		return 1;
    }
    PushBackChar = v2->u_Ptr.Count;
    return 0;
}

int DoStoN()
{
    int v0; // edx@1
    KEYPTR *v1; // eax@2
    int result; // eax@3
    char nptr[40]; // [sp+10h] [bp-28h]@4

    v0 = 0;
    if ( AStackPointer <= 0 )
    {
LABEL_5:
        nptr[v0] = 0;
        AStack[0].Type = 4;
        AStack[0].u_Ptr.Count = strtol(nptr, 0, 10);
        AStackPointer = 1;
        result = 0;
    }
    else
    {
        while ( 1 )
        {
            v1 = &AStack[v0];
            if ( v1->Type != 4 )
                break;
            nptr[v0++] = (char)(v1->u_Ptr.Count & 0xFF);
            if ( v0 >= AStackPointer )
                goto LABEL_5;
        }
        result = 1;
    }
    return result;
}

int DoSetRetVal()
{
    KEYPTR *v1; // eax@3

    if ( !AStackPointer )
    {
        fwrite("Stack empty\n", 1, 12, stderr);
        return 1;
    }
    --AStackPointer;
    v1 = &AStack[AStackPointer];
    if ( v1->Type != 4 )
    {
        fwrite("can only return digits\n", 1, 23, stderr);
        return 1;
    }
    GlobalReturnValue = v1->u_Ptr.Count;
    return 0;
}

int GetStackDigit(void)
{
    KEYPTR *v1; // eax@3

	if ( !AStackPointer )
    {
        fwrite("GetStackDigit Stack empty\n", 1, 26, stderr);
        return 0;
    }
    --AStackPointer;
    v1 = &AStack[AStackPointer];
    if ( v1->Type != 4 )                        // 4 indicates a numeric value (int) for stValue
    {
        fwrite("GetStackDigit can only return digits\n", 1, 37, stderr);
        return 0;
    }
    return v1->u_Ptr.Count;
}

/*	-- Not called in clenter!
char* GetStackString()
{
    KEYPTR *v0; // eax@2

    if ( !AStackPointer )
        return 0;
    --AStackPointer;
    v0 = &AStack[AStackPointer];
    if ( v0->Type != 2 )
    {
        fwrite("GetStackString can only return strings\n", 1u, 0x27u, stderr);
        return 0;
    }
    return v0->u_Ptr.Name;
}
*/

int DoPushKeyName()
{
    char *KeyName; // eax@1
    int result; // eax@2
    int v3; // edi@3

    KeyName = Arg_String();
    if ( AStackPointer <= 39 )
    {
        v3 = KeyToNumber(KeyName);
        if ( !v3 )
            fprintf(stderr, "'%s' is invalid key name\n", KeyName);
        AStack[AStackPointer].Type = 4;
        AStack[AStackPointer++].u_Ptr.Count = v3;
        result = 0;
    }
    else
    {
        fwrite("stack full", 1u, 10u, stderr);
        result = 1;
    }
    return result;
}

//===========================================

int DoMacro(KEY_Elist *a1)
{
    Elist *v1; // edi@1
    Elist_Elem *i; // esi@1

    v1 = Arg_List();
    a1->field_4->Current = a1->field_4->Prev;
    for ( i = v1->Next; (Elist *)i != v1; i = i->Next )
        elist_Add(a1->field_4, 0, (void *)((KEY_Elem*)i->UserData)->lx_CRstart);
    return 1;
}

int DoMacroAdd(KEY_Elist *a1)
{
    int v1; // eax@1

    a1->field_4->Current = a1->field_4->Prev;
    v1 = GetStackDigit();
    elist_Add(a1->field_4, 0, (void *)v1);
    return 1;
}

int DoMacroStr(KEY_Elist *KL)
{
    char *i; // esi@1

    //KL->field_4->Current = KL->field_4->field_8;
    //for ( i = Arg_String(); *i; ++i )
    //    elist_Add(KL->field_4, 0, (void *)*i);  // add each character of the string into the buffer
    return 1;
}

#endif
