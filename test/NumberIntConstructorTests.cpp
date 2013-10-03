#include "fixed/Number.h"
#include "TestsCommon.h"

#include <iostream>
#include <vector>
#include <unordered_map>

namespace fixed {
namespace test {

template <typename T>
class ConstructorTest {
  public:
    ConstructorTest (
        T intVal,
        uint64_t fracVal,
        uint8_t dp,
        bool negativeFlag,
        const std::string& strVal,
        const bool expectedVal64Set
    )
      : intVal_(intVal),
        fracVal_(fracVal),
        dp_(dp),
        negativeFlag_(negativeFlag),
        expectedNegative_(negativeFlag),
        strVal_(strVal),
        expectedVal64Set_ (expectedVal64Set),
        expectedIntVal_ (0),
        doIntConstructor_ (false)
    {
        if (std::is_same<T, int64_t>::value &&
            (intVal == std::numeric_limits<int64_t>::min ()))
        {
            expectedIntVal_ = std::numeric_limits<int64_t>::max () + 1;
            expectedNegative_ = true;
        }
        else if (std::is_signed<T>::value && (intVal < 0))
        {
            expectedIntVal_ = static_cast<uint64_t> (0 - intVal);
            expectedNegative_ = true;
        }
        else
        {
            expectedIntVal_ = static_cast<uint64_t> (intVal);
        }

        //
        // Means it was called with the defaults, try calling the Number
        // constructor with just the intval set, so it too using the defaults
        //
        doIntConstructor_ = fracVal == 0 && dp == 0 && ! negativeFlag;
    }

    bool operator() ()
    {
        //
        // Note, since we're doing constructor tests, must use the full version
        // of checkNumber here, not the shortened version that takes a
        // reference number object.  The other Number tests can use that
        // version, however here we must ensure the constructors themselves
        // are constructing the number properly, then it's valid for those
        // other tests to use the shortened version.
        //
        return
            (! doIntConstructor_ || checkNumber (
                "Int Constructor ",
                Number (intVal_),
                strVal_,
                expectedIntVal_,
                fracVal_,
                dp_,
                expectedNegative_,
                expectedVal64Set_
            ))
            &&
            checkNumber (
                "Full Constructor ",
                Number (intVal_, fracVal_, dp_, negativeFlag_),
                strVal_,
                expectedIntVal_,
                fracVal_,
                dp_,
                expectedNegative_,
                expectedVal64Set_
            )
            &&
            checkNumber (
                "String Constructor ",
                Number (strVal_),
                strVal_,
                expectedIntVal_,
                fracVal_,
                dp_,
                expectedNegative_,
                expectedVal64Set_
            )
        ;
    }

  private:
    T intVal_;
    uint64_t fracVal_;
    unsigned int dp_;
    bool negativeFlag_;
    bool expectedNegative_;
    std::string strVal_;
    uint64_t expectedVal64Set_;
    uint64_t expectedIntVal_;
    bool doIntConstructor_;
};

template <typename T>
constexpr ConstructorTest<T> createConstructorTest (
    const std::string& strVal,
    const T intVal,
    const uint64_t fracVal,
    const uint8_t dp,
    const bool negativeFlag,
    const bool val64Set
)
{
    return ConstructorTest<T> (
        intVal, fracVal, dp, negativeFlag, strVal, val64Set
    );
}

template <typename T>
constexpr Test createTest (
    const std::string& strVal,
    const T intVal,
    const uint64_t fracVal,
    const uint8_t dp,
    const bool negativeFlag,
    const bool val64Set
)
{
    return Test (
        createConstructorTest (
            strVal, intVal, fracVal, dp, negativeFlag, val64Set
        ),
        TestName (strVal)
    );
}

static const bool NEG = true;
static const bool POS = ! NEG;

static const bool V64 = true;
static const bool V128 = ! V64;

template <typename T>
constexpr Test createTest (
    const std::string& strVal,
    const T intVal,
    const bool val64Set
)
{
    return createTest (strVal, intVal, 0, 0, POS, val64Set);
}

std::vector<Test> NumberIntConstructorTestVec = {
    createTest ("0", 0, V64),
    createTest ("1", 1, V64),
    createTest ("-1", -1, V64),
    createTest ("2147483647", 2147483647, V64), // 2^31 - 1
    createTest ("-2147483647", -2147483647, V64), // -(2^31 - 1)
    createTest ("2147483648", 2147483648, V64), // 2^31"},
    createTest ("-2147483648", -2147483648, V64), // -2^31
    createTest ("4294967295", 4294967295, V64), // 2^32 - 1
    createTest ("-4294967295", -4294967295, V64), // -(2^32 - 1)
    createTest ("4294967296", 4294967296, V64), // 2^32,
    createTest ("-4294967296", -4294967296, V64), // -2^32
    createTest ("9223372036854775807", 9223372036854775807, V64), // 2^63-1
    createTest ("9223372036854775807", 9223372036854775807ULL, V64),
    createTest ("-9223372036854775807", -9223372036854775807, V64),// -(2^63-1)

    createTest ("-1", 1, 0, 0, NEG, V64),
    createTest ("-2147483648", 2147483648, 0, 0, NEG, V64), // -2^31
    createTest ("-4294967296", 4294967296, 0, 0, NEG, V64), // -2^32

    createTest ("0.0", 0, 0, 1, POS, V64),
    createTest ("1.0", 1, 0, 1, POS, V64),
    createTest ("-1.0", 1, 0, 1, NEG, V64),
    createTest ("0.00", 0, 0, 2, POS, V64),
    createTest ("1.00", 1, 0, 2, POS, V64),
    createTest ("-1.00", 1, 0, 2, NEG, V64),
    createTest ("0.01", 0, 1, 2, POS, V64),
    createTest ("-0.01", 0, 1, 2, NEG, V64),
    createTest ("1.01", 1, 1, 2, POS, V64),
    createTest ("-1.01", 1, 1, 2, NEG, V64),
    createTest ("0.23456", 0, 23456, 5, POS, V64),
    createTest ("-0.23456", 0, 23456, 5, NEG, V64),
    createTest ("1.23456", 1, 23456, 5, POS, V64),
    createTest ("-1.23456", 1, 23456, 5, NEG, V64),

    createTest ( // Max val
        "9223372036854775807.99999999999999",
        9223372036854775807,
        99999999999999,
        14,
        POS,
        V128
    ),

    createTest ( // Min Val
        "-9223372036854775807.99999999999999",
        9223372036854775807ULL,
        99999999999999,
        14,
        NEG,
        V128
    ),

    //
    // These test check the override on negative works, ie if we pass in
    // a negative value for the integer, the value of the negative flag should
    // be ignored by Number
    //
    createTest ("-1", -1, 0, 0, POS, V64),
    createTest ("-1.1", -1, 1, 1, POS, V64),

    //
    // A few more tests for V64 flag
    //
    createTest ("92233.72036854775807", 92233, 72036854775807, 14, POS, V64),
    createTest ("92233.72036854775808", 92233, 72036854775808, 14, POS, V128),
    createTest ("922337.2036854775807", 922337, 2036854775807, 13, POS, V64),
    createTest ("922337.2036854775808", 922337, 2036854775808, 13, POS, V128),
    createTest ("922337203685477580.7", 922337203685477580, 7, 1, POS, V64),
    createTest ("922337203685477580.8", 922337203685477580, 8, 1, POS, V128),
    createTest ("922337203685477581.7", 922337203685477581, 7, 1, POS, V128),
    createTest ("-92233.72036854775807", 92233, 72036854775807, 14, NEG, V64),
    createTest ("-92233.72036854775808", 92233, 72036854775808, 14, NEG, V128),
    createTest ("-92233.72036854775809", 92233, 72036854775809, 14, NEG, V128),
    createTest ("-922337.2036854775807", 922337, 2036854775807, 13, NEG, V64),
    createTest ("-922337.2036854775808", 922337, 2036854775808, 13, NEG, V128),
    createTest ("-922337.2036854775809", 922337, 2036854775809, 13, NEG, V128),
    createTest ("-922337203685477580.7", 922337203685477580, 7, 1, NEG, V64),
    createTest ("-922337203685477580.8", 922337203685477580, 8, 1, NEG, V128),
    createTest ("-922337203685477580.9", 922337203685477580, 9, 1, NEG, V128),
    createTest ("-922337203685477581.7", 922337203685477581, 7, 1, NEG, V128),

    //
    // Ensure the library won't downsize the precision on extra zeros
    //
    createTest ("-922337203685477580.7", 922337203685477580, 7, 1, NEG, V64),
    createTest (
        "-922337203685477580.70", 922337203685477580, 70, 2, NEG, V128
    )
};

} // namespace test
} // namespace fixed
