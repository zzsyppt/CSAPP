/* 
 * CS:APP Data Lab 
 * 
 * 10235501419 李佳亮
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
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


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

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

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
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
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
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  /* 
    考虑A^B = (~A&B) | (~B&A)
    再用 ~ 和 & 替换 |
    由X|Y = ~(~X&~Y)
    所以原式等于~(~(~A&B)&~(~B&A))
  */
  return ~(~(~x&y)&~(~y&x));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1<<31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  /*
   * Tmax 具有独特的性质 ~x = x+1，但 -1 也具有这个性质。
   * 然而，~Tmax = Tmax+1 = 100...00 = -1，但 ~( -1) = -1+1 = 0。
   * 所以判断条件即为 ~x == x+1 && !!(~x)（使用 !! 可把整数转为布尔数）。
   * 最后，由于 && 两侧都为布尔数，故可以用 & 代替 &&；
   * 由于 a=b 当且仅当 a^b=0，所以用 ! 和 ^ 可以替换 ==。
   */
  return !((~x)^(x+1)) & !!(~x);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  /*
   * 注意位的序号是从0开始计的。
   * 构造掩码mask = 1010...10，那么奇数位均为1的x应当满足x & mask == model。
   * 与第3题同理，用!和^代替==即可。
   */
  int tmp, mask; //注意C89的规则，变量要统一先声明
  tmp = 0xAA + (0xAA << 8);
  mask = tmp + (tmp << 16); 
  return !((x&mask)^mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  /*
   * 0x30 = 0b110000, 0x39 = 0b111001。
   * 观察到满足条件的x的第5,6位（位从1记）一定是1且x没有更高的位，并且我们要取x的后四位进行判断。
   * 考虑异或运算：与1异或取反，与0异或不变，这正好符合我们的要求。
   * 取mask = 0b111111，记a = x ^ mask。
   * 若x满足要求，那么a应该没有第4位及以后更高的位，且只有9种取值情况。观察共性得到判断条件。
   */
  int a = x^0x3F;   //a = x ^ 0b111111;
  return (!(a>>4)) & ( a>>3 | (a+1)>>3 | (a+2)>>3 ); 
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int val = (!!x)<<31>>31;
  return (val & y) + (~val & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*
   * 两种情况：
   * 1. 若x和y同号，y>=x，则y-x>=0，y-x的首位应为0。
   * 2. 若x和y的符号不同，y-x会产生溢出，数学上的y-x>=0当且仅当y为正，x为负。
   */
  int flag1, flag2;
  flag1 = (~(y>>31)^(x>>31)) & !((y+(~x+1))>>31&1);	//x,y同号且y-x首位为0
  flag2 = (~y>>31&1) & (x>>31&1);	//x负y正
  return flag1 | flag2;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  /*
   * 除了0之外的数，相反数的最高位和该数的最高位二者至少有一个是1，
   * 记表达式 a = (x|(~x+1))>>31。
   * 对于x = 0, a = 0； 对于x != 0, a = 11111111 = -1
   * 因此返回a + 1即可满足条件
   */
  return ((x|(~x+1))>>31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  M/ax ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  /*
   * 返回表示x需要的最小位数。
   * 对于正数，即求使x>>k=0（全0）的最小k再加1（正数的位模式首位必须为0）；
   * 对于负数，即求使x>>k=-1（全1）的最小k再加1（负数的位模式首位必须为1）
   * 我们可以把负数的情况化归为正数的情况，即若x为负数，howManyBits(x)=howManyBits(~x)
   * 回顾异或的两种理解方式：1.相同为0，不同为1，即x^x=0，x^~x=-1； 2.与0异或仍为本身，与1异或则取反，即x^0=x，x^-1=~x 
   * 通过异或可以实现把x统一“变为正数” 
   */
  int mask, k, disp, curHigh16BitsNonZero, curHigh8BitsNonZero, curHigh4BitsNonZero, curHigh2BitsNonZero, curHigh1BitNonZero;

  mask = x>>31; //若x为负数，mask=全1；若x为正数，mask=全0
  x = x ^ mask; //若x为负数则取反；若x为正数则不变
  
  /*
   * 要求k，我们可以用“二分”的想法，不断二分，可以画出决策树来理解
   */
  k = 0;
  curHigh16BitsNonZero = !!(x>>16);
  disp = curHigh16BitsNonZero << 4; //位移量，若k>16，disp=16；否则disp=0
  k = k + disp;  //若k>16，k先加16；否则k不变
  x = x >> disp;   //“框住”x的“未被确认”的那16位

  //后面同理
  curHigh8BitsNonZero = !!(x>>8);
  disp = curHigh8BitsNonZero << 3;
  k = k + disp;
  x = x >> disp;

  curHigh4BitsNonZero = !!(x>>4);
  disp = curHigh4BitsNonZero << 2;
  k = k + disp;
  x = x >> disp;

  curHigh2BitsNonZero = !!(x>>2);
  disp = curHigh2BitsNonZero << 1;
  k = k + disp;
  x = x >> disp;

  curHigh1BitNonZero = !!(x>>1);
  disp = curHigh1BitNonZero;
  k = k + disp;
  x = x >> disp;

  k = k + x;    //容易被遗忘的一步：若最后还剩的x是1，这个1也需要被计入（即curBitNonZero）

  return k+1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  // 我们要把uf解析出float变量的各个部分
  int s, exp, frac;
  s = uf>>31&1;
  exp = uf>>23&0xff;
  frac = uf&0x7fffff;
  // 针对三种类型进行判断
  // NaN或无穷大
  if(exp == 0xff){
    return uf;
  }
  //非规格化的数，表示0或接近0的数
  else if(exp == 0){
    // 可以直接把exp和frac部分的总体左移1位，就可以实现uf*2，也许它会变为结构化的数
    return s<<31 | uf<<1;
  }
  // 结构化的数
  else{
    // 只需要E+1即可实现乘以2
    return s<<31 | (exp+1)<<23 | frac;
  }
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int s, exp, frac, E, M;
  // 拆解
  s = uf>>31&1;
  exp = uf>>23&0xff;
  frac = uf&0x7fffff;
  // 非规格化的数：绝对值小于1，根据截断规则，返回0
  if(exp == 0){
    return 0;
  }
  // inf和NaN：一定会超出int的表示范围
  else if(exp == 255){
    return 0x80000000u;
  }
  // 规格化的数
  else{
    // 计算规格化的数的E
    E = exp - 127;
    // 超出范围
    if(E > 31){
      return 0x80000000u;
    }
    // E<0能确定|f|<1，这是因为2^E<=0.5，而M<2，故|f|<1
    else if(E < 0){
      return 0;
    }
    else{
      /* 
       * 注意float向int强制转换并不是直接简单地除去frac部分
       * M = 1.(frac), |V| = M * 2^E
       * 理解的关键：M乘以2^E等价于M的小数点右移E位
       * 我们要判断uf的小数点在哪个位置，也就是M*2^E的小数点在哪个位置
       */
      M = 1<<23 | frac; //这里的M实际上是M*2^23
      if(E >= 23){
        M = M << (E-23);  //把小数点少往右移的移过去
      }else{
        M = M >> (23-E);  //把小数点多往右移的移回来
      }
      // 考虑符号位
      if(s){
        M = ~M + 1; //取相反数
      }
      return M;
    }
  }
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  /*
   * 关注float各个分类所能表达的正数V(s=1)的范围  V = M * 2^E
   * 1.规格化的数 1 <= exp <= 254
   *   M = 1.(frac), E = exp - 127, exp∈[1, 254];
   *   M∈[1, 2), E∈[-126, 127];
   *   故V∈[2^{-126}, 2^128)
   * 2.非规格化的数 exp = 0
   *   只考虑frac不全为0时的情况（即正数的情况）
   *   M!=0时，有M∈[2^{-23}, 1),
   *   故V∈[2^{-149}, 2^{-126})
   * 3.NaN或+inf  exp = 255
   */
  int exp, frac;
  // 极小的数
  if(x < -149){
    exp = 0;
    frac = 0;
  }
  // 非规格化的数 V = frac * 2^{-126} = 2^x  =>  frac = 2^(x + 126) = 1 * 2^(x + 126)
  else if(x < -126){
    exp = 0;
    frac = 1 << (x + 126 + 23); //还要左移23是因为保证frac是小数部分的解释方式
  }
  // 规格化的数 V = M * 2^{exp - 127} = 2^x  => x由exp贡献，M = 1，即frac = 0，exp = x + 127
  else if(x < 128){
    frac = 0;
    exp = x + 127;
  }
  // inf 
  else{
    exp = 0xff;
    frac = 0;
  }
   
  return (exp<<23) | frac;
}
