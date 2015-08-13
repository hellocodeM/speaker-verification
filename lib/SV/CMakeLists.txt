project(speaker-verification)

set(LIB Aquila Ooura)

include_directories(D:/aquila/build/Debug/)

add_library(mfcc-features mfcc-features.cc)
target_link_libraries(mfcc-features ${LIB})

add_library(noise-reduction noise-reduction.cc)
target_link_libraries(noise-reduction ${LIB})
