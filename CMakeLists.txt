# Call with the following syntax:
#     cmake -B builds "." -DQt5_DIR="C:/path/to/Qt/lib/cmake/Qt5"
#     cmake --build builds/ --config Release

cmake_minimum_required(VERSION 3.8)

project(ElastixQtGui LANGUAGES CXX)

# Set C++14 standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt5
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Suppress ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# Add source files
set(SOURCES
    "src/main.cpp"
    "src/ElastixQtGui.h"
    "src/ElastixQtGui.cpp"
)

# Use main instead of WinMain for WIN32 application
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")

# Create the executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Link with Qt5 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

# Set Visual Studio startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Create a command to copy the required Qt dlls to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Copying Qt dlls to the build directory")
	