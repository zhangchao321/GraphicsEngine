#pragma once

#include <vector>
#include <string>
#include <cstdarg>
#include <cassert>

std::string format(const char * Message, ...);

class Csv
{
	struct Data
	{
		Data(
			std::string const & String,
			double Convergent, double Min, double Max) :
			String(String),
			Convergent(Convergent), Min(Min), Max(Max)
		{}

		std::string String;
		double Convergent;
		double Min;
		double Max;
	};

public:
	void log(char const* String, double Average, double Min, double Max);
	void save(char const* Filename);
	void print();

private:
	std::vector<Data> data;
};

