// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */
void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	/* The two outer for loops iterate through each pixel */

	#pragma omp parallel for
	for (int y = 0; y < imageHeight; y++)
	{

		for (int x = 0; x < imageWidth; x++)
		{	
			/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[y * imageWidth + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;
			float squaredDifference;
			__m128 total;
			__m128 l;
			__m128 r;
			int leftX;
			int leftY;
			int rightX;
			int rightY;
			__m128 difference;

			/* Iterate through all feature boxes that fit inside the maximum displacement box. 
			   centered around the current pixel. */
			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight) {
					continue;
				}
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					/* Skip feature boxes that dont fit in the displacement box. */
					if (x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					squaredDifference = 0;
					total = _mm_setzero_ps();


					/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
					for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{
						for (int boxX = -featureWidth; boxX + 4 <= featureWidth; boxX += 4)
						{
							leftX = x + boxX;
							leftY = y + boxY;
							rightX = x + dx + boxX;
							rightY = y + dy + boxY;

							l = _mm_loadu_ps(left + (leftY*imageWidth + leftX));  
							r = _mm_loadu_ps(right + (rightY*imageWidth + rightX));
							difference = _mm_sub_ps(l, r);
							difference = _mm_mul_ps(difference, difference);
							total = _mm_add_ps(difference, total);



							// float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
							// squaredDifference += difference * difference;
						}
					}

					squaredDifference = total[0] + total[1] + total[2] + total[3]; // dump total's data into sD
					int tailcase = featureWidth & 1; // Check if odd or even
					if (squaredDifference > minimumSquaredDifference && minimumSquaredDifference != -1) {
                            continue;
                    } else if (tailcase == 0) { // this means its even so take care of it normally like i the lab
                            for (int boxY = -featureHeight; boxY <= featureHeight; boxY++) {
                                leftX = x + featureWidth;
                                leftY = y + boxY;
                                rightX = x + dx + featureWidth;
                                rightY = y + dy + boxY;
                                float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
                                squaredDifference += difference * difference;
                            }
                        } else {
                            total = _mm_setzero_ps();
                            for (int boxY = -featureHeight; boxY <= featureHeight; boxY++) {
                                leftX = x + featureWidth - 3;
                                leftY = y + boxY;
                                rightX = x + dx + featureWidth - 3;
                                rightY = y + dy + boxY;

                                l = _mm_loadu_ps(left + (leftY * imageWidth + leftX));
                                r = _mm_loadu_ps(right + (rightY * imageWidth + rightX));
                                difference = _mm_sub_ps(l, r);
                                difference = _mm_mul_ps(difference, difference);
                                total = _mm_add_ps(difference, total);
                            }
                            // _mm_storeu_ps(Arr, total);
                            squaredDifference += total[1] + total[2] + total[3];
                        }
					


					/* 
					Check if you need to update minimum square difference. 
					This is when either it has not been set yet, the current
					squared displacement is equal to the min and but the new
					displacement is less, or the current squared difference
					is less than the min square difference.
					*/
					// sqrt in displacementNaive is expensive, maybe don't compute it all the time
					if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && ((dx * dx + dy * dy) < (minimumDx * minimumDx + minimumDy * minimumDy))) || (minimumSquaredDifference > squaredDifference)) 
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			/* 
			Set the value in the depth map. 
			If max displacement is equal to 0, the depth value is just 0.
			*/
			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
			else
			{
				depth[y * imageWidth + x] = 0;
			}
		}
	}
}
