#include "nodemenurenderer.hpp"

#include "../nodes/nodes.hpp"
#include "../editor.hpp"

namespace fsme
{
namespace visitors
{

void NodeMenuRenderer::visit(nodes::CondNode& node)
{
	ImGui::Text("Conditional block");
	render_generic_buttons(node);

	if (ImGui::Button("Clear all outputs"))
	{
		node.reset_output_pins();
		ImGui::CloseCurrentPopup();
	}
}

void NodeMenuRenderer::visit(nodes::IfNode& node)
{
	ImGui::Text("If node");
	render_generic_buttons(node);
}

void NodeMenuRenderer::visit(nodes::StateNode& node)
{
	ImGui::Text("State node");
	render_generic_buttons(node);
}

void NodeMenuRenderer::render_generic_buttons(Node& node)
{
	auto& editor = node.editor();

	if (ImGui::Button("Delete"))
	{
		editor.destroy_node(node.node_id());
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Erase links"))
	{
		for (const auto& input : node.inputs())
		{
			editor.destroy_links_involving(input);
		}

		for (const auto& output : node.outputs())
		{
			editor.destroy_links_involving(output);
		}

		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Duplicate"))
	{
		ImGui::CloseCurrentPopup();
	}
}

}
}
