#pragma once

#include "../visitor.hpp"

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to render a node within the FSM editor graph.
 */
class NodeRenderer : public NodeVisitor
{
public:
	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;
};

}
}
