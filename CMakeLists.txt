cmake_minimum_required(VERSION 3.7.0)
project(VitraePluginBasicComposition VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE SrcFiles CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS include/*.h include/*.hpp)

add_library(VitraePluginBasicComposition SHARED ${SrcFiles} ${HeaderFiles})
target_compile_features(VitraePluginBasicComposition PUBLIC cxx_std_20)
target_include_directories(VitraePluginBasicComposition PUBLIC include)

target_link_libraries(VitraePluginBasicComposition PUBLIC VitraeEngine)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
