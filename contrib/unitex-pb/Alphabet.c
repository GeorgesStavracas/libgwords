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
#include "Alphabet.h"
//---------------------------------------------------------------------------



Alphabet* new_alphabet() {
Alphabet* a=(Alphabet*)malloc(sizeof(Alphabet));
for (int i=0;i<0x10000;i++) {
    a->t[i]=NULL;
    a->t2[i]=0;
}
return a;
}


void free_alphabet(Alphabet* a) {
free(a);
}


void ajouter_min_maj(Alphabet* a,unichar min,unichar maj) {
if (a->t[min]==NULL) {
   a->t[min]=(unichar*)malloc(2*sizeof(unichar));
   a->t[min][0]=maj;
   a->t[min][1]='\0';
   return;
}
int L=u_strlen(a->t[min]);
a->t[min]=(unichar*)realloc(a->t[min],(L+2)*sizeof(unichar));
a->t[min][L]=maj;
a->t[min][L+1]='\0';
}



Alphabet* load_alphabet(char* n) {
FILE* f;
f=u_fopen(n,U_READ);
if (f==NULL) return NULL;
Alphabet* a=new_alphabet();
int c;
unichar min,maj;
while ((c=u_fgetc(f))!=EOF) {
      maj=(unichar)c;
      if (maj=='#') {
         // we are in the case of an interval #AZ -> [A..Z]
         min=(unichar)u_fgetc(f);
         maj=(unichar)u_fgetc(f);
         if (min>maj) {
            fprintf(stderr,"Error in alphabet file: for an interval like #AZ, A must be before Z\n");
            u_fclose(f);
            return NULL;
         }
         for (c=min;c<=maj;c++) {
           a->t2[c]=(char)(a->t2[c] | 1);
           a->t2[c]=(char)(a->t2[c] | 2);
           ajouter_min_maj(a,(unichar)c,(unichar)c);
         }
         u_fgetc(f); // reading the \n
      }
      else {
        a->t2[maj]=(char)(a->t2[maj] | 1);
        min=(unichar)u_fgetc(f);
        if (min!='\n') {
          a->t2[min]=(char)(a->t2[min] | 2);
          u_fgetc(f); // reading the \n
          ajouter_min_maj(a,min,maj);
        }
        else {
          // we are in the case of a single (no min/maj distinction like in thai)
          a->t2[maj]=(char)(a->t2[maj] | 2);
          ajouter_min_maj(a,maj,maj);
        }
      }
}
u_fclose(f);
return a;
}



int is_upper_of(unichar min,unichar maj,Alphabet* a) {
if (a->t[min]==NULL) return 0;
int i=0;
while (a->t[min][i]!='\0') {
      if (a->t[min][i]==maj) return 1;
      i++;
}
return 0;
}


int is_equal_or_case_equal(unichar dic_letter,unichar text_letter,Alphabet* a) {
return (dic_letter==text_letter || is_upper_of(dic_letter,text_letter,a));
}


int is_upper(unichar c,Alphabet* a) {
return (a->t2[c] & 1);
}



int is_lower(unichar c,Alphabet* a) {
return (a->t2[c] & 2);
}


int is_letter(unichar c,Alphabet* a) {
return is_upper(c,a)||is_lower(c,a);
}


int all_in_lower(unichar* s,Alphabet* a) {
int i=0;
while (s[i]!='\0') {
  if (!is_lower(s[i],a)) return 0;
  i++;
}
return 1;
}



int all_in_upper(unichar* s,Alphabet* a) {
int i=0;
while (s[i]!='\0') {
  if (!is_upper(s[i],a)) return 0;
  i++;
}
return 1;
}



int is_equal_ignore_case(unichar* dic,unichar* text,Alphabet* a) {
int i=0;
while (dic[i] && is_equal_or_case_equal(dic[i],text[i],a)) i++;
return (dic[i]=='\0' && text[i]=='\0');
}
