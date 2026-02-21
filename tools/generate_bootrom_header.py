#!/usr/bin/env python3
"""
Generate a C/C++ header file from a binary file (similar to xxd -i).
Usage: python3 generate_bootrom_header.py <rom_file> <output_header>
"""

import sys
import os

def generate_header(rom_file, output_file):
    """Generate C++ header with ROM data as byte array."""

    if not os.path.exists(rom_file):
        print(f"Error: ROM file '{rom_file}' not found", file=sys.stderr)
        sys.exit(1)

    # Read binary data
    with open(rom_file, 'rb') as f:
        rom_data = f.read()

    rom_size = len(rom_data)
    rom_name = os.path.splitext(os.path.basename(rom_file))[0]

    # Format bytes as hex, 8 per line
    hex_lines = []
    for i in range(0, rom_size, 8):
        chunk = rom_data[i:i+8]
        hex_bytes = ', '.join(f'0x{b:02x}' for b in chunk)
        hex_lines.append(f'    {hex_bytes}')

    # Join with commas and newlines
    formatted_data = ',\n'.join(hex_lines)

    # Generate header content
    header_content = f"""// Auto-generated file - DO NOT EDIT
// Generated from {os.path.basename(rom_file)}

#ifndef BOOTROM_H
#define BOOTROM_H

#ifdef __cplusplus
extern "C" {{
#endif

#define BOOTROM_SIZE {rom_size}

/* {os.path.basename(rom_file)} */
static const unsigned char bootrom[BOOTROM_SIZE] = {{
{formatted_data}
}};

#ifdef __cplusplus
}}
#endif

#endif // BOOTROM_H
"""

    # Write header file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)

    print(f"Generated {output_file} from {rom_file} ({rom_size} bytes)")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python3 generate_bootrom_header.py <rom_file> <output_header>")
        sys.exit(1)

    rom_file = sys.argv[1]
    output_file = sys.argv[2]
    generate_header(rom_file, output_file)
