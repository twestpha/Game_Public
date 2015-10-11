#!/bin/bash
echo
echo "Updating build count..."

# Move into the directory that contains this script (which also contains the generic buildcount script).
cd "$(dirname "$0")"

# Run the buildcount until it completes, or for the specified time
# The timeout command to use is specified by the user in the first argument and the time to wait is the second argument
$1 $2 ./buildcount.sh

# Handle the timeout error
if [[ $? -eq 124 ]]; then
    echo -e "\033[1;31m[Error]\033[0m Failed to contact the buildcount server."
fi
