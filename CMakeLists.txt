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

# Add subdirectories
add_subdirectory(app)
add_subdirectory(core)
add_subdirectory(editcontrol)
add_subdirectory(graphics)
add_subdirectory(io)
add_subdirectory(ui)

# Link all libraries into the executable
add_executable(QtWordEditor ${APP_SOURCES})

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

# Install target
install(TARGETS QtWordEditor DESTINATION bin)