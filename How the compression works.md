# Tiny Tiny Wordle - Dictionary compression algorithm
In order to fit a fairly large dictionary on an Arduino, a simple compression algorithm had to be devised.

Requirements:
* Can encode strings of identical length made of uppercase ASCII characters
* Smaller progmem size than just storing all the words in a long string
* Words must be individually addressable without decompressing the entire dictionary (seekable)
* Can run on an Arduino using very little RAM and progmem

## Compression
The input dictionary file must be a simple text file containing one word per line (lines can end with `\n` or `\r\n`), words can only contain uppercase letters from A to Z and must be of the same length.

This part must be done by a PC.

1. Read the entire dictionary file in a string array
2. Sort the array in alphabetical order
3. Generate a long string with all the words without any kind of separator
4. Create a byte array of size `ceil(nWords*wordLength*5/8)` initialized with all zero bits
5. Initialize a pointer `byteN` at `0` and a bit pointer `bitN` at `0` (LSB) that points at a bit inside the byte pointed by `byteN` (we use these to point to specific bits inside the byte array)
6. Compress each character `c` in the long string in this way:
    1. Subtract `'A'` from `c`, so that the letter `A` corresponds to `0`, `B` corresponds to `1`, etc.
    2. Use 5 bits to represent `c` (enough for 26 characters)
    3. For each bit `b` in `c` starting from the LSB:
        1. Set the bit pointed by `bitN` inside the byte pointed by `byteN` to the value of that bit
        2. Increase `bitN` by `1`
        3. If `bitN = 8` (current byte is full), increase `byteN` by `1` and reset `bitN` to `0` (move to a new byte)
7. On the output file:
    1. Write a 16 bit unsigned integer (little endian) containing the number of words
    2. Write an 8 bit unsigned integer containing the word length
    3. Write the byte array generated in step 6

The generated file can then be converted into an `.h` file to be included in the Arduino project.

## Decompression
The dictionary is seekable, so we can decode individual words. Let's call `x` the number of the word that we want to decode.

This can be done by the Arduino.

1. Read a 16 bit unsigned integer (little endian) containing the number of words and store it in a nWords variable
2. Read a 8 bit unsigned integer containing the word length
3. Calculate the starting position inside the stream (starting after the first 3 bytes):
    1. Calculate the number of the bit where the word starts: `startFrom=x*wordLength*5`
    2. `byteN=startFrom/8`
    3. `bitN=startFrom%8`
4. Create an empty output string
5. Repeat `wordLength` times:
    1. Create a character `c` initialized with all zero bits
    1. For `i` from `0` to `5` (five times):
        1. Set the `i`-th bit (starting from the LSB) of `c` to the value of the bit pointed by `bitN` inside the byte pointed by `byteN`
        2. Increase `bitN` by `1`
        3. If `bitN = 8`, increase `byteN` by `1` and reset `bitN` to `0` (move to next byte in the compressed stream)
    2. Add `c` to the output string
6. Add a terminator to the output string
