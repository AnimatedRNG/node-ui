cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
SET(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/modules )
message(${CMAKE_MODULE_PATH})
set(EXECUTABLE_NAME NodeUI)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("${PROJECT_BINARY_DIR}/include" "${PROJECT_BINARY_DIR}/include/lodepng")

set(SRC_FILES src/util.cpp src/model.cpp src/screen.cpp src/nodesprite.cpp src/lodepng/lodepng.cpp)
add_executable(${EXECUTABLE_NAME} ${SRC_FILES} src/nodeui.cpp)

find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

set(LIBS ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME} ${LIBS})
target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_range_for)

add_definitions(${OpenGL_DEFINITIONS} ${SDL_DEFINITIONS})

add_library("${EXECUTABLE_NAME}_core" ${SRC_FILES})
target_link_libraries("${EXECUTABLE_NAME}_core" ${LIBS})
target_compile_features("${EXECUTABLE_NAME}_core" PRIVATE cxx_range_for)

find_package(CxxTest)
if(CXXTEST_FOUND)
	# I sincerely apologize for this hack
    include_directories(${CXXTEST_INCLUDE_DIR} "${PROJECT_BINARY_DIR}/include" "${PROJECT_BINARY_DIR}/include/lodepng" ${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
    enable_testing()
    set(UNITTEST_NODE_HEADERS ${CMAKE_BINARY_DIR}/test/node_test.h)
    set(UNITTEST_MODEL_HEADERS ${CMAKE_BINARY_DIR}/test/model_test.h)
    CXXTEST_ADD_TEST(unittest_node gen/unittest_node.cc ${UNITTEST_NODE_HEADERS})
    CXXTEST_ADD_TEST(unittest_model gen/unittest_model.cc ${UNITTEST_MODEL_HEADERS})
    target_link_libraries(unittest_node "${EXECUTABLE_NAME}_core" ${LIBS})
    target_link_libraries(unittest_model "${EXECUTABLE_NAME}_core" ${LIBS})
    target_compile_features(unittest_node PRIVATE cxx_range_for)
    target_compile_features(unittest_model PRIVATE cxx_range_for)
endif()