#ifndef OSM_RESPONDER_HPP
#define OSM_RESPONDER_HPP

#include "handler.hpp"
#include "bbox.hpp"

#include <pqxx/pqxx>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

/**
 * utility class - use this as a base class when the derived class is going to
 * respond in OSM format (i.e: nodes, ways and relations). this class will take
 * care of the write() and types_available() methods, allowing derived code to
 * be more concise.
 *
 * if you want a <bounds> element to be written, include the bounds constructor
 * argument. otherwise leave it out and it'll default to no bounds element.
 */
class osm_responder : public responder {
public:
	 // construct, passing the mime type down to the responder. the current transaction 
   // optional bounds are stored at this level, but available to derived classes.
	 osm_responder(mime::type, pqxx::work &, boost::optional<bbox> bounds = boost::optional<bbox>());

	 // construct with specifying if the subclass will create the nodes, ways and / or relations tmp tables
	 osm_responder(mime::type mt, pqxx::work &x, bool has_nodes, bool has_ways, bool has_relations);

	 virtual ~osm_responder() throw();

	 // lists the standard types that OSM format can respond in, currently XML and, if 
   // the yajl library is provided, JSON.
	 std::list<mime::type> types_available() const;

	 // writes whatever is in the tmp_nodes/ways/relations tables to the given 
	 // formatter.
  void write(boost::shared_ptr<output_formatter> f);

protected:
	 // current transaction
	 pqxx::work &w;

	 // optional bounds element - this is only for information and has no effect on 
   // behaviour other than whether the bounds element gets written.
	 boost::optional<bbox> bounds;

	//specifies whether the appropriate tmp tables were created and thus should be outputted.
	bool write_nodes, write_ways, write_relations;
};

#endif /* OSM_RESPONDER_HPP */
