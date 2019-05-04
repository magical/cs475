#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

long nowYear  = 2019;
long nowMonth = 0;
	// can change starting values

double nowPrecip ;
double nowTemp   ;
double nowHeight = 1.0;
long nowNumDeer = 1;
double nowMystery = 1.0;


// plus fourth something

// can change if we want
const long maxYear = 2025;
const double grainGrowsPerMonth  = 8.0; // inches
const double oneDeerEatsPerMonth = 0.5;
const double numMonthsForGrainDensityToDouble = 18.0;

const double avgPrecipPerMonth = 6.0;
const double ampPrecipPerMonth = 6.0;
const double randomPrecip      = 2.0;

const double avgTemp    = 50.0;
const double ampTemp    = 20.0;
const double randomTemp = 10.0;

const double midTemp   = 40.0;
const double midPrecip = 10.0;

void Watcher();
void GrainDeer();
void GrainGrowth();
void MyAgent();
void updateTemperatureAndPrecipitation();
void printResults();
void updateYear();
double Ranf( unsigned int *seedp,  double low, double high );
void barrier();

static double square(double x ) { return x * x; }

int main() {
	omp_set_num_threads(4);
	updateTemperatureAndPrecipitation();
	#pragma omp parallel sections
	{
		#pragma omp section
		Watcher();
		#pragma omp section
		GrainDeer();
		#pragma omp section
		GrainGrowth();
		#pragma omp section
		MyAgent();
	}
	return 0;
}

void Watcher() {
	while (nowYear < maxYear) {
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		//. . .

		// DoneComputing barrier:
		barrier();

		// DoneAssigning barrier:
		barrier();

		//print results and increment time
		printResults();
		updateYear();
		updateTemperatureAndPrecipitation();

		// DonePrinting barrier:
		barrier();
	}

}

void printResults() {
	double temp = (nowTemp - 32.) * (5./9.); // convert to celcius
	printf("%ld %ld %f %f %ld %f %f\n", nowYear, nowMonth, temp, nowPrecip, nowNumDeer, nowHeight, nowMystery);
}

void updateYear() {
	nowMonth++;
	if (nowMonth == 12) {
		nowYear++;
		nowMonth = 0;
	}
}

void updateTemperatureAndPrecipitation() {
	static unsigned int seed = 0;
	double ang = ((double)(30.*nowMonth+15.)) * (M_PI / 180.);

	double temp = avgTemp - ampTemp*cos(ang);
	nowTemp = temp + Ranf(&seed, -randomTemp, randomTemp);

	double precip = avgPrecipPerMonth + ampPrecipPerMonth*sin(ang);
	nowPrecip = precip + Ranf(&seed, -randomPrecip, randomPrecip);
	if (nowPrecip < 0.) {
		nowPrecip = 0.;
	}
}

void GrainDeer() {
	//  The Carrying Capacity of the graindeer is the number of inches of height of the grain. If the number of graindeer exceeds this value at the end of a month, decrease the number of graindeer by one. If the number of graindeer is less than this value at the end of a month, increase the number of graindeer by one.
	//
	while (nowYear < maxYear) {
		double carryingCapacity = nowHeight;
		long nextNumDeer = nowNumDeer;

		if (((double)(nowNumDeer)) > carryingCapacity) {
			nextNumDeer -= 1;
		} else if (((double)(nowNumDeer)) < carryingCapacity) {
			nextNumDeer += nowMystery;
		}

		barrier();

		nowNumDeer = nextNumDeer;

		barrier();
		barrier();
	}
}

void GrainGrowth() {
	//  Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
	//
	//  You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor like this:

	while (nowYear < maxYear) {
		// this function peaks at 1 around midTemp, and peters off to 0 at plus or minus 10
		double tempFactor = exp(-square((nowTemp - midTemp) / 10));
		double precipFactor = exp(-square((nowPrecip - midPrecip) / 10));

		double nextHeight = nowHeight;
		nextHeight += tempFactor * precipFactor * grainGrowsPerMonth;
		nextHeight -= ((double)(nowNumDeer)) * oneDeerEatsPerMonth;
		if (nextHeight < 0) {
			nextHeight = 0;
		}

		barrier();
		nowHeight = nextHeight;
		barrier();
		barrier();
	}
}

/* ideas:
 * - grain strain (a certain percentage of grain is poisonous to graindeer
 * - farmer cuts grain every so often
 * - wolfs which eat deer
 * - drought in a certain year
 * - bugs which eat the grain too
 * - another type of grain
 * - periodic fires
 */
void MyAgent() {
	// Approximately every 18 months, grain density doubles: the grain
	// farmers are able to grow more grain in the same amount of space.
	// This has the side effect of promoting graindeer growth because they
	// can now raise more children because they don't have to spend as much
	// time searching for food. The number of additional graindeer each
	// month (assuming the grain is of a sufficient height) is precisely
	// proportional to the grain density.
	//
	double densityGrowthPerMonth = pow(2.0, 1./numMonthsForGrainDensityToDouble);
	while (nowYear < maxYear) {
		double nextMystery = nowMystery * densityGrowthPerMonth;

		barrier();

		nowMystery = nextMystery;

		barrier();
		barrier();
	}
}


double Ranf( unsigned int *seedp,  double low, double high )
{
	double r = (double) rand_r(seedp);  // 0 - RAND_MAX
	return low  +  r * ( high - low ) / (double)RAND_MAX;
}

void barrier() {
	#pragma omp barrier
}
