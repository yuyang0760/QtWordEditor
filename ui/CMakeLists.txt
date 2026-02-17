# UI module
set(UI_SOURCES
    mainwindow/MainWindow.cpp
    ribbon/RibbonBar.cpp
    dialogs/FontDialog.cpp
    dialogs/InsertImageDialog.cpp
    dialogs/PageSetupDialog.cpp
    dialogs/ParagraphDialog.cpp
    widgets/FormatToolBar.cpp
    widgets/Ruler.cpp
)

add_library(QtWordEditorUI STATIC ${UI_SOURCES})

target_include_directories(QtWordEditorUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow
        ${CMAKE_CURRENT_SOURCE_DIR}/ribbon
        ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

target_link_libraries(QtWordEditorUI
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
        QtWordEditorCore
        QtWordEditorEditControl
        QtWordEditorGraphics
)