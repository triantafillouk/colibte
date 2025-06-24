#include <stdio.h>
#include <string.h>

int utf8_to_unicode(char *s)
{
    unsigned int charcode = 0;
	int ind=0;
	
    unsigned int t = s[ind];
    // coded.pop_front();
    if (t < 128)
    {
        return t;
    }
    int high_bit_mask = (1 << 6) -1;
    int high_bit_shift = 0;
    int total_bits = 0;
    const int other_bits = 6;
    while((t & 0xC0) == 0xC0)
    {
        t <<= 1;
        t &= 0xff;
        total_bits += 6;
        high_bit_mask >>= 1; 
        high_bit_shift++;
        charcode <<= other_bits;
		printf("	t = %X cc=%X \n",t,charcode);
        charcode |= s[ind] & ((1 << other_bits)-1);
		printf("	t = %X cc=%X \n",t,charcode);
        t = s[ind++];
    } 
    charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;
    return charcode;
}

int main(int argc, char **argv)
{
	printf("input is %s\n",argv[1]);
	char *s=argv[1];
	int x = utf8_to_unicode(s);
	printf("out is %X\n",x);

	return 0;
}
