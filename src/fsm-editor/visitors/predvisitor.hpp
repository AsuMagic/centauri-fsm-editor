#pragma once

#include "../visitor.hpp"

#include <unordered_set>

namespace fsme
{

class NodePredecessorFinder : public NodeVisitor
{
public:
	static bool find(Node& root, Node& to_find);

	void visit(CondNode& node) override;
	void visit(IfNode& node) override;
	void visit(StateNode& node) override;

private:
	NodePredecessorFinder(Node& to_find);

	void check_cond_node(Node& node);

	bool check_node(Node& node);

	Node* m_to_find;
	bool m_found;
	std::unordered_set<ed::NodeId> m_visited;
};

}
