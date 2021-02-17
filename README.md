# avx_filters
Final project for Computer System Architecture course

Filters:

-Column sum (sum)

-Column sum of squared values (sum2)

-Normalization of values dividing by a reference signal (norm)

-Moving average smoothing (sample picture in folder)

-Butterworth high-pass (sample picture in folder)


Each filter takes an input matrix of signed 16 bit short values stored in row major order and its dimensions. The .asm files contain the SIMD (AVX or SSE) implementation of each filter and the .c files have the C implementation and the time comparisson between them.

report.pdf (spanish) summarizes the timing performance of each filter and contains real-world motivation, description and usage of these filters.
