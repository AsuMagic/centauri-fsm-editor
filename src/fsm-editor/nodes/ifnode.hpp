#pragma once

#include "../node.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{

class IfNode : public Node
{
	public:
	IfNode(FsmEditor& editor, ed::NodeId id);

	void render_context_menu() override;

	void accept(NodeVisitor& v) override;

	bool can_connect(ed::PinId from, ed::PinId to) const override;

	BoolExpressionInput& get_expression();

	private:
	BoolExpressionInput m_cond;
};

inline void IfNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

inline BoolExpressionInput& IfNode::get_expression()
{
	return m_cond;
}

}
