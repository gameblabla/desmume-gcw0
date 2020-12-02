//taken from fceux on 10/27/08
//subsequently modified for desmume

/*
	Copyright (C) 2008-2009 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "xstring.h"
#include <string>

//a vc-style substring operation (very kind and lenient)
std::string strsub(const std::string& str, int pos, int len) {
	int strlen = str.size();
	
	if(strlen==0) return str; //empty strings always return empty strings
	if(pos>=strlen) return str; //if you start past the end of the string, return the entire string. this is unusual, but there you have it

	//clipping
	if(pos<0) {
		len += pos;
		pos = 0;
	}

	if (pos+len>=strlen)
		len=strlen-pos+1;
	
	//return str.str().substr(pos,len);
	return str.substr(pos,len);
}

std::string strmid(const std::string& str, int pos, int len) { return strsub(str,pos,len); }
std::string strleft(const std::string& str, int len) { return strsub(str,0,len); }
std::string strright(const std::string& str, int len) { return len ? strsub(str,str.size()-len,len) : ""; }
std::string toupper(const std::string& str)
{
	std::string ret = str;
	for(u32 i=0;i<str.size();i++)
		ret[i] = toupper(ret[i]);
	return ret;
}

///Upper case routine. Returns number of characters modified
int str_ucase(char *str) {
	u32 i=0,j=0;

	while (i < strlen(str)) {
		if ((str[i] >= 'a') && (str[i] <= 'z')) {
			str[i] &= ~0x20;
			j++;
		}
		i++;
	}
	return j;
}


///Lower case routine. Returns number of characters modified
int str_lcase(char *str) {
	u32 i=0,j=0;

	while (i < strlen(str)) {
		if ((str[i] >= 'A') && (str[i] <= 'Z')) {
			str[i] |= 0x20;
			j++;
		}
		i++;
	}
	return j;
}


///White space-trimming routine

///Removes whitespace from left side of string, depending on the flags set (See STRIP_x definitions in xstring.h)
///Returns number of characters removed
int str_ltrim(char *str, int flags) {
	u32 i=0;

	while (str[0]) {
		if ((str[0] != ' ') || (str[0] != '\t') || (str[0] != '\r') || (str[0] != '\n')) break;

		if ((flags & STRIP_SP) && (str[0] == ' ')) {
			i++;
			strcpy(str,str+1);
		}
		if ((flags & STRIP_TAB) && (str[0] == '\t')) {
			i++;
			strcpy(str,str+1);
		}
		if ((flags & STRIP_CR) && (str[0] == '\r')) {
			i++;
			strcpy(str,str+1);
		}
		if ((flags & STRIP_LF) && (str[0] == '\n')) {
			i++;
			strcpy(str,str+1);
		}
	}
	return i;
}


///White space-trimming routine

///Removes whitespace from right side of string, depending on the flags set (See STRIP_x definitions in xstring.h)
///Returns number of characters removed
int str_rtrim(char *str, int flags) {
	u32 i=0;

	while (strlen(str)) {
		if ((str[strlen(str)-1] != ' ') ||
			(str[strlen(str)-1] != '\t') ||
			(str[strlen(str)-1] != '\r') ||
			(str[strlen(str)-1] != '\n')) break;

		if ((flags & STRIP_SP) && (str[0] == ' ')) {
			i++;
			str[strlen(str)-1] = 0;
		}
		if ((flags & STRIP_TAB) && (str[0] == '\t')) {
			i++;
			str[strlen(str)-1] = 0;
		}
		if ((flags & STRIP_CR) && (str[0] == '\r')) {
			i++;
			str[strlen(str)-1] = 0;
		}
		if ((flags & STRIP_LF) && (str[0] == '\n')) {
			i++;
			str[strlen(str)-1] = 0;
		}
	}
	return i;
}


///White space-stripping routine

///Removes whitespace depending on the flags set (See STRIP_x definitions in xstring.h)
///Returns number of characters removed, or -1 on error
int str_strip(char *str, int flags) {
	u32 i=0,j=0;
	char *astr,chr;

	if (!strlen(str)) return -1;
	if (!(flags & (STRIP_SP|STRIP_TAB|STRIP_CR|STRIP_LF))) return -1;
	if (!(astr = (char*)malloc(strlen(str)+1))) return -1;
	while (i < strlen(str)) {
		chr = str[i++];
		if ((flags & STRIP_SP) && (chr == ' ')) chr = 0;
		if ((flags & STRIP_TAB) && (chr == '\t')) chr = 0;
		if ((flags & STRIP_CR) && (chr == '\r')) chr = 0;
		if ((flags & STRIP_LF) && (chr == '\n')) chr = 0;

		if (chr) astr[j++] = chr;
	}
	astr[j] = 0;
	strcpy(str,astr);
	free(astr);
	return j;
}


///Character replacement routine

///Replaces all instances of 'search' with 'replace'
///Returns number of characters modified
int chr_replace(char *str, char search, char replace) {
	u32 i=0,j=0;

	while (i < strlen(str)) {
		if (str[i] == search) {
			str[i] = replace;
			j++;
		}
		i++;
	}
	return j;
}


///Sub-String replacement routine

///Replaces all instances of 'search' with 'replace'
///Returns number of sub-strings modified, or -1 on error
int str_replace(char *str, char *search, char *replace) {
	u32 i=0,j=0;
	int searchlen,replacelen;
	char *astr;

	searchlen = strlen(search);
	replacelen = strlen(replace);
	if ((!strlen(str)) || (!searchlen)) return -1; //note: allow *replace to have a length of zero!
	if (!(astr = (char*)malloc(strlen(str)+1))) return -1;
	while (i < strlen(str)) {
		if (!strncmp(str+i,search,searchlen)) {
			if (replacelen) memcpy(astr+j,replace,replacelen);
			i += searchlen;
			j += replacelen;
		}
		else astr[j++] = str[i++];
	}
	astr[j] = 0;
	strcpy(str,astr);
	free(astr);
	return j;
}

static const struct Base64Table
{
	Base64Table()
	{
		size_t a=0;
		for(a=0; a<256; ++a) data[a] = 0xFF; // mark everything as invalid by default
		// create value->ascii mapping
		a=0;
		for(unsigned char c='A'; c<='Z'; ++c) data[a++] = c; // 0..25
		for(unsigned char c='a'; c<='z'; ++c) data[a++] = c; // 26..51
		for(unsigned char c='0'; c<='9'; ++c) data[a++] = c; // 52..61
		data[62] = '+';                             // 62
		data[63] = '/';                             // 63
		// create ascii->value mapping (but due to overlap, write it to highbit region)
		for(a=0; a<64; ++a) data[data[a]^0x80] = a; // 
		data[((unsigned char)'=') ^ 0x80] = 0;
	}
	unsigned char operator[] (size_t pos) const { return data[pos]; }
private:
	unsigned char data[256];
} Base64Table;

std::string u32ToHexString(u32 val)
{
	char temp[16];
	sprintf(temp,"%08X",val);
	return temp;
}

///returns -1 if this is not a hex string
int HexStringToBytesLength(const std::string& str)
{
	if(str.size()>2 && str[0] == '0' && toupper(str[1]) == 'X')
		return str.size()/2-1;
	else return -1;
}

int Base64StringToBytesLength(const std::string& str)
{
	if(str.size() < 7 || (str.size()-7) % 4 || str.substr(0,7) != "base64:") return -1;
	
	size_t c = ((str.size() - 7) / 4) * 3;
	if(str[str.size()-1] == '=') { --c;
	if(str[str.size()-2] == '=') --c; }
	return c;
}


#include <string>
#include <vector>
/// \brief convert input string into vector of string tokens
///
/// \note consecutive delimiters will be treated as single delimiter
/// \note delimiters are _not_ included in return data
///
/// \param input string to be parsed
/// \param delims list of delimiters.

std::vector<std::string> tokenize_str(const std::string & str,
                                      const std::string & delims=", \t")
{
  using namespace std;
  // Skip delims at beginning, find start of first token
  string::size_type lastPos = str.find_first_not_of(delims, 0);
  // Find next delimiter @ end of token
  string::size_type pos     = str.find_first_of(delims, lastPos);

  // output vector
  vector<string> tokens;

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delims.  Note the "not_of". this is beginning of token
      lastPos = str.find_first_not_of(delims, pos);
      // Find next delimiter at end of token.
      pos     = str.find_first_of(delims, lastPos);
    }

  return tokens;
}

//this code was taken from WINE (LGPL)
//http://google.com/codesearch?hl=en&q=+lang:c+splitpath+show:CPvw9Z-euls:_RSotQzmLeU:KGzljMEYFbY&sa=N&cd=9&ct=rc&cs_p=http://gentoo.osuosl.org/distfiles/Wine-20050524.tar.gz&cs_f=wine-20050524/programs/winefile/splitpath.c
void splitpath(const char* path, char* drv, char* dir, char* name, char* ext)
{
    const char* end; /* end of processed string */
	const char* p;	 /* search pointer */
	const char* s;	 /* copy pointer */

	/* extract drive name */
	if (path[0] && path[1]==':') {
		if (drv) {
			*drv++ = *path++;
			*drv++ = *path++;
			*drv = '\0';
		} else path+=2;
	} else if (drv)
		*drv = '\0';

	/* search for end of string or stream separator */
	for(end=path; *end && *end!=':'; )
		end++;

	/* search for begin of file extension */
	for(p=end; p>path && *--p!='\\' && *p!='/'; )
		if (*p == '.') {
			end = p;
			break;
		}

	if (ext)
		for(s=end; (*ext=*s++); )
			ext++;
	else
		for(s=end; *s++; ) {}

	/* search for end of directory name */
	for(p=end; p>path; )
		if (*--p=='\\' || *p=='/') {
			p++;
			break;
		}

	if (name) {
		for(s=p; s<end; )
			*name++ = *s++;

		*name = '\0';
	} else
		for(s=p; s<end; )
			*s++;

	if (dir) {
		for(s=path; s<p; )
			*dir++ = *s++;

		*dir = '\0';
	}
}

//mbg 5/12/08
//for the curious, I tested U16ToHexStr and it was 10x faster than printf.
//so the author of these dedicated functions is not insane, and I will leave them.

static char TempArray[11];

uint16 FastStrToU16(char* s, bool& valid)
{
	int i;
	uint16 v=0;
	for(i=0; i < 4; i++)
	{
		if(s[i] == 0) return v;
		v<<=4;
		if(s[i] >= '0' && s[i] <= '9')
		{
			v+=s[i]-'0';
		}
		else if(s[i] >= 'a' && s[i] <= 'f')
		{
			v+=s[i]-'a'+10;
		}
		else if(s[i] >= 'A' && s[i] <= 'F')
		{
			v+=s[i]-'A'+10;
		}
		else
		{
			valid = false;
			return 0xFFFF;
		}
	}
	valid = true;
	return v;
}

char *U8ToDecStr(uint8 a)
{
	TempArray[0] = '0' + a/100;
	TempArray[1] = '0' + (a%100)/10;
	TempArray[2] = '0' + (a%10);
	TempArray[3] = 0;
	return TempArray;
}

char *U16ToDecStr(uint16 a)
{
	TempArray[0] = '0' + a/10000;
	TempArray[1] = '0' + (a%10000)/1000;
	TempArray[2] = '0' + (a%1000)/100;
	TempArray[3] = '0' + (a%100)/10;
	TempArray[4] = '0' + (a%10);
	TempArray[5] = 0;
	return TempArray;
}

char *U32ToDecStr(char* buf, uint32 a)
{
	buf[0] = '0' + a/1000000000;
	buf[1] = '0' + (a%1000000000)/100000000;
	buf[2] = '0' + (a%100000000)/10000000;
	buf[3] = '0' + (a%10000000)/1000000;
	buf[4] = '0' + (a%1000000)/100000;
	buf[5] = '0' + (a%100000)/10000;
	buf[6] = '0' + (a%10000)/1000;
	buf[7] = '0' + (a%1000)/100;
	buf[8] = '0' + (a%100)/10;
	buf[9] = '0' + (a%10);
	buf[10] = 0;
	return buf;
}
char *U32ToDecStr(uint32 a)
{
	return U32ToDecStr(TempArray,a);
}

char *U16ToHexStr(uint16 a)
{
	TempArray[0] = a/4096 > 9?'A'+a/4096-10:'0' + a/4096;
	TempArray[1] = (a%4096)/256 > 9?'A'+(a%4096)/256 - 10:'0' + (a%4096)/256;
	TempArray[2] = (a%256)/16 > 9?'A'+(a%256)/16 - 10:'0' + (a%256)/16;
	TempArray[3] = a%16 > 9?'A'+(a%16) - 10:'0' + (a%16);
	TempArray[4] = 0;
	return TempArray;
}

char *U8ToHexStr(uint8 a)
{
	TempArray[0] = a/16 > 9?'A'+a/16 - 10:'0' + a/16;
	TempArray[1] = a%16 > 9?'A'+(a%16) - 10:'0' + (a%16);
	TempArray[2] = 0;
	return TempArray;
}

std::string stditoa(int n)
{
	char tempbuf[16];
	sprintf(tempbuf, "%d", n);
	return tempbuf;
}


std::string readNullTerminatedAscii(std::istream* is)
{
	std::string ret;
	ret.reserve(50);
	for(;;) 
	{
		int c = is->get();
		if(c == 0) break;
		else ret += (char)c;
	}
	return ret;
}

// replace all instances of victim with replacement
std::string mass_replace(const std::string &source, const std::string &victim, const std::string &replacement)
{
	std::string answer = source;
	std::string::size_type j = 0;
	while ((j = answer.find(victim, j)) != std::string::npos )
	{
		answer.replace(j, victim.length(), replacement);
		j+= replacement.length();
	}
	return answer;
}

//TODO - dont we already have another  function that can do this
std::string getExtension(const char* input) {
	char buf[1024];
	strcpy(buf,input);
	char* dot=strrchr(buf,'.');
	if(!dot)
		return "";
	char ext [512];
	strcpy(ext, dot+1);
	int k, extlen=strlen(ext);
	for(k=0;k<extlen;k++)
		ext[k]=tolower(ext[k]);
	return ext;
}

