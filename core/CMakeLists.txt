# Core module
set(CORE_SOURCES
    document/Document.cpp
    document/Section.cpp
    document/Block.cpp
    document/ParagraphBlock.cpp
    document/Span.cpp
    document/CharacterStyle.cpp
    document/ParagraphStyle.cpp
    document/ImageBlock.cpp
    document/TableBlock.cpp
    document/Page.cpp
    commands/EditCommand.cpp
    commands/InsertTextCommand.cpp
    commands/RemoveTextCommand.cpp
    commands/InsertBlockCommand.cpp
    commands/RemoveBlockCommand.cpp
    commands/SetCharacterStyleCommand.cpp
    layout/LayoutEngine.cpp
    layout/PageBuilder.cpp
    styles/StyleManager.cpp
    utils/FontUtils.cpp
)

add_library(QtWordEditorCore STATIC ${CORE_SOURCES})

target_include_directories(QtWordEditorCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/document
        ${CMAKE_CURRENT_SOURCE_DIR}/commands
        ${CMAKE_CURRENT_SOURCE_DIR}/layout
        ${CMAKE_CURRENT_SOURCE_DIR}/styles
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(QtWordEditorCore
    PRIVATE
        Qt6::Core
        Qt6::Gui
)