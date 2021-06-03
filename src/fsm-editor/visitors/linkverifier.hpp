#pragma once

#include "../visitor.hpp"

namespace fsme
{
namespace visitors
{

/**
 * @brief Feasibility of a link. Any value other than LinkFeasibility::FEASIBLE means that a given link would not be
 * feasible.
 *
 * @see get_feasibility_string()
 */
enum class LinkFeasibility
{
	FEASIBLE,                   ///< This link can be created
	CANNOT_LINK_INPUTS,         ///< Both pins of the link are inputs
	CANNOT_LINK_OUTPUTS,        ///< Both pins of the link are outputs
	CANNOT_LINK_TO_ITSELF,      ///< Both pins of the link are from the same node
	CANNOT_LINK_TO_PREDECESSOR, ///< Outputting to a node that is a predecessor when it is not legal
	DUPLICATE_LINK,             ///< An identical link exists already
	INVALID_NODE                ///< Hints at an internal bug
};

/**
 * @brief Returns a user-friendly description of the feasibleness of a link.
 */
const char* get_feasibility_string(LinkFeasibility feasibility);

/**
 * @brief Visitor that allows verifying for the feasibility of an uncreated link.
 */
class LinkVerifier : public NodeVisitor
{
public:
	/**
	 * @brief Returns the feasibility of an uncreated link.
	 *
	 * @details Determines the feasibility of a link, i.e. whether the user should be allow to create a link between
	 * pins \p a and \p b.
	 * \p a and \p b may be swapped so that \p a becomes the "from" end of the link , which is why \p a and \p b are
	 * references.
	 *
	 * @param editor The FSM editor instance
	 * @param a One pin within the \p editor graph. Not necessarily the "from" end of a link.
	 * @param b Another pin within the \p editor graph. Not necessarily to "to" end of a link.
	 */
	static LinkFeasibility verify(const FsmEditor& editor, ed::PinId& a, ed::PinId& b);

	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	LinkVerifier(ed::PinId from, ed::PinId to);

	void check_prevent_conditional_loop(Node& node);

	bool is_feasible() const;

	ed::PinId m_a, m_b;
	LinkFeasibility m_feasibility;
};

}
}
