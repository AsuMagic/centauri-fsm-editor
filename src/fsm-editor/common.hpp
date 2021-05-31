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

template<class T>
struct id_hash_helper
{
	std::size_t operator()(const T& value) const noexcept
	{
		return std::size_t(value);
	}
};

template<class T, class U>
void erase(T& container, const U& to_erase)
{
	const auto it = std::find(begin(container), end(container), to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

template<class T, class U>
void map_erase(T& container, const U& to_erase)
{
	const auto it = container.find(to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

inline void imgui_set_default_keyboard_focus()
{
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
	{
		ImGui::SetKeyboardFocusHere(0);
	}
}

}

}

namespace std
{

template<> struct hash<ed::NodeId> : fsme::detail::id_hash_helper<ed::NodeId> {};
template<> struct hash<ed::PinId> : fsme::detail::id_hash_helper<ed::PinId> {};
template<> struct hash<ed::LinkId> : fsme::detail::id_hash_helper<ed::LinkId> {};

}
