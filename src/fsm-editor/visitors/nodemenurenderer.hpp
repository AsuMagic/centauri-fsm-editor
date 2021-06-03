#pragma once

#include "../visitor.hpp"

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to render the contextual menu of a node (typically when right-clicked).
 * @see FsmEditor::render()
 */
class NodeMenuRenderer : public NodeVisitor
{
public:
	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	void render_generic_buttons(Node& node);
};

}
}
