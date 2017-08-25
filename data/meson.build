gw_datadir = join_paths(gw_prefix, get_option('datadir'), 'words')


#################
# words/* files #
#################

gw_langs = [
  'words/pt_BR.words'
]

foreach lang : gw_langs
  install_data(lang, install_dir: gw_datadir)
endforeach
