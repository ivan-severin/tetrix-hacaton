#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>

void deletespace(std::string& word)
{
	int first=0,last=word.length();
	while (word[first]==' ') first++;
	while (word[last]==' ') last--;
	word=word.substr(first,(last-first+1));
}

void deletecomma(std::string& word)
{
	int first=0,last=word.length();
	while (word[first]==',') first++;
	while (word[last]==',') last--;
	word=word.substr(first,(last-first+1));
}

std::vector<std::string> separator(std::string line)
{
	int first=0, last,j;
	std::string buffer;
	std::vector<std::string> sep;
	for(int i=0;i<line.length();i++)
	{ 
		if (line[i]=='"')
		{
			first=i+1;
			last=first+1;
			while (line[last]!='"') last++;
			buffer=line.substr(first,(last-first));
			deletespace(buffer);
			sep.push_back(buffer);
			first=last+1;
			i=first;
		}
		else
		{
			if (line[i]==',')
			{
				last=i-1;
				buffer=line.substr(first,(last-first+1));
				deletespace(buffer);
				deletecomma(buffer);
				sep.push_back(buffer);
				first=last+2;
			}
			if (i==line.length()-1)
			{
				last=i;
				buffer=line.substr(first,(last-first+1));
				deletespace(buffer);
				sep.push_back(buffer);
			}
		}
	}
	return sep;
}

class drink{
	public:
	std::string country, description, designation, province, region_1, region_2, variety, winery;
	int id, points, price;
	void set_values(std::vector<std::string>);
};

void drink::set_values (std::vector<std::string> line)
{
	id=std::atoi(line[0].c_str());
	country=line[1];
	description=line[2];
	designation=line[3];
	points=std::atoi(line[4].c_str());
	price=std::atoi(line[5].c_str());
	province=line[6];
	region_1=line[7];
	region_2=line[8];
	variety=line[9];
	winery=line[10];	
}
int main()
{
	std::ifstream src("data/wine.csv");
	std::string buffer;
	std::vector<drink> bottles;
	int i=0;
	while(i<101)
	{
		drink bottle;
		std::getline(src,buffer);
		if (i++==0) continue;
		bottle.set_values(separator(buffer));
		bottles.push_back(bottle);
		std::cout<<bottle.id<<"\n"<<bottle.designation<<"\n"<<bottle.country<<"\n"<<bottle.variety<<std::endl;
		std::cout<<i<<std::endl;
	}
	std::cout<<bottles.size()<<std::endl;
	return 0;
}
