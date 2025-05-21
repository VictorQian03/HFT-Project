import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Get the absolute path to the challenge directory
current_dir = os.path.dirname(os.path.abspath(__file__))

# Construct full paths to the CSV files
packet_latency_path = os.path.join(current_dir, 'client_packet_latency_log.csv')
tcp_response_path = os.path.join(current_dir, 'client_tcp_response_latency_log.csv')

# Read the data using full paths
packet_latency_df = pd.read_csv(packet_latency_path)
tcp_response_df = pd.read_csv(tcp_response_path)

# Convert nanoseconds to microseconds for better readability
packet_latency_df['udp_arrival_to_queue_us'] = packet_latency_df['udp_arrival_to_queue_ns'] / 1000
packet_latency_df['queue_duration_us'] = packet_latency_df['queue_duration_ns'] / 1000
packet_latency_df['worker_processing_us'] = packet_latency_df['worker_processing_ns'] / 1000
tcp_response_df['target_parsed_to_tcp_sent_us'] = tcp_response_df['target_parsed_to_tcp_sent_ns'] / 1000

# Print basic statistics
print("\n=== Packet Latency Statistics (microseconds) ===")
print(packet_latency_df[['udp_arrival_to_queue_us', 'queue_duration_us', 'worker_processing_us']].describe())

print("\n=== TCP Response Statistics (microseconds) ===")
print(tcp_response_df['target_parsed_to_tcp_sent_us'].describe())


# Additional Analysis
print("\n=== Performance Metrics ===")
print(f"Total packets processed: {len(packet_latency_df)}")
print(f"Total challenges responded to: {len(tcp_response_df)}")
print(f"Average packets per challenge: {len(packet_latency_df)/len(tcp_response_df):.2f}")

# Calculate percentiles for key metrics
percentiles = [50, 75, 90, 95, 99, 99.9]
print("\n=== Key Percentiles (microseconds) ===")
for p in percentiles:
    print(f"\n{p}th percentile:")
    print(f"UDP arrival to queue: {np.percentile(packet_latency_df['udp_arrival_to_queue_us'], p):.2f}")
    print(f"Queue duration: {np.percentile(packet_latency_df['queue_duration_us'], p):.2f}")
    print(f"Worker processing: {np.percentile(packet_latency_df['worker_processing_us'], p):.2f}")
    print(f"TCP response: {np.percentile(tcp_response_df['target_parsed_to_tcp_sent_us'], p):.2f}")