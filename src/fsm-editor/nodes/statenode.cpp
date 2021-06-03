#include "statenode.hpp"

#include "../editor.hpp"

namespace fsme
{
namespace nodes
{

StateNode::StateNode(FsmEditor& editor, ax::NodeEditor::NodeId id) :
	Node(editor, id)
{
	resize_pins(m_inputs, 1);
	resize_pins(m_outputs, 1);

	m_name_input.set_hint("State name");
}

}
}
