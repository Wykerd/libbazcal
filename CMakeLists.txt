cmake_minimum_required(VERSION 3.10)

project(bazcal)

add_executable(bzcore src/main.c)

add_library(bazcal SHARED 
    src/regression.c 
    src/quickselect.c 
    src/parser.c 
    src/fetch.c 
    src/calc.c 
    src/db.c
    src/ahpredict.c
    src/loop.c
)

target_compile_options(bazcal PRIVATE -Wall)

include_directories(include)

target_link_libraries(bzcore PUBLIC bazcal)

target_link_libraries(bazcal PUBLIC curl sqlite3 jansson)