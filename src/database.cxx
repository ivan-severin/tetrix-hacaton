#include <string>;
#inlcude <math.h>;

void deletespace(std::string& word)
{
	int first=0,last=word.length();
	while (word[first]==' ') first++;
	while (word[last]==' ') last--;
	word=std::basic_string::substr(first,(last-first+1));
}

std::vector<std::string> separator(std::string line)
{
	int first=0, last;
	std::string buffer;
	std::vector<std::string> sep;
	for(i=0;i<line.length();i++)
	{
		if (line[i]=",")
		{
			last=i;
			buffer=std::basic_string::substr(first,(last-first+1));
			deletespace(buffer);
			sep.push_back(buffer);
			first=last+1;
		}
		if (i==line.length()-1)
		{
			last=i;
			buffer=std::basic_string::substr(first,(last-first+1));
			deletespace(buffer);
			sep.push_back(buffer);
		}
	}
	return sep;
}

class drink{
	std::string country, description, designation, province, region_1, region_2, variety, winery;
	int id, points, price;
	public:	
	void set_values(std::vector<std::string>)
}

void drink::set_values (std::vector<std::string> line)
{
	id=atoi(line[0]);
	country=line[1];
	description=line[2];
	designation=line[3];
	points=atoi(line[4]);
	price=atoi(line[5]);
	province=line[6];
	region_1=line[7];
	region_2=line[8];
	variety=line[9];
	winery=line[10];	
}
