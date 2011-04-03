#ifndef RELATIONS_HANDLER_HPP
#define RELATIONS_HANDLER_HPP

#include "handler.hpp"
#include "osm_responder.hpp"
#include "fcgi_helpers.hpp"
#include <fcgiapp.h>
#include <pqxx/pqxx>
#include <string>
#include <list>

using std::list;

class relations_responder
	: public osm_responder {
public:
	 relations_responder(mime::type, list<id_t>, pqxx::work &);
	 ~relations_responder() throw();

private:
	 list<id_t> ids;
};

class relations_handler 
	: public handler {
public:
	 relations_handler(FCGX_Request &request);
	 ~relations_handler() throw();

	 std::string log_name() const;
	 responder_ptr_t responder(pqxx::work &x) const;

private:
	 list<id_t> ids;

	static list<id_t> validate_request(FCGX_Request &request);
};

#endif /* RELATION_HANDLER_HPP */
