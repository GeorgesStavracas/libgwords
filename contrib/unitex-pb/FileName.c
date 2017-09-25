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
//---------------------------------------------------------------------------
#include "FileName.h"

#include <string.h>
//---------------------------------------------------------------------------


void name_without_extension(char* s, char* res) {
int l;
strcpy(res,s);
l=strlen(res)-1;
while (l>=0 && res[l]!='/' && res[l]!='\\' && res[l]!='.') {
   l--;
}
if (res[l]=='.') res[l]='\0';
}



void get_filename_path(char* s,char* res) {
int l;
strcpy(res,s);
l=strlen(res)-1;
while (l>=0 && res[l]!='/' && res[l]!='\\') {
   l--;
}
if (l>=0) res[l+1]='\0';
else res[0]='\0';
}


void get_snt_path(char* s, char* res) {
int l;
strcpy(res,s);
l=strlen(res)-1;
while (l>=0 && res[l]!='/' && res[l]!='\\' && res[l]!='.') {
   l--;
}
if (res[l]=='.') res[l]='\0';
strcat(res,"_snt/");
}



void name_without_path(char* s,char* res) {
int l=strlen(s)-1;
while (l>=0 && s[l]!='/' && s[l]!='\\') {
   l--;
}
if (l<0) {
   strcpy(res,s);
   return;
}
int k=0;
for (int i=l+1;s[i]!='\0';i++) {
   res[k++]=s[i];
}
res[k]='\0';
}
