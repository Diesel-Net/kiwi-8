#!/usr/bin/env python3
"""
Generate a C++ header file containing the LICENSE text as a string constant.
Usage: python3 generate_license_header.py <license_file> <output_header>
"""

import sys
import os
import textwrap

def wrap_text(text, width=72):
    """Wrap text to specified width while preserving paragraph breaks."""
    paragraphs = text.split('\n\n')
    wrapped_paragraphs = []

    for para in paragraphs:
        # Remove existing line breaks within the paragraph
        para = ' '.join(para.split())
        if para:
            # Wrap to width
            wrapped = textwrap.fill(para, width=width)
            wrapped_paragraphs.append(wrapped)
        else:
            wrapped_paragraphs.append('')

    return '\n\n'.join(wrapped_paragraphs)

def escape_string(text):
    """Escape special characters for C++ string literal."""
    return text.replace('\\', '\\\\').replace('"', '\\"')

def generate_header(license_file, output_file):
    """Generate C++ header with license text."""

    if not os.path.exists(license_file):
        print(f"Error: License file '{license_file}' not found", file=sys.stderr)
        sys.exit(1)

    with open(license_file, 'r', encoding='utf-8') as f:
        license_text = f.read().strip()

    # Wrap text to ~72 characters
    wrapped_text = wrap_text(license_text)

    # Split into lines and format as C++ string literals
    lines = wrapped_text.split('\n')
    cpp_lines = []
    for i, line in enumerate(lines):
        escaped_line = escape_string(line)
        if i < len(lines) - 1:
            cpp_lines.append(f'    "{escaped_line}\\n"')
        else:
            cpp_lines.append(f'    "{escaped_line}"')

    # Generate header content
    header_content = f"""// Auto-generated file - DO NOT EDIT
// Generated from LICENSE file

#ifndef LICENSE_H
#define LICENSE_H

#ifdef __cplusplus
extern "C" {{
#endif

static const char* LICENSE_TEXT =
{chr(10).join(cpp_lines)};

#ifdef __cplusplus
}}
#endif

#endif // LICENSE_H
"""

    # Write header file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)

    print(f"Generated {output_file} from {license_file}")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python3 generate_license_header.py <license_file> <output_header>")
        sys.exit(1)

    license_file = sys.argv[1]
    output_file = sys.argv[2]
    generate_header(license_file, output_file)
