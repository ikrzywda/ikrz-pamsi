#!/bin/bash

# Define the CSV header
# echo "MESSAGE_GENERATED,TRANSMISSION_DONE,MESSAGE_REASSEMBLED,MESSAGE_SIZE" > output.csv

# # Set the length of the message in bytes
# MESSAGE_SIZE=$1

# # Loop n times (adjust the loop count as needed)
# for ((i=1; i<=100; i++))
# do
#     # Run the benchmark script with the specified message size
#     message_size=$((MESSAGE_SIZE * i))
#     echo "Running with message size: $message_size"
    
#     TEMPLATE_FILE_PATH=words.txt DRIVER_PATH=build/packet-transfer ./benchmark.sh $message_size 1000 0 | \
#     # Grep the desired lines and extract the relevant information
#     grep -E "MESSAGE_GENERATED|TRANSMISSION_DONE|MESSAGE_REASSEMBLED" | \
#     awk -F': ' '{print $2}' | \
#     tr '\n' ',' | \
#     sed -E "s/,$/,${message_size}\n/" >> output.csv
# done


python -c "
import csv
import matplotlib.pyplot as plt

# Read the CSV file
with open('output.csv', 'r') as file:
    reader = csv.DictReader(file)
    data = list(reader)

# Extract data columns
message_sizes = [int(row['MESSAGE_SIZE']) for row in data]
message_generated = [int(row['MESSAGE_GENERATED']) for row in data]
transmission_done = [int(row['TRANSMISSION_DONE']) for row in data]
message_reassembled = [int(row['MESSAGE_REASSEMBLED']) for row in data]

# Plot the data
plt.figure(figsize=(10, 6))

plt.plot(message_sizes, message_generated, label='MESSAGE_GENERATED')
plt.plot(message_sizes, transmission_done, label='TRANSMISSION_DONE')
plt.plot(message_sizes, message_reassembled, label='MESSAGE_REASSEMBLED')

plt.xlabel('Message Size [b]')
plt.ylabel('Time [ms]')
plt.title('Benchmark Data')
plt.legend()
plt.grid(True)
plt.savefig('benchmark.png')
plt.show()
"
