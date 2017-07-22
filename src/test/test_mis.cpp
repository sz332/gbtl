/*
 * Copyright (c) 2015 Carnegie Mellon University and The Trustees of Indiana
 * University.
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
 * MELLON UNIVERSITY AND THE TRUSTEES OF INDIANA UNIVERSITY EXPRESSLY DISCLAIM
 * TO THE FULLEST EXTENT PERMITTED BY LAW ALL EXPRESS, IMPLIED, AND STATUTORY
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
 *
 * This Program is distributed under a BSD license.  Please see LICENSE file or
 * permission@sei.cmu.edu for more information.  DM-0002659
 */

#include <iostream>

#include <algorithms/mis.hpp>
#include <algorithms/bfs.hpp>
#include <graphblas/graphblas.hpp>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE mis_suite

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mis_suite)

//****************************************************************************
BOOST_AUTO_TEST_CASE(mis_test)
{
    typedef GraphBLAS::Matrix<double> GBMatrix;
    typedef GraphBLAS::Vector<double> GBVector;

    GraphBLAS::IndexType const NUM_NODES(7);
    GraphBLAS::IndexArrayType i = {0, 0, 1, 1, 2, 2, 3, 3, 3,
                                   4, 4, 5, 5, 5, 6, 6};
    GraphBLAS::IndexArrayType j = {1, 2, 0, 3, 0, 5, 1, 4, 6,
                                   3, 5, 2, 4, 6, 3, 5};
    std::vector<double> v(i.size(), 1);
    GBMatrix graph(NUM_NODES, NUM_NODES);
    graph.build(i, j, v);

    std::cout << "*********** MIS1 *********" << std::endl;
    double set_sizes(0);
    double num_samples(0);
    // Run MANY experiments to get different IS's
    for (unsigned int seed = 1000; seed < 1200; ++seed)
    {
        //GBMatrix independent_set(NUM_NODES, 1, 0);
        GraphBLAS::Vector<bool> independent_set(NUM_NODES);
        algorithms::mis(graph, independent_set, float(seed));
        //GraphBLAS::print_vector(std::cout, independent_set,
        //                        "independent_set (flags)");

        // Get the set of vertex ID's
        GraphBLAS::IndexType set_size(independent_set.nvals());
        GraphBLAS::IndexArrayType result(set_size);
        std::vector<bool> vals(set_size);
        independent_set.extractTuples(result.begin(), vals.begin());
        //algorithms::get_vertex_IDs(independent_set));
        //std::cout << "Seed=" << seed << ": ";
        //for (auto it = result.begin(); it != result.end(); ++it)
        //    std::cerr << *it << " ";
        //std::cerr << std::endl;
        set_sizes += set_size;
        num_samples += 1.0;

        // Check the result by performing bfs_level() from the independent_set
        // All levels should be 1 (if in IS) or 2 (if not in IS)
        GBVector levels(NUM_NODES);
        algorithms::bfs_level_masked(graph, independent_set, levels);
        //GraphBLAS::print_vector(std::cout, levels, "BFS levels from IS");

        for (GraphBLAS::IndexType ix = 0; ix < NUM_NODES; ++ix)
        {
            if (levels.hasElement(ix))
            {
                double lvl = levels.extractElement(ix);
                BOOST_CHECK((lvl < 3) && (lvl > 0));
                if (lvl != 1)
                {
                    BOOST_CHECK_EQUAL(independent_set.hasElement(ix), false);
                }
                else
                {
                    BOOST_CHECK_EQUAL(independent_set.hasElement(ix), true);
                    BOOST_CHECK_EQUAL(independent_set.extractElement(ix), true);
                }
            }
        }
    }
    std::cerr << "Avg. maximal independent set size = "
              << set_sizes/num_samples << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
