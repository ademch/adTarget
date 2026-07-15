#ifndef VIDEO_ANIMATED_PARAMS_H
#define VIDEO_ANIMATED_PARAMS_H

#include "vector_math.h"
#include "Quaternion.h"

class ParamKeyframeFloat
{
public:
	double time;
	float value;
};

class ParamKeyframeTRSTransform
{
public:
	double time;
	TRSTransform value;
};

class ParamKeyframePolyline2D
{
public:
	double time;
	std::vector<Vec2> value;
};

class AnimatedParamFloat
{
public:
	std::vector<ParamKeyframeFloat> liKeys;

	float Evaluate(double time);
	void  SetValueAt(double time, float value);
};


class AnimatedParamTRSTransform
{
public:
	std::vector<ParamKeyframeTRSTransform> liKeys;

	Matr4 Evaluate(double time);
	void  SetValueAt(double time, TRSTransform value);
	const std::vector<ParamKeyframeTRSTransform>* GetKeys() const;
};


class AnimatedParamPolyline2D
{
public:
	std::vector<ParamKeyframePolyline2D> liKeys;

	std::vector<Vec2> Evaluate(double time);
	void SetValueAt(double time, std::vector<Vec2> value);
	const std::vector<ParamKeyframePolyline2D>* GetKeys() const;
};


#endif