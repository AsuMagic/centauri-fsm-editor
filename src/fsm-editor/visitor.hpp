#pragma once

namespace fsme
{
namespace nodes
{
class CondNode;
class StateNode;
class IfNode;
}

/**
 * @brief Interface for a visitor over the node type (as defined by the visitor pattern).
 * @see Node::accept()
 */
class NodeVisitor
{
	public:
	virtual void visit(nodes::CondNode&) = 0;
	virtual void visit(nodes::StateNode&) = 0;
	virtual void visit(nodes::IfNode&) = 0;
};

}
