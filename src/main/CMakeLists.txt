file(GLOB_RECURSE
     MAIN_FILES
     "${PROJECT_SOURCE_DIR}/src/main/*.cpp"
     "${PROJECT_SOURCE_DIR}/src/main/*.h")

add_library(vgmcore OBJECT ${MAIN_FILES})

target_include_directories(vgmcore PUBLIC . ./formats ./loaders ./util ./io ./conversion ./components ./components/instr ./components/seq)
#fixme: this stuff shouldn't be public
target_link_libraries(vgmcore
                      PUBLIC fmt::fmt
                             ZLIB::ZLIB
                             minizip
                             tinyxml
														 mio
														 gsl-lite
                      PRIVATE g_options
                              g_warnings)
