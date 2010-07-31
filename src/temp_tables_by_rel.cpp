#include <stdexcept>
#include <sstream>
#include <iostream>
#include "temp_tables.hpp"
#include "quad_tile.hpp"
#include "logger.hpp"

using std::set;
using std::runtime_error;
using std::stringstream;
using std::cerr;
using std::endl;

tmp_nodes::tmp_nodes(pqxx::work &w, const int rid)
  : work(w) {

  // hack around problem with postgres' statistics, which was 
  // making it do seq scans all the time on smaug...
  w.exec("set enable_mergejoin=false");
  w.exec("set enable_hashjoin=false");

  stringstream query;
  query << "create temporary table tmp_nodes as "
		<< "select distinct(member_id) as id from current_relation_members where member_type = 'Node' and id = " << rid;
  logger::message("Creating tmp_nodes");

  // assume this throws if it fails?
  work.exec(query);
}

tmp_ways::tmp_ways(pqxx::work &w, const int rid) 
  : work(w) {
  // we already did this in tmp_nodes, but it can't hurt to do it twice
  work.exec("set enable_mergejoin=false");
  work.exec("set enable_hashjoin=false");

  logger::message("Creating tmp_ways");
  stringstream query;
  query << "create temporary table tmp_ways as "
		<< "select distinct(member_id) as id from current_relation_members where member_type = 'Way' and id = " << rid;

  work.exec(query);
  work.exec("create index tmp_ways_idx on tmp_ways(id)");
}

tmp_relations::tmp_relations(pqxx::work &w, const int rid) 
  : work(w) {
  // we already did this in tmp_nodes and tmp_ways, but three times is a charm
  work.exec("set enable_mergejoin=false");
  work.exec("set enable_hashjoin=false");

  logger::message("Creating tmp_relations");
  stringstream query;
  query << "create temporary table tmp_relations as "
		<< "select distinct(member_id) as id from current_relation_members where member_type = 'Relation' and id = " << rid;

  work.exec(query);
  query.str("");
  query << "INSERT INTO tmp_relations "
		<< "select id from current_relations where id = " << rid;

  work.exec(query);
  //TODO: Do we need this?
  work.exec("create index tmp_relations_idx on tmp_relations(id)");
}
