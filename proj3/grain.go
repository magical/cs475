package main

import (
	"math"
	"math/rand"
)

var (
	nowYear  = 2019
	nowMonth = 0
	// can change starting values
	nowPrecip  float64
	nowTemp    float64
	nowHeight  float64 = 1
	nowNumDeer int     = 1
)

// plus fourth something

// can change if we want
const (
	grainGrowsPerMonth  = 8.0 // inche
	oneDeerEatsPerMonth = 0.5

	avgPrecipPerMonth = 6.0
	ampPrecipPerMonth = 6.0
	randomPrecip      = 2.0

	avgTemp    = 50.0
	ampTemp    = 20.0
	randomTemp = 10.0

	midTemp   = 40.0
	midPrecip = 10.0
)

func main() {
	go GrainDeer()
	go GrainGrowth()
	go Watcher() // increments year and month
	go MyAgent()
}

func thing() {
	for {
		// compute next state
		//
		// barrier
		//
		// copy next state to now state
		//
		// barrier
		//
		// ... let watcher thread run ...
		//
		// barrier
	}
}

func watchertemplate() {
	// barrier
	// barrier
	// print current state
	// barrier
}

func watcher() {
	for nowYear < 2025 {
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		//. . .

		// DoneComputing barrier:
		//#pragma omp barrier
		//. . .

		// DoneAssigning barrier:
		//#pragma omp barrier
		//. . .

		// DonePrinting barrier:
		//#pragma omp barrier
		//. . .
	}

}

func TemperatureAndPrecipitation() {
	ang := (30*nowMonth + 15) * (math.Pi / 180)

	temp := avgTemp - ampTemp*math.Sin(ang)
	rng := rand.New(rand.NewSource(0))

	nowTemp = temp + ranf(rng, -randomTemp, randomTemp)

	precip := avgPrecipPerMonth + ampPrecipPerMonth*math.Sin(ang)
	nowPrecip = precip + ranf(rng, -randomPrecip, randomPrecip)
	if nowPrecip < 0 {
		nowPrecip = 0
	}
}

func GrainDeer() {
	//  The Carrying Capacity of the graindeer is the number of inches of height of the grain. If the number of graindeer exceeds this value at the end of a month, decrease the number of graindeer by one. If the number of graindeer is less than this value at the end of a month, increase the number of graindeer by one.
	//
	carryingCapacity := nowHeight
	nextNumDeer = nowNumDeer

	if float64(nowNumDeer) > carryingCapacity {
		nextNumDeer += 1
	} else {
		nextNumDeer -= 1
	}
}

func GrainGrowth() {
	//  Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
	//
	//  You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor like this:

	tempFactor := math.Exp(-square((nowTemp - midTemp) / 10))
	precipFactor := math.Exp(-square((nowPrecip - midPrecip) / 10))

	nowHeight += tempFactor * precipFactor * grainGrowsPerMonth
	nowHeight -= nowNumDeer * oneDeerEatsPerMonth
	if nowHeight < 0 {
		nowHeight = 0
	}

}

func square(x float64) float64 { return x * x }

func ranf(rng *rand.Rand, low, high float64) float64 {
	return rng.Float64()*(high-low) - low
}
