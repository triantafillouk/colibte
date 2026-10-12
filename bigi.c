// Source - https://stackoverflow.com/a/30077899
// Posted by jww, modified by community. See post 'Timeline' for change history
// Retrieved 2026-10-10, License - CC BY-SA 3.0

#include <stdio.h>
#include <openssl/bn.h>

int main ()
{
  static const
  char p_str[] = "82019154470699086128524248488673846867876336512717";
  char p_str2[] = "2535";

  BIGNUM *p = BN_new();
  BIGNUM *p2 = BN_new();
  BIGNUM *res = BN_new();

  BN_CTX *c=BN_CTX_new();
  BN_dec2bn(&p, p_str);
  BN_dec2bn(&p2, p_str2);
  BN_CTX_start(c);
  BN_mul(res, p, p2,c);
  BN_CTX_end(c);
  char * number_str = BN_bn2dec(res);

  printf("s=%s\n",p_str);
  printf("r=%s\n", number_str);

  OPENSSL_free(number_str);
  BN_free(p);
  BN_free(p2);
  BN_free(res);
  return 0;
}
