#ifndef Math_H
#define Math_H

//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace glm;

#define ORIGIN vec3(0.0f,0.0f,0.0f)
#define EPSILON 1e-04f

#define MOD 1000000007
inline long long fast_power(long long base, long long power)
{
    long long result = 1;
    while(power > 0) {

        if(power % 2 == 1) { // Can also use (power & 1) to make code even faster
            result = (result*base) % MOD;
        }
        base = (base * base) % MOD;
        power = power / 2; // Can also use power >>= 1; to make code even faster
    }
    return result;
}

inline float fast_round(float data,int decimal)
{
	int power = fast_power(10,decimal);

	float round_data = float(int(data * power)) / power;

    return round_data;
}

#endif