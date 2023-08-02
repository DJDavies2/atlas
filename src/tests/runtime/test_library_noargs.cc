/*
 * (C) Copyright 2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <stdexcept>
#include <iostream>

#include "eckit/runtime/Main.h"

#include "atlas/library.h"

int main(int argc, char** argv) {
    try {
        atlas::initialise();
    }
    catch (std::exception& e) {
        // Attempting to access a non-existent instance of eckit::Main()
        
        eckit::Main::initialise(argc,argv);
        atlas::initialise();
        atlas::finalise();
        return 0; // SUCCESS
    }
    return 1; // FAILED
}
