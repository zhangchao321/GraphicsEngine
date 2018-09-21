#include "csv.hpp"
#include <cstdio>
#include <cstring>

std::string format(const char * Message, ...)
{
	assert(Message);
	
	char Text[1024];
	std::memset(Text, 0, sizeof(Text));

	va_list ap;
	va_start(ap, Message);
		std::vsprintf(Text, Message, ap);
	va_end(ap);

	return Text;
}

void Csv::log(char const* String, double Convergent, double Min, double Max)
{
	this->data.push_back(Data(String, Convergent, Min, Max));
}

void Csv::save(char const* Filename)
{
	FILE* File(fopen(Filename, "a+"));
	assert(File);
	fprintf(File, "%s;%s;%s;%s\n", "Tests", "average", "max", "min");

	for(std::size_t i = 0; i < this->data.size(); ++i)
	{
		fprintf(File, "%s;%d;%d;%d\n",
			data[i].String.c_str(),
			static_cast<int>(data[i].Convergent),
			static_cast<int>(data[i].Max), static_cast<int>(data[i].Min));
	}
	fclose(File);
}

void Csv::print()
{
	fprintf(stdout, "\n");
	for(std::size_t i = 0; i < this->data.size(); ++i)
	{
		fprintf(stdout, "%s, %2.5f, %2.5f, %2.5f\n",
			data[i].String.c_str(),
			data[i].Convergent / 1000.0,
			data[i].Min / 1000.0, data[i].Max / 1000.0);
	}
}

