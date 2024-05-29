#add_library(contrib_catch_main
#  tools/catch/catch_main.cpp
#        ../../04_structs/war/war_test.cpp
#        ../../04_structs/war/war.h)

target_include_directories(contrib_catch_main
  PUBLIC tools/catch)
