#include "boolexprinput.hpp"

#include "../common.hpp"

#include "imgui_internal.h" // YOLO: needed for the SelectableDontClosePopup flag

#include <fstream>

namespace fsme
{
namespace widgets
{

BoolExpressionInput::BoolExpressionInput(std::size_t id) :
	m_id(id)
{}

void BoolExpressionInput::input_render(bool editable)
{
	switch (m_input_type)
	{
	case ExpressionInputType::PlainLuaExpression:
	{
		if (!editable)
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::Text("%s", m_lua_input.text_buffer.data());
			ImGui::PopFont();
			break;
		}

		if (ImGui::Button("Lua"))
		{
			m_input_type = ExpressionInputType::SimpleExpression;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);
		detail::imgui_set_default_keyboard_focus();
		ImGui::InputText("", m_lua_input.text_buffer.data(), m_lua_input.text_buffer.size());
		break;
	}

	case ExpressionInputType::SimpleExpression:
	{
		if (!editable)
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::Text("%s", m_expr_input.text_preview().c_str());
			ImGui::PopFont();
			break;
		}

		if (ImGui::Button("Simple"))
		{
			m_input_type = ExpressionInputType::PlainLuaExpression;
		}

		ImGui::SameLine();

		ImGui::Button(m_expr_input.text_preview().c_str());

		if (m_autocomplete_provider == nullptr)
		{
			break;
		}

		if (ImGui::IsItemActive())
		{
			ImGui::OpenPopup("##boolExprMenu");
		}

		break;
	}
	}
}

void BoolExpressionInput::popup_render()
{
	const bool is_in_node_editor = !ed::IsSuspended();

	if (is_in_node_editor)
	{
		ed::Suspend();
	}

	ImGui::SetNextWindowPos(ed::CanvasToScreen(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y)), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(200,400));
	if (ImGui::BeginPopup("##boolExprMenu"))
	{
		ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

		switch (m_input_type)
		{
		case ExpressionInputType::PlainLuaExpression:
		{
			break;
		}

		case ExpressionInputType::SimpleExpression:
		{
			ImGui::Text("True if ALL conditions are met:");

			for (auto option_it = m_expr_input.options.begin(); option_it != m_expr_input.options.end();)
			{
				const auto& option = **option_it;
				if (ImGui::MenuItem(option.shorthand.c_str(), nullptr, true))
				{
					option_it = m_expr_input.options.erase(option_it);
				}
				else
				{
					++option_it;
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("%s", option.lua_expression.c_str());
				}
			}

			if (m_expr_input.options.empty())
			{
				ImGui::MenuItem("[select one below]", nullptr, false, false);
			}

			ImGui::Separator();

			BoolExpressionAutocomplete::FilterOptions filter{};
			filter.selected_already = &m_expr_input.options;

			const auto* chosen_option = m_autocomplete_provider->render(filter);

			if (chosen_option != nullptr)
			{
				const auto option_it = m_expr_input.options.find(chosen_option);
				if (option_it != m_expr_input.options.end())
				{
					m_expr_input.options.erase(option_it);
				}
				else
				{
					m_expr_input.options.insert(chosen_option);
				}
			}

			break;
		}
		}

		ImGui::PopItemFlag();
		ImGui::EndPopup();
	}

	if (is_in_node_editor)
	{
		ed::Resume();
	}
}

std::string BoolExpressionInput::as_lua_expression() const
{
	if (m_input_type == ExpressionInputType::PlainLuaExpression)
	{
		return m_lua_input.text_buffer.data();
	}

	std::string ret;

	std::size_t shown = 0;
	for (const auto* option : m_expr_input.options)
	{
		++shown;

		ret += option->lua_expression;
		if (shown != m_expr_input.options.size())
		{
			ret += " and ";
		}
	}

	return ret;
}

const BoolExpressionOption* BoolExpressionAutocomplete::render(FilterOptions options)
{
	const BoolExpressionOption* selected_option = nullptr;

	for (const auto& category_pair : m_categories)
	{
		if (ImGui::BeginMenu(category_pair.first.c_str()))
		{
			for (const auto& option : category_pair.second.options)
			{
				const bool is_selected = options.selected_already->find(&option) != options.selected_already->end();

				if (ImGui::MenuItem(option.get_main_text(), option.get_help_text(), is_selected))
				{
					selected_option = &option;
				}
			}
			ImGui::EndMenu();
		}
	}

	return selected_option;
}

}
}
