# Graphics module
set(GRAPHICS_SOURCES
    factory/BlockItemFactory.cpp
    items/BaseBlockItem.cpp
    items/CursorItem.cpp
    items/ImageBlockItem.cpp
    items/PageItem.cpp
    items/SelectionItem.cpp
    items/TableBlockItem.cpp
    items/TextBlockItem.cpp
    scene/DocumentScene.cpp
    view/DocumentView.cpp
)

add_library(QtWordEditorGraphics STATIC ${GRAPHICS_SOURCES})

target_include_directories(QtWordEditorGraphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/factory
        ${CMAKE_CURRENT_SOURCE_DIR}/items
        ${CMAKE_CURRENT_SOURCE_DIR}/scene
        ${CMAKE_CURRENT_SOURCE_DIR}/view
)

target_link_libraries(QtWordEditorGraphics
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        QtWordEditorCore
)