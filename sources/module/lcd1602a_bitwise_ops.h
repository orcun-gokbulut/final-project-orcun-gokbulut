#pragma once

#define LCD1602A_BIT_SET(variable, offset) ((variable) = ((variable) | (1U << (offset))))
#define LCD1602A_BIT_SET_VALUE(variable, offset, value) ((variable) = (((variable) & ~(1U << (offset))) | (((value) == 0 ? 0 : 1) << (offset))))
#define LCD1602A_BIT_GET(variable, offset) (((variable) & (1U << (offset))) >> (offset))
#define LCD1602A_BIT_CLEAR(variable, offset) ((variable) = ((variable) & ~(1U << (offset))))
#define LCD1602A_BIT_MASK(size) ((1U << (size)) - 1)

#define LCD1602A_BITFIELD_MASK(offset, size) (LCD1602A_BIT_MASK(size) << (offset))
#define LCD1602A_BITFIELD_SET(variable, offset, size, value) ((variable) = (((variable) & ~(LCD1602A_BITFIELD_MASK((offset), (size)))) | (((value) & LCD1602A_BIT_MASK(size)) << (offset))))
#define LCD1602A_BITFIELD_GET(variable, offset, size) (((variable) >> (offset)) & LCD1602A_BIT_MASK(size))
