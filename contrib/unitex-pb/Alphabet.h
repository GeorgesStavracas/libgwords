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
#ifndef AlphabetH
#define AlphabetH

#include "unicode.h"


struct alphabet_ {
  unichar* t[0x10000];
  char t2[0x10000];
       // t2[i] & 1 -> maj
       // t2[i] & 2 -> min
};

typedef struct alphabet_ Alphabet;

Alphabet* load_alphabet(char*);
void free_alphabet(Alphabet*);
int is_upper_of(unichar,unichar,Alphabet*);
int is_equal_or_case_equal(unichar,unichar,Alphabet*);
int is_lower(unichar,Alphabet*);
int is_upper(unichar,Alphabet*);
int is_letter(unichar,Alphabet*);
int all_in_lower(unichar*,Alphabet*);
int all_in_upper(unichar*,Alphabet*);
int is_equal_ignore_case(unichar*,unichar*,Alphabet*);


//---------------------------------------------------------------------------
#endif
