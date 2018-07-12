#include<stdio.h>

#define P 17
#define Q 14
#define F 1 << 14

int int_to_fd(int n);
int fd_to_int_rount (int x);
int add_fd(int x, int y);
int sub_fd(int x, int y);
int add_fd_int(int x, int n);
int sub_fd_int (int x, int n);
int mul_fd (int x, int y);
int mul_fd_int (int x, int n);
int div_fd (int x , int y);
int div_fd_int (int x, int n);

int int_to_fd(int n)
{
  return n * F;
} 

int fd_to_int_round(int x)
{
  if(x >= 0 )
    return ( x + F / 2) / F;
  else
    return ( x - F / 2) / F;
}

int add_fd(int x, int y)
{
  return x + y;
}

int sub_fd(int x, int y)
{
  return x - y;
}
int add_fd_int(int x, int n)
{
  return x + n * F;
}
int sub_fd_int(int x, int n)
{
  return x - n * F;
}
int mul_fd (int x, int y)
{
  return ((int64_t) x ) * y / F; 
}
int mul_fd_int (int x, int n)
{
  return x * n;
}
int div_fd (int x, int y)
{
  return ((int64_t)x)* F /y;

}

int div_fd_int(int x, int n)
{
  return x / n;
}




