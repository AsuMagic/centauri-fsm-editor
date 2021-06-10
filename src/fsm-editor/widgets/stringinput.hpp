#pragma once

#include <array>
#include <string>

namespace fsme
{
namespace widgets
{

class StringInput
{
public:
	using Buffer = std::array<char, 4096>;

	StringInput();

	void render(bool editable = true);

	void set_text(const std::string& value);
	std::string get_text() const;

	Buffer& get_buffer();

	void set_hint(std::string value);

private:
	Buffer m_buffer;
	std::string m_hint;
};

inline StringInput::Buffer& StringInput::get_buffer()
{
	return m_buffer;
}

}
}
