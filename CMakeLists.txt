cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
SET(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/modules )
message(${CMAKE_MODULE_PATH})
set(EXECUTABLE_NAME NodeUI)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

find_package(Jsoncpp REQUIRED)

# XLib will be required for the time being.
# Later I'll try to find a workaround.
find_package(XLib REQUIRED)

set(SRC_FILES src/util.cpp src/model.cpp src/screen.cpp src/controller.cpp src/keyboard_input.cpp src/nodesprite.cpp)

set(DEFINITIONS ${Qt5Widgets_DEFINITIONS} ${Qt5Core_DEFINITIONS} ${Qt5Gui_DEFINITIONS})
add_definitions(${DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_cpp(SCREEN_MOC "${PROJECT_BINARY_DIR}/include/screen.h")

add_executable(${EXECUTABLE_NAME} ${SRC_FILES} ${SCREEN_MOC} src/nodeui.cpp)

set(INCLUDE_DIRS "${PROJECT_BINARY_DIR}/include" ${JSONCPP_INCLUDE_DIRS} ${Qt5Core_INCLUDES} ${Qt5Gui_INCLUDES} ${Qt5Widgets_INCLUDES} ${XLIB_INCLUDE_PATH})
include_directories(${INCLUDE_DIRS})

set(LIBS ${JSONCPP_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${XLIB_LIBRARY})

add_library("${EXECUTABLE_NAME}_core" ${SRC_FILES} ${SCREEN_MOC})

target_link_libraries("${EXECUTABLE_NAME}_core" ${LIBS})
target_compile_features("${EXECUTABLE_NAME}_core" PRIVATE cxx_range_for)

target_link_libraries(${EXECUTABLE_NAME} "${EXECUTABLE_NAME}_core")
target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_range_for)

find_package(CxxTest)
if(CXXTEST_FOUND)
	# I sincerely apologize for this hack
    include_directories(${CXXTEST_INCLUDE_DIR} ${INCLUDE_DIRS})
    enable_testing()
    set(UNITTEST_NODE_HEADERS ${CMAKE_BINARY_DIR}/test/node_test.h)
    set(UNITTEST_MODEL_HEADERS ${CMAKE_BINARY_DIR}/test/model_test.h)
    add_definitions(${DEFINITIONS})
    CXXTEST_ADD_TEST(unittest_node gen/unittest_node.cc ${UNITTEST_NODE_HEADERS})
    CXXTEST_ADD_TEST(unittest_model gen/unittest_model.cc ${UNITTEST_MODEL_HEADERS})
    target_link_libraries(unittest_node "${EXECUTABLE_NAME}_core" ${LIBS})
    target_link_libraries(unittest_model "${EXECUTABLE_NAME}_core" ${LIBS})
    target_compile_features(unittest_node PRIVATE cxx_range_for)
    target_compile_features(unittest_model PRIVATE cxx_range_for)
endif()