/*
 with GMP library installed
  apt install libgmp-dev
  dnf install gmp-devel
  zypper install gmp-devel
*/

#include <stdio.h>
#include <gmp.h>

int main(void)
{
    mpz_t a,b, sum, product, power, factorial;
    unsigned int n = 50;

	mpf_t  power2,p2;
    /* Initialize variables */
    mpz_inits(a, b, sum, product, power, factorial, NULL);
	mpf_inits(power2,p2,NULL);
    /* Set values from strings */
    mpz_set_str(a,
        "123456789123456789123456789123456789", 10);

    mpz_set_str(b,
        "987654321987654321987654321987654321", 10);

	mpf_set_str(p2,"2.0",10);

    /* Addition */
    mpz_add(sum, a, b);

    /* Multiplication */
    mpz_mul(product, a, b);

    /* Compute 2^100 */
    mpz_ui_pow_ui(power, 2, 100);

    mpf_pow_ui(power2, p2, 100);

    /* Compute 50! */
    mpz_fac_ui(factorial, n);

    printf("a        = ");
    mpz_out_str(stdout, 10, a);
    printf("\n");

    printf("b        = ");
    mpz_out_str(stdout, 10, b);
    printf("\n");

    printf("a + b    = ");
    mpz_out_str(stdout, 10, sum);
    printf("\n");

    printf("a * b    = ");
    mpz_out_str(stdout, 10, product);
    printf("\n");

    printf("2^100    = ");
    mpz_out_str(stdout, 10, power);
    printf("\n");

    printf("2.2^100  = ");
    mpf_out_str(stdout, 10, 31, power2);
    printf("\n");

    printf("50!      = ");
    mpz_out_str(stdout, 10, factorial);
    printf("\n");

    /* Free memory */
    mpz_clears(a, b, sum, product, power, factorial, NULL);

    return 0;
}
