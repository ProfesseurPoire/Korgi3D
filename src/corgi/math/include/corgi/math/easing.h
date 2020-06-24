#pragma once

#include <corgi/math/Vec3.h>

namespace easing
{
	static float PI = 3.141597f;

	/*!
	 * @brief   t equals time, d equals duration, b equals start value, c is the varying value
	 *          over time
	 */
    static float linear_easing(float t, float b, float c, float d)
    {
        return( ( c*t ) / d ) + b;
    }

	/*!
	 * @brief   Interpolate a value between start and end by using the t value that must be inside the
	 *          [0.0f, 1.0f] range
	 */
	static Vec3 linear_easing(float t, Vec3 start, Vec3 end)
    {
        return Vec3
        (
            linear_easing(t, start.x, end.x - start.x, 1.0f),
            linear_easing(t, start.y, end.y - start.y, 1.0f),
            linear_easing(t, start.z, end.z - start.z, 1.0f)
        );
    }

	static float quadratic_easing_in(float t, float b, float c, float d)
    {
        return c * ( t /= d )*t + b;
    }

	static float quadratic_easing_out(float t, float b, float c, float d)
    {
        return -c * ( t /= d )*( t - 2 ) + b;
    }

	static float quadratic_easing_in_out(float t, float b, float c, float d)
    {
        if (( t /= d / 2 ) < 1) return ( ( c / 2 )*( t*t ) ) + b;
        return -c / 2 * ( ( ( t - 2 )*( --t ) ) - 1 ) + b;
    }

    /*!
     * @brief   Interpolate a value between start and end by using the t value that must be inside the
     *          [0.0f, 1.0f] range
     */
    static Vec3 quadratic_easing_in_out(float t, Vec3 start, Vec3 end)
    {
        return Vec3
        (
            quadratic_easing_in_out(t, start.x, end.x - start.x, 1.0f),
            quadratic_easing_in_out(t, start.y, end.y - start.y, 1.0f),
            quadratic_easing_in_out(t, start.z, end.z - start.z, 1.0f)
        );
    }

	static float bounce_easing_out(float t, float b, float c, float d);

	static float bounce_easing_in(float t, float b, float c, float d)
    {
        return c - bounce_easing_out(d - t, 0, c, d) + b;
    }

	static float bounce_easing_out(float t, float b, float c, float d)
    {
        if (( t /= d ) < ( 1 / 2.75f ))
        {
            return c * ( 7.5625f*t*t ) + b;
        }
        else if (t < ( 2 / 2.75f ))
        {
            float postFix = t -= ( 1.5f / 2.75f );
            return c * ( 7.5625f*( postFix )*t + .75f ) + b;
        }
        else if (t < ( 2.5 / 2.75 ))
        {
            float postFix = t -= ( 2.25f / 2.75f );
            return c * ( 7.5625f*( postFix )*t + .9375f ) + b;
        }
        else
        {
            float postFix = t -= ( 2.625f / 2.75f );
            return c * ( 7.5625f*( postFix )*t + .984375f ) + b;
        }
    }

	static float bounce_easing_in_out(float t, float b, float c, float d)
    {
        if (t < d / 2) return bounce_easing_in(t * 2, 0, c, d) * .5f + b;
        else return bounce_easing_out(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
    }

	static float elastic_easing_in(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if (( t /= d ) == 1) return b + c;
        const float p = d * .3f;
        const float a = c;
        const float s = p / 4;
        const float postFix = a * powf(2, 10 * ( t -= 1 )); // this is a fix, again, with post-increment operators
        return -( postFix * sinf(( t*d - s )*( 2 * PI ) / p) ) + b;
    }

	static float elastic_easing_out(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if (( t /= d ) == 1) return b + c;
        float p = d * .3f;
        float a = c;
        float s = p / 4;

        // update the elasticity value to affect how elastic the thing is
        float elasticity = -10;
        return ( a*powf(2, elasticity  * t) * sinf(( t*d - s )*( 2 * PI ) / p) + c + b );
    }

	static float elastic_easing_in_out(float t, float b, float c, float d)
    {
        if (t == 0) return b;  if (( t /= d / 2 ) == 2) return b + c;
        float p = d * ( .3f*1.5f );
        float a = c;
        float s = p / 4;

        if (t < 1)
        {
            float postFix = a * powf(2, 10 * ( t -= 1 )); // postIncrement is evil
            return -.5f*( postFix* sinf(( t*d - s )*( 2 * PI ) / p) ) + b;
        }
        float postFix = a * powf(2, -10 * ( t -= 1 )); // postIncrement is evil
        return postFix * sinf(( t*d - s )*( 2 * PI ) / p)*.5f + c + b;
    }
}