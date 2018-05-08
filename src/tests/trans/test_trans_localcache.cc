/*
 * (C) Copyright 2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <algorithm>
#include <iomanip>

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/NodeColumns.h"
#include "atlas/functionspace/Spectral.h"
#include "atlas/functionspace/StructuredColumns.h"
#include "atlas/grid.h"
#include "atlas/grid/Distribution.h"
#include "atlas/grid/Partitioner.h"
#include "atlas/grid/detail/partitioner/EqualRegionsPartitioner.h"
#include "atlas/grid/detail/partitioner/TransPartitioner.h"
#include "atlas/library/Library.h"
#include "atlas/mesh/Mesh.h"
#include "atlas/mesh/Nodes.h"
#include "atlas/meshgenerator/StructuredMeshGenerator.h"
#include "atlas/output/Gmsh.h"
#include "atlas/parallel/mpi/mpi.h"
#include "atlas/runtime/Trace.h"
#include "atlas/trans/Trans.h"
#include "atlas/trans/LegendreCacheCreator.h"
#include "atlas/trans/local_noopt/FourierTransforms.h"
#include "atlas/trans/local_noopt/LegendrePolynomials.h"
#include "atlas/trans/local_noopt/LegendreTransforms.h"
#include "atlas/util/Constants.h"
#include "atlas/util/Earth.h"
#include "eckit/utils/MD5.h"

#include "tests/AtlasTestEnvironment.h"

#if ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif

using namespace eckit;

using atlas::array::Array;
using atlas::array::ArrayView;
using atlas::array::make_view;

namespace atlas {
namespace test {

//-----------------------------------------------------------------------------

struct AtlasTransEnvironment : public AtlasTestEnvironment {
    AtlasTransEnvironment( int argc, char* argv[] ) : AtlasTestEnvironment( argc, argv ) {
#if ATLAS_HAVE_TRANS
        trans_use_mpi( mpi::comm().size() > 1 );
        trans_init();
#endif
    }

    ~AtlasTransEnvironment() {
#if ATLAS_HAVE_TRANS
        trans_finalize();
#endif
    }
};

using trans::Trans;
using trans::LegendreCache;
using trans::LegendreCacheCreator;
using trans::Cache;
using grid::StructuredGrid;
using grid::GaussianGrid;
using XSpace = StructuredGrid::XSpace;
using YSpace = StructuredGrid::YSpace;
using LinearSpacing = grid::LinearSpacing;

eckit::PathName CacheFile(const std::string& path) {
    eckit::PathName cachefile(path);
    if( cachefile.exists() ) cachefile.unlink();
    return cachefile;
}

std::string hash( const trans::Cache& c ) {
    return eckit::MD5( c.legendre().data(), c.legendre().size() ).digest();
}

std::string hash( const eckit::PathName& f ) {
    return hash( LegendreCache(f) );
}

std::string F(int n)    { return "F"   +std::to_string(n); }
std::string O(int n)    { return "O"   +std::to_string(n); }
std::string N(int n)    { return "N"   +std::to_string(n); }
std::string L(int n)    { return "L"   +std::to_string(n); }
std::string S(int n)    { return "S"   +std::to_string(n); }
std::string Slon(int n) { return "Slon"+std::to_string(n); }
std::string Slat(int n) { return "Slat"+std::to_string(n); }

//-----------------------------------------------------------------------------

CASE( "test_global_grids" ) {
    // auto resolutions = { 32, 64, 160, 320, 640 };
    auto resolutions = { 32, 64 };
    for( int n : resolutions ) {
        int t = n-1;
        auto cases = {
            std::make_pair(F(n),t),
            std::make_pair(O(n),t),
            std::make_pair(N(n),t),
            std::make_pair(L(n),t),
            std::make_pair(S(n),t),
            std::make_pair(Slon(n),t),
            std::make_pair(Slat(n),t),
        };

        LegendreCacheCreator F_cache_creator( Grid(F(n)), t, option::type("local") );
        EXPECT( F_cache_creator.supported() );
        auto F_cachefile = CacheFile("leg_"+F_cache_creator.uid()+".bin");
        F_cache_creator.create( F_cachefile );
        Cache F_cache = LegendreCache( F_cachefile );
        auto F_cache_hash = hash(F_cache);

        for( auto _case : cases )
        {
            auto gridname   = _case.first;
            auto truncation = _case.second;
            Log::info() << "Case "+gridname+" T"+std::to_string(truncation) << std::endl;
            ATLAS_TRACE("Case "+gridname+" T"+std::to_string(truncation));
            Grid grid(gridname);

            LegendreCacheCreator cache_creator( grid, truncation, option::type("local") );
            EXPECT( cache_creator.supported() );
            auto cachefile = CacheFile("leg_"+cache_creator.uid()+".bin");
            cache_creator.create( cachefile );
            if( GaussianGrid(grid) ) {
                EXPECT( hash(cachefile) == F_cache_hash );
            }

            ATLAS_TRACE_SCOPE("create without cache")
                Trans( grid, truncation, option::type("local") );

            Cache cache;
            ATLAS_TRACE_SCOPE("read cache")
                cache = LegendreCache( cachefile );
            ATLAS_TRACE_SCOPE("create with cache")
                Trans( cache, grid, truncation, option::type("local") );

        }
    }
}

CASE( "test_global_grids_with_subdomain" ) {
    int n = 64;
    int t = n-1;
    auto cases = {
        std::make_pair(F(n),t),
        std::make_pair(O(n),t),
        std::make_pair(N(n),t),
        std::make_pair(L(n),t),
        std::make_pair(S(n),t),
        std::make_pair(Slon(n),t),
        std::make_pair(Slat(n),t)
    };
    auto domains = std::vector<Domain>{
        ZonalBandDomain  ( {-10., 5.} ),
        RectangularDomain( {-1., 1.}, {50., 55.} ),
        RectangularDomain( {-1., 1.}, {-5., 40.} ),
    };
    for( auto _case : cases )
    {
        auto gridname   = _case.first;
        auto truncation = _case.second;

        ATLAS_TRACE("Case "+gridname+" T"+std::to_string(truncation));

        Grid global_grid( gridname );

        LegendreCacheCreator global_cache_creator( Grid(gridname), truncation, option::type("local") );
        EXPECT( global_cache_creator.supported() );
        auto global_cachefile = CacheFile( "leg_" + global_cache_creator.uid() + ".bin" );
        ATLAS_TRACE_SCOPE( "Creating cache " + std::string( global_cachefile ) )
            global_cache_creator.create( global_cachefile );

        Cache global_cache;
        ATLAS_TRACE_SCOPE("read cache")
            global_cache = LegendreCache( global_cachefile );
        auto global_hash = hash(global_cache);

        for( auto domain : domains ) {
            Grid grid( gridname, domain );
            ATLAS_TRACE_SCOPE("create with cache")
                Trans( global_cache, global_grid, domain, truncation, option::type("local") );
        }
    }
}

CASE( "test_regional_grids nested_in_global" ) {
    auto cachefile = CacheFile("regional_lonlat.bin");
    auto truncation = 89;
    Cache cache;
    StructuredGrid grid_global(
        LinearSpacing( {  0., 360.}, 360, false ),
        LinearSpacing( { 90., -90.}, 181, true  )
    );
    EXPECT( grid_global.domain().global() );

    LegendreCacheCreator global_cache_creator( grid_global, truncation, option::type("local") );
    EXPECT( global_cache_creator.supported() );
    auto global_cachefile = CacheFile( "leg_" + global_cache_creator.uid() + ".bin" );
    ATLAS_TRACE_SCOPE( "Creating cache "+std::string(cachefile) )
        global_cache_creator.create( global_cachefile );



    StructuredGrid regional( LinearSpacing( {0.,180.}, 181 ), LinearSpacing( {0.,45.}, 46 ) );


    ATLAS_TRACE_SCOPE("create without cache")
        Trans( grid_global, regional.domain(), truncation, option::type("local") );
    ATLAS_TRACE_SCOPE("read cache")
        cache = LegendreCache( global_cachefile );
    ATLAS_TRACE_SCOPE("create with cache")
        Trans( cache, grid_global, regional.domain(), truncation, option::type("local") );
}

CASE( "test_regional_grids not nested" ) {
    auto cachefile = CacheFile("cache-regional.bin");
    auto truncation = 89;
    Cache cache;

    StructuredGrid grid( LinearSpacing( {0.,180.}, 181 ), LinearSpacing( {0.,45.}, 46 ) );

    LegendreCacheCreator cache_creator( grid, truncation, option::type("local") );
    EXPECT( cache_creator.supported() );
    ATLAS_TRACE_SCOPE( "Creating cache "+std::string(cachefile) )
        cache_creator.create( cachefile );

    ATLAS_TRACE_SCOPE("create without cache")
        Trans( grid, truncation, option::type("local") );
    ATLAS_TRACE_SCOPE("read cache")
        cache = LegendreCache( cachefile );
    ATLAS_TRACE_SCOPE("create with cache")
        Trans( cache, grid, truncation, option::type("local") );
}

CASE( "test_regional_grids with projection" ) {
    auto cachefile = CacheFile("cache-regional.bin");
    auto truncation = 89;
    Cache cache;

    Projection projection( util::Config
       ( "type",      "rotated_lonlat")
       ("north_pole", std::vector<double>{ 4., 54.} ) );

    StructuredGrid grid( LinearSpacing( {0.,180.}, 181 ), LinearSpacing( {0.,45.}, 46 ), projection );
    ATLAS_TRACE_SCOPE("create without cache")
        Trans( grid, truncation, option::type("local") );

    // Note: caching not yet implemented for unstructured and projected grids
}


CASE( "test cache creator" ) {

    auto truncation = 89;
    StructuredGrid grid_global(
        LinearSpacing( {  0., 360.}, 360, false ),
        LinearSpacing( { 90., -90.}, 181, true  )
    );

    LegendreCacheCreator legendre_cache_creator( grid_global, truncation, option::type("local") );
    auto cachefile = CacheFile( legendre_cache_creator.uid() );
    ATLAS_TRACE_SCOPE( "Creating cache "+std::string(cachefile) )
      legendre_cache_creator.create( cachefile );
}

}  // namespace test
}  // namespace atlas

int main( int argc, char** argv ) {
    return atlas::test::run<atlas::test::AtlasTransEnvironment>( argc, argv );
}
