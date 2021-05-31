#include "noderenderer.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"

namespace fsme
{

void NodeRenderer::visit(CondNode& node)
{
	const auto& editor = node.editor();
	const bool editable = editor.is_node_selected(node.node_id());

	ed::PushStyleColor(ed::StyleColor_NodeBg, ImVec4(0.5, 0.0, 1.0, 0.4));
	ed::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(0.5, 0.0, 1.0, 1.0));
	ed::PushStyleVar(ed::StyleVar_NodeRounding, 10.0f);
	ed::BeginNode(node.node_id());

	if (editable)
	{
		ImGui::Text("Conditional block");
		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			node.set_output_count(node.outputs().size() + 1);
		}
	}
	else
	{
		ImGui::Text("Cond");
	}

	ImGui::BeginGroup();

		ed::BeginPin(node.inputs()[0], ed::PinKind::Input);
		ImGui::Text("->");
		ed::EndPin();

	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();

	for (long i = 0; i < node.outputs().size();)
	{
		const auto& output = node.outputs()[i];
		bool do_erase = false;

		auto& condition = node.get_expression(output);
		ImGui::PushID(&condition);

		if (editable)
		{
			if (ImGui::Button("-"))
			{
				do_erase = true;
			}

			ImGui::SameLine();
		}

		condition.set_autocomplete_provider(editor.get_autocomplete_provider());
		condition.input_render(editable);

		ImGui::SameLine();
		ed::BeginPin(output, ed::PinKind::Output);
		ImGui::Text("->");
		ed::EndPin();

		if (do_erase)
		{
			node.erase_output_pin(i);
		}
		else
		{
			++i;
		}

		ImGui::PopID();
	}

	ImGui::EndGroup();

	ed::EndNode();
	ed::PopStyleColor(2);

	for (const auto& output : node.outputs())
	{
		auto& condition = node.get_expression(output);

		ImGui::PushID(&condition);
		condition.popup_render();
		ImGui::PopID();
	}
}

void NodeRenderer::visit(IfNode& node)
{
	const auto& editor = node.editor();
	const bool editable = editor.is_node_selected(node.node_id());

	ed::PushStyleColor(ed::StyleColor_NodeBg, ImVec4(0.5, 0.0, 1.0, 0.4));
	ed::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(0.5, 0.0, 1.0, 1.0));
	ed::PushStyleVar(ed::StyleVar_NodeRounding, 10.0f);
	ed::BeginNode(node.node_id());

	ed::BeginPin(node.inputs()[0], ed::PinKind::Input);
	ImGui::Text("->");
	ed::EndPin();

	ImGui::SameLine();
	ImGui::Text("If");
	ImGui::SameLine();

	auto& cond = node.get_expression();

	cond.set_autocomplete_provider(editor.get_autocomplete_provider());
	cond.input_render(editable);
	cond.popup_render();

	ImGui::SameLine();
	ImGui::BeginGroup();

		ImGui::SameLine();
		ed::BeginPin(node.outputs()[0], ed::PinKind::Output);
		ImGui::Text("then");
		ed::EndPin();

		ed::BeginPin(node.outputs()[1], ed::PinKind::Output);
		ImGui::Text("else");
		ed::EndPin();

	ImGui::EndGroup();

	ed::EndNode();
	ed::PopStyleVar(1);
	ed::PopStyleColor(2);
}

void NodeRenderer::visit(StateNode& node)
{
	const auto& editor = node.editor();
	const bool editable = editor.is_node_selected(node.node_id());

	ed::PushStyleColor(ed::StyleColor_NodeBg, ImVec4(0.2, 0.6, 0.8, 0.4));
	ed::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(0.2, 0.6, 0.8, 1.0));
	ed::PushStyleVar(ed::StyleVar_NodeRounding, 2.0f);
	ed::BeginNode(node.node_id());

	ImGui::BeginGroup();

	for (const auto& input : node.inputs())
	{
		ed::BeginPin(input, ed::PinKind::Input);
		ImGui::Text("->");
		ed::EndPin();
	}

	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();

	if (editable)
	{
		ImGui::SetNextItemWidth(100.0f);
		detail::imgui_set_default_keyboard_focus();
	}
	node.get_name_input().render(editable);

	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();

	for (const auto& output : node.outputs())
	{
		ed::BeginPin(output, ed::PinKind::Output);
		ImGui::Text("->");
		ed::EndPin();
	}

	ImGui::EndGroup();

	ed::EndNode();
	ed::PopStyleVar(1);
	ed::PopStyleColor(2);
}

}
