#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 input_file max_digit"
    exit 1
fi

input_file="$1"
max_digit="$2"

filename="${input_file%.*}"
extension="${input_file##*.}"
output_file="${filename}_S${max_digit}.${extension}"

awk -v max="$max_digit" '
{
  # Only search substring after char 50 (index 51 in awk 1-based)
  line = $0
  if (length(line) < 52) {
    # too short to have ": " + digit after 50, keep line
    print
    next
  }
  
  # substring from position 51 to end
  subline = substr(line, 51)
  
  # Try to find ": " followed by a digit in subline
  pos = match(subline, /: [0-9]/)
  
  if (pos == 0) {
    # no match found, keep line
    print
  } else {
    # extract digit (pos+2 is digit position in subline)
    digit_char = substr(subline, pos+2, 1)
    
    if (digit_char <= max) {
      print
    }
    # else digit > max, omit line
  }
}
' "$input_file" > "$output_file"

echo "Filtered output saved to: $output_file"
