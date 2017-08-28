gw_datadir = join_paths(gw_prefix, get_option('datadir'), 'gwords')


#################
# words/* files #
#################

gw_langs = [
  'words/pt_BR.gwdict'
]

foreach lang : gw_langs
  install_data(lang, install_dir: gw_datadir)
endforeach
