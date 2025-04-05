# HFT-Project
FINM 32700 Group Project
Phase 1 Discussion Questions:

1. A pointer is a variable that stores the memory address of another object, while a reference is an alias or an alternative name for an existing object. A pointer can be uninitialized (though dangerous and a bad practice) or initialized later, but a reference must be initialized when declared and cannot be null. A pointer can be reassigned to point to a different object's address later, but a reference cannot be reassigned to refer to a different object after initialization. 

In the context of implementing numerical algorithms, when we are passing function arguments that are non-optional, we prefer to use references over pointers because they are safer than pointers (since an object must be provided) and have cleaner syntax. We can also choose to return a reference, but only if the object being referred to will continue to exist after the function returns. On the other hand, if a function parameter or an output parameter is optional, a pointer is the better choice because it can be set to nullptr to indicate absence, but references cannot be null. If the algorithm involves data structures like linked lists or trees, which inherently rely on nodes storing the addresses (pointers) of other nodes, it would also be suitable to use pointers instead as references cannot be reassigned. 

2. In general, accessing data in the order it is stored (rows for row-major, columns for column-major) yields sequential memory access, maximizing spatial locality and cache hits.

For the first function multiply_mv_row_major, we assume matrix is stored in row-major order, so the outer loop iterates through rows (i), and the inner loop iterates through columns (k). In the inner loop, we increment k and sequentially access consecutive elements within row i, which gives good spatial locality for the matrix and vector access. result[i] is accessed once per outer loop iteration. 

For the second function multiply_mv_col_major, we assume matrix is stored in column-major order, so the outer loop iterates through columns (i), and the inner loop iterates through rows (j). In the inner loop, we increment j and sequentially accesses consecutive elements within column i, which gives good spatial locality for the matrix access. result[j] is accessed sequentially in the inner loop, which also gives spatial locality benefits. Additionally, notice here that vector[i] is accessed repeatedly in the inner loop for the same i, which provides great temporal locality benefits. As a result, the second function would result in a high cache hit rate due to sequential matrix column access, sequential result access, and good temporal locality for the vector element. 

From the benchmarking result, we see that for small matrices, the difference is negligible; data likely fits entirely in cache. However, for larger matrices, multiply_mv_col_major is significantly faster (5x-8x) than multiply_mv_row_major.

For the third function multiply_mm_naive, we assume matrixA, matrixB, result are stored in row-major order. Thus, result[i\*colsB + j] is accessed once per (i, j) pair after the k loop finishes. matrixA[i\*colsA + k] accesses elements of row i sequentially in the innermost loop (k), which provides excellent spatial locality for A. For matrixB[k\*colsB + j], in the innermost loop (k), k increments, but j is fixed. This accesses elements down column j. Since B is row-major, these elements are not contiguous in memory. The access jumps by colsB elements each time (k increments), so this has poor spatial locality. As a result, cache misses are frequent when accessing matrixB, especially when colsB is large, as each access might require loading a new cache line, potentially evicting useful data from matrix A or the result.

For the fourth function multiply_mm_transposed_b, we assumes matrixA, matrixB_transposed, result are stored in row-major order, and matrixB_transposed is the transpose of the original B. result[i * rowsB_T + j] is accessed once per (i, j) pair. In the innermost loop (k), matrixA[i * colsA + k] accesses row i of A sequentially, which gives great spatial locality for A. In the innermost loop (k), matrixB_transposed[j * colsB_T + k] accesses elements of row j of matrixB_transposed sequentially. Since matrixB_transposed is stored row-major, this is sequential access, so it has excellent spatial locality for B_T. This means high cache hit rate for both matrixA and matrixB_transposed in the inner loop due to sequential access patterns. 

From our benchmarking results, we see that multiply_mm_naive is the slowest, especially for large matrices. This confirms the significant performance penalty of poor spatial locality when accessing matrixB column-wise in row-major storage. multiply_mm_transposed_b is consistently faster than multiply_mm_naive (about 1.3x - 1.6x faster) because by converting the column-wise access of B into a row-wise access of B_T, spatial locality is restored for the second matrix operand. 

3. Modern CPUs employ a cache hierarchy (L1, L2, and L3) consisting of small, fast memory banks located closer to the processor cores than the main memory. The L1 cache is the smallest, fastest, and closest to its core (often split into data and instruction caches), L2 is larger and slightly slower, and L3 is the largest and slowest cache, usually shared across multiple cores. When the CPU needs data, it checks L1 first, then L2, then L3; if the data is found (a cache hit), it's retrieved very quickly. If not found (a cache miss), the CPU must fetch it from the slower main memory, incurring a significant delay, and typically storing a copy in the caches for potential future use.

Caches work effectively because programs usually have locality of reference. Temporal locality means that if a program accesses a particular memory location, it is likely to access that same location again soon; caches keep recently used data readily available to exploit this. Spatial locality means that if a program accesses a particular memory location, it is likely to access nearby memory locations soon after; caches exploit this by fetching data from RAM not as single bytes, but in contiguous blocks called cache lines. Thus, accessing one piece of data often pre-loads adjacent data into the cache, as it anticipates that it will be needed shortly. Both temporal and spatial locality are crucial for high performance as they help maximize cache hits and minimize slow trips to main memory.

Optimization example to come soon...

4. To write soon...

5. To write soon...

6. To write soon...

7. To reflect soon...
