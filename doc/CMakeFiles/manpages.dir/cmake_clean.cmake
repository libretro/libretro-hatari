FILE(REMOVE_RECURSE
  "CMakeFiles/manpages"
  "hatari.1.gz"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/manpages.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
