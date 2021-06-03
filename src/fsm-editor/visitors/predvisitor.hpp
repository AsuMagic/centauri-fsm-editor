#pragma once

#include "../visitor.hpp"

#include <unordered_set>

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to find a predecessor node within the FSM editor graph.
 */
class NodePredecessorFinder : public NodeVisitor
{
public:
	/**
	 * @brief Looks for a predecessor within a condition tree.
	 * @details This allows detecting whether a conditional (typically CondNode and IfNode) node is a predecessor to
	 * another, which is useful in order to detect loops within conditional logic.
	 * @param root    The node to start the search from.
	 * @param to_find The node to find within the predecessors of \p root.
	 * @return Whether \p to_find is a predecessor of \p root.
	 */
	static bool find_in_cond_tree(Node& root, Node& to_find);

	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	NodePredecessorFinder(Node& to_find);

	void check_cond_node(Node& node);
	bool check_node(Node& node);

	ed::NodeId m_to_find;
	bool m_found;
	std::unordered_set<ed::NodeId> m_visited;
};

}
}
