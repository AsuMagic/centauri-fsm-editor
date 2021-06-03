#include "linkverifier.hpp"

#include "../nodes/nodes.hpp"
#include "../editor.hpp"
#include "predvisitor.hpp"

namespace fsme
{
namespace visitors
{

const char* get_feasibility_string(LinkFeasibility feasibility)
{
	switch (feasibility)
	{
	case LinkFeasibility::FEASIBLE:
	return "Feasible.";

	case LinkFeasibility::CANNOT_LINK_INPUTS:
	return "Cannot link inputs together.";

	case LinkFeasibility::CANNOT_LINK_OUTPUTS:
	return "Cannot link outputs together.";

	case LinkFeasibility::CANNOT_LINK_TO_ITSELF:
	return "Cannot link a node to itself.";

	case LinkFeasibility::CANNOT_LINK_TO_PREDECESSOR:
	return "This node cannot be connected to a predecessor.";

	case LinkFeasibility::DUPLICATE_LINK:
	return "An indentical link exists already.";

	case LinkFeasibility::INVALID_NODE:
	return "One of the nodes within the link is invalid, for some reason.";
	}
}

LinkFeasibility LinkVerifier::verify(const FsmEditor& editor, ed::PinId& a, ed::PinId& b)
{
	Node* a_node = editor.get_node_by_pin_id(a);
	Node* b_node = editor.get_node_by_pin_id(b);

	const auto type_pair = std::make_pair(a_node->pin_type(a), b_node->pin_type(b));

	if (type_pair == std::make_pair(PinType::Input, PinType::Input))
	{
		return LinkFeasibility::CANNOT_LINK_INPUTS;
	}

	if (type_pair == std::make_pair(PinType::Output, PinType::Output))
	{
		return LinkFeasibility::CANNOT_LINK_OUTPUTS;
	}

	if (a_node == b_node)
	{
		return LinkFeasibility::CANNOT_LINK_TO_ITSELF;
	}

	if (a_node->pin_type(a) == PinType::Input)
	{
		// we want a to be the source and b to be the target.
		std::swap(a_node, b_node);
		std::swap(a, b);
	}

	const auto* a_pin_info = editor.get_pin_info(a);
	const auto& a_links = a_pin_info->links;

	if (std::find_if(a_links.begin(), a_links.end(), [&](const LinkInfo& link) {
		return link.pins.to == b;
	}) != a_links.end())
	{
		return LinkFeasibility::DUPLICATE_LINK;
	}

	LinkVerifier verifier(a, b);

	a_node->accept(verifier);
	b_node->accept(verifier);

	return verifier.m_feasibility;
}

LinkVerifier::LinkVerifier(ed::PinId a, ed::PinId b) :
	m_a(a),
	m_b(b),
	m_feasibility(LinkFeasibility::FEASIBLE)
{}

void LinkVerifier::visit(nodes::CondNode& node)
{
	check_prevent_conditional_loop(node);
}

void LinkVerifier::visit(nodes::IfNode& node)
{
	check_prevent_conditional_loop(node);
}

void LinkVerifier::visit(nodes::StateNode& node)
{
	(void)node;
}

void LinkVerifier::check_prevent_conditional_loop(Node& a_node)
{
	if (!is_feasible())
	{
		return;
	}

	const auto& editor = a_node.editor();

	// If a is an output, assume that b is an input (we checked this earlier in the caller).
	// If we are creating a link from a to b, we are checking whether a is a predecessor to b.
	// If it is, then we have a loop in our conditional logic, which is not feasible.

	if (a_node.pin_type(m_a) == PinType::Output)
	{
		Node& b_node = *editor.get_node_by_pin_id(m_b);

		if (NodePredecessorFinder::find_in_cond_tree(a_node, b_node))
		{
			m_feasibility = LinkFeasibility::CANNOT_LINK_TO_PREDECESSOR;
		}
	}
}

bool LinkVerifier::is_feasible() const
{
	return m_feasibility == LinkFeasibility::FEASIBLE;
}

}
}
