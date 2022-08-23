
internal random_generator 
MakeRandomGenerator(u32 Seed, u32 A, u32 C, u32 M)
{
    random_generator Result;
    Result.RandomNumber = Seed;
    Result.A = A;
    Result.C = C;
    Result.M = M;
    NextRandomNumber(&Result);
    return Result;
}

internal random_generator
MakeLinearRandomGenerator(u32 Seed)
{
    // NOTE(fakhri): linear congruential generator
    // see https://en.wikipedia.org/wiki/Linear_congruential_generator
    // paramters for the glibc used by GCC
    u32 A = 1103515245;
    u32 C = 12345;
    u32 M = 1u << 31;
    random_generator Result = MakeRandomGenerator(Seed, A, C, M);
    return Result;
}

internal u32
NextRandomNumberMinMax(random_generator *RandomGenerator, u32 min, u32 max)
{
    u32 RandomNumber = NextRandomNumber(RandomGenerator);
    u32 Result = min + RandomNumber / (RandomGenerator->M / (max - min) + 1);
    return Result;
}

internal u32 
NextRandomNumber(random_generator *RandomGenerator)
{
    RandomGenerator->RandomNumber = (RandomGenerator->A * RandomGenerator->RandomNumber + RandomGenerator->C) % RandomGenerator->M; 
    u32 Result = RandomGenerator->RandomNumber;
    return Result;
}

internal r32
NextRandomNumberNF(random_generator *RandomGenerator)
{
    u32 RandomNumber = NextRandomNumber(RandomGenerator);
    r32 NormalizedRealNumber = (r32)RandomNumber / (r32)RandomGenerator->M;
    return NormalizedRealNumber;
}

internal string8
CreateBase64StringNotInSet(m_arena *Arena, random_generator *RandomGenerator, hashtable *Blacklist, u32 StringLength)
{
    string8 Result;
    Result.size = StringLength;
    Result.str = PushArray(Arena, u8, Result.size);
    
    // NOTE(fakhri): make a base64 string of StringLength digits
    for(u32 Index = 0;
        Index < StringLength;
        ++Index)
    {
        u32 RandomNumber = NextRandomNumberMinMax(RandomGenerator, 0, 64);
        Result.str[Index] = Base64Encoding[RandomNumber];
    }
    
    // NOTE(fakhri): see if it already exist in the blacklist
    hashtable_key Key = MakeHashtableKey(Result);
    Assert(!Hashtable_Find(Blacklist, Key));
    return Result;
}