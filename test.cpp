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

class Foo
{
    public:
        Foo(size_t aSize)
            : mSize(aSize), mBuffer(new char[mSize])
        {
            for (size_t i = 0; i < aSize; i++)
            {
                *(mBuffer + i) = std::rand() & 0x7f;
            }
        }

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

        Foo(Foo &&aOther) // move ctor
            : mSize(aOther.mSize), mBuffer(aOther.mBuffer)
        {
            aOther.mSize = 0;
            aOther.mBuffer = nullptr;
            gMoveCtorCnt++;
        }

        Foo &operator=(Foo &&aRhs) // move assign ctor
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
        FooNomove(size_t aSize)
            : mSize(aSize), mBuffer(new char[mSize])
        {
            for (size_t i = 0; i < aSize; i++)
            {
                *(mBuffer + i) = std::rand() & 0x7f;
            }
        }

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
    fprintf(stderr, "Usage: test LOOPCNT OBJSIZE COPY/MOVE\n");
    exit(1);
}

static std::vector<FooNomove> copyTest(int32_t aLoopCnt, size_t aObjSize)
{
    int32_t i = 0;

    std::vector<FooNomove> sVector;;

    while (i++ < aLoopCnt)
    {
        FooNomove sFoo(aObjSize);

        sVector.push_back(sFoo);
    }

    return sVector;
}

static std::vector<Foo> moveTest(int32_t aLoopCnt, size_t aObjSize)
{
    int32_t i = 0;

    std::vector<Foo> sVector;;

    while (i++ < aLoopCnt)
    {
        sVector.push_back(Foo(aObjSize));
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

int32_t main(int32_t argc, char *argv[])
{
    std::srand(std::time(0));

    if (argc != 4)
    {
        printUsageAndExit();
    }

    int32_t sLoopCnt = std::strtol(argv[1], NULL, 10);
    size_t sObjSize = std::strtoull(argv[2], NULL, 10);
    std::string sCopyOrMove = argv[3];

    std::vector<FooNomove> sVectorNoMove;
    std::vector<Foo> sVector;

    if (sCopyOrMove == "copy")
    {
        sVectorNoMove = copyTest(sLoopCnt, sObjSize);
        printf("vector size = %zu\n", sVectorNoMove.size());
    }
    else
    {
        if (sCopyOrMove == "move")
        {
            sVector = moveTest(sLoopCnt, sObjSize);
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