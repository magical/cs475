#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]

use std::sync::Arc;
use std::sync::Barrier;

struct Now {
    Year: i32,
    Month: i32,
    Precip: f64,
    Temp: f64,
    Height: f64,
    NumDeer: i32,
}

// can change starting values
static mut now: Now = Now{
    Year: 2019,
    Month: 0,
    Precip: 0.0,
    Temp: 0.0,
    Height: 1.0,
    NumDeer: 1,
};

// plus fourth something

// can change if we want
const maxYear: i32 = 2025;
const grainGrowsPerMonth: f64  = 8.0; // inches
const oneDeerEatsPerMonth: f64 = 0.5;

const avgPrecipPerMonth: f64 = 6.0;
const ampPrecipPerMonth: f64 = 6.0;
const randomPrecip: f64      = 2.0;

const avgTemp: f64    = 50.0;
const ampTemp: f64    = 20.0;
const randomTemp: f64 = 10.0;

const midTemp: f64   = 40.0;
const midPrecip: f64 = 10.0;

fn square(x : f64 ) -> f64 { x * x }

fn main() {
    unsafe {
        updateTemperatureAndPrecipitation(&mut now);
    }

    let b = Arc::new(Barrier::new(4));
    {
        let b = b.clone();
        std::thread::spawn(move || GrainDeer(&b) );
    }
    {
        let b = b.clone();
        std::thread::spawn(move || GrainGrowth(&b) );
    }
    {
        let b = b.clone();
        std::thread::spawn(move || MyAgent(&b) );
    }
    Watcher(&b);
}

fn Watcher(b: &Barrier) {
    loop {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        //. . .

        // DoneComputing barrier:
        b.wait();

        // DoneAssigning barrier:
        b.wait();

        //print results and increment time
        unsafe {
            printResults(&now);
            updateYear(&mut now);
            updateTemperatureAndPrecipitation(&mut now);

            if now.Year >= maxYear {
                return
            }
        }

        // DonePrinting barrier:
        b.wait();
    }

}

fn printResults(n: &Now) {
    println!("{} {} {} {} {} {}", n.Year, n.Month, n.Temp, n.Precip, n.NumDeer, n.Height);
}

fn updateYear(n: &mut Now) {
    n.Month += 1;
    if n.Month == 12 {
        n.Year += 1;
        n.Month = 0;
    }
}

fn updateTemperatureAndPrecipitation(n: &mut Now) {
    let ang: f64 = ((30*n.Month+15) as f64) * (std::f64::consts::PI / 180.0);

    let temp: f64 = avgTemp - ampTemp*ang.cos();
    n.Temp = temp + Ranf(-randomTemp, randomTemp);

    let precip: f64 = avgPrecipPerMonth + ampPrecipPerMonth*ang.sin();
    n.Precip = precip + Ranf(-randomPrecip, randomPrecip);
    if n.Precip < 0.0 {
        n.Precip = 0.0;
    }
}

fn GrainDeer(b: &Barrier) {
    //  The Carrying Capacity of the graindeer is the number of inches of height of the grain. If the number of graindeer exceeds this value at the end of a month, decrease the number of graindeer by one. If the number of graindeer is less than this value at the end of a month, increase the number of graindeer by one.
    //
    loop {
        unsafe {
            let carryingCapacity = now.Height;
            let mut nextNumDeer = now.NumDeer;
            if (now.NumDeer as f64) > carryingCapacity {
                nextNumDeer -= 1;
            } else if (now.NumDeer as f64) < carryingCapacity {
                nextNumDeer += 1;
            }

            b.wait();

            now.NumDeer = nextNumDeer;

            b.wait();
            b.wait();
        }
    }
}

fn GrainGrowth(b: &Barrier) {
    //  Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
    //
    //  You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor like this:

    loop {
        unsafe {
            // this function peaks at 1 around midTemp, and peters off to 0 at plus or minus 10
            let tempFactor = (-square((now.Temp - midTemp) / 10.0)).exp();
            let precipFactor = (-square((now.Precip - midPrecip) / 10.0)).exp();

            let mut nextHeight = now.Height;
            nextHeight += tempFactor * precipFactor * grainGrowsPerMonth;
            nextHeight -= (now.NumDeer as f64) * oneDeerEatsPerMonth;
            if nextHeight < 0.0 {
                nextHeight = 0.0;
            }

            b.wait();

            now.Height = nextHeight;

            b.wait();
            b.wait();
        }
    }
}

fn MyAgent(b: &Barrier) {
    loop {
        b.wait();
        b.wait();
        b.wait();
    }
}


fn Ranf( low: f64, high: f64 ) -> f64
{
    return 0.0;
    //double r = (double) rand_r(seedp);  // 0 - RAND_MAX
    //return low  +  r * ( high - low ) / (double)RAND_MAX;
}
