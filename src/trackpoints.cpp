#include "trackpoints.hpp"
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <pqxx/pqxx>

#include "cache.hpp"
#include "logger.hpp"
#include "quad_tile.hpp"
#include "temp_tables.hpp"

using std::vector;
using std::string;
using std::transform;
using boost::shared_ptr;
using std::stringstream;
using std::set;

static void
construct_query(const bbox & bounds, const int page, stringstream & query)
{
	const
	    set < unsigned int >
	    tiles = tiles_for_area(bounds.minlat, bounds.minlon,
				   bounds.maxlat, bounds.maxlon);

	//                                stringstream query;
	query <<
	    "select gpx_id, trackid, to_char(timestamp,'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"') as timestampt, latitude, longitude from gps_points where ((";
	unsigned int first_id = 0, last_id = 0;
	for (set < unsigned int >::const_iterator itr = tiles.begin();
	     itr != tiles.end(); ++itr) {
		if (first_id == 0) {
			last_id = first_id = *itr;
		} else if (*itr == last_id + 1) {
			++last_id;
		} else {
			if (last_id == first_id) {
				query << "tile = " << last_id << " or ";
			} else {
				query << "tile between " << first_id
				    << " and " << last_id << " or ";
			}
			last_id = first_id = *itr;
		}
	}
	if (last_id == first_id) {
		query << "tile = " << last_id << ") ";
	} else {
		query << "tile between " << first_id << " and " << last_id <<
		    ") ";
	}
	query << "and latitude between " << int (bounds.minlat * SCALE) << " and " << int (bounds.maxlat * SCALE) << " and longitude between " << int (bounds.minlon * SCALE) << " and " << int (bounds.maxlon * SCALE) << ")" << " order by gpx_id DESC, trackid ASC, timestamp ASC" << " limit " << TRACKPOINTS_PAGE << " offset " << (page * TRACKPOINTS_PAGE);	// limit here as a quick hack to reduce load...

}

static void write_trackdesc(xml_writer & writer, shared_ptr < gpxfile const >gf)
{
	if (!gf->identifiable) return;
	writer.start("name");
	writer.text(gf->file_name);
	writer.end();
	writer.start("desc");
	writer.text(gf->file_description);
	writer.end();
	writer.start("url");
	writer.text("http://api.openstreetmap.org/trace/1234/view");
	writer.end();
}

void
write_trackpoints(pqxx::work & w,
		  xml_writer & writer,
		  const bbox & bounds,
		  const int page, cache < long int, gpxfile > &gpxfile_cache)
{

	stringstream query;

	construct_query(bounds, page, query);
	logger::message("Fetching  gps_points");
	logger::message(query.str());
	pqxx::result points = w.exec(query);

	logger::message("Got  gps_points, streaming result");

	try {
		writer.start("gpx");
		writer.attribute("version", string("1.0"));
		writer.attribute("creator", string(PACKAGE_STRING));

		long int cur_gpx_id = -1;
		long int cur_track_id = -1;
		bool include_timestamps = false;

		shared_ptr < gpxfile const >gf;
		for (pqxx::result::const_iterator itr = points.begin();
		     itr != points.end(); ++itr) {
			int lat = itr["latitude"].as < int >();
			int lon = itr["longitude"].as < int >();
			long int gpx_id = itr["gpx_id"].as < long int >();
			int track_id = itr["trackid"].as < long int >();

			if (cur_gpx_id != gpx_id) {
				gf = gpxfile_cache.get(gpx_id);
				include_timestamps = gf->trackable;
				if (cur_gpx_id != -1) {
					writer.end();	//track segment
					writer.end();	//track
				}
				writer.start("trk");
				write_trackdesc(writer, gf);
				cur_gpx_id = gpx_id;
				cur_track_id = -1;
			}

			if (cur_track_id != track_id) {
				if ((cur_track_id != -1) && gf->trackable) {
					writer.end();
				}
				if (gf->trackable || (cur_track_id == -1)) {
					writer.start("trkseg");
				}
				cur_track_id = track_id;
			}

			writer.start("trkpt");
			writer.attribute("lat", double (lat) / double (SCALE));
			writer.attribute("lon", double (lon) / double (SCALE));
			if (include_timestamps) {
				writer.start("time");
				writer.text(itr["timestampt"].c_str());
				writer.end();
			}
			writer.end();
		}
		if (cur_gpx_id != -1) {
			writer.end();	//track segment
			writer.end();	//track
		}

	}
	catch(const std::exception & e) {
		// write out an error element to the xml file - we've probably
		// already started writing, so its useless to try and alter the
		// HTTP code.
		writer.start("error");
		writer.text(e.what());
		writer.end();
	}

	writer.end();

}
