#!/usr/bin/env bash
set -euo pipefail

# Simple linter to detect forbidden C++/OO tokens in the Core/ folder.
# Usage: tools/check_no_oo.sh [TARGET_DIR]

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
TARGET_DIR="${1:-Core}"

EXCLUDE_PATTERNS=(
  "Core/imgui_impl_sdl"
  "Core/open_file_dialog.cc"
)

PATTERNS=(
  '\bclass\b'
  '\bvirtual\b'
  '\btemplate\b'
  '\boperator\b'
  '\bnew\b'
  '\bdelete\b'
  'std::'
  '#include[[:space:]]*<iostream>'
)

echo "Checking ${TARGET_DIR} for forbidden C++/OO tokens..."
fail=0

# Find source files (C/C++ headers and sources)
# Use a portable process-substitution + read loop instead of mapfile (macOS bash lacks mapfile)
# Strip C/C++ comments (both /* */ and //) before searching so tokens in comments are ignored
while IFS= read -r -d '' file; do
  skip=false
  for p in "${EXCLUDE_PATTERNS[@]}"; do
    if [[ "$file" == "$p"* ]]; then
      skip=true
      break
    fi
  done
  $skip && continue

  # Produce a comment-stripped copy (preserving line structure) using awk
  filtered=$(awk '
    BEGIN { in_comment=0 }
    {
      line = $0
      out = ""
      while (length(line) > 0) {
        if (in_comment) {
          pos = index(line, "*/")
          if (pos > 0) {
            line = substr(line, pos+2)
            in_comment = 0
          } else {
            # entire line inside comment
            line = ""
            break
          }
        } else {
          pos1 = index(line, "/*")
          pos2 = index(line, "//")
          if (pos2 > 0 && (pos1 == 0 || pos2 < pos1)) {
            out = out substr(line, 1, pos2-1)
            line = ""
          } else if (pos1 > 0) {
            out = out substr(line, 1, pos1-1)
            line = substr(line, pos1+2)
            in_comment = 1
          } else {
            out = out line
            line = ""
          }
        }
      }
      print out
    }
  ' "$file")

  for pat in "${PATTERNS[@]}"; do
    if echo "$filtered" | grep -nE "${pat}" >/dev/null 2>&1; then
      echo "$filtered" | grep -nE "${pat}" | sed "s|^|${file}:|"
      fail=1
    fi
  done
done < <(find "${TARGET_DIR}" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.mm" -o -name "*.m" \) -print0 2>/dev/null || true)

if [ "$fail" -ne 0 ]; then
  echo "\nForbidden tokens found in ${TARGET_DIR}. See output above."
  exit 1
else
  echo "No forbidden tokens found in ${TARGET_DIR}."
fi
