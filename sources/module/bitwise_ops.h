#pragma once

#define BIT_SET(variable, bit) ((variable) = ((variable) | (1U << (bit))))
#define BIT_SET_VALUE(variable, bit, value) BITFIELD_SET(variable, bit, 1, value)
#define BIT_GET(variable, bit) (((variable) & (1U << (bit))) >> (bit))
#define BIT_CLEAR(variable, bit) ((variable) = ((variable) & ~(1U << (bit))))

#define BITFIELD_MASK(offset, size) (((1U << (size)) - 1) << (offset))
#define BITFIELD_SET(variable, offset, size, value) ((variable) = (((variable) & ~(BITFIELD_MASK((offset), (size)))) | ((variable) & BITFIELD_MASK(0, (size)) << (offset))))
#define BITFIELD_GET(variable, offset, size) (((variable) >> (offset)) & ((1U << (size)) - 1))
