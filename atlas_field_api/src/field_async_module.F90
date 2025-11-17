! (C) Copyright 2025- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! In applying this licence, ECMWF does not waive the privileges and immunities
! granted to it by virtue of its status as an intergovernmental organisation
! nor does it submit to any jurisdiction.

module field_async_module
private
public :: init_queue, wait_for_async_queue

contains

subroutine init_queue(queue)
  use, intrinsic :: iso_fortran_env, only : int32
  integer(int32), intent(in) :: queue
end subroutine

subroutine wait_for_async_queue(queue)
  use, intrinsic :: iso_fortran_env, only : int32
  integer(int32), intent(in) :: queue
end subroutine

end module
