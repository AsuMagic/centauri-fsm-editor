#pragma once

#include <array>
#include <string>

namespace fsme
{

class StringInput
{
public:
	StringInput();

	void render(bool editable = true);

	void set_text(const std::string& value);
	std::string get_text() const;

	void set_hint(std::string value);

private:
	std::array<char, 4096> m_buffer;
	std::string m_hint;
};

}
