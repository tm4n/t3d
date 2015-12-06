#ifndef __VEC_H__
#define __VEC_H__

#include <iostream>

class vec
{
    public:
        float x, y, z;

        vec();
        vec(vec *);
        vec(float, float, float);

        float dist(const vec *) const;
        float dist2d(const vec *) const;
        void set(const vec *);
        void set(float ax, float ay, float az);
        void zero();
		float length();

		static float angle(float in);

		friend std::ostream& operator<< (std::ostream& o, vec const& vec);

    protected:
    private:
};

#endif // __VEC_H__
