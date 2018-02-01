#ifndef FREQLUT_H
#define FREQLUT_H

#include <stdint.h>

#define CHANNELS 40

/*
 * Frequency look up table :
 * To avoid Wifi channels, 40 Bluetooth channels are defined below (they already
 * avoid Wifi common channels) : from 2402 MHz to 2480 MHz, step 2 MHz.
 * User can define channel count for Ranging run, and it is optimized to have
 * several frequencies in the largest band as possible. Also the 40 frequencies 
 * are generated by random sorting to preferate the 10 first in the largest band
 * as possible (10 is the shortest channel count the user can choose).
 */
const uint32_t Channels[] =
{
 2450000000u,
 2402000000u,
 2476000000u,
 2436000000u,
 2430000000u,
 2468000000u,
 2458000000u,
 2416000000u,
 2424000000u,
 2478000000u,
 2456000000u,
 2448000000u,
 2462000000u,
 2472000000u,
 2432000000u,
 2446000000u,
 2422000000u,
 2442000000u,
 2460000000u,
 2474000000u,
 2414000000u,
 2464000000u,
 2454000000u,
 2444000000u,
 2404000000u,
 2434000000u,
 2410000000u,
 2408000000u,
 2440000000u,
 2452000000u,
 2480000000u,
 2426000000u,
 2428000000u,
 2466000000u,
 2418000000u,
 2412000000u,
 2406000000u,
 2470000000u,
 2438000000u,
 2420000000u,
};

#endif
