#if __cplusplus > 201100L
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include <cstring>

size_t gCopyCtorCnt = 0;
size_t gMoveCtorCnt = 0;

size_t gCopyAssignCnt = 0;
size_t gMoveAssignCnt = 0;

#define INITIALIZER(aMethod) do \
{ \
    switch (aMethod) \
    { \
        case 1: /* memcpy */ \
            std::memcpy(mBuffer, aSrcMem, aSize); \
            break; \
        case 2: /* three */ \
            *(mBuffer) = std::rand() & 0x7f; \
            *(mBuffer+mSize-1) = std::rand() & 0x7f; \
            *(mBuffer+mSize/2) = std::rand() & 0x7f;\
            break; \
        case 3: /* allrand */ \
            initMem(mBuffer, aSize); \
            break; \
        default: \
            abort(); \
    } \
} while (0)

#define CTOR(aClassName) aClassName(size_t aSize, char *aSrcMem, int32_t aMethod) \
    : mSize(aSize), mBuffer(new char[mSize]) \
    { \
        INITIALIZER(aMethod); \
    }

static void initMem(char *aBuffer, size_t aSize)
{
    for (size_t i = 0; i < aSize; i++)
    {
        *(aBuffer + i) = std::rand() & 0x7f;
    }
}

class Foo
{
    public:
        CTOR(Foo);

        ~Foo()
        {
            delete [] mBuffer;
        }

        Foo(const Foo &aOther)  // copy ctor
            : mSize(aOther.mSize), mBuffer(new char[mSize])
        {
            std::memcpy(mBuffer, aOther.mBuffer, mSize);
            gCopyCtorCnt++;
        }

        Foo &operator=(Foo aRhs) // copy assign op
        {
            std::swap(mSize, aRhs.mSize);
            std::swap(mBuffer, aRhs.mBuffer);
            gCopyAssignCnt++;

            return *this;
        }

        Foo(Foo &&aOther) noexcept // move ctor
            : mSize(aOther.mSize), mBuffer(aOther.mBuffer)
        {
            aOther.mSize = 0;
            aOther.mBuffer = nullptr;
            gMoveCtorCnt++;
        }

        Foo &operator=(Foo &&aRhs) noexcept // move assign ctor
        {
            mSize = 0;
            mBuffer = nullptr;

            std::swap(mSize, aRhs.mSize);
            std::swap(mBuffer, aRhs.mBuffer);

            gMoveAssignCnt++;

            return *this;
        }

    private:
        size_t mSize;
        char *mBuffer;
};

class FooNomove
{
    public:
        CTOR(FooNomove);

        ~FooNomove()
        {
            delete [] mBuffer;
        }

        FooNomove(const FooNomove &aOther)  // copy ctor
            : mSize(aOther.mSize), mBuffer(new char[mSize])
        {
            std::memcpy(mBuffer, aOther.mBuffer, mSize);
            gCopyCtorCnt++;
        }

        FooNomove &operator=(FooNomove aRhs) // copy assign op
        {
            std::swap(mSize, aRhs.mSize);
            std::swap(mBuffer, aRhs.mBuffer);
            gCopyAssignCnt++;

            return *this;
        }

        FooNomove(FooNomove &&aOther) = delete; // move ctor
        FooNomove &operator=(FooNomove &&aRhs) = delete; // move assign op

    private:
        size_t mSize;
        char *mBuffer;
};

static void printUsageAndExit(void)
{
    fprintf(stderr, "Usage: test LOOPCNT OBJSIZE MEMCPY/THREE/ALLRAND COPY/MOVE RESERVE/NORESERVE\n");
    exit(1);
}

static std::vector<FooNomove> copyTest(int32_t aLoopCnt, size_t aObjSize, char *aSrcMem, int32_t aInitMethod, bool aUseReserve)
{
    int32_t i = 0;

    std::vector<FooNomove> sVector;;

    if (aUseReserve == true)
    {
        sVector.reserve(aObjSize);
    }

    while (i++ < aLoopCnt)
    {
        FooNomove sFoo(aObjSize, aSrcMem, aInitMethod);

        sVector.emplace_back(sFoo);
    }

    return sVector;
}

static std::vector<Foo> moveTest(int32_t aLoopCnt, size_t aObjSize, char *aSrcMem, int32_t aInitMethod, bool aUseReserve)
{
    int32_t i = 0;

    std::vector<Foo> sVector;;

    if (aUseReserve == true)
    {
        sVector.reserve(aObjSize);
    }

    while (i++ < aLoopCnt)
    {
        Foo sFoo(aObjSize, aSrcMem, aInitMethod);

        sVector.emplace_back(std::move(sFoo));
    }

    return sVector;
}

static void checkCalls(void)
{
    printf("gCopyCtorCnt = %zu\n", gCopyCtorCnt);
    printf("gMoveCtorCnt = %zu\n", gMoveCtorCnt);

    printf("gCopyAssignCnt = %zu\n", gCopyAssignCnt);
    printf("gMoveAssignCnt = %zu\n", gMoveAssignCnt);
}

static int32_t getMethodNumber(const char *aUserInput)
{
    std::string sInput(aUserInput);

    if (sInput == "memcpy")
    {
        return 1;
    }
    else if (sInput == "three")
    {
        return 2;
    }
    else if (sInput == "allrand")
    {
        return 3;
    }
    else
    {
        return -1;
    }
}

int32_t main(int32_t argc, char *argv[])
{
    std::srand(std::time(0));

    if (argc != 6)
    {
        printUsageAndExit();
    }

    int32_t sLoopCnt = std::strtol(argv[1], NULL, 10);
    size_t sObjSize = std::strtoull(argv[2], NULL, 10);
    int32_t sInitMethod = getMethodNumber(argv[3]);
    if (sInitMethod < 0)
    {
        printUsageAndExit();
    }

    std::string sCopyOrMove = argv[4];

    std::unique_ptr<char []> sSrcMem = std::make_unique<char []>(sObjSize);
    initMem(sSrcMem.get(), sObjSize);

    std::string sReserveNoReserve = argv[5];
    bool sUseReserve;
    if (sReserveNoReserve == "reserve")
    {
        sUseReserve = true;
    }
    else if (sReserveNoReserve == "noreserve")
    {
        sUseReserve = false;
    }
    else
    {
        printUsageAndExit();
    }

    if (sCopyOrMove == "copy")
    {
        std::vector<FooNomove> sVector;
        sVector = copyTest(sLoopCnt, sObjSize, sSrcMem.get(), sInitMethod, sUseReserve);
        printf("vector size = %zu\n", sVector.size());
    }
    else
    {
        if (sCopyOrMove == "move")
        {
            std::vector<Foo> sVector;
            sVector = moveTest(sLoopCnt, sObjSize, sSrcMem.get(), sInitMethod, sUseReserve);
            printf("vector size = %zu\n", sVector.size());
        }
        else
        {
            printUsageAndExit();
        }
    }

    checkCalls();

    return 0;
}
