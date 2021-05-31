#pragma once

#include "../node.hpp"
#include "../widgets/boolexprinput.hpp"

#include <unordered_map>

namespace fsme
{

class CondNode : public Node
{
	public:
	CondNode(FsmEditor& editor, ed::NodeId id);

	void render_context_menu() override;

	void accept(NodeVisitor& v) override;

	bool can_connect(ed::PinId from, ed::PinId to) const override;

	void set_output_count(std::size_t i);
	void erase_output_pin(std::size_t i);

	BoolExpressionInput& get_expression(ed::PinId output);

	private:
	std::unordered_map<ed::PinId, BoolExpressionInput> m_conditions;
};

inline void CondNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

}
