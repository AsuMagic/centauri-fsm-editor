cmake_minimum_required(VERSION 3.5)

project(centauri-fsm-editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/fsm-editor/editor.cpp
    src/fsm-editor/node.cpp
    src/fsm-editor/nodes/condnode.cpp
    src/fsm-editor/nodes/ifnode.cpp
    src/fsm-editor/nodes/statenode.cpp
    src/fsm-editor/visitors/centauriserializer.cpp
    src/fsm-editor/visitors/linkverifier.cpp
    src/fsm-editor/visitors/predvisitor.cpp
    src/fsm-editor/visitors/noderenderer.cpp
    src/fsm-editor/visitors/nodemenurenderer.cpp
    src/fsm-editor/widgets/boolexprinput.cpp
    src/fsm-editor/widgets/stringinput.cpp
    src/imgui-node-editor/crude_json.cpp
    src/imgui-node-editor/imgui_canvas.cpp
    src/imgui-node-editor/imgui_node_editor.cpp
    src/imgui-node-editor/imgui_node_editor_api.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(ImGui-SFML CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML::ImGui-SFML)
