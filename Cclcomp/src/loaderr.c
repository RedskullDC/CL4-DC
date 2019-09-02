#ifndef LOADERR_C
#define LOADERR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"

typedef struct 
{
	bool	ShowArg;
	char	*ErrorMsg;
}EMSG;

EMSG	emsgs[] = 
{0, "\t",													// 0
 1, "invalid in block construct",
 0, "missing block name",
 1, "invalid date",
 1,	"invalid data type",
 1,	"endif without if",										// 5
 0,	"unexpected end of file",
 0,	"unexpected end of line",
 1, "invalid field name or number",
 1, "unexpected elsif",
 1, "invalid in \'if\' or \'while\' construct",				// 10
 1, "label not found",
 1, "too many labels",
 1, "too many nested if/whiles",
 1, "table not open",
 1, "invalid operator",										// 15
 1, "duplicate label",
 1, "variable redefined",
 1, "cannot join and range check",
 1, "select error",
 1, "previous field not accept field",						// 20
 1, "too many variables defined",
 1, "wend / endif mismatch",
 1, "wend without while",
 1, "invalid operation on \'set\' or \'key\' field",
 1, "invalid time",											// 25
 1, "expected a \'(\' or \')\'",
 1, "not valid in assignment expression",
 1, "not valid in conditional expression",
 1, "invalid set statement",
 1, "undefined variable",									// 30
 1, "array subscript must be +ve integer",
 1, "constant definition",
 1, "unexpected symbol",
 1, "invalid column specification",
 1, "invalid row specification",							// 35
 1, "invalid screen attribute",
 1, "invalid field width",
 1, "expected \'from\'",
 1, "invalid table/alias",
 0, "missing table/alias",									// 40
 1, "invalid label (must end with \':\')",
 1, "invalid maintain mode",
 1, "must have character expression",
 1, "invalid label",
 1, "can't create table",									// 45
 1, "table/alias redefined",
 1, "no key fields defined",
 1, "key length too big",
 1, "record length too big",
 0, "missing file/pipe name",								// 50
 1, "invalid file/pipe name",
 1, "file/pipe name redefined",
 1, "too many file/pipes open",
 0, "missing file name / pipe command",
 1, "file name / pipe command must be character",			// 55
 1, "invalid in read statement",
 1, "file/pipe not open for reading",
 1, "file/pipe not open for writing",
 1, "previously defined as a file",
 1, "previously defined as a pipe",							// 60
 1, "file/pipe not open",
 1, "pclose required for pipes",
 1, "file/pipe not open",
 1, "\'fork\' command must be character",
 1, "must be \'create unique\' to be empty",				// 65
 1, "only valid with \'get\' and \'getnext\' statement",
 0, "first accept field can't be \'required\'",
 1, "\'do/format\' block name not found",
 1, "not an array",
 1, "\'pause\' prompt must be character",					// 70
 1, "invalid \'include\' filename",
 1, "can\'t open include file",
 1, "invalid \'escape\' character",
 1, "\'escape\' block name not found",
 1, "\'on_delete\' block name not found",					// 75
 1, "expected \',\' before next argument",
 1, "must have integer expression",
 0, "\'continue/endloop\' must be inside \'while\'",
 1, "\'on_exit\' block name not found",
 1, "'on_exception' block name not found",					// 80
 1, "duplicate block name",
 1, "\t",
 1, "can\'t assign to read-only variable",
 1, "invalid sort specification",
 1, "unknown error",										// 85
 1, "unknown error",
 1, "unknown error",
 1, "unknown error",
 1, "unknown error",
 1, "unknown error",										// 90
// DC extended error codes below
 1, "Extended DC features not enabled!",
 0, "libcall - Function accepts Variables or Literals *ONLY*",
 0, "libcall - Return value must be a Variable",
 0, "libcall - Library Handle must be a Variable",
 0, "libcall - Function Handle must be a Variable",
 0, "libcall - FunctionName parameter must be a string literal"
};

void loaderr(int ErrorNo, char *arg)
{
	char *EntFileName;

	comp_errs++;

	if ( comp_errs > 19 || ErrorNo == 6 )					// give up after 20 "soft" errors, or hard error# 6 == "unexpected end of file"
		comp_abort = 1;

	if ( ErrorNo <= 0 || ErrorNo > 96 )						// 90
		ErrorNo = 90;

	EntFileName = clbasename(FileTree->FileName);
	eprint("\"%s\", line %4d - ", EntFileName, (unsigned short)ll);

	if ( emsgs[ErrorNo].ShowArg )                // should we show the argument in the error display?
		eprint("[%s] ", arg);

	eprint("%s (%2d)\n", emsgs[ErrorNo].ErrorMsg, ErrorNo);
}

#endif
