 /*
  * Unitex
  *
  * Copyright (C) 2001-2002 Université de Marne-la-Vallée <unitex@univ-mlv.fr>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  *
  */

#include "unicode.h"


char tab_is_letter[8192];


//
// unicode version of strcpy
//
void u_strcpy(unichar *dest,unichar *src) {
int i=0;
while ((dest[i]=src[i])!=0) i++;
}


//
// unicode version of strcpy
//
void u_strcpy_char(unichar *dest,char *src) {
int i=0;
while ((dest[i]=(unichar)((unsigned char)src[i]))!=0) i++;
}


//
// convert the unichar* src into a char* dest
//
void u_to_char(char *dest,unichar *src) {
int i=0;
while ((dest[i]=(char)src[i])!=0) i++;
}



//
// unicode version of strlen
//
int u_strlen(unichar *s) {
int i=0;
while (s[i++]);
return (i-1);
}


//
// unicode version of strcat
//
void u_strcat(unichar *dest,unichar *src) {
int i,j=0;
i=u_strlen(dest);
while ((dest[i++]=src[j++])!=0);
}


//
// unicode version of strcat
//
void u_strcat_char(unichar *dest,char *src) {
int i,j=0;
i=u_strlen(dest);
while ((dest[i++]=(unichar)((unsigned char)src[j++]))!=0);
}



//
// unicode version of strcmp
//
int u_strcmp(unichar *a,unichar *b) {
int i=0;
while (a[i] && a[i]==b[i]) i++;
return (a[i]-b[i]);
}


//
// unicode version of strcmp
//
int u_strcmp_char(unichar *a,char *b) {
int i=0;
while (a[i] && a[i]==b[i]) i++;
return (a[i]-b[i]);
}



//
// unicode version of fgetc
//
int u_fgetc(FILE *f) {
int c=0;
if (!fread(&c,2,1,f)) return EOF; // reads 2 bytes
if (c==0x0D) {      // case of \n which is coded by 2 unicode chars (000D 000A)
  fread(&c,2,1,f);
  return '\n';
}
return c;
}


//
// unicode version of fputc
//
int u_fputc(unichar c,FILE *f) {
if (c=='\n') {
  int i=0x000A000D;
  return fwrite(&i,4,1,f);
}
return fwrite(&c,2,1,f);
}


//
// UTF-8 version of fputc
//
int u_fputc_utf8(unichar c,FILE *f) {
if (c=='\n') {
  return fprintf(f,"\n");
}
if (c<=0x7F) {
   return fprintf(f,"%c",c);
}
unsigned char a;
unsigned char b;
if (c<=0x7FF) {
   a=(unsigned char) (0xC0 | (c>>6));
   b=(unsigned char) (0x80 | (c & 0x3F));
   fputc(a,f);
   return fputc(b,f);
}
unsigned char C;
a=(unsigned char) (0xE0 | (c>>12));
//b=(unsigned char) (0x80 | ((c-(c&12))>>6));   //$CD:20021119 old
b=(unsigned char) (0x80 | ((c>>6)&0x3F));       //$CD:20021119
C=(unsigned char) (0x80 | (c&0x3F));
fputc(a,f);
fputc(b,f);
return fputc(C,f);
}



//
// reads a line and save it in res
//
int u_read_line(FILE* f,unichar* res) {
int c;
int i=0;
while ((c=u_fgetc(f))!=EOF && c!='\n') {
  res[i++]=(unichar)c;
}
if (i==0 && c==EOF) {
  // case of an end of file
  return 0;
}
res[i]='\0';
return 1;
}



//
// opens a file in binary mode for unicode I/O
// MODE should be U_READ, U_WRITE or U_APPEND
//
FILE* u_fopen(char *name,char* MODE) {
if (!strcmp(MODE,U_APPEND)) {
   // if we are in APPEND mode
   // we check first if the file allready exists
   FILE* f=fopen(name,U_READ);
   if (f!=NULL) {
      // if the file exists
      fclose(f);
      // we return it
      return fopen(name,U_APPEND);
   }
   else {
      // if the file does not exists, we are in WRITE mode
      FILE* f=fopen(name,U_WRITE);
      // as the file is new, we must insert the byte order char
      u_fputc(U_BYTE_ORDER_MARK,f);
      return f;
   }
}
FILE* f=fopen(name,MODE);
int c;
if (f==NULL) return NULL;
// if the file is opened in read mode, we verify the presence of FEFF
if (!strcmp(MODE,U_READ)) {
  c=u_fgetc(f);
  if (c!=U_BYTE_ORDER_MARK && c!=U_NOT_A_CHAR) {
    fprintf(stderr,"%s is not a unicode text file\n",name);
    fclose(f);
    return NULL;
  }
  return f;
}
// if the file is open in write mode, we insert the FEFF unicode char
if (!strcmp(MODE,U_WRITE))
  u_fputc(U_BYTE_ORDER_MARK,f);
return f;
}


//
// closes a unicode file
//
int u_fclose(FILE *f) {
return fclose(f);
}



//
// returns 1 if the string contains only spaces, 0 else
//
int only_spaces(unichar* s) {
int i=0;
while (s[i]!='\0') {
  if (s[i]!=' ') return 0;
  i++;
}
return 1;
}



//
// unicode version of isdigit
//
int u_is_digit(unichar c) {
return (c>='0' && c<='9');
}



//
// convert an hexadecimal digit to the corresponding unichar
//
unichar to_hex(int i) {
if (i<10) {
   return (unichar)(i+'0');
}
else {
   return (unichar)(i-10+'A');
}
}



//
// stores into s the hexadecimal representation of c
//
void u_char_to_hexa(unichar c,unichar* s) {
s[0]=to_hex(c/(16*16*16));
c=(unichar)(c%(16*16*16));
s[1]=to_hex(c/(16*16));
c=(unichar)(c%(16*16));
s[2]=to_hex(c/16);
c=(unichar)(c%16);
s[3]=to_hex(c);
s[4]='\0';
}


//
// if c is a space s=SPACE
// if c is a tabulation s=TABULATION
// if c is non ascii char s=hexa representation of c
// else s=c
//
void u_char_to_hexa_or_code(unichar c,unichar* s) {
if (c==' ') {
   u_strcpy_char(s,"SPACE");
}
else if (c=='\t') {
   u_strcpy_char(s,"TABULATION");
}
else if (c<=128) {
   s[0]=c;
   s[1]='\0';
}
else {
   u_char_to_hexa(c,s);
  }
}



//
// returns true if c is a basic latin letter
//
int u_is_basic_latin_letter(unichar c) {
return ((c>='a' && c<='z') || (c>='A' && c<='Z'));
}


//
// returns true if c is a latin-1 supplement letter
//
int u_is_latin1_supplement_letter(unichar c) {
return (c>=0xC0 && c<=0xFF && c!=0xD7 && c!=0xF7);
}


//
// returns true if c is a latin extended-A letter
//
int u_is_latin_extendedA_letter(unichar c) {
return (c>=0x0100 && c<=0x017F);
}


//
// returns true if c is a latin extended-B letter
//
int u_is_latin_extendedB_letter(unichar c) {
return (c>=0x0180 && c<=0x0233 && c!=0x0220 && c!=0x221);
}


//
// returns true if c is in the IPA extensions letters
//
int u_is_IPA_extensions_letter(unichar c) {
return (c>=0x0250 && c<=0x02AD);
}


//
// returns true if c is a greek letter
//
int u_is_greek_letter(unichar c) {
return (c>=0x0386 && c<=0x03F5 && c!=0x0387 && c!=0x038B
        && c!=0x038D && c!=0x03A2 && c!=0x03CF && c!=0x03D8 && c!=0x03D9);
}


//
// returns true if c is a cyrillic letter
//
int u_is_cyrillic_letter(unichar c) {
return (c>=0x0400 && c<=0x4F9 && (c<0x0482 || c>0x048B) && c!=0x04C5 && c!=0x04C6
        && c!=0x04C9 && c!=0x04CA && c!=0x04CD && c!=0x04CE && c!=0x04CF && c!=0x04F6
        && c!=0x04F7);
}


//
// returns true if c is an armenian letter
//
int u_is_armenian_letter(unichar c) {
return (c>=0x0531 && c<=0x587 && (c<0x0557 || c>0x0560));
}


//
// returns true if c is an hebrew letter
//
int u_is_hebrew_letter(unichar c) {
return (c>=0x05D0 && c<=0x05EA && (c==0x05F0 || c==0x05F1 || c==0x05F2));
}


//
// returns true if c is an arabic letter
//
int u_is_arabic_letter(unichar c) {
return ((c>=0x0621 && c<=0x063A) || (c>=0x0641 && c<=0x64A)
        || (c>=0x0671 && c<=0x06D3) || c==0x06D5
        || (c>=0x06FA && c<=0x06FC));
}


//
// returns true if c is a syriac letter
//
int u_is_syriac_letter(unichar c) {
return (c>=0x0710 && c<=0x072C);
}


//
// returns true if c is a thaana letter
//
int u_is_thaana_letter(unichar c) {
return (c>=0x0780 && c<=0x07A5);
}


//
// returns true if c is a devanagari letter
//
int u_is_devanagari_letter(unichar c) {
return ((c>=0x0905 && c<=0x0939) || (c>=0x093C && c<=0x094D)
        || (c>=0x0950 && c<=0x0954) || (c>=0x0958 && c<=0x0970));
}


//
// returns true if c is a bengali letter
//
int u_is_bengali_letter(unichar c) {
return (c>=0x0985 && c<=0x09B9 && c!=0x098D && c!=0x098E
        && c!=0x0991 && c!=0x0992 && c!=0x09B1 && c!=0x09B3
        && c!=0x09B4 && c!=0x09B5) ||
       (c>=0x09BE && c<=0x09CC && c!=0x09C5 && c!=0x09C6
        && c!=0x09C9 && c!=0x09CA) ||
       (c>=0x09DC && c<=0x09E3 && c!=0x09DE) ||
       (c==0x09F0 || c==0x09F1);
}


//
// returns true if c is a gurmukhi letter
//
int u_is_gurmukhi_letter(unichar c) {
return (c>=0x0A05 && c<=0x0A0A) ||
       (c==0x0A0F || c==0x0A10) ||
       (c>=0x0A13 && c<=0x0A39 && c!=0x0A29 && c!=0x0A31
        && c!=0x0A34 && c!=0x0A37) ||
       (c>=0x0A3E && c<=0x0A42) ||
       (c==0x0A47 || c==0x0A48) ||
       (c>=0x0A4B && c<=0x0A4D) ||
       (c>=0x0A59 && c<=0x0A5E && c!=0x0A5D) ||
       (c>=0x0A70 && c<=0x0A74);
}


//
// returns true if c is a gujarati letter
//
int u_is_gujarati_letter(unichar c) {
return (c>=0x0A85 && c<=0x0ACC && c!=0x0A8C && c!=0x0A8E
        && c!=0x0A92 && c!=0x0AA9 && c!=0x0AB1 && c!=0x0AB4
        && c!=0x0ABA && c!=0x0ABB && c!=0x0AC6 && c!=0x0ACA);
}


//
// returns true if c is an oriya letter
//
int u_is_oriya_letter(unichar c) {
return (c>=0x0B05 && c<=0x0B39 && c!=0x0B0D && c!=0x0B0E
        && c!=0x0B11 && c!=0x0B12 && c!=0x0B29 && c!=0x0B31
        && c!=0x0B34 && c!=0x0B35) ||
       (c>=0x0B3E && c<=0x0B43) ||
       (c==0x0B47 || c==0x0B48 || c==0x0B4B || c==0x0B4C) ||
       (c>=0x0B5C && c<=0x0B61 && c!=0x0B5E);
}


//
// returns true if c is a tamil letter
//
int u_is_tamil_letter(unichar c) {
return (c>=0x0B85 && c<=0x0BCC && c!=0x0B8B && c!=0x0B8C
        && c!=0x0B8D && c!=0x0B91 && c!=0x0B96 && c!=0x0B97
        && c!=0x0B98 && c!=0x0B9B && c!=0x0B9D && c!=0x0BA0
        && c!=0x0BA1 && c!=0x0BA2 && c!=0x0BA5 && c!=0x0BA6
        && c!=0x0BA7 && c!=0x0BAB && c!=0x0BAC && c!=0x0BAD
        && c!=0x0BB6 && c!=0x0BBA && c!=0x0BBB && c!=0x0BBC
        && c!=0x0BBD && c!=0x0BC3 && c!=0x0BC4 && c!=0x0BC5
        && c!=0x0BC9);
}


//
// returns true if c is a telugu letter
//
int u_is_telugu_letter(unichar c) {
return (c>=0x0C05 && c<=0x0C4C && c!=0x0C0D && c!=0x0C11
        && c!=0x0C29 && c!=0x0C34 && c!=0x0C3A && c!=0x0C3B
        && c!=0x0C3C && c!=0x0C3D && c!=0x0C45 && c!=0x0C49);
}


//
// returns true if c is a kannada letter
//
int u_is_kannada_letter(unichar c) {
return (c>=0x0C85 && c<=0x0CCC && c!=0x0C8D && c!=0x0C91
        && c!=0x0CA9 && c!=0x0CB4 && c!=0x0CBA && c!=0x0CBB
        && c!=0x0CBC && c!=0x0CBD && c!=0x0CC5 && c!=0x0CC9) ||
       (c==0x0CDE || c==0x0CE0 || c==0x0CE1);
}


//
// returns true if c is a malayalam letter
//
int u_is_malayalam_letter(unichar c) {
return (c>=0x0D05 && c<=0x0D4C && c!=0x0D0D && c!=0x0D11
        && c!=0x0D29 && c!=0x0D3A && c!=0x0D3B && c!=0x0D44
        && c!=0x0D3C && c!=0x0D3D && c!=0x0D45 && c!=0x0D49)||
       (c==0x0D60 || c==0x0D61);
}


//
// returns true if c is a sinhala letter
//
int u_is_sinhala_letter(unichar c) {
return (c>=0x0D85 && c<=0x0DC6 && c!=0x0D97 && c!=0x0D98
        && c!=0x0D99 && c!=0x0DB2 && c!=0x0DBC && c!=0x0DBE
        && c!=0x0DBF && c!=0x0D) ||
       (c>=0x0DCF && c<=0x0DDF && c!=0x0DD5 && c!=0x0DD7) ||
       (c==0x0DF2 || c==0x0DF3);
}


// returns true if c is a tahi letter
//
int u_is_thai_letter(unichar c) {
return (c>=0x0E01 && c<=0x0E39 && c!=0x0E3F) ||
       (c>=0x0E40 && c<=0x0E4B);
}

// returns true if c is a greek extended letter
//
int u_is_greek_extended_letter(unichar c) {							//$CD:20021115
return (c>=0x1F00 && c<=0x1F15) || (c>=0x1F18 && c<=0x1F1D) ||		//$CD:20021115
       (c>=0x1F20 && c<=0x1F45) || (c>=0x1F48 && c<=0x1F4D) ||		//$CD:20021115
       (c>=0x1F50 && c<=0x1F57) || 									//$CD:20021115
        c==0x1F59 || c==0x1F5B || c==0x1F5D || 						//$CD:20021115
       (c>=0x1F5F && c<=0x1F7D) || (c>=0x1F80 && c<=0x1FB4) ||		//$CD:20021115
       (c>=0x1FB6 && c<=0x1FBC) || (c>=0x1FC2 && c<=0x1FC4) ||		//$CD:20021115
       (c>=0x1FC6 && c<=0x1FCC) || (c>=0x1FD0 && c<=0x1FD3) ||		//$CD:20021115
       (c>=0x1FD6 && c<=0x1FDB) || (c>=0x1FE0 && c<=0x1FEC) ||		//$CD:20021115
       (c>=0x1FF2 && c<=0x1FF4) || (c>=0x1FF6 && c<=0x1FFC);		//$CD:20021115
}																	//$CD:20021115


//
// returns true if c is a letter in a naive way
//
int u_is_letter_internal(unichar c) {
return u_is_basic_latin_letter(c)
       || u_is_latin1_supplement_letter(c)
       || u_is_latin_extendedA_letter(c)
       || u_is_latin_extendedB_letter(c)
       || u_is_IPA_extensions_letter(c)
       || u_is_greek_letter(c)
       || u_is_cyrillic_letter(c)
       || u_is_armenian_letter(c)
       || u_is_hebrew_letter(c)
       || u_is_arabic_letter(c)
       || u_is_thaana_letter(c)
       || u_is_devanagari_letter(c)
       || u_is_bengali_letter(c)
       || u_is_gurmukhi_letter(c)
       || u_is_gujarati_letter(c)
       || u_is_oriya_letter(c)
       || u_is_tamil_letter(c)
       || u_is_telugu_letter(c)
       || u_is_kannada_letter(c)
       || u_is_malayalam_letter(c)
       || u_is_sinhala_letter(c)
       || u_is_thai_letter(c)
       || u_is_greek_extended_letter(c);	//$CD:20021115
}


//
// initializes the array tab_is_letter: bit i = 1 if i is a letter, 0 else
//
char init_unicode_table() {
int i;
for (i=0;i<8192;i++)
  tab_is_letter[i]=0;
for (i=0;i<=0xFFFF;i++)
  if (u_is_letter_internal((unichar)i))
    tab_is_letter[i/8]=(char)(tab_is_letter[i/8]|(1<<(i%8)));
return 1;
}


// initialize automatically the unicode table
int initialize_unicode_table(void) {
  return init_unicode_table()+make_CR();
}


//
// returns true if c is a letter looking up at the unicode table
//
int u_is_letter(unichar c) {
return (tab_is_letter[c/8]&(1<<(c%8)));
}


//
// prints a unicode string
//
void u_prints(unichar *s) {
int i=0;
if (s==NULL) return;
while (s[i]!='\0')
  printf("%c",s[i++]);
}



//
// prints a unicode string in a file
//
void u_fprints(unichar *s,FILE *f) {
int i=0;
while (s[i]!='\0')
  u_fputc((unichar)s[i++],f);
}



//
// prints the reversed of the unicode string s in a file
//
void u_fprints_reverse(unichar *s,FILE *f) {
int i=u_strlen(s)-1;
while (i>=0)
  u_fputc((unichar)s[i--],f);
}



//
// prints a UTF-8 string in a file
//
void u_fprints_utf8(unichar *s,FILE *f) {
int i=0;
while (s[i]!='\0')
  u_fputc_utf8((unichar)s[i++],f);
}



//
// prints a reversed version of a UTF-8 string in a file
//
void u_fprints_utf8_reverse(unichar *s,FILE *f) {
int i=u_strlen(s)-1;
while (i>=0)
  u_fputc_utf8((unichar)s[i--],f);
}



//
// prints a UTF-8 string in a file, replacing spaces by non breakable html spaces
//
void u_fprints_html(unichar *s,FILE *f) {
int i=0;
while (s[i]!='\0') {
  if (s[i]==' ') {
     i++;
     fprintf(f,"&nbsp;");
  }
  else if (s[i]=='<') {
     i++;
     fprintf(f,"&lt;");
  }
  else if (s[i]=='>') {
     i++;
     fprintf(f,"&gt;");
  }
  else {
     u_fputc_utf8((unichar)s[i++],f);
  }
  }
}



//
// prints a reversed version of aUTF-8 string in a file,
// replacing spaces by non breakable html spaces
//
void u_fprints_html_reverse(unichar *s,FILE *f) {
int i=u_strlen(s)-1;
while (i>=0) {
  if (s[i]==' ') {
     i--;
     fprintf(f,"&nbsp;");
  }
  else if (s[i]=='<') {
     i--;
     fprintf(f,"&lt;");
  }
  else if (s[i]=='>') {
     i--;
     fprintf(f,"&gt;");
  }
  else {
     u_fputc_utf8((unichar)s[i--],f);
  }
  }
}



//
// prints a char string in a file
//
void u_fprints_char(char *s,FILE *f) {
int i=0;
while (s[i]!='\0')
  u_fputc((unichar)((unsigned char)s[i++]),f);
}


//
// reads an integer in a file assuming the current char is a digit
//
int u_read_int(FILE *f) {
int n=0;
unichar c;
while (u_is_digit(c=(unichar)u_fgetc(f))) n=(int) n*10+c-'0';
return n;
}


//
// returns true if c is a thai diacritic
//
int u_is_diacritic_thai(unichar c) {
return (c>=0x0e47 && c<=0x0e4c);
}


//
// returns true if c is a thai vowel
//
int u_is_vowel_thai(unichar c) {
return (c>=0x0e40 && c<=0x0e44);
}


//
// returns true if c is a thai diacritic that must be ignored for counting
// left and riht context length
//
int u_is_to_be_ignored_thai(unichar c) {
return (c==0x0e31 || (c>=0x0e34 && c<=0x0e3a) || (c>=0x0e47 && c<=0x0e4e));
}



int u_strlen_thai_without_diacritic(unichar* s) {
int n=0;
int i=0;
while (s[i]!='\0') {
  if (!u_is_to_be_ignored_thai(s[i])) n++;
  i++;
}
return n;
}



//
// puts in the string s the unicode decimal representation of the integer n
//
void u_int_to_string(int n,unichar* s) {
int i=0;
int k=0;
unichar tmp[100];
if (n<0) {
  n=-n;
  s[k++]=(unichar)'-';
}
while (n>=10) {
  tmp[i++]=(unichar)('0'+(n%10));
  n=n/10;
}
tmp[i++]=(unichar)('0'+n);
tmp[i]='\0';
s[i+k]='\0';
for (int j=0;j<i;j++)
  s[j+k]=tmp[i-1-j];
}



void u_reverse_string(unichar* s) {
if (s==NULL) return;
unichar tmp[3000];
int k=0;
for (int i=u_strlen(s)-1;i>=0;i--) {
  tmp[k++]=s[i];
}
tmp[k]='\0';
u_strcpy(s,tmp);
}


long int u_file_size(char* s) {
FILE* f=fopen(s,"rb");
int l;
if (f==NULL) return -1;
fseek(f,0,SEEK_END);
l=ftell(f);
fclose(f);
return l;
}


char make_CR() {
for (int i=0;i<2048;i++) CR[i]='\r';
return 0;
}



int u_fgets(unichar * str, int size, FILE * f) {

  int i = 0;
  int c;

  while ((i < (size - 1)) && ((c = u_fgetc(f)) != EOF)) {
    if (c != '\r') {
      str[i++] = (unichar) c;
      if (c == '\n') { size = i; }
    }
  }

  str[i] = 0;
  return i;
}




int u_parse_int(unichar * str, unichar ** next) {

  int res = 0;

  while (u_is_digit(*str)) { res = res * 10 + (*str - '0'); str++; }

  if (next) { *next = str; }

  return res;
}



unichar * u_strdup(unichar * str) {

  unichar * res = (unichar *) malloc((u_strlen(str) + 1) * sizeof(unichar));

  for (int i = 0; (res[i] = str[i]) != 0; i++);

  return res;
}



void u_fprintf(FILE * f, char * fmt, ...) {


  int i;

  char c;
  unichar uc;

  char * s;
  unichar * us;

  unichar buf[256];

  va_list plist;

  va_start(plist, fmt);

  while (*fmt) {

    //    fprintf(stderr, "c=%c\n", *fmt);

    if (*fmt == '%') {

      fmt++;

      switch (*fmt) {

      case '%':
	u_fputc('%', f);
	break;

      case 'c':
	c = va_arg(plist, int);
	u_fputc((unsigned char) c, f);
	break;

      case 'C':
	uc = va_arg(plist, int);
	u_fputc(uc, f);
	break;

      case 's':
	s = va_arg(plist, char *);
	while (*s) { u_fputc((unsigned char) *s, f); s++; }
	break;

      case 'S':
	us = va_arg(plist, unichar *);
	u_fprints(us, f);
	break;

      case 'd':
	i = va_arg(plist, int);
	u_int_to_string(i, buf);
	u_fprints(buf, f);
	break;

      case 0:    // bad format '%' terminating
	return;

      default:
	u_fputc((unsigned char) *fmt, f);
	break;
      }

    } else {

      u_fputc((unsigned char) *fmt, f);
    }

    fmt++;
  }

  va_end(plist);
}


