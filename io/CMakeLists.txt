# IO module
set(IO_SOURCES
    serializers/XmlSerializer.cpp
    exporters/PdfExporter.cpp
    importers/TxtImporter.cpp
)

add_library(QtWordEditorIO STATIC ${IO_SOURCES})

target_include_directories(QtWordEditorIO
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/serializers
        ${CMAKE_CURRENT_SOURCE_DIR}/exporters
        ${CMAKE_CURRENT_SOURCE_DIR}/importers
)

target_link_libraries(QtWordEditorIO
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::PrintSupport
        QtWordEditorCore
)