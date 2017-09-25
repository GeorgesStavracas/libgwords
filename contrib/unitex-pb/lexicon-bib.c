//---------------------------------------------------------------------------

#include "lexicon-bib.h"

//---------------------------------------------------------------------------




/*----------------------------------------------------------------------
A   rotina  'CarregaDicionario'   carrega   o  dicionário  compactado.
O parâmetro 'path_dic'  deve fornecer a localização (absoluta ou
relativa) do arquivo que contém este dicionário. O arquivo com os
códigos de flexão, deve estar no mesma mesma localização mas deve ter
a extensão .inf ao contrário de .bin do dicionário compactado.
Os  códigos  de erro  que podem  ser devolvidos são:
'DIC_Status_Alph',  'DIC_Status_Inf' e 'DIC_Status_Bin'.  Os
parâmetro 'path_dic' e 'path_alph' não são alterados.
----------------------------------------------------------------------*/

void CarregaDicionario(char *path_dic, char *path_alph , DIC_Status *status)
{
   char nom_inf[1000];
   char nom_bin[1000];

   initialize_unicode_table ();

   ALPH=load_alphabet(path_alph);
   if (ALPH==NULL) {
      *status = DIC_Status_Alph;
   } else {
      name_without_extension(path_dic,nom_inf);
      strcat(nom_inf,".inf");
      name_without_extension(path_dic,nom_bin);
      strcat(nom_bin,".bin");
      INF=load_INF_file(nom_inf);
      if (INF==NULL) {
         *status = DIC_Status_Inf;
      } else {
         BIN=load_BIN_file(nom_bin);
         if (BIN==NULL) {
            free_INF_codes(INF);
            *status = DIC_Status_Bin;
         } else {
			*status = DIC_Status_OK;
		 }
	  }
   }
}

/*----------------------------------------------------------------------
A rotina 'LiberaDicionario' libera a   memória dinâmica alocada   pelo
pacote, que pode então ser desativado, ou então reativado com uma nova
execução da  rotina 'CarregaDicionario'.  O  código retornado deve ser
sempre 'KLS_Status_OK'.
----------------------------------------------------------------------*/

void LiberaDicionario()
{
   free_INF_codes(INF);
   free(BIN);
   free(ALPH);
//   status = DIC_Status_OK;
}


void explorer_bin_simples_tokens(int pos,unichar* contenu,unichar* entry,
                                 int string_pos,int token_number, unichar* saida) {
   int n_transitions;
   int ref;
   n_transitions=((unsigned char)BIN[pos])*256+(unsigned char)BIN[pos+1];
   pos=pos+2;
   if (contenu[string_pos]=='\0') {
      // if we are at the end of the string
      entry[string_pos]='\0';
      if (!(n_transitions & 32768)) {
         // we save the token only if it has the good priority
         ref=((unsigned char)BIN[pos])*256*256+((unsigned char)BIN[pos+1])*256+(unsigned char)BIN[pos+2];
         struct token_list* tmp=INF->tab[ref];
         while (tmp!=NULL) {
			unichar temp[100];
			temp[0] = '\0';
            uncompress_entry(entry,tmp->token,temp);
			u_strcat(saida,temp);
            u_strcat_char(saida,"\n");
            tmp=tmp->suivant;
         }
      }
      return;
   }
   if ((n_transitions & 32768)) {
      // if we are in a normal node, we remove the control bit to
      // have the good number of transitions
      n_transitions=n_transitions-32768;
   } else {
     // if we are in a final node, we must jump after the reference to the INF line number
      pos=pos+3;
   }
   for (int i=0;i<n_transitions;i++) {
      unichar c=(unichar)(((unsigned char)BIN[pos])*256+(unsigned char)BIN[pos+1]);
      pos=pos+2;
      int adr=((unsigned char)BIN[pos])*256*256+((unsigned char)BIN[pos+1])*256+(unsigned char)BIN[pos+2];
      pos=pos+3;
      if (is_equal_or_case_equal(c,contenu[string_pos],ALPH)) {
         // we explore the rest of the dictionary only
         // if the dico char is compatible with the token char
         entry[string_pos]=c;
         explorer_bin_simples_tokens(adr,contenu,entry,string_pos+1,token_number,saida);
      }
   }
}

void DescompactaEntrada(char *entrada, unichar* saida)
{
   unichar entry[1000];
   unichar palavra[1000];

   palavra[0] = '\0';
   saida[0]= '\0';
   u_strcat_char(palavra, entrada);
   explorer_bin_simples_tokens(4,palavra,entry,0,0,saida);
}
