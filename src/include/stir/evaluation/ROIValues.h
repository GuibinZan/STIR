//
// $Id$
//
/*!
  \file
  \ingroup eval_buildblock

  \brief Definition of class ROIValues

  \author Damiano Belluzzo
  \author Kris Thielemans
  \author PARAPET project
  $Date$
  $Revision$
*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/

#ifndef __stir_eval_buildblock_ROIValues__H__
#define __stir_eval_buildblock_ROIValues__H__

#include "stir/common.h"

#include <string>
#include <iostream>
#ifndef STIR_NO_NAMESPACES
using std::string;
#endif

START_NAMESPACE_STIR


/*!
  \ingroup eval_buildblock
  \brief A class to store and get results of an ROI calculation.
  
  This class stores the volume of the ROI (in cubic mm), the integral over the ROI of the functions and
  its square and the min and max values in the ROI. These values are used to compute mean, 
  standard deviation and coefficient of variance.
*/
class ROIValues
{

public:

  ROIValues()
    {init();};

  ROIValues(float roi_volume, float integral, float integral_of_square, float min_value, float max_value)
    : roi_volume(roi_volume), integral(integral), integral_of_square(integral_of_square), 
      min_value(min_value), max_value(max_value)
    {
      update();
    };

  //! Combine the ROI values appropriately
  ROIValues operator+= (const ROIValues &iv)	
    {
      roi_volume += iv.roi_volume;
      integral += iv.integral;
      integral_of_square += iv.integral_of_square;


#ifndef STIR_NO_NAMESPACES
      min_value = std::min(min_value, iv.min_value);
      max_value = std::max(max_value, iv.max_value);
#else
      min_value = min(min_value, iv.min_value);
      max_value = max(max_value, iv.max_value);
#endif

      update();
      return *this;
    };

  //! Return a string with all info, one per line
  string report() const;

  //! Total valume (in mm^3)
  float get_roi_volume() const
    { return roi_volume; }
  //! Sum of elements times voxel volume
  float get_integral() const
    { return integral; }
  //! Sum of squares times voxel volume
  float get_integral_of_square() const
    { return integral_of_square; }
  //! Mean value
  float get_mean() const
    { return mean_value; }
  //! Variance
  float get_variance() const
    { return variance_value; }
  //! Standard deviation
  float get_stddev() const
    {return std_value; }
  //! Coefficient of Variance =stddev/mean)
  float get_CV() const
    {return std_value/mean_value; }
  //! Minimum value in the ROI
  float get_min() const 
    { return min_value; }
  //! Maximum value in the ROI
  float get_max() const 
    { return max_value; }


//friend ostream &operator <<( ostream &stream, ROIValues val);

private:

  float roi_volume;
  float integral;
  float integral_of_square;

  float mean_value;
  float variance_value;
  float std_value;

  float min_value;
  float max_value;
  
  void init();
  void update();
};

END_NAMESPACE_STIR

#endif
