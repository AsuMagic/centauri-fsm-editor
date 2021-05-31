#include "ifnode.hpp"

#include "../editor.hpp"
#include "../visitors/predvisitor.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{

IfNode::IfNode(FsmEditor& editor, ed::NodeId id) :
	Node(editor, id),
	m_cond(editor.new_unique_id())
{
	resize_pins(m_inputs, 1);
	resize_pins(m_outputs, 2);
}

void IfNode::render_context_menu()
{
	ImGui::Text("If node");
	Node::render_context_menu();
}

bool IfNode::can_connect(ed::PinId from, ed::PinId to) const
{
	if (!Node::can_connect(from, to))
	{
		return false;
	}

	if (pin_type(from) == PinType::Output)
	{
		return !NodePredecessorFinder::find(const_cast<IfNode&>(*this), *editor().get_node_by_pin_id(to));
	}

	return true;
}

}
