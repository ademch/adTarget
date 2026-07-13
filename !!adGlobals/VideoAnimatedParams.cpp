#include "stdafx.h"
#include "VideoAnimatedParams.h"
#include <assert.h>


float AnimatedParamFloat::Evaluate(double time)
{
	if (liKeys.empty())
		return 0;

	if (liKeys.size() == 1)
		return liKeys[0].value;

	if (time <= liKeys.front().time)
		return liKeys.front().value;

	if (time >= liKeys.back().time)
		return liKeys.back().value;

	for (size_t i = 0; i + 1 < liKeys.size(); ++i)
	{
		const ParamKeyframeFloat& a = liKeys[i];
		const ParamKeyframeFloat& b = liKeys[i + 1];

		if (time >= a.time && time <= b.time)
		{
			double u = (time - a.time) / (b.time - a.time);

			return a.value*(1.0f-u) + b.value*u;
		}
	}

	return liKeys.back().value;
}


void AnimatedParamFloat::SetValueAt(double time, float value)
{
	for (ParamKeyframeFloat& k : liKeys)
	{
		if (fabs(k.time - time) < 1e-6)
		{
			k.value = value;
			return;
		}
	}

	ParamKeyframeFloat k{time, value};

	auto it = std::lower_bound(	liKeys.begin(),
								liKeys.end(),
								time,
								[](const ParamKeyframeFloat& k, double time)
								{
									return k.time < time;
								}
							  );

	liKeys.insert(it, k);
}


//========================================================================================


Matr4 AnimatedParamTRSTransform::Evaluate(double time)
{
	if (liKeys.empty())
		return Mat4MakeIdent();

	if (liKeys.size() == 1)
		return Mat4Compose(liKeys[0].value);

	if (time <= liKeys.front().time)
		return Mat4Compose(liKeys.front().value);

	if (time >= liKeys.back().time)
		return Mat4Compose(liKeys.back().value);

	for (size_t i = 0; i + 1 < liKeys.size(); ++i)
	{
		const ParamKeyframeTRSTransform& a = liKeys[i];
		const ParamKeyframeTRSTransform& b = liKeys[i + 1];

		if (time >= a.time && time <= b.time)
		{
			double u = (time - a.time) / (b.time - a.time);

			return Mat4Compose( TRSTransformInterpolate(a.value,b.value, u) );
		}
	}

	return Mat4Compose(liKeys.back().value);
}


void AnimatedParamTRSTransform::SetValueAt(double time, TRSTransform value)
{
	for (ParamKeyframeTRSTransform& k : liKeys)
	{
		if (fabs(k.time - time) < 1e-4)
		{
			k.value = value;
			return;
		}
	}

	ParamKeyframeTRSTransform k{time, value};

	auto it = std::lower_bound(	liKeys.begin(),
								liKeys.end(),
								time,
								[](const ParamKeyframeTRSTransform& k, double time)
								{
									return k.time < time;
								}
							  );

	liKeys.insert(it, k);
}

const std::vector<ParamKeyframeTRSTransform>* AnimatedParamTRSTransform::GetKeys() const
{
	return &liKeys;
}



// ====================================================================================

std::vector<Vec2> AnimatedParamPolyline2D::Evaluate(double time)
{
	if (liKeys.empty())
	{
		std::vector<Vec2> empty;
		return empty;
	}

	if (liKeys.size() == 1)
		return liKeys[0].value;

	if (time <= liKeys.front().time)
		return liKeys.front().value;

	if (time >= liKeys.back().time)
		return liKeys.back().value;

	for (size_t i = 0; i + 1 < liKeys.size(); ++i)
	{
		const ParamKeyframePolyline2D& a = liKeys[i];
		const ParamKeyframePolyline2D& b = liKeys[i + 1];

		if (time >= a.time && time <= b.time)
		{
			double u = (time - a.time) / (b.time - a.time);

			assert(a.value.size() == b.value.size());

			std::vector<Vec2> vNew;
			vNew.reserve(a.value.size());

			for (size_t i = 0; i < a.value.size(); ++i)
			{
				vNew.push_back( VecMix(a.value[i], b.value[i], u) );
			}
			
			return vNew;
		}
	}

	return liKeys.back().value;
}

void AnimatedParamPolyline2D::SetValueAt(double time, std::vector<Vec2> value)
{
	for (ParamKeyframePolyline2D& k : liKeys)
	{
		if (fabs(k.time - time) < 1e-4)
		{
			k.value = value;
			return;
		}
	}

	ParamKeyframePolyline2D k{time, value};

	auto it = std::lower_bound(	liKeys.begin(),
								liKeys.end(),
								time,
								[](const ParamKeyframePolyline2D& k, double time)
								{
									return k.time < time;
								}
							  );

	liKeys.insert(it, k);
}

const std::vector<ParamKeyframePolyline2D>* AnimatedParamPolyline2D::GetKeys() const
{
	return &liKeys;
}
