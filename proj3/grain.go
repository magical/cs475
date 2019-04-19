package main

var (
	nowYear = 2019
	nowMonh = 0
	// can change starting values
	nowPrecip  float64
	nowTemp    float64
	nowHeight  float64
	nowNumDeer int
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
