# Part 5:

1. The three biggest risks are memory leaks, dangling pointers, and double-free errors. Failing to release allocated memory can exhaust available resources over time; accessing references to freed memory may lead to undefined behavior; and releasing the same memory more than once can corrupt the program's state and cause a segmentation fault. 

2. The biggest issue is performance degradation. Memory leaks or fragmentation can slow down processing and increase latency. Over time, they may cause crashes or segmentation fault, which risk missed trading opportunities. 

3. RAII reduces the risk of bugs because it does automatic cleanup. Resources are acquired during object creation and automatically released when the object goes out of scope, even during exceptions. This automatic lifetime management prevents resource leaks and ensures that resources are handled predictably.

4. Manual memory management offers fine-grained control and may yield optimal performance when managed correctly, but it is error-prone, making bugs like memory leaks, dangling pointers, and double frees more likely. Automatic memory management simplifies coding by handling resource allocation and deallocation automatically. However, it may incur overhead or unpredictable timing due to garbage collection, which can impact performance in real-time, performance-critical scenarios.