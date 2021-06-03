#pragma once

#include "../node.hpp"
#include "../widgets/boolexprinput.hpp"

#include <unordered_map>

namespace fsme
{
namespace nodes
{

/**
 * @brief A node with an arbitrary number of conditional outputs.
 */
class CondNode : public Node
{
	public:
	CondNode(FsmEditor& editor, ed::NodeId id);

	void accept(NodeVisitor& v) override;

	void set_output_count(std::size_t i);
	void erase_output_pin(std::size_t i);
	void reset_output_pins();

	widgets::BoolExpressionInput& get_expression(ed::PinId output);

	private:
	std::unordered_map<ed::PinId, widgets::BoolExpressionInput> m_conditions;
};

inline void CondNode::accept(NodeVisitor& v)
{
	v.visit(*this);
}

}
}
