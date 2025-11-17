! (C) Copyright 2025- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! In applying this licence, ECMWF does not waive the privileges and immunities
! granted to it by virtue of its status as an intergovernmental organisation
! nor does it submit to any jurisdiction.

module field_defaults_module
  logical :: INIT_PINNED_VALUE  = .FALSE.
  logical :: INIT_SYNC_ON_FINAL = .TRUE.
  logical :: INIT_MAP_DEVPTR    = .TRUE.
  logical :: POOL_OWNED_FIELDS  = .FALSE.
end module
