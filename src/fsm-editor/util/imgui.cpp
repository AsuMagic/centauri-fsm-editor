#include "imgui.hpp"

namespace fsme
{
namespace detail
{

void imgui_set_default_keyboard_focus()
{
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
	{
		ImGui::SetKeyboardFocusHere(0);
	}
}

}
}
