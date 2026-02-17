cmake_minimum_required(VERSION 3.16)
project(QtWordEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui PrintSupport)

# Enable automatic moc, uic, rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Collect all source files
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE EDITCONTROL_SOURCES "src/editcontrol/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES "src/graphics/*.cpp")
file(GLOB_RECURSE IO_SOURCES "src/io/*.cpp")
file(GLOB_RECURSE UI_SOURCES "src/ui/*.cpp")
file(GLOB_RECURSE APP_SOURCES "src/app/*.cpp")

# Create libraries
add_library(QtWordEditorCore STATIC ${CORE_SOURCES})
add_library(QtWordEditorEditControl STATIC ${EDITCONTROL_SOURCES})
add_library(QtWordEditorGraphics STATIC ${GRAPHICS_SOURCES})
add_library(QtWordEditorIO STATIC ${IO_SOURCES})
add_library(QtWordEditorUI STATIC ${UI_SOURCES})

# Set include directories for libraries
target_include_directories(QtWordEditorCore PUBLIC include)
target_include_directories(QtWordEditorEditControl PUBLIC include)
target_include_directories(QtWordEditorGraphics PUBLIC include)
target_include_directories(QtWordEditorIO PUBLIC include)
target_include_directories(QtWordEditorUI PUBLIC include)

# Set library dependencies
target_link_libraries(QtWordEditorCore PRIVATE Qt6::Core Qt6::Gui)
target_link_libraries(QtWordEditorEditControl PRIVATE Qt6::Core Qt6::Gui QtWordEditorCore)
target_link_libraries(QtWordEditorGraphics PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets QtWordEditorCore)
target_link_libraries(QtWordEditorIO PRIVATE Qt6::Core Qt6::Gui Qt6::PrintSupport QtWordEditorCore)
target_link_libraries(QtWordEditorUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport QtWordEditorCore QtWordEditorEditControl QtWordEditorGraphics)

# Create main executable
add_executable(QtWordEditor ${APP_SOURCES})

# Link libraries to main executable
target_link_libraries(QtWordEditor
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::PrintSupport
    QtWordEditorCore
    QtWordEditorEditControl
    QtWordEditorGraphics
    QtWordEditorIO
    QtWordEditorUI
)

# Set include directories for main executable
target_include_directories(QtWordEditor PRIVATE include)

# Install target
install(TARGETS QtWordEditor DESTINATION bin)