#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

#include <algorithm>
#include <cstddef>
#include <functional>

namespace ed = ax::NodeEditor;

namespace fsme
{

class FsmEditor;
class Node;

namespace detail
{

/**
 * @brief A helper to inherit from in specializations of std::hash for types that can be explicitly converted to
 *        std::size_t.
 */
template<class T>
struct id_hash_helper
{
	std::size_t operator()(const T& value) const noexcept
	{
		return std::size_t(value);
	}
};

/**
 * @brief Erases any element from \p container that is equal to \p to_erase (using std::find).
 */
template<class T, class U>
void erase(T& container, const U& to_erase)
{
	const auto it = std::find(begin(container), end(container), to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

/**
 * @brief Erases any element from \p container that is equal to \p to_erase (using container.find()).
 */
template<class T, class U>
void map_erase(T& container, const U& to_erase)
{
	const auto it = container.find(to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

/**
 * @brief Sets the default keyboard focus to the next ImGui item.
 *        This is useful when you want to set the focus on a text input by default, for instance.
 */
inline void imgui_set_default_keyboard_focus()
{
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
	{
		ImGui::SetKeyboardFocusHere(0);
	}
}

}

/**
 * @brief Types of nodes for the FSM editor graph.
 * @see Node
 */
namespace nodes {}

/**
 * @brief Visitors as defined in the visitor pattern, which operate over nodes.
 * @see Node::accept()
 */
namespace visitors {}

/**
 * @brief dear imgui widgets that abstract some kinds of controls.
 */
namespace widgets {}

}

namespace std
{

template<> struct hash<ed::NodeId> : fsme::detail::id_hash_helper<ed::NodeId> {};
template<> struct hash<ed::PinId> : fsme::detail::id_hash_helper<ed::PinId> {};
template<> struct hash<ed::LinkId> : fsme::detail::id_hash_helper<ed::LinkId> {};

}
