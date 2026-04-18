#pragma once
#include <vector>
typedef std::vector<double>	dvector;

double interp_linear(
	const dvector&	x_in,
	const dvector&	y_in,
	const double	x
	);

dvector como_increasing_concave(
	const dvector&	x,
	const dvector&	y,
	const double	lambda
	);

dvector como(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const dvector&	x_out
	);

dvector calculate_mid_slopes(
	const dvector&	x_in,
	const dvector&	y_in,
	const bool		monotonic,
	const double	slope_begin,
	const double	slope_end,
	const double	slope_wght			// = 0.5
	);

dvector interp_conique_no_inflexion(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const double	lambda,
	const dvector&	x_out,
	const bool		linear_if_concave,
	const bool		linear_if_convex,
	const bool		lower_branche		// = false
	);

dvector interp_conique_increasing_concave(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const double	lambda,
	const dvector&	x_out
	); 

/// returns the increasing concave envelope.
/// 1. cap points greater than last point / floor point less than first point
/// 2. remove any non concave point
/// 3. step 1 and 2 are enough to guarantee we are increasing provided that first point less than last point
dvector increasing_concave_envelop(
	const dvector&	x_in,
	const dvector&	y_in
	);