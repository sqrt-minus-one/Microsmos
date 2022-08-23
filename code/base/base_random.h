/* date = May 15th 2022 1:22 pm */

#ifndef BASE_RANDOM_H
#define BASE_RANDOM_H

typedef struct random_generator random_generator;
struct random_generator
{
    u32 RandomNumber;
    u32 A;
    u32 C;
    u32 M;
};

internal random_generator MakeRandomGenerator(u32 Seed, u32 A, u32 C, u32 M);
internal random_generator MakeLinearRandomGenerator(u32 Seed);
internal u32      NextRandomNumber(random_generator *RandomGenerator);
internal u32      NextRandomNumberMinMax(random_generator *RandomGenerator, u32 min, u32 max);
// NOTE(fakhri): get normalized random float number
internal r32      NextRandomNumberNF(random_generator *RandomGenerator);
internal string8  CreateBase64StringNotInSet(m_arena *Arena, random_generator *RandomGenerator, hashtable *Blacklist, u32 StringLength);
#endif //BASE_RANDOM_H