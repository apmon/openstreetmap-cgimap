#ifndef GPXFILE_HPP
#define GPXFILE_HPP

#include <string>
#include <pqxx/pqxx>

struct gpxfile {
  std::string file_name;
  std::string file_description;
  std::string visibility;
  bool identifiable;
  bool trackable;
  long int file_id;
  

	gpxfile(const std::string &fn, const std::string &fd, const std::string &vis, long int id);
};

gpxfile *fetch_gpxfile(pqxx::transaction_base &w, long int id);

#endif /* GPXFILE_HPP */
