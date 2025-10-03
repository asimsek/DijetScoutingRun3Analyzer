#include <iostream>
#include <getopt.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <memory>

#include "jsonParser.h"

JSONParser::JSONParser(){}
JSONParser::~JSONParser(){}

void JSONParser::addToMap ( int run_number , const std::vector<std::pair<int,int> > & lumi_ranges ) { 

  GoodLumiMapIterator map_entry     = m_good_lumi_map.find ( run_number ) ;
  GoodLumiMapIterator map_entry_end = m_good_lumi_map.end();
  
  if ( map_entry == map_entry_end ) 
    m_good_lumi_map . insert ( GoodLumiMapEntry ( run_number , lumi_ranges ) ) ;
  
  else { 
    std::vector < std::pair <int, int > > * current_lumi_ranges = & (*map_entry).second;
    current_lumi_ranges -> insert ( current_lumi_ranges -> end(), lumi_ranges.begin() , lumi_ranges.end() );
  }
  
}

bool JSONParser::isAGoodLumi ( int run_number, int lumi ) {

  bool decision = false;

  GoodLumiMapIterator map_entry     = m_good_lumi_map.find ( run_number ) ;
  GoodLumiMapIterator map_entry_end = m_good_lumi_map.end();

  if ( map_entry == map_entry_end ) return false;
  
  else { 
    std::vector < std::pair < int, int > > * lumi_ranges = & map_entry -> second;
    std::vector < std::pair < int, int > >::iterator lumi_range     = lumi_ranges -> begin();
    std::vector < std::pair < int, int > >::iterator lumi_range_end = lumi_ranges -> end();
    
    for (; lumi_range != lumi_range_end; ++lumi_range) 
      if ( lumi >= lumi_range -> first && lumi <= lumi_range -> second ) return true;

  }
  
  return decision;

}

void JSONParser::printGoodLumis () { 
  
  
  GoodLumiMapIterator map_entry     = m_good_lumi_map.begin();
  GoodLumiMapIterator map_entry_end = m_good_lumi_map.end();
  
  std::cout << "For JSON file: " << m_file_name << std::endl;
  std::cout << "Good run/lumi ranges are: " << std::endl;

  for (; map_entry != map_entry_end; ++map_entry ) {
    
    int run_number = map_entry -> first;
    std::vector < std::pair < int, int > > * lumi_ranges = & map_entry -> second;
    std::vector < std::pair < int, int > >::iterator lumi_range     = lumi_ranges -> begin();
    std::vector < std::pair < int, int > >::iterator lumi_range_end = lumi_ranges -> end();
    
    std::cout << run_number << " : ";

    int n_ranges = (int) lumi_ranges -> size();
    int i_range  = 0;

    for (; lumi_range != lumi_range_end; ++lumi_range) {
      int lumi_range_min = lumi_range -> first;
      int lumi_range_max = lumi_range -> second;
      std::cout << "[ " << lumi_range_min << ", " << lumi_range_max << " ]";
      if ( i_range < n_ranges - 1 ) std::cout << ", ";
      i_range ++;
    }

    std::cout << std::endl;
  }

}

void JSONParser::parseJSONFile( const std::string & file_name ) {

  m_file_name = file_name;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json( m_file_name, pt );

  BOOST_FOREACH( boost::property_tree::ptree::value_type const& runPair, pt.get_child( "" ) )
  {
      int run_number = atoi(runPair.first.c_str());
      std::vector<std::pair < int, int > > lumi_ranges;

      BOOST_FOREACH( boost::property_tree::ptree::value_type const& lumiRange, runPair.second )
      {
          std::vector<int> lumis;

          BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, lumiRange.second )
          {
              lumis.push_back( node.second.get_value<int>() );
          }

          int startLumi = ( lumis.size() > 0 ? lumis[0] : -1 );
          int endLumi   = ( lumis.size() > 1 ? lumis[1] : -1 );

          lumi_ranges.push_back( std::pair< int, int >( startLumi, endLumi ) );
      }

      addToMap ( run_number , lumi_ranges ) ;
  }
}
