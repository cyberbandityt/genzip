# genZip

$genZip$ is a simple command-line file compression and decompression tool written in C, using the Huffman coding algorithm. It reduces file sizes for efficient storage and allows decompression to restore the original files.

## Features
- Compression: Reduces the size of files by encoding data using Huffman coding.
- Decompression: Restores compressed files back to their original form.

## Usage
### Compression
```bash
./genZip -c <input_file> <output_file>
```

- `<input_file>`: Path to the file you want to compress.
- `<output_file>`: Path where the compressed file will be saved.

### Decompression
```bash
./genZip -d <input_file> <output_file>
```
- `<input_file>`: Path to the compressed file.
- `<output_file>`: Path where the decompressed file will be saved.
