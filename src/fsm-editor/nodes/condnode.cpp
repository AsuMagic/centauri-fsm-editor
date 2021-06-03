#include "condnode.hpp"

#include "../editor.hpp"
#include "../visitors/predvisitor.hpp"
#include "../visitors/noderenderer.hpp"

namespace fsme
{
namespace nodes
{

CondNode::CondNode(FsmEditor& editor, ax::NodeEditor::NodeId id) :
	Node(editor, id)
{
	resize_pins(m_inputs, 1);
	set_output_count(1);
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

void CondNode::reset_output_pins()
{
	resize_pins(m_outputs, 0);
	set_output_count(0);
}

widgets::BoolExpressionInput& CondNode::get_expression(ed::PinId output)
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
}
