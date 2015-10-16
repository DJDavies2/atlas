! (C) Copyright 2013-2015 ECMWF.



!------------------------------------------------------------------------------
TYPE, extends(atlas_object) :: atlas_GridDistribution

! Purpose :
! -------
!   *GridDistribution* : Object passed from atlas to inspect grid distribution

! Methods :
! -------

! Author :
! ------
!   12-Mar-2014 Willem Deconinck     *ECMWF*

!------------------------------------------------------------------------------
contains

  procedure, public :: final => atlas_GridDistribution__final

  procedure, public :: delete => atlas_GridDistribution__delete

END TYPE atlas_GridDistribution

!------------------------------------------------------------------------------

interface atlas_GridDistribution
  module procedure atlas_GridDistribution__ctor
end interface
