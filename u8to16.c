#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <locale.h>

// #define __STD_UTF_16__


#include <stdint.h>

/**
 * Encode a code point using UTF-8
 * 
 * @author Ondřej Hruška <ondra@ondrovo.com>
 * @license MIT
 * 
 * @param out - output buffer (min 5 characters), will be 0-terminated
 * @param utf - code point 0-0x10FFFF
 * @return number of bytes on success, 0 on failure (also produces U+FFFD, which uses 3 bytes)
 */
int utf8_encode(char *out, uint32_t utf)
{
  if (utf <= 0x7F) {
    // Plain ASCII
    out[0] = (char) utf;
    out[1] = 0;
    return 1;
  }
  else if (utf <= 0x07FF) {
    // 2-byte unicode
    out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
    out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
    out[2] = 0;
    return 2;
  }
  else if (utf <= 0xFFFF) {
    // 3-byte unicode
    out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
    out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
    out[3] = 0;
    return 3;
  }
  else if (utf <= 0x10FFFF) {
    // 4-byte unicode
    out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
    out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
    out[4] = 0;
    return 4;
  }
  else { 
    // error - use replacement character
    out[0] = (char) 0xEF;  
    out[1] = (char) 0xBF;
    out[2] = (char) 0xBD;
    out[3] = 0;
    return 0;
  }
}

//Pointer arrays must always include the array size, because pointers do not know about the size of the supposed array size.
void utf8_to_utf16(unsigned char* const utf8_str, int utf8_str_size, char16_t* utf16_str_output, int utf16_str_output_size) 
{
	//First, grab the first byte of the UTF-8 string
	unsigned char* utf8_currentCodeUnit = utf8_str;
	char16_t* utf16_currentCodeUnit = utf16_str_output;
	int utf8_str_iterator = 0;
	int utf16_str_iterator = 0;

	//In a while loop, we check if the UTF-16 iterator is less than the max output size. If true, then we check if UTF-8 iterator
	//is less than UTF-8 max string size. This conditional checking based on order of precedence is intentionally done so it
	//prevents the while loop from continuing onwards if the iterators are outside of the intended sizes.
	while (*utf8_currentCodeUnit && (utf16_str_iterator < utf16_str_output_size || utf8_str_iterator < utf8_str_size)) {
		//Figure out the current code unit to determine the range. It is split into 6 main groups, each of which handles the data
		//differently from one another.
		if (*utf8_currentCodeUnit < 0x80) {
			//0..127, the ASCII range.

			//We directly plug in the values to the UTF-16 code unit.
			*utf16_currentCodeUnit = (char16_t) (*utf8_currentCodeUnit);
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xC0) {
			//0x80..0xBF, we ignore. These are reserved for UTF-8 encoding.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xE0) {
			//128..2047, the extended ASCII range, and into the Basic Multilingual Plane.

			//Work on the first code unit.
			char16_t highShort = (char16_t) ((*utf8_currentCodeUnit) & 0x1F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Work on the second code unit.
			char16_t lowShort = (char16_t) ((*utf8_currentCodeUnit) & 0x3F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Create the UTF-16 code unit, then increment the iterator.
			//Credits to @tbeu. 
			//Thanks to @k6l2 for explaining why we need 6 instead of 8.
			//It's because 0x3F is 6 bits of information from the low short. By shifting 8 bits, you are 
			//adding 2 extra zeroes in between the actual data of both shorts.
			int unicode = (highShort << 6) | lowShort;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 2;
		}
		else if (*utf8_currentCodeUnit < 0xF0) {
			//2048..65535, the remaining Basic Multilingual Plane.

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 1110aaaa 10bbbbcc 10ccdddd
			//Where a is 4th byte, b is 3rd byte, c is 2nd byte, and d is 1st byte.
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			//Create the resulting UTF-16 code unit, then increment the iterator.
			int unicode = (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			//According to math, UTF-8 encoded "unicode" should always fall within these two ranges.
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 3;
		}
		else if (*utf8_currentCodeUnit < 0xF8) {
			//65536..10FFFF, the Unicode UTF range

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 11110abb 10bbcccc 10ddddee 10eeffff
			//Where a is 6th byte, b is 5th byte, c is 4th byte, and so on.
			char16_t sixthChar = (char16_t) ((*utf8_currentCodeUnit) & 0x4) >> 2;
			char16_t fifthCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t fifthCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

#if	0
			int unicode = (sixthChar << 4) | (fifthCharHigh << 2) | fifthCharLow | (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;

			printf("F0  unicode = %X\n",unicode);
			printf("	 1=%X 2l=%X 2h=%X 3=%X 4=%X\n",firstChar,secondCharLow,secondCharHigh,thirdChar,fourthChar);
			printf("	5l=%X 5h=%X 6=%X\n",fifthCharLow,fifthCharHigh,sixthChar);
			char16_t highSurrogate = (unicode - 0x10000) / 0x400 + 0xD800;
			char16_t lowSurrogate = (unicode - 0x10000) % 0x400 + 0xDC00;
			printf("	low=%X high=%X\n",lowSurrogate,highSurrogate);
#else		
			int unicode = firstChar | ((secondCharLow+4*secondCharHigh)) << 4 | (thirdChar << 8) | (fourthChar <<12) | (fifthCharLow+fifthCharHigh*4) << 16; 
			printf("unicode : %X\n",unicode);
			// printf("1 %08X\n",firstChar);
			// printf("2 %08X\n",((secondCharLow+4*secondCharHigh)) << 4);
			// printf("3 %08X\n",thirdChar << 8);
			// printf("4 %08X\n",fourthChar << 12);
			// printf("5 %08X\n",(fifthCharLow+fifthCharHigh*4) << 16);
#endif
			printf("	-> %X %X %X %X %X\n",fifthCharLow+fifthCharHigh*4,fourthChar,thirdChar,secondCharLow+4*secondCharHigh,firstChar);
			//Set the UTF-16 code units
			// *utf16_currentCodeUnit = lowSurrogate;
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Check to see if we're still below the output string size before continuing, otherwise, we cut off here.
			if (utf16_str_iterator < utf16_str_output_size) {
				// *utf16_currentCodeUnit = highSurrogate;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 4;
		}
		else {
			//Invalid UTF-8 code unit, we ignore.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
	}

	//We clean up the output string if the UTF-16 iterator is still less than the output string size.
	while (utf16_str_iterator < utf16_str_output_size) {
		*utf16_currentCodeUnit = '\0';
		utf16_currentCodeUnit++;
		utf16_str_iterator++;
	}
}

int main(int argc, unsigned char **argp) {
	// unsigned char array[] = u8"我是誰?";
	unsigned char *array = argp[1];
	char16_t output[25];

	utf8_to_utf16(&array[0], sizeof(array) / sizeof(array[0]), &output[0], sizeof(output) / sizeof(output[0]));
	printf("%s -> %X %X %X %X\n",array,output[0],output[1],output[2],output[3]);
	//Set debug breakpoint on the return statement, and inspect "output" variable.
	return 0;
}