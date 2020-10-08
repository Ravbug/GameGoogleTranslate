cmake_minimum_required(VERSION 3.17)
project(GameGoogleTranslate)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable("MinecraftSplitter" 
    "src/OperationBase.cpp" 
    "src/OperationBase.h" 
    "src/MinecraftTranslate.cpp" 
    "src/MinecraftTranslate.h" 
)
target_include_directories("MinecraftSplitter" PRIVATE "include")

add_executable("HadesSJSON" 
    "src/HadesSJSON.cpp"
    "src/OperationBase.cpp" 
    "src/OperationBase.h" 
)