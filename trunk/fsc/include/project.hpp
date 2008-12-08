//  (c) Copyright 2004 - HYDROWIDE - Project LIQUID
//
//  Distributed under the Hydrowide Software License,
//  (See accompanying file license.txt
//  or copy at http://www.hydrowide.com/license.txt)
//
//	$Revision: 1.1 $
//	$History: $


#ifndef __HYDROWIDE_FIWER_PREPROC_PROJECT_H__
#define __HYDROWIDE_FIWER_PREPROC_PROJECT_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <liquid/basic_types.h>


///Project loader (for testing purpose)
class project
{
public:
	void load(const std::string& _path)
	{
		boost::property_tree::ptree pt;

		read_xml(_path, pt);
		duration_ = pt.get<float64>("project.sim.duration");
		min_dt_out_ = pt.get<float64>("project.sim.min_dt_out");

		//boost::property_tree::ptree my_pt = pt.get_child("project.bouss2d");
		//bool has_out_level = my_pt.get<bool>("out.level");

		data_ = pt.get<std::string>("project.data");
		
		dem_path_ = pt.get<std::string>("project.in.dem_path");
		slopes_path_ = pt.get<std::string>("project.in.slopes_path");
		contrib_area_path_ = pt.get<std::string>("project.in.contrib_area_path");		
		topo_index_path_ = pt.get<std::string>("project.in.topo_index_path");
		watershed_path_ = pt.get<std::string>("project.in.watershed_path");
		channel_path_ = pt.get<std::string>("project.in.channel_path");

		simplify_tolerance_ = pt.get<float64>("project.pre.simplify_tolerance");

		min_angle_ = pt.get<float64>("project.pre.min_angle");
		min_face_area_ = pt.get<float64>("project.pre.min_face_area");
		max_face_area_ = pt.get<float64>("project.pre.max_face_area");
	}

	std::string data() const {return data_;}
	float64 duration() const {return duration_;}
	float64 min_dt_out() const {return min_dt_out_;}

	std::string dem_path() const {return  dem_path_;}
	std::string slopes_path() const {return  slopes_path_;}
	std::string contrib_area_path() const {return  contrib_area_path_;}
	std::string topo_index_path() const {return topo_index_path_;}
	std::string watershed_path() const {return watershed_path_;}
	std::string channel_path() const {return channel_path_;}

	float64 simplify_tolerance() const {return simplify_tolerance_;}

	float64 min_angle() const {return min_angle_;}
	float64 min_face_area() const {return min_face_area_;}
	float64 max_face_area() const {return max_face_area_;}

private:	
	std::string data_;
	float64 duration_;
	float64 min_dt_out_;

	std::string dem_path_;				/* The digital elevation model path */
	std::string slopes_path_;			/* The local pixel slopes path */
	std::string contrib_area_path_;		/* The pixel contributing area path */
	std::string topo_index_path_;
	std::string watershed_path_;		/* The watershed or domain boundary */
	std::string channel_path_;			/* The channel network */

	float64 simplify_tolerance_;		/* Peucker Douglas algorithm tolerance */

	float64 min_angle_;					/* Minimum interior angle in a face [degree] */
	float64 min_face_area_;				/* Minimum face area */
	float64 max_face_area_;				/* Maximum face area */
};

#endif // !defined(__HYDROWIDE_FIWER_PREPROC_PROJECT_H__)
