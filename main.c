//padded to the nearest power of two
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint16_t val : 12;
} address;
typedef struct {
  uint8_t val;
} opcode;

typedef struct {
 int64_t val: 40; 
} number;
typedef struct{
  opcode opcode;
  address address;
} instruction;

typedef struct {
  instruction left;
  instruction right;
} instruction_word;

typedef union {
  number number;
  instruction_word instruction;
} word;

number AC;
number MQ;
address PC;
opcode IR;
instruction IBR;
word MBR;
address MAR;

number abs_num(number number) {
  if(number.val < 0) {
    number.val *= -1;
  }
  return number;
}
void mul(number an, number bn) {
  uint64_t a = an.val;
  uint64_t b = bn.val;

  uint64_t p = a >> 20;
  uint64_t q = a & 0xFFFFF;

  uint64_t r = b >> 20;
  uint64_t s = b & 0xFFFFF;

  uint64_t u = p * r;
  uint64_t v = (q - p) * (s - r);
  uint64_t w = q * s;
  uint64_t x = u + w - v;

  AC.val = u;

  uint64_t y = x >> 20;
  uint64_t z = x & 0xFFFFF;

  uint64_t c = w >> 20;
  uint64_t d = w & 0xFFFFF;

  uint64_t e = c + z;
  uint64_t f = e >> 20;
  uint64_t g = e & 0xFFFFF;

  AC.val += y + f;
  MQ.val = d + (g << 20);
}

int main() {
  word memory[2 << 12] = {0};

  PC.val = 0;

  uint8_t is_next_in_IBR = 0;

  uint8_t flag = 1;
  while(flag) {
    if(is_next_in_IBR) {
      IR = IBR.opcode;
      MAR = IBR.address;
      is_next_in_IBR = 0;
      PC.val++;
    } else {
      MAR = PC;
      MBR = memory[MAR.val];
      IBR = MBR.instruction.right;
      IR = MBR.instruction.left.opcode;
      MAR = MBR.instruction.left.address;
      is_next_in_IBR = 1;
    }

    switch (IR.val) {
      case 0:
        flag = 0;
        break;
      case 1:
        AC = memory[MAR.val].number;
        break;   
      case 2:
        AC.val = -memory[MAR.val].number.val;
        break;
      case 3:
        AC = abs_num(memory[MAR.val].number);
        break;
      case 4:
        AC.val = -abs_num(memory[MAR.val].number).val;
        break;
      case 5:
        AC.val += memory[MAR.val].number.val;
        break;
      case 6:
        AC.val += abs_num(memory[MAR.val].number).val;
        break;
      case 7:
        AC.val -= memory[MAR.val].number.val;
        break;
      case 8:
        AC.val -= abs_num(memory[MAR.val].number).val;
        break;
      case 9:
        MQ = memory[MAR.val].number;
        break;
      case 10:
        AC = MQ;
        break;
      case 11:
        mul(AC, memory[MAR.val].number);
        break;
      case 12:
        MQ.val = AC.val / memory[MAR.val].number.val;
        AC.val %= memory[MAR.val].number.val;
        break;
      case 13:
        PC.val = MAR.val;
        is_next_in_IBR = 0;
        break;
      case 14:
        PC.val = MAR.val;
        is_next_in_IBR = 1;
        break;
      case 15:
        if(AC.val >= 0) {
          PC.val = MAR.val;
          is_next_in_IBR = 0;
        }
        break;
      case 16:
        if(AC.val >= 0) {
          PC.val = MAR.val;
          is_next_in_IBR = 1;
        }
        break;
      case 18:
        memory[MAR.val].instruction.left.address.val = AC.val & 8191;
        break;
      case 19:
        memory[MAR.val].instruction.right.address.val = AC.val & 8191;
        break;
      case 20:
        AC.val <<= 2;
        break;
      case 21:
        AC.val >>= 2;
        break;
      case 33:
        memory[MAR.val].number.val = AC.val;
        break;
      default:
        break;
    }
  }
}
