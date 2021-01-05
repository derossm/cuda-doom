/**********************************************************************************************************************************************\

\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"
#include <filesystem>
#include <fstream>

#include "w_wad.h"

namespace cudadoom::src::deh
{

class Context
{
private:
	std::fstream stream;
	std::filesystem::path path;

	std::list<std::string> buffer;

public:
	Context() noexcept
	{}

	Context(const char* filename) noexcept
	{}

	Context(const std::string& filename) noexcept
	{}

	Context(const std::fstream& _stream) noexcept
	{}

	Context(std::fstream&& _stream) noexcept
	{}

	Context(const std::filesystem::path& _path) noexcept
	{}

	Context(std::filesystem::path&& _path) noexcept
	{}

	Context(Context&& rhs) noexcept
	{}

	Context& operator=(Context&& rhs) noexcept
	{}

	Context(const Context& rhs) noexcept
	{}

	Context& operator=(const Context& rhs) noexcept
	{}

	bool is_open() const noexcept
	{
		if (stream.is_open())
		{
			return true;
		}

		return false;
	}

	std::ios_base::iostate open() noexcept
	{
		close();

		stream.open(path, std::ios_base::in);

		return stream.rdstate();
	}

	std::ios_base::iostate open(const std::filesystem::path& _path) noexcept
	{
		path = _path;
		return open();
	}

	std::ios_base::iostate open(const std::string& _filename) noexcept
	{
		path = std::filesystem::path(_filename);
		return open();
	}

	std::ios_base::iostate open(const char* _filename) noexcept
	{
		path = std::filesystem::path(_filename);
		return open();
	}

	std::string IWAD_DIR{"somefolder"};
	std::ios_base::iostate open(int lumpnum)
	{
		auto lump = W_CacheLumpNum<char>(lumpnum, pu_tags_t::PU_STATIC);

		//context->type = deh_input_type_t::DEH_INPUT_LUMP;
		//context->lumpnum = lumpnum;
		//context->input_buffer = std::string((std::string)lump);
		//context->input_buffer_pos = 0;

		path = std::filesystem::path(IWAD_DIR + lumpinfo[lumpnum]->name);
		open();
	}

	void close() noexcept
	{
		if (stream.is_open())
		{
			stream.close();
		}

		stream.clear();
	}

	void clear() noexcept
	{
		close();
		buffer.clear();
		path.clear();
	}

	const char* c_str() const noexcept
	{
		return buffer.back().c_str();
	}

	// NOTE: EXPERIMENTAL
	std::string& operator >> (std::string& output) noexcept
	{
		if (stream.is_open())
		{
			stream >> output;
		}

		return buffer.emplace_back(output);

		//return output;
	}

	void swap(std::fstream& other) noexcept
	{}

	void swap(std::filesystem::path& other) noexcept
	{}

	void swap(Context& other) noexcept
	{}

	std::filebuf* rdbuf() const noexcept
	{
		return stream.rdbuf();
	}

	auto get() noexcept
	{}

	/* auto peek() noexcept
	{
	} */

	/* auto unget() noexcept
	{
	} */

	/* auto putback() noexcept
	{
	} */

	std::istream& getline(char* str, std::streamsize count) noexcept
	{
		return stream.getline(str, count);
	}

	std::istream& getline(char* str, std::streamsize count, char delim) noexcept
	{
		return stream.getline(str, count, delim);
	}

	std::istream& ignore(std::streamsize count = 1, int delim = std::char_traits<char>::eof()) noexcept
	{
		return stream.ignore(count, delim);
	}

	/* auto read() noexcept
	{
	} */

	/* auto readsome() noexcept
	{
	} */

	std::streamsize gcount() noexcept
	{
		return stream.gcount();
	}

	/* auto tellg() noexcept
	{
	} */

	/* auto seekg() noexcept
	{
	} */

	int sync() noexcept
	{
		return stream.sync();
	}

	bool CheckSignatures() noexcept
	{}

	bool HadError() noexcept
	{}

	void Parse() noexcept
	{
		std::list<std::string> list;
		char section_name[20];
		void* tag = NULL;
		bool extended;
		std::string line;

		// Read the header and check it matches the signature
		if (!CheckSignatures())
		{
			fprintf(stderr, "This is not a valid dehacked patch file!\n");
		}

		// Read the file
		while (!HadError())
		{
			// Read the next line. We only allow the special extended parsing
			// for the BEX [STRINGS] section.
			extended = current_section != NULL && !iequals(current_section->name, "[STRINGS]");
			// [crispy] save pointer to start of line, just in case
			DEH_SaveLineStart(context);
			line = DEH_ReadLine(context, extended);

			// end of file?
			if (line == NULL)
			{
				return;
			}

			while (line[0] != '\0' && isspace(line[0]))
				++line;

			if (line[0] == '#')
			{
				// comment
				DEH_ParseComment(line);
				continue;
			}

			if (IsWhitespace(line))
			{
				if (current_section != NULL)
				{
					// end of section
					if (current_section->end != NULL)
					{
						current_section->end(context, tag);
					}

					// [crispy] if this was a BEX line parser, remember it in case
					// the next section does not start with a section marker
					if (current_section->name[0] == '[')
					{
						prev_section = current_section;
					}
					else
					{
						prev_section = NULL;
					}

					//printf("end %s tag\n", current_section->name);
					current_section = NULL;
				}
			}
			else
			{
				if (current_section != NULL)
				{
					// parse this line
					current_section->line_parser(context, line, tag);
				}
				else
				{
					// possibly the start of a new section
					sscanf(line, "%19s", section_name);

					current_section = GetSectionByName(section_name);

					if (current_section != NULL)
					{
						tag = current_section->start(context, line);
						//printf("started %s tag\n", section_name);
					}
					else
						if (prev_section != NULL)
						{
							// [crispy] try this line again with the previous line parser
							DEH_RestoreLineStart(context);
							current_section = prev_section;
							prev_section = NULL;
						}
						else
						{
							//printf("unknown section name %s\n", section_name);
						}
				}
			}
		}
	}
};

}
