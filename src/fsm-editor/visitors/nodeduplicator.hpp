#pragma once

#include "../fwd.hpp"
#include "../visitor.hpp"

namespace fsme
{
namespace visitors
{

/**
 * @brief Duplicates a node and copies properties, but does not attempt to copy links.
 */
class NodeDuplicator : public NodeVisitor
{
public:
	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	void setup_generic_clone(Node& original, Node& clone);
	void place_node_under(Node& bottom, Node& top);
};

}
}
