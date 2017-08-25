words_dir = join_paths(words_prefix, get_option('datadir'), 'words')


#################
# words/* files #
#################

words_langs = [
  'words/pt_BR.words'
]

foreach lang : words_langs
  install_data(lang, install_dir: words_dir)
endforeach
