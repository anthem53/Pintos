#include<stdio.h>

#define P 17
#define Q 14
#define F 1 << 14

static int f = F;

int int_to_fd(int n);
int fd_to_int_round (int x);
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
  return n * f;
} 

int fd_to_int_round(int x)
{
  return x / f;
  if(x >= 0 )
    return ( x + (f / 2)) / f;
  else
    return ( x - (f / 2)) / f;
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
  return x + n * f;
}
int sub_fd_int(int x, int n)
{
  return x - n * f;
}
int mul_fd (int x, int y)
{
  return ((int64_t) x ) * y / f; 
}
int mul_fd_int (int x, int n)
{
  return x * n;
}
int div_fd (int x, int y)
{
  return ((int64_t)x)* f /y;
}

int div_fd_int(int x, int n)
{
  return x / n;
}




