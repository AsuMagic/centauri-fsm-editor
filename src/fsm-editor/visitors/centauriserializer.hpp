#pragma once

#include "../visitor.hpp"

#include <ostream>

namespace fsme
{

class CentauriSerializer : public NodeVisitor
{
	public:
	[[nodiscard]] static bool serialize(std::ostream& output, Node& node);

	void visit(CondNode& node) override;
	void visit(IfNode& node) override;
	void visit(StateNode& node) override;

	private:
	CentauriSerializer(std::ostream& output);

	void visit_outputs(Node& node);

	void emit_state(std::uint32_t id, const std::string& name);
	void emit_branch(std::uint32_t id, const std::string& lua_expression, std::uint32_t on_true, std::uint32_t on_false);

	std::uint32_t get_node_id_for_pin(const FsmEditor& editor, ed::PinId pin);

	bool m_visited_root;
	std::ostream* m_out;
};

}
