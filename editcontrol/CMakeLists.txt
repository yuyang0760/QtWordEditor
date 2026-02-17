# EditControl module
set(EDITCONTROL_SOURCES
    cursor/Cursor.cpp
    formatting/FormatController.cpp
    handlers/EditEventHandler.cpp
    selection/Selection.cpp
)

add_library(QtWordEditorEditControl STATIC ${EDITCONTROL_SOURCES})

target_include_directories(QtWordEditorEditControl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cursor
        ${CMAKE_CURRENT_SOURCE_DIR}/formatting
        ${CMAKE_CURRENT_SOURCE_DIR}/handlers
        ${CMAKE_CURRENT_SOURCE_DIR}/selection
)

target_link_libraries(QtWordEditorEditControl
    PRIVATE
        Qt6::Core
        Qt6::Gui
        QtWordEditorCore
)