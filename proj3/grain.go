package main

import (
	"fmt"
	"math"
	"math/rand"
	"sync"
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
	grainGrowsPerMonth  = 8.0 // inches
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
	updateTemperatureAndPrecipitation()
	//printResults()
	go GrainDeer()
	go GrainGrowth()
	go MyAgent()
	Watcher() // increments year and month
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

func Watcher() {
	for nowYear < 2025 {
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		//. . .

		// DoneComputing barrier:
		barrier()

		// DoneAssigning barrier:
		barrier()

		//print results and increment time
		printResults()
		updateYear()
		updateTemperatureAndPrecipitation()

		// DonePrinting barrier:
		barrier()
	}

}

func printResults() {
	fmt.Println(nowYear, nowMonth, nowTemp, nowPrecip, nowNumDeer, nowHeight)
}

func updateYear() {
	nowMonth++
	if nowMonth == 12 {
		nowYear++
		nowMonth = 0
	}
}

var rng = rand.New(rand.NewSource(0))

func updateTemperatureAndPrecipitation() {
	ang := float64(30*nowMonth+15) * (math.Pi / 180)

	temp := avgTemp - ampTemp*math.Cos(ang)
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
	for {
		carryingCapacity := nowHeight
		nextNumDeer := nowNumDeer

		if float64(nowNumDeer) > carryingCapacity {
			nextNumDeer -= 1
		} else if float64(nowNumDeer) < carryingCapacity {
			nextNumDeer += 1
		}

		barrier()

		nowNumDeer = nextNumDeer

		barrier()
		barrier()
	}
}

func GrainGrowth() {
	//  Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
	//
	//  You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor like this:

	for {
		// this function peaks at 1 around midTemp, and peters off to 0 at plus or minus 10
		tempFactor := math.Exp(-square((nowTemp - midTemp) / 10))
		precipFactor := math.Exp(-square((nowPrecip - midPrecip) / 10))

		nextHeight := nowHeight
		nextHeight += tempFactor * precipFactor * grainGrowsPerMonth
		nextHeight -= float64(nowNumDeer) * oneDeerEatsPerMonth
		if nextHeight < 0 {
			nextHeight = 0
		}

		barrier()
		nowHeight = nextHeight
		barrier()
		barrier()
	}
}

func MyAgent() {}

func square(x float64) float64 { return x * x }

func ranf(rng *rand.Rand, low, high float64) float64 {
	return rng.Float64()*(high-low) - low
}

var (
	mu         sync.Mutex
	turnstile1 = make(chan struct{}, 1)
	turnstile2 = make(chan struct{}, 1)
	nwait      int
)

func init() {
	turnstile2 <- struct{}{}
}

const numthreads = 3

func barrier() {
	mu.Lock()
	nwait++
	if nwait == numthreads {
		<-turnstile2
		turnstile1 <- struct{}{}
	}
	mu.Unlock()

	<-turnstile1
	turnstile1 <- struct{}{}

	mu.Lock()
	nwait--
	if nwait == 0 {
		<-turnstile1
		turnstile2 <- struct{}{}
	}
	mu.Unlock()
	<-turnstile2
	turnstile2 <- struct{}{}
}
