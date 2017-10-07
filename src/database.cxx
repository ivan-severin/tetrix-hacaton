#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <ctime>


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
	void set_values_d(std::vector<std::string>);
};

void drink::set_values_d(std::vector<std::string> line)
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

class client{
	public:
	int id, productid;
	std::time_t date;
	void set_values_c(std::vector<std::string>);
};

//tm date atot(std::string)
//{
	//std::tm a;
	//get_time(&a, L"%Y-%b-%d %H:%M:%S");
	//return a;
//}
//void client::set_values_c(std::vector<std::string> line)
//{
	//id=std::atoi(line[0].c_str());
	//date=atot(line[1]);
	//productid=atoi(line[2].c_str());
//}

int main()
{
	std::ifstream src("data/wine.csv");
	std::string buffer;
	std::vector<drink> bottles;
	std::vector<client> people;
	int i=0;
	while(i<101)
	{
		drink bottle;
		std::getline(src,buffer);
		if (i++==0) continue;
		bottle.set_values_d(separator(buffer));
		bottles.push_back(bottle);
//		std::cout<<bottle.id<<"\n"<<bottle.designation<<"\n"<<bottle.country<<"\n"<<bottle.variety<<std::endl;
//		std::cout<<i<<std::endl;
	}
//	std::cout<<bottles.size()<<std::endl;
	i=0;
	while(i<100)
	{
		client alcoholic;
		std::getline(src,buffer);
		alcoholic.id=(i*7)%10;
		alcoholic.productid=(i*7)%100;
		long long t=i;
		alcoholic.date=40*365*24*3600+t*10000;
		people.push_back(alcoholic);
		i++;
		std::cout<<alcoholic.id<<" "<<alcoholic.productid<<" "<<alcoholic.date<<std::endl;
	}
	return 0;
}
