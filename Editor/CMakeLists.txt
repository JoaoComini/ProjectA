cmake_minimum_required(VERSION 3.20)
project(Editor VERSION 0.0.1)

set(EDITOR_SRC_FILES
    "src/Editor.cpp"
    "src/System/CameraSystem.cpp"
    "src/Widget/SceneHierarchy.cpp"
    "src/Widget/EntityInspector.cpp"
    "src/Widget/Controls.cpp"
    "src/Widget/MainMenuBar.cpp"
    "src/Widget/ContentBrowser.cpp"
)
    
add_executable(Editor ${EDITOR_SRC_FILES})

target_include_directories(Editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Editor Engine)
