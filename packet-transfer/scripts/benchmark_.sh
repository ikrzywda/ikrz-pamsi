``#!/bin/bash

template_file_path=$TEMPLATE_FILE_PATH
driver_path=$DRIVER_PATH
checksum_path=$CHECKSUM_PATH

message_length=$1
part_size=$2
message_offset=$3
uuid=$(uuidgen)


generated_input_path="/tmp/input-$uuid"
output_file="/tmp/output-$uuid"

echo "Files uuid: $uuid"
echo "Running with message_length: $message_length, part_size: $part_size, message_offset: $message_offset"


LOG_LEVEL=INFO GENERATED_INPUT_PATH="$generated_input_path" REASSEMBLED_INPUT_PATH="$output_file" "$driver_path" "$message_length" "$part_size" "$message_offset" "$template_file_path"
# Check if program ended successfully
if [ $? -eq 0 ]; then
  echo "Program ended successfully"
else
  echo "Program ended with error"
  exit 1
fi

# Checksum test
md5 "$generated_input_path" > "$generated_input_path.md5"
md5 "$output_file" > "$output_file.md5"
diff "$generated_input_path.md5" "$output_file.md5"


# Cleanup
rm "$generated_input_path" "$output_file" "$generated_input_path.md5" "$output_file.md5"
