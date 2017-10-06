/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

//-----------------------------------------------------------------------------------------------------------

namespace eckit { class Configuration; }
namespace atlas { namespace util { namespace detail { class CallStack; } } }

namespace atlas {
namespace runtime {
namespace timer {

class Timings {
public:
    using CallStack = util::detail::CallStack;
    using Identifier = size_t;

public: // static methods

    static Identifier add( const CallStack& stack, const std::string& title );

    static void update( const Identifier& id, double seconds );

    static std::string report();

    static std::string report( const eckit::Configuration& );

};


} // namespace timer
} // namespace runtime
} // namespace atlas

