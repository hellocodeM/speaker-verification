project(speaker-verification)


aux_source_directory(lib/SV/ LIB_SRC)

add_executable(mfcc-features ${LIB_SRC})
