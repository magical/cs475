package main

import (
	"fmt"
	"math"
	"math/rand"
)

type state struct {
	Year  int
	Month int

	Precip  float64
	Temp    float64
	Height  float64
	NumDeer int
}

// can change starting values
var now = &state{
	Year:    2019,
	Month:   0,
	Height:  1,
	NumDeer: 1,
}

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
	updateTemperatureAndPrecipitation(now)
	//printResults()
	deerInC := make(chan *state, 1)
	grainInC := make(chan *state, 1)

	deerC := make(chan int, 1)
	grainC := make(chan float64, 1)

	go GrainDeer(deerInC, deerC)
	go GrainGrowth(grainInC, grainC)
	go MyAgent()

	for now.Year < 2025 {
		deerInC <- now
		grainInC <- now

		nextHeight := <-grainC
		nextDeer := <-deerC

		now.Height = nextHeight
		now.NumDeer = nextDeer

		//print results and increment time
		printResults()
		updateYear(now)
		updateTemperatureAndPrecipitation(now)
	}
}

func printResults() {
	fmt.Println(now.Year, now.Month, now.Temp, now.Precip, now.NumDeer, now.Height)
}

func updateYear(now *state) {
	now.Month++
	if now.Month == 12 {
		now.Year++
		now.Month = 0
	}
}

var rng = rand.New(rand.NewSource(0))

func updateTemperatureAndPrecipitation(now *state) {
	ang := float64(30*now.Month+15) * (math.Pi / 180)

	temp := avgTemp - ampTemp*math.Cos(ang)
	now.Temp = temp + ranf(rng, -randomTemp, randomTemp)

	precip := avgPrecipPerMonth + ampPrecipPerMonth*math.Sin(ang)
	now.Precip = precip + ranf(rng, -randomPrecip, randomPrecip)
	if now.Precip < 0 {
		now.Precip = 0
	}
}

func GrainDeer(in <-chan *state, out chan<- int) {
	//  The Carrying Capacity of the graindeer is the number of inches of height of the grain. If the number of graindeer exceeds this value at the end of a month, decrease the number of graindeer by one. If the number of graindeer is less than this value at the end of a month, increase the number of graindeer by one.
	//
	for {
		now := <-in
		carryingCapacity := now.Height
		nextNumDeer := now.NumDeer

		if float64(now.NumDeer) > carryingCapacity {
			nextNumDeer -= 1
		} else if float64(now.NumDeer) < carryingCapacity {
			nextNumDeer += 1
		}

		out <- nextNumDeer
	}
}

func GrainGrowth(in <-chan *state, out chan<- float64) {
	//  Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
	//
	//  You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor like this:

	for {
		now := <-in
		// this function peaks at 1 around midTemp, and peters off to 0 at plus or minus 10
		tempFactor := math.Exp(-square((now.Temp - midTemp) / 10))
		precipFactor := math.Exp(-square((now.Precip - midPrecip) / 10))

		nextHeight := now.Height
		nextHeight += tempFactor * precipFactor * grainGrowsPerMonth
		nextHeight -= float64(now.NumDeer) * oneDeerEatsPerMonth
		if nextHeight < 0 {
			nextHeight = 0
		}

		out <- nextHeight
	}
}

func MyAgent() {}

func square(x float64) float64 { return x * x }

func ranf(rng *rand.Rand, low, high float64) float64 {
	return rng.Float64()*(high-low) - low
}
