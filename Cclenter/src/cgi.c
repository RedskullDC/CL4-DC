#ifndef CGI_FNS_C
#define CGI_FNS_C

#include <string.h>
#include "DBdefs.h"
#include "cl4.h"

// dummy xl routines. replace with expat later!

char buf_4[8192];

char *xmlInit(char *StreamName)
{
	buf_4[0] = 0;
	return buf_4;	// null string means no error occured
}

char *xmlGetValue(char *ValueType, int a2)
{
	return "dummy";
}

void xmlPreEncFile(int a1)
{
	//preEncFile = a1;
}


//=============================================================

form_entry *_cgiGetCookies(char *a1)
{
	char *v3;
	char *v5;
	char *v6;
	char *v7;
	form_entry *fme;
	
	fme = 0;
	if ( a1 )
	{
		v3 = mstrcpy(a1, 0);	// make a local copy in global mem
		fme = 0;
		v5 = strchr(v3, '=');
		if ( v5 )
		{
			*v5++ = 0;
			fme = (form_entry *)mmalloc(12);
			fme->name = mstrcpy(v3, 0);
			v6 = strchr(v5, ';');			// look for seperator indicating next cookie
			v7 = "";
			if ( v6 )
			{
				*v6 = 0;
				v7 = v6 + 2;
			}
			fme->val = mstrcpy(v5, 0);
			if ( *v7 )
				fme->next = _cgiGetCookies(v7);           // recursive
		}
		mfree_0(v3);
	}
	return fme;
}

bool _cgiGetInfo(CGI_VAR *cgarr)
{
	CGI_VAR *i;
	
	for ( i = cgarr; i->name; i++ )
	{
		i->val = mstrcpy(getenv(i->name), 0);
		if ( i->val && !strncasecmp(i->name, "HTTP_COOKIE", 12))
			ck = _cgiGetCookies(i->val);	// get linked list of cookie values
	}
	return cgarr[4].val != 0;				// SERVER_NAME != NULL
}

void soapFault(char *SoapFault, char *SoapFaultStr, char *SoapExtFaultStr)
{
  puts("Content-Type: text/xml; charset=\"utf-8\"");
  puts("Status: 500\n");
  printf("<SOAP-ENV:Envelope ");
  puts("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">");
  puts(" <SOAP-ENV:Body>");
  puts("  <SOAP-ENV:Fault>");
  printf("   <faultcode>%s</faultcode>\n", SoapFault);
  printf("   <faultstring>%s", SoapFaultStr);
  if ( *SoapExtFaultStr )
    printf(" [%s]", SoapExtFaultStr);
  puts("</faultstring>");
  puts("  </SOAP-ENV:Fault>");
  puts(" </SOAP-ENV:Body>");
  puts("</SOAP-ENV:Envelope>");
  fflush(stdout);
  exit(1);
}

void xmlRpcFault(char *FaultCode, char *FaultString, char *ExtendedFaultCode)
{
  puts("Content-Type: text/plain");
  puts("Status: 500\n");
  puts("<XML-ENV:Fault>");
  printf(" <faultcode>%s</faultcode>\n", FaultCode);
  printf(" <faultstring>%s", FaultString);
  if ( *ExtendedFaultCode )
    printf(" [%s]", ExtendedFaultCode);
  puts("</faultstring>");
  puts("</XML-ENV:Fault>");
  fflush(stdout);
  exit(1);
}

//form_entry *_cgiGetFormData(CGI_VAR *a1)
form_entry *_cgiGetFormData(CGI_VAR *a1)
{
	form_entry *v67; // [sp+30h] [bp-2528h]@73
	form_entry *v61; // esi@91
	int length; // esi@1
	char *v7; // eax@20
	FILE *v10; // esi@23
	size_t v12; // eax@27
	char *v39; // eax@51
	unsigned int v44; // eax@64
	char *v50; // esi@76
	char *ProgramName; // eax@86
	char *v59; // edi@89
	char *v63; // ST3C_4@93
	int v64; // [sp+Ch] [bp-254Ch]@1
	bool v65; // [sp+28h] [bp-2530h]@49
	char *SoapAction; // [sp+2Ch] [bp-252Ch]@17
	char *a3; // [sp+34h] [bp-2524h]@67
	const char *s; // [sp+38h] [bp-2520h]@52
	char *ptr; // [sp+3Ch] [bp-251Ch]@31
	
	char path[128]; // [sp+40h] [bp-2518h]@51
	char FileName[128]; // [sp+C0h] [bp-2498h]@21				// ********************
	char SrcString[1024]; // [sp+140h] [bp-2418h]@23
	char dest[1024]; // [sp+540h] [bp-2018h]@12

	length = strtol(a1->val, 0, 10);											// a1[0] = CONTENT_LENGTH value read

	if ( length && a1[1].val && strstr(a1[1].val, "multipart/form-data") )		// a1[1] = CONTENT_TYPE
		return _cgiGetMultipartDataFromStream();

	if ( a1[2].val )										// REQUEST_METHOD has been specified
	{
		if (!strncasecmp(a1[2].val, "POST", 4u) && a1[1].val && !strncasecmp(a1[1].val, "application/x-www-form-urlencoded", 33u))
		{
			return _cgiGetDataFromStream(length, stdin);
		}
		if ( !strncasecmp(a1[2].val, "GET", 3u) )			// a1[2] = REQUEST_METHOD
		{													// a1[3] = QUERY_STRING
			v7 = strcpy(a1[3].val, cnvstr(dest, "url", "de", a1[3].val)); // decode URL and copy back into array (may not require decoding)
			return _cgiGetDataFromString(v7);
		}

		if ( !strncasecmp(a1[2].val, "POST", 4u) && a1[1].val && !strncasecmp(a1[1].val, "text/xml", 8u) )		// REQUEST_METHOD and CONTENT_TYPE
		{
			SoapAction = getenv("HTTP_SOAPACTION");
			if ( SoapAction != 0 )
				isSoap = 1;							// once set, never returns to ZERO
        
			xmlPreEncFile(1);
			if ( isSoap )                           // if HTTP_SOAPACTION enviro variable exists, treat XML as SOAP , else RPC
			{
				v7 = getevar("CLTMPDIR");
				if ( *v7 )
				{
					strcpy(FileName, v7);
					strcat(FileName, "/");
				}
				else
					strcpy(FileName, "/tmp/");

				//-  save XML data to file in case required later
				sprintf(SrcString, "soap.%d",getpid());				//user pid tacked onto filename to uniqueificate
				strcat(FileName, SrcString);
				v10 = fopen64(FileName, "w");
				if ( v10 )
				{
					putenv(mstrcpy("XML_FILENAME=", FileName, 0));
					while ( 1 )
					{
						v12 = read(0, SrcString, 1024u);
						if ( !v12 )
							break;
						fwrite(SrcString, v12, 1u, v10);
					}
					fclose(v10);
				}
				else
					strcpy(FileName, "stdin");
				//--------------------------------------------------------------
				xml = 1;
				v50 = xmlInit(FileName);
				if ( *v50 )											// NULL string means no error occured.
					soapFault("SOAP-ENV:Client", "SOAP Parse Error", v50);

				ptr = xmlGetValue("elem", 0);
				if (strncasecmp(ptr, "SOAP-ENV:Envelope", 18) || strncasecmp(ptr, "soap:Envelope", 14))
					soapFault("SOAP-ENV:Client", "No SOAP Envelope Detected", "");
				//----------------------------------------------------------------
				ptr = xmlGetValue("elem", 0);
				if (strncasecmp(ptr, "SOAP-ENV:Body", 14) || strncasecmp(ptr, "soap:Body", 10))
					soapFault("SOAP-ENV:Client", "No SOAP Body Detected", "");

				//----------------------------------------------------------------
				v65 = false;
				if ( isCGI == 2 )						// called as clcgi?
				{
					v39 = getenv("DOCUMENT_ROOT");
					if ( v39 )
					{
						strcpy(path, v39);
						v39 = getenv("SCRIPT_NAME");
						if ( v39 )
						{
							strcpy(scriptName, v39);
							s = scriptName;					// char[100]
							if ( scriptName[1] == '~' )
							{
								ptr = strchr(&scriptName[1], '/');
								if ( ptr )
								  *ptr = 0;

								strcpy(path, guserdir(&scriptName[2]));
								strcat(path, "/public_html/");

								if ( ptr )
									s = ptr + 1;
								else
									s = scriptName;
							}
							ptr = strrchr(s, '/');
							if ( ptr )
							{
								*ptr = 0;
								strcat(path, s);
							}
						}
						if ( chdir(path) >= 0 )
						{
							setupEnv();
							strcpy(cwd, path);
						}
						else
							v65 = true;
					}
					else
						v65 = true;
				}
				//----------------------------------------------------------------
				if ( *ptr == 's' )
				{
					v44 = strlen(SoapAction);
					if ( *SoapAction = '"' && SoapAction[v44 - 1] == '"' )
					{
						SoapAction[v44 - 1] = 0;	// avoid the quotes
						a3 = SoapAction + 1;
					}
					else
						a3 = SoapAction;
				}
				else
					a3 = strchr(xmlGetValue("elem", 0), ':') + 1;

				if ( v65 || !chkpath(a3, 0, "CLEPATH", 1, 256) )
					soapFault("SOAP-ENV:Client", "Method name not found", a3);

				sprintf(SrcString, "E=%s", a3);
				fe = _cgiGetDataFromString(SrcString);
				v67 = fe;
				putenv("RPC_TYPE=SOAP1.1");
				a1[5].val = mstrcpy("SOAP1.1", 0);			// RPC_TYPE value
			}
			else		// not SOAP			XML-RPC handler code
			{
				v50 = xmlInit("stdin");						// ZERO result == no error
				if ( *v50 )
				  xmlRpcFault("XML:Client", "Parse Error", v50);

				//----------------------------------------------------------------
				v50 = xmlGetValue("elem", 0);
				if (strncmp(v50, "methodCall", 11))
					xmlRpcFault("XML:Client", "No <methodCall> Detected", "");
				//----------------------------------------------------------------
				v50 = xmlGetValue("elem", 0);
				if (strncmp(v50, "methodName", 11))
					xmlRpcFault("XML-RPC:Client", "No <methodName> Detected", "");
				//----------------------------------------------------------------
				ProgramName = xmlGetValue("text", 0);
				sprintf(SrcString, "E=%s", ProgramName);// ".enc" module to call.

				fe = _cgiGetDataFromString(SrcString);
				v67 = fe;
				putenv("RPC_TYPE=XML-RPC");
				a1[5].val = mstrcpy("XML-RPC", 0);		// RPC_TYPE value
			}
		
			//========= common code ============
			while ( 1 )
			{
				do
				{
					v59 = xmlGetValue("elem", 0);
					if ( !*v59 )
					{
						xmlPreEncFile(0);
						return fe;						// exit success?
					}
					a3 = xmlGetValue("text", 0);
				}
				while ( !a3 );

				if ( !isSoap )
					v59 = "param";

				if ( !strtol(xmlGetValue("cdata", 0), 0, 10))
				{
					v63 = mstrcpy(v59, "=", a3, 0);
					v61 = _cgiGetDataFromString(v63);		// ****	recursive ****
					mfree_0(v63);
					if ( v61 )
					{
						v67->next = v61;
						v67 = v61;
					}
				}
				else
				{
					v61 = (form_entry *)mmalloc(12);
					if ( v61 )
					{
						v61->name = mstrcpy(v59, 0);
						v61->val = mstrcpy(a3, 0);		// funny GCC opt in real clenter!!!
						v67->next = v61;
						v67 = v61;
					}
				}
			}
		}
	}
	return 0;		// error_exit no REQUEST_METHOD value
}

char *_cgiCleanSSI(char *haystack)
{
	char *v1; // eax@2

	if ( cleanSSI )
	{
		v1 = strstr(haystack, "<!--#");
		if ( v1 )
		{
			*(int*)v1 = '--!<';                   // replace instance of "<!--#" with "<!-- "
			v1[4] = ' ';
		}
	}
	return haystack;
}

char *_cgiNextCookieValue()
{
	if ( !started_0 )
	{
		started_0 = 1;
		pck_1 = ck;				// linked list array
	}

	if ( pck_1 )
	{
		mfree_0(nameval_2);
		nameval_2 = mstrcpy("Cookie: ", pck_1->name, "=", pck_1->val, 0);
		pck_1 = pck_1->next;
		return _cgiCleanSSI(nameval_2);
	}
	else
		return 0;
}

char *_cgiNextNameValue()
{
	char *v0;

	if ( !started_3 )
	{
		v0 = _cgiNextCookieValue();
		if ( v0 )
			return v0;
		started_3 = 1;
		pfe_4 = fe;					// linked array of argumanets
	}
	if ( pfe_4 )
	{
		mfree_0(nameval_5);
		nameval_5 = mstrcpy(pfe_4->name, "=", pfe_4->val, 0);
		pfe_4 = pfe_4->next;
		return _cgiCleanSSI(nameval_5);
	}
	else
		return 0;
}

char *_cgiGetPartValue(char *s2, int a2)
{
	char *v2;
	int v3;

	if ( !svName_6 || strcmp(s2, svName_6) )
	{
		mfree_0(svName_6);
		svName_6 = mstrcpy(s2, 0);
		pfe_7 = fe;
		lenSeen_8 = 0;
	}
	while ( pfe_7 )
	{
		if ( !strcasecmp(pfe_7->name, s2) )
		{
			if ( pfe_7 )
			{
				v2 = pfe_7->val;
				if ( strlen(pfe_7->val) > lenSeen_8 )
				{
					v3 = lenSeen_8;
					lenSeen_8 += a2;
					return _cgiCleanSSI(&v2[v3]);
				}
			}
			break;
		}
		pfe_7 = pfe_7->next;
	}
	svName_6 = (char *)mfree_0(svName_6);
	return 0;
}

char *_cgiGetMultiValue(char *s2)
{
	form_entry *i;
	char *v2;
	
	if ( svName_9 && !strcmp(s2, svName_9) )
		goto LABEL_6;

	mfree_0(svName_9);
	svName_9 = mstrcpy(s2, 0);
	for ( i = fe; ; i = pfe_10->next )
	{
		pfe_10 = i;
LABEL_6:
		if ( !pfe_10 )
			break;
		if ( !strcasecmp(pfe_10->name, s2) )
		{
			if ( pfe_10 )
			{
				v2 = pfe_10->val;
				pfe_10 = pfe_10->next;
				return _cgiCleanSSI(v2);
			}
			break;
		}
	}
	svName_9 = (char *)mfree_0(svName_9);
	v2 = 0;
	return _cgiCleanSSI(v2);
}

char *_cgiGetValue(form_entry *fm, const char *s2)
{
	while (fm)		// fm points to env variables list here
	{
		if ( !strcasecmp(fm->name, s2))
			return _cgiCleanSSI(fm->val);			// found a match, return cleaned up version
		fm = fm->next;
	}
	return 0;
}

char *_cgiGetCookieValue(form_entry *fm, const char *ck_name)
{
  	while (fm)		// fm points to cookie list here
	{
		if ( !strcasecmp(fm->name, ck_name))
			return _cgiCleanSSI(fm->val);			// found a match, return cleaned up version
		fm = fm->next;
	}
	return 0;
}

unsigned char dd2c(char a1, char a2)				// double digit to char
{													// convert XX to char
	char v2;
	unsigned char v3;
	int v4;
	
	if ( a1 <= '@' )
		v2 = a1 - '0';
	else
		v2 = (a1 & 0xDF) - 55;
	
	v3 = 16 * v2;
	if ( a2 <= '@' )
		v4 = a2 + v3 - '0';
	else
		v4 = (a2 & 0xDF) + v3 - 55;
	return v4;
}

form_entry *_cgiGetDataFromString(char *Src)
{
	int		v5;
	int		j;
	int		asize;
	form_entry *ptr;
	
	if ( Src )
	{
		while ( isspace(*Src) || *Src == '&' ) 
			++Src;
		if ( !*Src )
			return 0;
		
		ptr = (form_entry *)mmalloc(12);
		if ( !ptr )
			return 0;
		
		asize = 64;
		ptr->name = mmalloc(asize);
		if ( !ptr->name )
		{
		  free(ptr);
		  return 0;
		}
		for ( j = 0; *Src && *Src != '&' && *Src != '='; ++j )
		{
			v5 = *Src;
			if ( v5 == '%' )									// encoded HEX digit %xx
			{
				ptr->name[j] = dd2c(Src[1], Src[2]);
				Src += 2;
			}
			else
				ptr->name[j] = v5 == '+' ? ' ' : v5;			// replace all '+' with <SPC> 

			if ( j + 1 >= asize )
			{
				ptr->name = (char *)mrealloc(ptr->name, asize, 2 * asize);
				asize *= 2;
				if ( !ptr->name )
					return 0;
			}
			Src++;
		}
		ptr->name[j] = 0;
		
		v5 = *Src++;
		if ( v5 == '&' )
		{
			ptr->val = 0;
		}
		else if ( v5 == '=' )
		{
			asize = 64;
			ptr->val = mmalloc(asize);
			if (ptr->val)
			{
				for ( j = 0; *Src && *Src != '&'; j++ )
				{
					v5 = *Src;
					if ( v5 == '%' )								// encoded HEX digit %xx
					{
						ptr->val[j] = dd2c(Src[1], Src[2]);
						Src += 2;
					}
					else
						ptr->val[j] = v5 == '+' ? ' ' : v5;			// replace all '+' with <SPC> 

					if ( j + 1 >= asize )
					{
						ptr->val = (char *)mrealloc(ptr->val, asize, 2 * asize);
						asize *= 2;
						if ( !ptr->val )
							return 0;
					}
					Src++;
				}
				ptr->val[j] = 0;
				
				v5 = *Src++;
				if ( v5 && v5 == '&' )
					ptr->next = _cgiGetDataFromString(Src);		// recursive
				else
					ptr->next = 0;
			}
		}
		else
		{
			ptr->val = 0;
			ptr->next = 0;
		}
		return ptr;
	}
	return 0;
}

form_entry *_cgiGetDataFromStream(int length, FILE *fp)
{
	form_entry *result;
	form_entry *frm;

	int		v3;
	int		v9;
	int		v14;
	int		asize;
	char	v6;
	char	v7;
	
	do
	{
		--length;
		v3 = -1;
		if ( length != -1 )
			v3 = getc(fp);
	}
	while ( isspace(v3) || v3 == '&' );

	result = 0;
	if ( v3 != -1 )
	{
		frm = (form_entry *)mmalloc(12);
		result = 0;
		if ( frm )
		{
			asize = 64;
			frm->name = mmalloc(asize);
			if ( !frm->name )			// error
			{
				free(frm);
				return 0;
			}
			for ( v14 = 0; v3 != -1 && v3 != '&' && v3 != '='; v14++ )
			{
				if ( v3 == '%' )									// encoded HEX digit %xx
				{
					length--;
					v6 = -1;
					if ( length != -1 )
						v6 = getc(fp);

					length--;
					v7 = -1;
					if ( length != -1 )
						v7 = getc(fp);

					frm->name[v14] = dd2c(v6, v7);
				}
				else
					frm->name[v14] = v3 == '+' ? ' ' : (char)v3;	// replace all '+' with <SPC> 

				if ( v14 + 1 >= asize )
				{
					frm->name = (char *)mrealloc(frm->name, asize, 2 * asize);	// try to double buffer size
					asize *= 2;
					if ( !frm->name )
						return 0;
				}
				--length;
				v3 = -1;
				if ( length != -1 )
					v3 = getc(fp);
			}
			frm->name[v14] = 0;

			switch ( v3 )
			{
				case -1:			// '\xFF\xFF\xFF\xFF':		EOF error
					frm->val	= 0;
					frm->next	= 0;
					break;
				case '&':
					frm->val	= 0;
					break;
				case '=':
					asize = 64;
					frm->val = mmalloc(asize);
					
					v14 = 0;
					length--;
					v9 = -1;
					if ( length != -1 )
						v9 = getc(fp);

					for ( ; v9 != -1 && v9 != '&'; ++v14 )
					{
						if ( v9 == '%' )						// encoded HEX digit %xx
						{
							length--;
							v6 = -1;
							if ( length != -1 )
								v6 = getc(fp);
	
							length--;
							v7 = -1;
							if ( length != -1 )
								v7 = getc(fp);

							frm->val[v14] = dd2c(v6, v7);
						}
						else
							frm->val[v14] = v9 == '+' ? ' ' : (char)v9;

						if ( v14 + 1 >= asize )
						{
							frm->val = (char *)mrealloc(frm->val, asize, 2 * asize);
							asize *= 2;
							if ( !frm->val )
								return 0;
						}
						--length;
						v9 = -1;
						if ( length != -1 )
							v9 = getc(fp);
					}
					
					frm->val[v14] = 0;
					if ( v9 == '&' )
						frm->next = _cgiGetDataFromStream(length, fp);
					else
						frm->next = 0;
					break;
			}
			result = frm;
		}
	}
	return result;
}

int getbuf_0(char *Buffer, int MaxLen)
{
	int		c = 0;
	int		length;
	
	for ( length = 0; length < MaxLen; length++ )		// read byte at a time, return at end of line
	{
		c = getc(stdin);
		if ( c == '\n' || c == -1 )			// -1 EOF or IO_ERROR, \n == EOL
			break;
		Buffer[length] = c;
	}
	if ( c != '\n' )
		return length;
	Buffer[length++] = '\n';
	return length;
}

form_entry *_cgiGetMultipartDataFromStream()
{
	form_entry **v0;
	form_entry *result;
	form_entry *v22;
	
	char *v1;
	char *v4;
	char *v7;
	char *v12;
	char *v16;
	
	int v6;
	int v8;
	
	int k; // edi@59
	
	int v23; // [sp+Ch] [bp-409Ch]@0
	char *v24; // [sp+38h] [bp-4070h]@4
	int v25; // [sp+3Ch] [bp-406Ch]@1
	int v26; // [sp+40h] [bp-4068h]@54
	int v28; // [sp+44h] [bp-4064h]@53
	int asize; // [sp+48h] [bp-4060h]@10
	
	bool v31; // [sp+4Ch] [bp-405Ch]@10
	bool v32; // [sp+50h] [bp-4058h]@53
	bool v33; // [sp+54h] [bp-4054h]@26
	bool io_err; // [sp+58h] [bp-4050h]@1
	
	const char *needle; // [sp+5Ch] [bp-404Ch]@5
	char *arg; // [sp+60h] [bp-4048h]@1
	FILE *stream; // [sp+64h] [bp-4044h]@1
	
	form_entry *v38; // [sp+68h] [bp-4040h]@5
	form_entry *v39; // [sp+6Ch] [bp-403Ch]@4
	
	char v40[8192]; // [sp+70h] [bp-4038h]@57
	char haystack[8232]; // [sp+2080h] [bp-2028h]@7

	v0		= 0;
	stream	= 0;
	arg		= 0;
	io_err	= false;
	v25		= 0;

	v1 = getevar("CLTMPDIR");
	if ( *v1 )
		v24 = mstrcpy(v1, "/", 0);
	else
		v24 = mstrcpy("/tmp/", 0);

	v39 = (form_entry *)mmalloc(12);
	result = 0;
	if ( v39 )
	{
		v38 = v39;
		//v4 = strstr(dword_80A466C, "boundary=");		// CONTENT_TYPE value
		v4 = strstr(ce[1].val, "boundary=");			// CONTENT_TYPE value

		needle = mstrcpy(v4 + 9, 0);					// no sanity check on v4!
		while ( !strstr(haystack, needle) )
			getbuf_0(haystack, 0x2000);
		
		while ( 1 )
		{
      
			v6 = getbuf_0(haystack, 0x2000);
			if ( !v6 )
				break;
			haystack[v6] = 0;
			v31 = true;
			
			asize = 64;
			v38->name = mmalloc(asize);
			
			v6 = 0;
			v7 = strstr(haystack, " name=\"");
			if (v7)
			{
				v7 += 7;				// skip past name="
				while ( *v7 && *v7 != '"' && *v7 != '\n' )
				{
					v8 = *v7;
					if ( v8 == '%' )
					{
						v38->name[v6] = dd2c(v7[1], v7[2]);			// encoded HEX digit %xx
						v7 += 2;
					}
					else
						v38->name[v6] = v8 == '+' ? ' ' : v8;		// convert '+' into <SPC> 

					if ( v6 + 1 >= asize )
					{
						v38->name = (char *)mrealloc(v38->name, asize, 2 * asize);	// try to double array size
						asize *= 2;
						if ( !v38->name )
							return 0;
					}
					v6++;
					v7++;
				}
			}
			v38->name[v6] = 0;
			
			asize = 64;
			v38->val = mmalloc(asize);
			v12 = strstr(haystack, "filename=\"");
			if ( !v12 )
				goto LABEL_93;
			
			v33 = true;
			v6 = 0;
			v12 += 10;					// skip past filename="
			
			while ( *v12 && *v12 != '"' && *v12 != '\n' )
			{
				v8 = *v12;
				if ( v8 == '%' )
				{
					v38->val[v6] = dd2c(v12[1], v12[2]);		// encoded HEX digit %xx
					v12 += 2;
				}
				else
					v38->val[v6] = v8 == '+' ? ' ' : v8;		// convert '+' into <SPC> 

				if ( v6 + 1 >= asize )
				{
					v38->val = (char *)mrealloc(v38->val, asize, 2 * asize);
					asize *= 2;
					if ( !v38->val )
						return 0;
				}
				v6++;
				v12++;
			}
			v38->val[v6] = 0;

			if ( *v38->val )
			{
				sprintf(&v38->val[v6], ".%d", getpid());	// Tack PID on the end as a string. Guarantee uniqueness
				if ( arg )
					mfree_0(arg);

				if ((v16 = strrchr(v38->val, '\\')) != 0 || (v16 = strrchr(v38->val, '/')) != 0 || (v16 = strrchr(v38->val, ':')) != 0 )
					arg = mstrcpy(v24, v16 + 1, 0);
				else
					arg = mstrcpy(v24, v38->val, 0);

				stream = fopen64(arg, "w");
				if ( !stream )
				{
					eprint("CGI: cannot open file [%s]\n", arg);
					io_err = true;
				}
				mfree_0(v38->val);
				v38->val = mstrcpy(arg, 0);
			}
			else
			{
LABEL_93:
				v33 = false;
			}
//-----------------------------------------------------------      
			
			getbuf_0(haystack, 0x2000);
			if ( v33 )
				getbuf_0(haystack, 0x2000);
			
			v32 = false;
			v28 = 0;
			do
			{
				v26 = getbuf_0(haystack, 0x2000);
				haystack[v26] = 0;
				if ( strstr(haystack, needle) )				// found needle in a haystack :o)
				{
					v32 = true;
					for ( k = 0; k < v25 - 2; v28++ )
					{
						if ( v33 )
						{
							if ( !io_err && fputc(v40[k], stream) == -1 )
								io_err = true;
						}
						else
						{
							v38->val[v28] = v40[k];
							if ( v28 + 1 >= asize )
							{
								v38->val = (char *)mrealloc(v38->val, asize, 2 * asize);
								asize *= 2;
								if ( !v38->val )
									return 0;
							}
						}
						k++;
					}
					if ( !v33 )
						v38->val[v28] = 0;
				}
				else if ( v31 )
				{
					for ( k = 0; k < v26; k++ )
						v40[k] = haystack[k];
					
					v25 = v26;
					v31 = false;
				}
				else
				{
					for ( k = 0; k < v25; v28++ )
					{
						if ( v33 )
						{
							if ( !io_err && fputc(v40[k], stream) == -1 )
								io_err = true;
						}
						else
						{
							v38->val[v28] = v40[k];
							if ( v28 + 1 >= asize )
							{
								v38->val = (char *)mrealloc(v38->val, asize, 2 * asize);
								asize *= 2;
								if ( !v38->val )
									return 0;
							}
						}
						k++;
					}
					if ( !v33 )
						v38->val[v28] = 0;
					
					for ( k = 0; k < v26; k++ )
						v40[k] = haystack[k];
					
					v25 = v26;
				}
			}
			while ( !v32 );
			
			if ( v33 && !io_err )
				fclose(stream);
			
			v22 = (form_entry *)mmalloc(12);
			v38->next = v22;
			v0 = &v38->next;
			v38 = v22;
		}
		*v0 = 0;
		if ( io_err )
			eprint("CGI: Error writing to [%s]\n", v24);
		result = v39;
	}
	return result;
}

char *cgiGetValue(char *StrParam, int MaxLen)
{
	unsigned int Len;
	char *result;

	Len = strlen(StrParam);
	if ( Len <= 1 || StrParam[Len - 1] != '!' )
	{
		cleanSSI = 0;
	}
	else
	{
		StrParam[Len - 1] = 0;
		cleanSSI = 1;
	}
	if ( *StrParam != '?' || StrParam[1] )
	{
		switch ( *StrParam )
		{
		case '+':
			result = _cgiGetPartValue(StrParam + 1, MaxLen);
			break;
		case '*':
			result = _cgiGetMultiValue(StrParam + 1);
			break;
		case '%':
			result = _cgiGetCookieValue(ck, StrParam + 1);
			break;
		default:
			result = _cgiGetValue(fe, StrParam);
			break;
		}
	}
	else
	{
		result = _cgiNextNameValue();
	}
	return result;
}

char *cgiGetEname(void)
{
	char	*v2;
	const char *v5;
	char	*progname;
	char	*sa;
	char	path[256];
	
	if (!_cgiGetInfo(ce))				// load all env variables into ce structure
		return 0;			// error

	fe = _cgiGetFormData(ce);			// load all env variables into linked lists

	progname = _cgiGetValue(fe, "E");				// get CL4 program name to execute
	if ( !progname )
		progname = _cgiGetValue(fe, "ename");
	
	if ( isCGI == 1 || isSoap && isCGI == 2 )
		return progname;							// normal exit_success point??

	v2 = getenv("REQUEST_URI");
	if ( v2 && v2[1] != '?' )
		progname = 0;

	v2 = getenv("DOCUMENT_ROOT");
	if ( !v2 )
		return 0;
	strcpy(path, v2);

	v2 = getenv("SCRIPT_NAME");
	if ( !v2 )
		return 0;
	strcpy(scriptName, v2);

	v5 = scriptName;
	if ( v5[1] == '~' )				// user relative. user == apache/www ?
	{
		sa = strchr(&scriptName[1], '/');
		if ( sa )
			*sa = 0;
		strcpy(path, guserdir(&scriptName[2]));
		strcat(path, "/public_html/");
		if ( sa )
			v5 = sa + 1;
	}
	v2 = strrchr(v5, '/');
	sa = v2;
	if ( !v2 )
		return 0;
	*v2 = 0;
	strcat(path, v5);

	if ( chdir(path) < 0 )					// whoops, couldn't get access to that directory
	{
		putenv("CLHTTP_ERROR=404");			// not found
	}
	else
	{
		setupEnv();							// get CL4 specific env variables loaded
		if ( getenv("CLEPATH") )
		{
			sa++;
			v2 = strchr(sa, '.');
			if ( v2 )
			{
				*v2 = 0;
				if ( progname )
					return progname;
				else
					return sa;
			}
		}
		else
			putenv("CLHTTP_ERROR=403");		// forbidden
	}

	v2 = getenv("DOCUMENT_ROOT");
	if ( v2 )
	{
		if ( chdir(v2) >= 0 )
		{
			setupEnv();
			v2= getenv("CL_DEFAULT");
			if ( v2 )
				return v2;
		}
	}
	return 0;
}

#endif



