cmake_minimum_required(VERSION 3.19)

project(micro_lang)

add_subdirectory(deps/qc-parser-comb)

file(GLOB_RECURSE sources include/*.hpp src/*.cpp)

add_executable(micro_lang ${sources})
target_include_directories(micro_lang PRIVATE include)
target_link_libraries(micro_lang PRIVATE qcpc)
target_compile_features(micro_lang PRIVATE cxx_std_20)
if (MSVC)
    target_compile_options(micro_lang PRIVATE /W4)
else ()
    target_compile_options(micro_lang PRIVATE -Wall -Wextra -pedantic -Wno-format-security)
endif ()
