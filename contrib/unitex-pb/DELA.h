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


#ifndef DELAH
#define DELAH
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "String_hash.h"


// maximum size of a DELA line
#define DIC_LINE_SIZE 2000
// maximum number of semantic codes (Hum,Conc,z1,...) per line
#define MAX_SEMANTIC_CODES 20
// maximum number of flexional codes (ms,Kf,W,...) per line
#define MAX_FLEXIONAL_CODES 20




struct dic_entry_ {
  unichar* inflected;
  unichar* lemma;
  char n_semantic_codes;  // number of semantic codes
                          // the first may be the grammatical code
  unichar* semantic_codes[MAX_SEMANTIC_CODES];
  char n_flexional_codes; // number of flexional codes
  unichar* flexional_codes[MAX_FLEXIONAL_CODES];
};

typedef struct dic_entry_ dic_entry;



struct token_list {
  unichar* token;
  struct token_list* suivant;
};


struct INF_codes {
   struct token_list** tab;
   int N;
};


int read_DELA_line(FILE*,unichar*);
dic_entry* tokenize_DELA_line(unichar*);
void get_compressed_line(dic_entry*,unichar*);
struct token_list* tokenize_compressed_info(unichar*);
void free_token_list(struct token_list*);
void uncompress_entry(unichar*,unichar*,unichar*);
struct INF_codes* load_INF_file(char*);
void free_INF_codes(struct INF_codes*);
unsigned char* load_BIN_file(char*);
void rebuild_dictionary(unsigned char*,struct INF_codes*,FILE*);
void tokenize_DELA_line_into_inflected_and_code(unichar*,unichar*,unichar*);
void extract_semantic_codes(char*,struct string_hash*);
void tokenize_DELA_line_into_3_parts(unichar*,unichar*,unichar*,unichar*);
void check_DELAS_line(unichar*,FILE*,int,char*,struct string_hash*,struct string_hash*);
void check_DELAF_line(unichar*,FILE*,int,char*,struct string_hash*,struct string_hash*);
int warning_on_code(unichar*,unichar*);
int contains_unprotected_equal_sign(unichar*);
void replace_unprotected_equal_sign(unichar*,unichar);
void unprotect_equal_signs(unichar*);

#endif

