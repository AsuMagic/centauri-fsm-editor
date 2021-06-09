#pragma once

#include "../fwd.hpp"
#include "../visitor.hpp"
#include "../util/idhash.hpp"

#include <ostream>
#include <unordered_set>

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to help serialize the FSM into the centauri FSM graph format.
 */
class CentauriSerializer : public NodeVisitor
{
public:
	/**
	 * @brief Serializes a node into the centauri FSM graph format to a standard output stream.
	 * @param output The text output. Nothing will be written to it if serialization does not happen.
	 * @param node An input node. If this is not a state node, serialization won't happen.
	 * @return true if serialization could occur, i.e. if node was indeed a StateNode.
	 */
	[[nodiscard]] static bool serialize(std::ostream& output, Node& node);

	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	CentauriSerializer(std::ostream& output);

	bool mark_visited(const Node& node);

	void visit_outputs(Node& node);

	void emit_state(std::uint32_t id, const std::string& name);
	void emit_branch(std::uint32_t id, const std::string& lua_expression, std::uint32_t on_true, std::uint32_t on_false);

	std::uint32_t get_node_id_for_pin(const FsmEditor& editor, ed::PinId pin);

	std::unordered_set<ed::NodeId> m_visited_nodes;
	bool m_visited_root;
	std::ostream* m_out;
};

}
}
