cmake_minimum_required(VERSION 3.20)
project(Editor VERSION 0.0.1)

set(EDITOR_SRC_FILES
    "src/Editor.cpp"
    "src/EditorCamera.cpp"
    "src/Widget/SceneHierarchy.cpp"
    "src/Widget/EntityInspector.cpp"
    "src/Widget/Controls.cpp"
    "src/Widget/MainMenuBar.cpp"
    "src/Widget/ContentBrowser.cpp"
    "src/Widget/ViewportDragDrop.cpp"
    "src/Widget/EntityGizmo.cpp"
    "src/Widget/Toolbar.cpp"
    "src/Util/ResourceTree.cpp"
    "src/Util/FileWatcher.cpp"
)
    
add_executable(Editor ${EDITOR_SRC_FILES})

target_include_directories(Editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Editor Engine)
