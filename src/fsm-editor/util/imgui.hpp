#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

namespace ed = ax::NodeEditor;

namespace fsme
{

namespace detail
{

/**
 * @brief Sets the default keyboard focus to the next ImGui item.
 *        This is useful when you want to set the focus on a text input by default, for instance.
 */
void imgui_set_default_keyboard_focus();

}

}
