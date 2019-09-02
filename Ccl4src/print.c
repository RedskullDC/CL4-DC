#ifndef PRINT_MISC_C
#define PRINT_MISC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

void eprint(const char *format, ...)
{
	va_list va;

	//printf("eprint() called\n");
	va_start(va, format);
	if ( !fp_1 )
		fp_1 = stderr;
	vfprintf(fp_1, format, va);
	fflush(fp_1);
	va_end(va);	// keep va macros happy

}

void print(const char *format, ...)
{
	va_list va;

	va_start(va, format);
	vfprintf(stdout, format, va);
	fflush(stdout);
  	va_end(va);	// keep va macros happy

}

//	***	not called by libcl4 ***

#ifdef clenter  // not needed for clcomp

void oprint(FILE *s, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	vfprintf(s, format, va);
	va_end(va);	// keep va macros happy

}

void dprint(const char *format, ...)
{
    FILE *v1; // edi@1
    time_t timer; // [sp+18h] [bp-10h]@1
    va_list va; // [sp+34h] [bp+Ch]@1

    va_start(va, format);
    v1 = fopen64("/tmp/cldebug", "a+");
    time(&timer);
    if ( v1 )
    {
        fprintf(v1, "%.24s: pid=%d ", ctime(&timer), getpid());
        vfprintf(v1, format, va);
        fflush(v1);
        fclose(v1);
    }
	va_end(va);

}

char buf_0_2[32];

void cgiprint(int ErrorCode)
{
	char	*v2;
	const char *v3;
	int	http_err = 0;

	sprintf(buf_0_2, "CL_ERROR=%d", ErrorCode);
	putenv(buf_0_2);

	v2 = getenv("CLHTTP_ERROR");
	if ( v2 && *v2 )
		http_err = strtol(v2, 0, 10);

	puts("Content-type: text/html\r");
	if ( http_err == 403 )
	{
		puts("Status: 403 Forbidden\r\n\r");
		puts("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">");
		puts("<html>\n<head>\n<title>403 Forbidden</title>\n</head>\n<body>");
		puts("<h2>Forbidden</h2>");
		v3 = "You don't have permission to access <span style=\"color:#CC3300\">%s</span> on this server.<P>\n";
	}
	else
	{
		puts("Status: 404 Not Found\r\n\r");
		puts("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">");
		puts("<html>\n<head>\n<title>404 Not Found</title>\n</head>\n<body>");
		puts("<h2>Not Found</h2>");
		v3 = "The requested URL <span style=\"color:#CC3300\">%s</span> was not found on this server.<P>\n";
	}
	printf(v3, getenv("REQUEST_URI"));

	printf("<hr>\n%s", getenv("SERVER_SIGNATURE"));
	printf("<span style=\"color: #CC3300; font: italic normal bold 100%% verdana,sans-serif;\">cl4</span>");
	printf(" <i>%s</i>\n", getclver());
	puts("</body>\n</html>");
}

void cgi_test_print(char* Message)
{
	puts("Content-type: text/html\r");
	puts("Status: 404 Not Found\r\n\r");
	puts("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">");
	puts("<html>\n<head>\n<title>404 Not Found</title>\n</head>\n<body>");
	puts("<h2>Not Found</h2>");
	printf("Message returned <span style=\"color:#CC3300\">%s</span>.<P>\n", Message);

	printf("<hr>\n%s", getenv("SERVER_SIGNATURE"));
	printf("<span style=\"color: #CC3300; font: italic normal bold 100%% verdana,sans-serif;\">cl4</span>");
	printf(" <i>%s</i>\n", getclver());
	puts("</body>\n</html>");

}
#endif

#endif
