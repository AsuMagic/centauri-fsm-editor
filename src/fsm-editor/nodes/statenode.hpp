#pragma once

#include "../node.hpp"
#include "../widgets/stringinput.hpp"

#include <string>

namespace fsme
{
namespace nodes
{

/**
 * @brief A state node class, which holds information for a specific state.
 *        Its output represents transitions that occur from this state towards other states.
 */
class StateNode : public Node
{
	public:
	StateNode(FsmEditor& editor, ed::NodeId id);

	widgets::StringInput& get_name_input();

	void accept(NodeVisitor& v) override;

	private:
	widgets::StringInput m_name_input;
};

inline widgets::StringInput& StateNode::get_name_input()
{
	return m_name_input;
}

inline void StateNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

}
}
