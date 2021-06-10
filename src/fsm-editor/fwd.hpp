#pragma once

/**
 * @file fwd.hpp
 * @brief Provides forward declarations for most classes within the fsme:: namespace
 */

namespace fsme
{

struct PinPair;
struct LinkInfo;
struct PinInfo;
class FsmEditor;
class Node;
class NodeVisitor;

/**
 * @brief Types of nodes for the FSM editor graph.
 * @see Node
 */
namespace nodes
{
class CondNode;
class IfNode;
class StateNode;
}

/**
 * @brief Visitors as defined in the visitor pattern, which operate over nodes.
 * @see Node::accept()
 */
namespace visitors
{
class CentauriSerializer;
class LinkVerifier;
class NativeSerializer;
class NativeDeserializer;
class NodeDuplicator;
class NodeMenuRenderer;
class NodeRenderer;
class NodePredecessorFinder;
}

/**
 * @brief dear imgui widgets that abstract some kinds of controls.
 */
namespace widgets
{
class BoolExpressionOption;
class BoolExpressionCategory;
class BoolExpressionAutocomplete;
class BoolExpressionInput;
class PlainLuaInput;
class SimpleExpressionInput;
class BoolExpressionInput;
class StringInput;
}

}
