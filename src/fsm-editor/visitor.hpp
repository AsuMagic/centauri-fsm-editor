#pragma once

#include "common.hpp"

namespace fsme
{

class CondNode;
class StateNode;
class IfNode;

class NodeVisitor
{
	public:
	virtual void visit(CondNode&) = 0;
	virtual void visit(StateNode&) = 0;
	virtual void visit(IfNode&) = 0;
};

}
