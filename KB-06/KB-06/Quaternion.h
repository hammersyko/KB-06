#ifndef _PENGINE_QUATERNION_H_
#define _PENGINE_QUATERNION_H_

#define MINFLOAT 1.17549435E-38F

#include "PengineDefinitions.h"

namespace pengine
{
	class Quaternion {
	public:
		float data[4];
		Quaternion()
		{
			memset(data, 0, 4 * sizeof(float));
		};
		Quaternion(const Quaternion &pq)
		{
			memcpy(data, pq.data, 4 * sizeof(float));
		};
		Quaternion(const float* pT)
		{
			memcpy(data, pT, 4 * sizeof(float));
		};
		void Zero()
		{
			memset(data, 0, 4 * sizeof(float));
		};
		float operator[](int index)const
		{
			return data[index];
		};
		float& operator[](int index)
		{
			return data[index];
		};
		Quaternion& operator=(const Quaternion &pq)
		{
			memcpy(data, pq.data, 4 * sizeof(float));
			return *this;
		};
		Quaternion operator+(Quaternion pq)
		{
			float rdata[4];
			rdata[0] = pq.data[0] + data[0];
			rdata[1] = pq.data[1] + data[1];
			rdata[2] = pq.data[2] + data[2];
			rdata[3] = pq.data[3] + data[3];
			return Quaternion(rdata);
		};
		Quaternion operator*(Quaternion pq);//wasn't implemented, unused?
		Quaternion operator*(float pT)
		{
			float rdata[4];
			rdata[0] = pT * data[0];
			rdata[1] = pT * data[1];
			rdata[2] = pT * data[2];
			rdata[3] = pT * data[3];
			return Quaternion(rdata);
		};
		float Dot(Quaternion &pq)
		{
			return ((data[0] * pq.data[0]) + (data[1] * pq.data[1]) + (data[2] * pq.data[2]) + (data[3] * pq.data[3]));
		};
		Quaternion Slerp(float pT, Quaternion &pq)
		{
			//We calculate the angle spread between both quaternions
			float AngleCos = pq.Dot(*this);
			float Angle = qACos(AngleCos); //see the function ACos above

			if (Angle < MINFLOAT)
			{
				return Quaternion(*this);
			}
			//We calculate the interpolated angle and deduce the resulting quaternion
			float InvAngleSin = (1.0f / sin(Angle));

			float Coeff0 = sin((1 - pT) * Angle) * InvAngleSin;
			float Coeff1 = sin(pT * Angle) * InvAngleSin;
			return Quaternion((*this * Coeff0) + (pq * Coeff1));
		};
	private:
		float qACos(float pValue)
		{
			if (-1.0f < pValue)
			{
				if (pValue < 1.0f)
				{
					return (float)acos(pValue);
				}
				else
				{
					return 0.0f;
				}
			}
			else
			{
				return M_PI;
			}
		};
	};
}
#endif