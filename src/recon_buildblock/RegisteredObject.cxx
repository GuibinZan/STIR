//
//
/*!

  \file
  \ingroup recon_buildblock
  \brief instantiations of stir::RegisteredObject for classes in recon_buildblock
  (only useful for Microsoft Visual Studio 6.0)

  \author Kris Thielemans

*/
/*
    Copyright (C) 2000- 2009, Hammersmith Imanet Ltd
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
// note: include has to be before #ifdef as it's in this file that
// __STIR_REGISTRY_NOT_INLINE is defined
#include "stir/RegisteredObject.h"

#ifdef __STIR_REGISTRY_NOT_INLINE

#pragma message("instantiating RegisteredObject<GeneralisedPrior<float> >")
#include "stir/recon_buildblock/GeneralisedPrior.h"

#pragma message("instantiating RegisteredObject<ProjMatrixByBin>")
#include "stir/recon_buildblock/ProjMatrixByBin.h"

#pragma message("instantiating RegisteredObject<ProjectorByBinPair>")
#include "stir/recon_buildblock/ProjectorByBinPair.h"

#pragma message("instantiating RegisteredObject<ForwardProjectorByBin>")
#include "stir/recon_buildblock/ForwardProjectorByBin.h"

#pragma message("instantiating RegisteredObject<BackProjectorByBin>")
#include "stir/recon_buildblock/BackProjectorByBin.h"

#pragma message("instantiating RegisteredObject<BinNormalisation>")
#include "stir/recon_buildblock/BinNormalisation.h"

// and others
START_NAMESPACE_STIR

template <typename Root>
RegisteredObject<Root>::RegistryType& 
RegisteredObject<Root>::registry ()
{
  static RegistryType the_registry("None", 0);
  return the_registry;
}

#  ifdef _MSC_VER
// prevent warning message on reinstantiation, 
// note that we get a linking error if we don't have the explicit instantiation below
#  pragma warning(disable:4660)
#  endif


template RegisteredObject<GeneralisedPrior<float> >;

template RegisteredObject<ProjMatrixByBin>; 

template RegisteredObject<ProjectorByBinPair>;

template RegisteredObject<ForwardProjectorByBin>;

template RegisteredObject<BackProjectorByBin>;

template RegisteredObject<BinNormalisation>;
END_NAMESPACE_STIR

#endif
