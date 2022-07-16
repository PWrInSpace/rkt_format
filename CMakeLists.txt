cmake_minimum_required(VERSION 3.16)

# Project setup
set(PROJECT_NAME "parser")
set(TARGET_NAME "parse")
set(BIN_DIR ${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE})
set(DEPS_DIR ${CMAKE_SOURCE_DIR}/code/dependencies)
project(${PROJECT_NAME})

######### TARGET RULES #########

# Defines
set(CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS /fp:except- /W4 /sdl")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} /DCON_DEVELOPER /DCON_DEBUG /DCON_SLOW_BUILD")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /DCON_DEBUG /DCON_DEVELOPER")

# Main project files
set(INC_DIR code)
set(SRC_DIR code)
set(SRC_LIST "${SRC_DIR}/main.cpp")
#set(SRC_LIST "${SRC_DIR}/sys_windows.cpp"
#             "${DEPS_DIR}/implot/implot.cpp" "${DEPS_DIR}/implot/implot_items.cpp")

# Target setup
add_executable(${TARGET_NAME} ${SRC_LIST})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20 )
target_link_libraries(${TARGET_NAME} PRIVATE opengl32 )
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})


# Includes
target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIR})
