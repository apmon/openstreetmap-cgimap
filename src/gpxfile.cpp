#include "gpxfile.hpp"

using
  std::string;

gpxfile::gpxfile (const string & fn, const string & fd, const string & vis,
		  long int id):
file_name (fn),
file_description (fd),
visibility (vis),
file_id (id)
{
	identifiable = false;
	trackable = false;
	if (visibility.compare("identifiable") == 0) {
		identifiable = true;
		trackable = true;
	}
	if (visibility.compare("trackable") == 0) {
		trackable = true;
	}
}

gpxfile *
fetch_gpxfile (pqxx::transaction_base & w, long int id)
{
  pqxx::result res =
    w.exec ("select name, description , visibility from gpx_files "
	    "where id=" + pqxx::to_string (id));

  return new gpxfile (res[0][0].as < string > (),
		      res[0][1].as < string > (),
		      res[0][2].as < string > (), id);
}
