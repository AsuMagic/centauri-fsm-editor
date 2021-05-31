#pragma once

#include "../node.hpp"
#include "../widgets/stringinput.hpp"

#include <string>

namespace fsme
{

class StateNode : public Node
{
	public:
	StateNode(FsmEditor& editor, ed::NodeId id);

	void render_context_menu() override;

	StringInput& get_name_input();

	void accept(NodeVisitor& v) override;

	private:
	StringInput m_name_input;
};

inline StringInput& StateNode::get_name_input()
{
	return m_name_input;
}

inline void StateNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

}
