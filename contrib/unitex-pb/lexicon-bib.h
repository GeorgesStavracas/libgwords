//---------------------------------------------------------------------------
#ifndef lexicon_bibH
#define lexicon_bibH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DELA.h"
#include "String_hash.h"
#include "unicode.h"
#include "Alphabet.h"
#include "FileName.h"


unsigned char* BIN;
struct INF_codes* INF;
Alphabet* ALPH;

typedef enum {
    DIC_Status_OK         =  0,  /* "Término normal da operação"              */
    DIC_Status_Alph       =  1,  /* "Erro na abertura de arquivo do alphabeto"*/
    DIC_Status_Bin        =  2,  /* "Erro na abertura de arquivo dic.bin"     */
    DIC_Status_Inf        =  3   /* "Erro na abertura de arquivo dic.inf"     */
 } DIC_Status;


void CarregaDicionario(char *path_dic, char *path_alph , DIC_Status *status);
void LiberaDicionario();
void DescompactaEntrada(char *entrada, unichar* saida);

#endif
