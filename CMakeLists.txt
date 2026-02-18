cmake_minimum_required(VERSION 3.16)
project(QtWordEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /FS)
endif()

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui PrintSupport OpenGLWidgets LinguistTools)

# Enable automatic moc, uic, rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Collect all source files and headers
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/core/*.h")
file(GLOB_RECURSE EDITCONTROL_SOURCES "src/editcontrol/*.cpp")
file(GLOB_RECURSE EDITCONTROL_HEADERS "include/editcontrol/*.h")
file(GLOB_RECURSE GRAPHICS_SOURCES "src/graphics/*.cpp")
file(GLOB_RECURSE GRAPHICS_HEADERS "include/graphics/*.h")
file(GLOB_RECURSE IO_SOURCES "src/io/*.cpp")
file(GLOB_RECURSE IO_HEADERS "include/io/*.h")
file(GLOB_RECURSE UI_SOURCES "src/ui/*.cpp")
file(GLOB_RECURSE UI_HEADERS "include/ui/*.h")
file(GLOB_RECURSE APP_SOURCES "src/app/*.cpp")
file(GLOB_RECURSE APP_HEADERS "include/app/*.h")

# Create libraries
add_library(QtWordEditorCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})
add_library(QtWordEditorEditControl STATIC ${EDITCONTROL_SOURCES} ${EDITCONTROL_HEADERS})
add_library(QtWordEditorGraphics STATIC ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})
add_library(QtWordEditorIO STATIC ${IO_SOURCES} ${IO_HEADERS})
add_library(QtWordEditorUI STATIC ${UI_SOURCES} ${UI_HEADERS})

# Set MSVC specific compile options for all targets
if(MSVC)
    target_compile_options(QtWordEditorCore PRIVATE /Zc:__cplusplus /permissive- /FS)
    target_compile_options(QtWordEditorEditControl PRIVATE /Zc:__cplusplus /permissive- /FS)
    target_compile_options(QtWordEditorGraphics PRIVATE /Zc:__cplusplus /permissive- /FS)
    target_compile_options(QtWordEditorIO PRIVATE /Zc:__cplusplus /permissive- /FS)
    target_compile_options(QtWordEditorUI PRIVATE /Zc:__cplusplus /permissive- /FS)
endif()

# Set include directories for libraries
target_include_directories(QtWordEditorCore PUBLIC include)
target_include_directories(QtWordEditorEditControl PUBLIC include)
target_include_directories(QtWordEditorGraphics PUBLIC include)
target_include_directories(QtWordEditorIO PUBLIC include)
target_include_directories(QtWordEditorUI PUBLIC include)

# Set library dependencies
target_link_libraries(QtWordEditorCore PRIVATE Qt6::Core Qt6::Gui)
target_link_libraries(QtWordEditorEditControl PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets QtWordEditorCore QtWordEditorGraphics)
target_link_libraries(QtWordEditorGraphics PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets QtWordEditorCore)
target_link_libraries(QtWordEditorIO PRIVATE Qt6::Core Qt6::Gui Qt6::PrintSupport QtWordEditorCore)
target_link_libraries(QtWordEditorUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport QtWordEditorCore QtWordEditorEditControl QtWordEditorGraphics)

# Create main executable
add_executable(QtWordEditor ${APP_SOURCES} ${APP_HEADERS})

# Add translation files to executable
if(TRANSLATION_FILES)
    target_sources(QtWordEditor PRIVATE ${QM_FILES})
endif()

# Set MSVC compile options for executable
if(MSVC)
    target_compile_options(QtWordEditor PRIVATE /Zc:__cplusplus /permissive- /FS)
endif()

# Link libraries to main executable
target_link_libraries(QtWordEditor
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::PrintSupport
    Qt6::OpenGLWidgets
    QtWordEditorCore
    QtWordEditorEditControl
    QtWordEditorGraphics
    QtWordEditorIO
    QtWordEditorUI
)

# Set include directories for main executable
target_include_directories(QtWordEditor PRIVATE include)

# Translation files
set(TRANSLATIONS_DIR "${CMAKE_SOURCE_DIR}/translations")
file(GLOB TRANSLATION_FILES "${TRANSLATIONS_DIR}/*.ts")

if(TRANSLATION_FILES)
    qt6_add_translation(QM_FILES ${TRANSLATION_FILES} OPTIONS -silent)
    add_custom_target(translations ALL DEPENDS ${QM_FILES})
    
    # Install translation files
    install(FILES ${QM_FILES} DESTINATION translations)
endif()

# Install target
install(TARGETS QtWordEditor DESTINATION bin)