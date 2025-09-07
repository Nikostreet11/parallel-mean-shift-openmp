#ifndef PARALLEL_MEAN_SHIFT_OPENMP_OPTIONS_H
#define PARALLEL_MEAN_SHIFT_OPENMP_OPTIONS_H

#define INPUT_PATH  "../img/balloons_50.ppm"
#define OUTPUT_PATH_AOS_SEQ "../img/output_aos_sequential.ppm"
#define OUTPUT_PATH_AOS_OMP "../img/output_aos_parallel.ppm"
#define OUTPUT_PATH_SOA_SEQ "../img/output_soa_sequential.ppm"
#define OUTPUT_PATH_SOA_OMP "../img/output_soa_parallel.ppm"

#define ITERATIONS    10
#define BANDWIDTH     0.4
#define RGB_CHANNELS  3
#define RGB_MAX_VALUE 255

#endif //PARALLEL_MEAN_SHIFT_OPENMP_OPTIONS_H
