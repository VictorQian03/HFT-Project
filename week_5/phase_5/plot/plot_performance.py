import matplotlib.pyplot as plt
import numpy as np

def plot_from_console_output():
    """
    Manually paste the data from the C++ console output here.
    """
    order_sizes_str = "1000 5000 10000 50000 100000" 
    original_times_str = "0.00147 0.00876 0.01325 0.04309 0.08999"  
    optimized_times_str = "0.00221 0.00769 0.01097 0.03734 0.07668" 

    try:
        order_sizes = [int(s) for s in order_sizes_str.split()]
        original_execution_times_s = [float(s) for s in original_times_str.split()]
        optimized_execution_times_s = [float(s) for s in optimized_times_str.split()]
    except ValueError as e:
        print(f"Error parsing data. Ensure strings are space-separated numbers. Error: {e}")
        return
    except Exception as e:
        print(f"An unexpected error occurred during data parsing: {e}")
        return

    if not (len(order_sizes) == len(original_execution_times_s) == len(optimized_execution_times_s)):
        print("Error: Data lists have inconsistent lengths.")
        print(f"Order Sizes: {len(order_sizes)}, Original Times: {len(original_execution_times_s)}, Optimized Times: {len(optimized_execution_times_s)}")
        return
    if not order_sizes:
        print("Error: No data to plot.")
        return

    plt.figure(figsize=(10, 6))

    plt.plot(order_sizes, original_execution_times_s, marker='o', linestyle='-', color='r', label='Original OrderBook (Total Time)')
    plt.plot(order_sizes, optimized_execution_times_s, marker='s', linestyle='--', color='b', label='Optimized OrderBook (Total Time)')

    plt.xlabel("Number of Orders (Add + Modify + Delete sequence)")
    plt.ylabel("Total Execution Time (seconds)")
    plt.title("HFT Order Book Performance Comparison (Total Workload)")
    plt.legend()
    plt.grid(True)
    plt.xscale('log') 
    plt.yscale('log')

    for i, txt in enumerate(original_execution_times_s):
        plt.annotate(f"{txt:.3f}s", (order_sizes[i], original_execution_times_s[i]), textcoords="offset points", xytext=(0,5), ha='center', color='r')
    for i, txt in enumerate(optimized_execution_times_s):
        plt.annotate(f"{txt:.3f}s", (order_sizes[i], optimized_execution_times_s[i]), textcoords="offset points", xytext=(0,-15), ha='center', color='b')

    plt.tight_layout()
    output_filename = "performance_chart_total_workload.png"
    plt.savefig(output_filename)
    print(f"Chart saved as {output_filename}")
    plt.show()

if __name__ == "__main__":
    plot_from_console_output()