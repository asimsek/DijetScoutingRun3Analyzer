#ifndef JSONPARSER_H
#define JSONPARSER_H

#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <vector>
#include <map>

class JSONParser {
 public:
  JSONParser();
  ~JSONParser();

  void parseJSONFile(const std::string& file_name);
  bool isAGoodLumi(int run_number, int lumi);
  void printGoodLumis();

 private:
  using GoodLumiMap = std::map<int, std::vector<std::pair<int,int>>>;
  using GoodLumiMapIterator = GoodLumiMap::iterator;
  using GoodLumiMapEntry = std::pair<int, std::vector<std::pair<int,int>>>;

  GoodLumiMap m_good_lumi_map;
  std::string m_file_name;

  void addToMap(int run_number, const std::vector<std::pair<int,int>>& lumi_ranges);
};

#endif

