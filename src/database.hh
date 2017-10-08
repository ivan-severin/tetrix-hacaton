#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <ctime>
#include <map>



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

std::vector<std::tuple<std::string, int>> high1(std::map<std::string, int> mcountry)
{
	std::vector<std::tuple<std::string, int>> res;
	std::copy(mcountry.begin(), mcountry.end(),  std::back_inserter(res));
	std::sort(res.begin(), res.end(), [](const std::tuple<std::string, int> &a, const std::tuple<std::string, int> &b){
		return std::get<1>(a) > std::get<1>(b);});
	return res;
}
std::vector<std::tuple<std::string, int>> high2(std::map<std::string, int> mcountry)
{
	std::vector<std::tuple<std::string, int>> res;
	std::copy(mcountry.begin(), mcountry.end(),  std::back_inserter(res));
	std::sort(res.begin(), res.end(), [](const std::tuple<std::string, int> &a, const std::tuple<std::string, int> &b){
		return std::get<1>(a) > std::get<1>(b);});
	return res;
}
std::vector<int> recomm(std::vector<std::tuple<std::string, int>> high1, std::vector<std::tuple<std::string, int>> high2, std::vector<drink> bottles)
{
	std::vector<int> k;
	for(int i=0;i<100;i++)
	if ((std::get<0>(high1[0])==bottles[i].country) && (std::get<0>(high2[0])==bottles[i].variety)) k.push_back(i);	
	if (k.size()==0)
	for(int i=0;i<100;i++)
	if (std::get<0>(high2[0])==bottles[i].variety) k.push_back(i); 
	return k;
}

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

void addalco(int idnew, std::vector<int> products, std::vector<drink> people)
{
	for(int i=0; i<products.size();i++)
	{
		client buffer;
		buffer.id=idnew;
		buffer.productid=products[i];
		buffer.date=time(nullptr);
		people.push_back({});
	}
}

class DataBase {
	std::ifstream src;
	// std::ofstream res("data/alcoholic.txt");
	std::string buffer;
	std::vector<drink> bottles;
	std::vector<client> people;

	public: DataBase() {
		src.open("data/wine.csv");

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
	//		std::cout<<alcoholic.id<<" "<<alcoholic.productid<<" "<<alcoholic.date<<std::endl;
		}
	}

	public: std::vector<int> getRecomendations(int h_id) {
		int i=0;
		std::map<std::string, int> mcountry;
		for(i=0;i<100;i++)
		{
			int human_id = h_id;
			if (people[i].id!=human_id) continue;
			auto search = mcountry.find(bottles[people[i].productid].country);
			if (search != mcountry.end()) search->second+=1;
			else mcountry[bottles[people[i].productid].country]=1;
		}
		std::map<std::string, int> mvariety;
		for(i=0;i<100;i++)
		{
			int human_id = h_id;
			if (people[i].id!=human_id) continue;
			auto search = mvariety.find(bottles[people[i].productid].variety);
			if (search != mvariety.end()) search->second+=1;
			else mvariety[bottles[people[i].productid].variety]=1;
		}
		int g=0;
		//std::map<std::string, int> m; 
		//for (auto &el : mcountry)
		//if (el.second>g)
		//{g=el.second; m[0]=el;}
		
		std::vector<std::tuple<std::string, int>> m1;
		std::vector<std::tuple<std::string, int>> m2;
		m1=high1(mcountry);
		m2=high2(mvariety);
		//std::copy(mcountry.begin(), mcountry.end(),  std::back_inserter(m));
		//std::sort(m.begin(), m.end(), [](const std::tuple<std::string, int> &a, const std::tuple<std::string, int> &b){
			//return std::get<1>(a) > std::get<1>(b);});
		
		
		//std::cout<<m.find(g)->first<<" "<<g<<std::endl<<std::endl;
		//for (auto &el : m1) {
			//std::cout << std::get<0>(el) << ": " << std::get<1>(el) << std::endl;
		//}
		//for (auto &el : m2) {
			//std::cout << std::get<0>(el) << ": " << std::get<1>(el) << std::endl;
		//}
		std::vector<int> k=recomm(m1,m2,bottles);
		for (auto &el : k) {
			// std::cout <<std::endl<< el << ": " << bottles[el].country<<" "<<bottles[el].variety << std::endl;
			std::cout << el << ": " << bottles[el].country << " " << bottles[el].variety << " " << bottles[el].winery << std::endl;
		}
		//res<<std::get<0>(m1[0])<<" "<<std::get<1>(m1[0])<<std::endl;

		return k;
	}
};
