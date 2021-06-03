#include "ifnode.hpp"

#include "../editor.hpp"
#include "../visitors/predvisitor.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{
namespace nodes
{

IfNode::IfNode(FsmEditor& editor, ed::NodeId id) :
	Node(editor, id),
	m_cond(editor.new_unique_id())
{
	resize_pins(m_inputs, 1);
	resize_pins(m_outputs, 2);
}

}
}
