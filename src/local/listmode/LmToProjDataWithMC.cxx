/*!
  \file
  \ingroup listmode
  \brief Implementation of class LmToProjDataWithMC
  \author Sanida Mustafovic
  \author Kris Thielemans
  $Date$
  $Revision $
*/
/*
    Copyright (C) 2003- $Date$, Hammersmith Imanet Ltd
    See STIR/LICENSE.txt for details
*/
#include "local/stir/listmode/LmToProjDataWithMC.h"
#include "stir/ProjDataInfoCylindricalNoArcCorr.h"
#include "local/stir/listmode/CListRecordECAT966.h"
#include "stir/IO/stir_ecat7.h"
#include "stir/Succeeded.h"
#include <time.h>
#include "stir/is_null_ptr.h"


START_NAMESPACE_STIR

void 
LmToProjDataWithMC::set_defaults()
{
  LmToProjData::set_defaults();
  attenuation_filename ="";
  ro3d_ptr = 0;
  transmission_duration = 300; // default value 5 min.
  polaris_time_offset =-1234567.F; // note: do not change constant unless changing it below as well
}

void 
LmToProjDataWithMC::initialise_keymap()
{
  LmToProjData::initialise_keymap();
  parser.add_start_key("LmToProjDataWithMC Parametres");
  parser.add_parsing_key("Rigid Object 3D Motion Type", &ro3d_ptr); 
  parser.add_key("attenuation_filename", &attenuation_filename);
  parser.add_key("transmission_duration", &transmission_duration);
  parser.add_key("polaris_time_offset", &polaris_time_offset);
  parser.add_stop_key("END");
}

LmToProjDataWithMC::
LmToProjDataWithMC(const char * const par_filename)
{
  set_defaults();
  if (par_filename!=0)
    parse(par_filename) ;
  else
    ask_parameters();

}

bool
LmToProjDataWithMC::
post_processing()
{
  if (LmToProjData::post_processing())
    return true;
   
  if (is_null_ptr(ro3d_ptr))
  {
    warning("Invalid Rigid Object 3D Motion object\n");
    return true;
  }

   // compute average motion in respect to the transmission scan
  float att_start_time, att_end_time;
  if (attenuation_filename !="")
  {
  find_ref_pos_from_att_file (att_start_time, att_end_time,transmission_duration,
			       attenuation_filename);

  RigidObject3DTransformation av_motion = ro3d_ptr->compute_average_motion(att_start_time,att_end_time);
 
  ro3d_move_to_reference_position =av_motion.inverse();
    
  }
  else
  { 
    att_start_time=0;
    att_end_time=0;
    Quaternion<float> quat(1,0,0,0);
    RigidObject3DTransformation av_motion(quat,CartesianCoordinate3D<float>(0,0,0));
    ro3d_move_to_reference_position =av_motion.inverse();
  }

  // TODO move to RigidObject3DMotion
  if (polaris_time_offset==-1234567.F)
    ro3d_ptr->synchronise(*lm_data_ptr);
  else
  {
    cerr << " Polaris time offset is: " << polaris_time_offset << endl;
    dynamic_cast<RigidObject3DMotionFromPolaris* >(ro3d_ptr.get())->set_polaris_time_offset(polaris_time_offset);
  }

  move_from_scanner =
    RigidObject3DTransformation(Quaternion<float>(0.00525584F, -0.999977F, -0.00166456F, 0.0039961F),
                               CartesianCoordinate3D<float>( -1981.93F, 3.96638F, 20.1226F));
  move_to_scanner = move_from_scanner;
  move_to_scanner.inverse();

  return false;
}


void 
LmToProjDataWithMC::
find_ref_pos_from_att_file (float& att_start_time, float& att_end_time, 
			    float transmission_duration,
			    const string attenuation_filename)
{
	MatrixFile* AttnFile = matrix_open(attenuation_filename.c_str(), MAT_READ_ONLY, AttenCor );
	if (AttnFile==NULL)
	  error("Error opening attenuation file %s\n", attenuation_filename.c_str());

		/* Acquisition date and time - main head */
	time_t sec_time = AttnFile->mhptr->scan_start_time;

	struct tm* AttnTime = localtime( &sec_time  ) ;
	matrix_close( AttnFile ) ;
	att_start_time = ( AttnTime->tm_hour * 3600.0 ) + ( AttnTime->tm_min * 60.0 ) + AttnTime->tm_sec ;
	att_end_time = att_start_time + transmission_duration;
}
 
void 
LmToProjDataWithMC::get_bin_from_event(Bin& bin, const CListEvent& event_of_general_type) const
{
  const CListRecordECAT966& record = 
    static_cast<CListRecordECAT966 const&>(event_of_general_type);// TODO get rid of this
  const CListEventDataECAT966& event = 
    static_cast<CListRecordECAT966 const&>(event_of_general_type).event_data;// TODO get rid of this

  const ProjDataInfoCylindricalNoArcCorr& proj_data_info =
    static_cast<const ProjDataInfoCylindricalNoArcCorr&>(*template_proj_data_info_ptr);


  record.get_uncompressed_bin(bin);
  const float bin_efficiency = normalisation_ptr->get_bin_efficiency(bin);
   
  //Do the motion correction
  
  // find cartesian coordinates on LOR
  CartesianCoordinate3D<float> coord_1;
  CartesianCoordinate3D<float> coord_2;

  record.get_uncompressed_proj_data_info_sptr()->
    find_cartesian_coordinates_of_detection(coord_1,coord_2, bin);
  
  // now do the movement
  
  RigidObject3DTransformation ro3dtrans;

  ro3d_ptr->get_motion(ro3dtrans,current_time);
   

  
  const CartesianCoordinate3D<float> coord_1_transformed =
     move_to_scanner.
     transform_point(ro3d_move_to_reference_position.
                   transform_point(
                                  ro3dtrans.
                                   transform_point(move_from_scanner.
                                                   transform_point(coord_1))));
  const CartesianCoordinate3D<float> coord_2_transformed =
     move_to_scanner.
     transform_point(ro3d_move_to_reference_position.
                   transform_point(
                                  ro3dtrans.
                                   transform_point(move_from_scanner.
                                                   transform_point(coord_2))));


  proj_data_info.
    find_bin_given_cartesian_coordinates_of_detection(bin,
                                                      coord_1_transformed,
					              coord_2_transformed);

  // given two CartesianCoordinates find the intersection     
  if (bin.get_bin_value() > 0)
  {
    // now normalise event taking into account the
    // normalisation factor before motion correction and the number of
    // uncompressed bins that contribute to this bin
    // TODO this normalisation is not really correct
    bin.set_bin_value(1/ 
		      (bin_efficiency*
		       proj_data_info.
		         get_num_ring_pairs_for_segment_axial_pos_num(bin.segment_num(),
								    bin.axial_pos_num())*
		       proj_data_info.get_view_mashing_factor()));
  }

  
}


void 
LmToProjDataWithMC::transform_detector_pair_into_view_bin (int& view,int& bin, 
					    const int det1,const int det2, 
					    const Scanner& scanner) const
{ 
  const int num_detectors = scanner.get_num_detectors_per_ring();
  const int h=num_detectors/2;
  const int x = (det1>det2)?det1:det2;
  const int y = (det1<det2)?det1:det2;
  const int a=((x+y+h+1)%num_detectors)/2;
  const int b=a+h;
  int te=abs(x-y-h);
  if ((y<a)||(b<x)) te = -te;
  bin=te;
  view=a;
}





END_NAMESPACE_STIR
