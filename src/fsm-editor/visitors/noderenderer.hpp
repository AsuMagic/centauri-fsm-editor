#pragma once

#include "../visitor.hpp"

namespace fsme
{

class NodeRenderer : public NodeVisitor
{
public:
	void visit(CondNode& node) override;
	void visit(IfNode& node) override;
	void visit(StateNode& node) override;
};

}
