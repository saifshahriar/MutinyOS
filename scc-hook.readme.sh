#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

file="README.md"
start="<!-- current-status start -->"
end="<!-- current-status end -->"
tmp=$(mktemp)

command -v scc >/dev/null 2>&1 || {
	echo "${RED}scc not found, skipping README update...${RESET}"
	exit 127
}

{
	echo "$start"
	echo '```bash'
	scc
	echo '```'
	echo "$end"
} >"$tmp"

awk -v start="$start" -v end="$end" '
    $0 ~ start {while ((getline line < "'"$tmp"'") > 0) print line; inblock=1; next}
    $0 ~ end {inblock=0; next}
    !inblock {print}
' "$file" >"${file}.tmp" && mv "${file}.tmp" "$file"

rm "$tmp"

echo "${GREEN}README updated successfully!${RESET}"
