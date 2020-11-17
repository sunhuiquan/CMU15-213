/* 
 * CS:APP Data Lab 
 * 
 *<Sun Huiquan>
 *<2020/9/22/21:33 -- 2020/9/>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~((~x) | (~y));
}


/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    return (x >> (n << 3)) & 0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    int mask = ((1 << 31) >> n) << 1;
    // 0 => 0x80000000 << 1 => 0 it doesn't matter the result
    return (x >> n) & ~mask;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int mask1 = 0x55 | (0x55 << 8);
    int mask_1 = mask1 |(mask1 << 16);
    int mask2 = 0x33 | (0x33 << 8);
    int mask_2 = mask2 | (mask2 << 16);
    int mask3 = 0x0f | (0x0f << 8);
    int mask_3 = mask3 | (mask3 << 16);
    int mask_4 = 0xff | (0xff << 16);
    int mask_5 = 0xff | (0xff << 8);

    int res = (x & mask_1) + ((x >> 1) & mask_1);
    res = (res & mask_2) + ((res >> 2) & mask_2);
    res = (res & mask_3) + ((res >> 4) & mask_3);
    res = (res & mask_4) + ((res >> 8) & mask_4);
    res = (res & mask_5) + ((res >> 16) & mask_5);

    return res;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    return (~((x >> 31)|((~x + 1) >> 31))) & 1;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    return !(((x >> (n + (~0))) + 1)>> 1);
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int is_neg = ((x & (1 << 31)) >> 31) & 1;
    int under_zero_1 = is_neg;
    int bias;
    under_zero_1 = ~under_zero_1 + 1;
    bias = (is_neg << n) +under_zero_1;
    // 0 doesn't matter it's still 0,and 1 will be used
    return (x + bias) >> n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    // (~1 + 1) ==  (~0)
    return !(((x >> 31) & 1) + (((x + (~0)) >> 31) & 1));
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    //if they have same symbol,it won't overflow,just judge easily
    //if they have different symbol,it's easiler
    int sym_x = (x >> 31) & 1;
    int sym_y = (y >> 31) & 1;

    int d =  x + ~y; // x + (~y + 1) - 1
    int sym_d = (d >> 31) & 1;

    return ((!(sym_x ^ sym_y)) & sym_d) | (sym_x & (!sym_y));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    // we already know that x > 0 from the problem content
    int ans = 0;
    ans = ans + ((!!(x >> (16 + ans))) << 4);
    ans = ans + ((!!(x >> (8 + ans))) << 3);
    ans = ans + ((!!(x >> (4 + ans))) << 2);
    ans = ans + ((!!(x >> (2 + ans))) << 1);
    ans = ans + ((!!(x >> (1 + ans))) << 0);
    
    return ans;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    //IEEE 754:
    //float: 1 | 8 | 23
    //NaN: x | 11111111 | not all 0
    //infinite x | 11111111 | all 0
    unsigned tmp = uf & 0x7fffffff; // to avoid pos or neg
    int s;
    if(tmp > 0x7f800000) // NaN
	return uf;
    
    s = !((uf >> 31) & 1);
    return (s << 31) | tmp;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    //because int can't be (-1,0) or (0,1),so it must be format number    
    unsigned result;
    int sign = x & (0x80000000);
    int res = 31;
    int ff = 0,ss = 0;
    int tmp = 0;

    if(!x)
	result = 0;
    else if(x == 0x80000000)
	result = 0xcf000000;
    else{
	if(sign)
	    x = (~x) + 1;

	while(!((1 << res) & x))
	   res -= 1;
	x ^= (1 << res); // format number minus 1
	if(res < 23){
	     x <<= (23 - res);
	}else{
	    tmp = res - 24;
	    if(tmp >= 0){
		ss = (x >> tmp) & 1;
		ff = ((1 << tmp) - 1) & x;
	    }
	    x >>= (res - 23);
	}
	x = x | ((res + 127) << 23);
	if(ff == 0)
	    ss = (ss & x); // the last bit of x must be 1
	x += ss;
        x |= sign;
	result = x;
    }

    return result;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned tmp = uf & 0x7fffffff;
    unsigned e,f;
    if(tmp >= 0x7f800000) // infinte and NaN
	return uf;

    e = uf & 0x7f800000;
    f = uf & 0x7fffff;

    if(tmp >= 0x800000)
	e += (1 << 23);
    else if(f >= (1 << 22)){
	e += (1 << 23);
	f <<= 1;
	f &= 0x7fffff;
    }
    else
	f <<= 1;   
    
    return ((uf & 0x80000000) | e) | f;
}
