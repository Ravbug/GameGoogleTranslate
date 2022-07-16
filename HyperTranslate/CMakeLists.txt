cmake_minimum_required(VERSION 3.17)
project(MLKitTranslate)

macro(make_lib dir)
    file(GLOB_RECURSE sources "${dir}/*.c" "${dir}/*.cpp" "${dir}/*.h" "${dir}/*.hpp" "${dir}/*.m" "${dir}/*.mm")
    add_library("${dir}" ${sources})
    target_include_directories("${dir}" PUBLIC 
        "${dir}" 
        "." 
        "PromisesObjC/Sources/FBLPromises/include" 
        "GoogleUtilities/GoogleUtilities/Environment/Public"
        "GoogleToolboxForMac/DebugUtils"
        "GoogleUtilitiesComponents/GoogleUtilitiesComponents/Sources/Public"
        "GoogleUtilities/GoogleUtilities/Logger/Public"
    )
endmacro()

macro(enable_arc target)
set_property (TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endmacro()

# GoogleDataTransport
make_lib("GoogleDataTransport")

enable_arc(GoogleDataTransport)
target_compile_definitions("GoogleDataTransport" PUBLIC PB_ENABLE_MALLOC)

make_lib("GoogleToolboxForMac")

make_lib("GoogleUtilities")

make_lib("GoogleUtilitiesComponents")
enable_arc(GoogleUtilitiesComponents)

make_lib("nanopb")

make_lib("PromisesObjC")
enable_arc(PromisesObjC)

make_lib("Protobuf")

make_lib("SSZipArchive")
target_compile_definitions("SSZipArchive" PUBLIC ZLIB_COMPAT)

