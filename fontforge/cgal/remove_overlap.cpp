/* Copyright 2025 Maxim Iorsh <iorsh@users.sourceforge.net>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include "remove_overlap.hpp"

typedef CGAL::CORE_algebraic_number_traits Nt_traits;
typedef Nt_traits::Rational Rational;
typedef Nt_traits::Algebraic Algebraic;
typedef CGAL::Cartesian<Rational> Rat_kernel;
typedef CGAL::Cartesian<Algebraic> Alg_kernel;
typedef CGAL::Arr_Bezier_curve_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
    Traits;
typedef Traits::Curve_2 Bezier_curve;
typedef CGAL::Arrangement_2<Traits> Arrangement;

SplineSet* CGAL_SplineSetRemoveOverlap(SplineChar* sc, SplineSet* base,
                                       enum overlap_type ot) {
    // Create a traits class for Bézier curves
    Traits traits;

    // Create an arrangement with history
    Arrangement arr(&traits);

    // Define a Bézier polycurve
    std::vector<Rat_kernel::Point_2> control_points = {
        Rat_kernel::Point_2(0, 0), Rat_kernel::Point_2(1, 2),
        Rat_kernel::Point_2(3, 2), Rat_kernel::Point_2(4, 0)};

    // Create a Bézier curve from the control points
    Bezier_curve curve(control_points.begin(), control_points.end());

    // Insert the curve into the arrangement
    CGAL::insert(arr, curve);

    // Define another Bézier polycurve
    control_points = {Rat_kernel::Point_2(1, 1), Rat_kernel::Point_2(2, 3),
                      Rat_kernel::Point_2(3, 1), Rat_kernel::Point_2(4, 3)};

    // Create a Bézier curve from the control points
    curve = Bezier_curve(control_points.begin(), control_points.end());

    // Insert the curve into the arrangement
    CGAL::insert(arr, curve);

    return nullptr;
}
