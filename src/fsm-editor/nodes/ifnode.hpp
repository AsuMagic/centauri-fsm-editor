#pragma once

#include "../node.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{
namespace nodes
{

/**
 * @brief A node that implements a traditional if-else block.
 */
class IfNode : public Node
{
	public:
	IfNode(FsmEditor& editor, ed::NodeId id);

	void accept(NodeVisitor& v) override;

	widgets::BoolExpressionInput& get_expression();

	private:
	widgets::BoolExpressionInput m_cond;
};

inline void IfNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

inline widgets::BoolExpressionInput& IfNode::get_expression()
{
	return m_cond;
}

}
}
