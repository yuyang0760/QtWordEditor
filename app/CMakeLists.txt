set(APP_SOURCES
    app/Application.cpp
    app/main.cpp
)

# Propagate to parent scope for use in root CMakeLists.txt
set(APP_SOURCES ${APP_SOURCES} PARENT_SCOPE)