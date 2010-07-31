#ifndef TRACKPOINTS_HPP
#define TRACKPOINTS_HPP

#include "writer.hpp"
#include "bbox.hpp"
#include "cache.hpp"
#include "gpxfile.hpp"
#include <pqxx/pqxx>

#define TRACKPOINTS_PAGE 50000

/**
 * writes the trackpoints to the xml_writer. Gpx file properties are looked up directly from the 
 * cache rather than joined in SQL.
 */
void write_trackpoints(pqxx::work &work,
	       xml_writer &writer,
	       const bbox &bounds,
		   int page,
	       cache<long int, gpxfile> &gpxfile_cache);

#endif /* TRACKPOINTS_HPP */
