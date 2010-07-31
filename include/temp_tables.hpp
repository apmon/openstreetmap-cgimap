#ifndef TEMP_TABLES_HPP
#define TEMP_TABLES_HPP

#include <pqxx/pqxx>
#include "bbox.hpp"

#define SCALE (10000000.0)

class tmp_nodes {
public:
  tmp_nodes(pqxx::work &work, const bbox &bounds);
  tmp_nodes(pqxx::work &work, const int rid);
private:
  pqxx::work &work;
};

class tmp_ways {
public:
  tmp_ways(pqxx::work &work);
  tmp_ways(pqxx::work &work, const int rid);
private:
  pqxx::work &work;
};

class tmp_relations {
public:
  tmp_relations(pqxx::work &work);
  tmp_relations(pqxx::work &work, const int rid);

private:
  pqxx::work &work;
};

#endif /* TEMP_TABLES_HPP */
