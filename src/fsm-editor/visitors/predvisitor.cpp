#include "predvisitor.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"

namespace fsme
{

NodePredecessorFinder::NodePredecessorFinder(Node& to_find) :
	m_to_find(&to_find),
	m_found(false)
{}

bool NodePredecessorFinder::find(Node& root, Node& to_find)
{
	NodePredecessorFinder visitor(to_find);
	root.accept(visitor);
	return visitor.m_found;
}

void NodePredecessorFinder::visit(CondNode& node)
{
	check_cond_node(node);
}

void NodePredecessorFinder::visit(IfNode& node)
{
	check_cond_node(node);
}

void NodePredecessorFinder::visit(StateNode& node)
{
	check_node(node);
}

void NodePredecessorFinder::check_cond_node(Node& node)
{
	// is this the node we're looking for?
	if (check_node(node))
	{
		return;
	}

	// visited this node already?
	if (m_visited.find(node.node_id()) != m_visited.end())
	{
		return;
	}

	// visit predecessors
	const FsmEditor& editor = node.editor();
	for (const ed::PinId pin : node.inputs())
	{
		m_visited.emplace(node.node_id());
		for (const LinkInfo& link : editor.get_pin_info(pin)->links)
		{
			editor.get_node_by_pin_id(link.pins.start)->accept(*this);
		}
	}
}

bool NodePredecessorFinder::check_node(Node& node)
{
	m_found |= (&node == m_to_find);
	return m_found;
}

}
