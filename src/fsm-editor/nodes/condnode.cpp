#include "condnode.hpp"

#include "../editor.hpp"
#include "../visitors/predvisitor.hpp"
#include "../visitors/noderenderer.hpp"

namespace fsme
{

CondNode::CondNode(FsmEditor& editor, ax::NodeEditor::NodeId id) :
	Node(editor, id)
{
	resize_pins(m_inputs, 1);
	set_output_count(1);
}

void CondNode::render_context_menu()
{
	ImGui::Text("Conditional block");
	Node::render_context_menu();

	if (ImGui::Button("Clear all outputs"))
	{
		resize_pins(m_outputs, 0);
		ImGui::CloseCurrentPopup();
	}
}

bool CondNode::can_connect(ed::PinId from, ed::PinId to) const
{
	if (!Node::can_connect(from, to))
	{
		return false;
	}

	if (pin_type(from) == PinType::Output)
	{
		return !NodePredecessorFinder::find(const_cast<CondNode&>(*this), *editor().get_node_by_pin_id(to));
	}

	return true;
}

void CondNode::set_output_count(std::size_t i)
{
	resize_pins(m_outputs, std::max(i, std::size_t(1)));
}

void CondNode::erase_output_pin(std::size_t i)
{
	erase_pin(m_outputs, m_outputs.begin() + i);
	set_output_count(m_outputs.size()); // force to 1 minimum
}

BoolExpressionInput& CondNode::get_expression(ed::PinId output)
{
	const auto it = m_conditions.find(output);

	if (it == m_conditions.end())
	{
		const auto p = m_conditions.emplace(std::make_pair(output, editor().new_unique_id()));
		return p.first->second;
	}

	return it->second;
}

}
