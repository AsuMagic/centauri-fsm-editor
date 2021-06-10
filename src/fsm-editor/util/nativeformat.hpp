#pragma once

#include <cstdint>

namespace fsme
{
namespace native_format
{

enum class NodeType : std::uint8_t
{
	STATE = 0x00,
	IF    = 0x01,
	COND  = 0x02
};

const std::uint32_t magic_header = 0xCCAAFFEE;
const std::uint16_t version = 0x0001;

// Chunk headers
const std::uint32_t pins_magic = 0x01C0FFEE;
const std::uint32_t links_magic = 0x02C0FFEE;
const std::uint32_t nodes_magic = 0x03C0FFEE;

}
}
