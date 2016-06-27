//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef BITSTREAMS_H_
#define BITSTREAMS_H_

//TODO: move in separate modules
// Each set of bitstreams is a propriety of the operation

extern const void *bits_blur[NUM_SLOTS];

extern const void *bits_sharp[NUM_SLOTS];

extern const void *bits_sobel[NUM_SLOTS];

extern const void *bits_mult[NUM_SLOTS];

void load_bitstreams();

#endif /* BITSTREAMS_H_ */
