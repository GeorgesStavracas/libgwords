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

#ifndef UnicodeH
#define UnicodeH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


//
// these are the binary open modes for unicode text file
//
#define U_READ "rb"
#define U_WRITE "wb"
#define U_APPEND "ab"


// this caracter is used as the first one of a unicode text file
#define U_BYTE_ORDER_MARK 0xFEFF
// and this one is used by contrast with 0xFEFF to determine the byte order
// in the file
#define U_NOT_A_CHAR 0xFFFE

char CR[2048];

typedef unsigned short int unichar;

void u_strcpy(unichar*,unichar*);
void u_strcpy_char(unichar*,char*);
int u_strlen(unichar*);
void u_strcat(unichar*,unichar*);
void u_strcat_char(unichar*,char*);
int u_strcmp(unichar*,unichar*);
int u_strcmp_char(unichar*,char*);
int u_fgetc(FILE*);
int u_fputc(unichar,FILE*);
int u_fputc_utf8(unichar,FILE*);
FILE* u_fopen(char*,char*);
int u_fclose(FILE*);
void u_prints(unichar*);
void u_fprints(unichar*,FILE*);
void u_fprints_reverse(unichar*,FILE*);
void u_fprints_utf8(unichar*,FILE*);
void u_fprints_utf8_reverse(unichar*,FILE*);
void u_fprints_html(unichar*,FILE*);
void u_fprints_html_reverse(unichar*,FILE*);
void u_fprints_char(char*,FILE*);
void u_to_char(char*,unichar*);
int u_read_int(FILE*);
void u_char_to_hexa(unichar,unichar*);
void u_char_to_hexa_or_code(unichar,unichar*);
void u_int_to_string(int,unichar*);
int u_read_line(FILE*,unichar*);
int only_spaces(unichar*);
void u_reverse_string(unichar*);
int u_strlen_thai_without_diacritic(unichar*);

int u_is_digit(unichar);
int u_is_basic_latin_letter(unichar);
int u_is_latin1_supplement_letter(unichar);
int u_is_latin_extendedA_letter(unichar);
int u_is_latin_extendedB_letter(unichar);
int u_is_IPA_extensions_letter(unichar);
int u_is_greek_letter(unichar);
int u_is_cyrillic_letter(unichar);
int u_is_armenian_letter(unichar);
int u_is_hebrew_letter(unichar);
int u_is_arabic_letter(unichar);
int u_is_thaana_letter(unichar);
int u_is_devanagari_letter(unichar);
int u_is_bengali_letter(unichar);
int u_is_gurmukhi_letter(unichar);
int u_is_gujarati_letter(unichar);
int u_is_oriya_letter(unichar);
int u_is_tamil_letter(unichar);
int u_is_telugu_letter(unichar);
int u_is_kannada_letter(unichar);
int u_is_malayalam_letter(unichar);
int u_is_sinhala_letter(unichar);
int u_is_thai_letter(unichar);
int u_is_greek_extended_letter(unichar);	//$CD:20021115
int u_is_letter(unichar);
int u_is_diacritic_thai(unichar);
int u_is_to_be_ignored_thai(unichar);
int u_is_vowel_thai(unichar);
long int u_file_size(char*);
char make_CR();


int u_fgets(unichar * str, int size, FILE * f);
int u_parse_int(unichar * str, unichar ** next);
unichar * u_strdup(unichar * str);

void u_fprintf(FILE * f, char * fmt, ...);

int initialize_unicode_table (void);

#endif
