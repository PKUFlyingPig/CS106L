// Runtime assertion check
// Equivalent to CHECK_TRUE

struct CheckTrueAssertionFailure {
    int line;
};

void CHECK_TRUE(bool condition, int line) {
    if (!condition) {
        throw CheckTrueAssertionFailure{line};
    }
}

struct TestCrashesWarning {
    int line;
    std::string test_name;
};

void NOT_YET_FIXED(const std::string& test_name, int line) {
    throw TestCrashesWarning{line, test_name};
}

template <typename T>
int run_test(const T& test, const std::string& test_name) {
    try {
        test();
        std::cout << "Test "  << std::setw(30) << std::left << test_name  << std::right << " PASS " << std::endl;
        return 1;
    } catch (const CheckTrueAssertionFailure& e)  {
        std::cout << "Test "  << std::setw(30) << std::left << test_name << std::right
             << " FAIL: CHECK_TRUE assertion failure at line number " << e.line
             << " in tests.cpp" << std::endl;
    } catch (const TestCrashesWarning& e) {
        std::cout << "Test "  << std::setw(30) << std::left << test_name << std::right
             << " FAIL: NOT_YET_FIXED statement on line " << e.line
             << " in tests.cpp. Remove once you've implemented " << e.test_name << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Test "  << std::setw(30) << std::left << test_name << std::right
             << " FAIL: exception thrown - " << e.what() << std::endl;
    } catch (const std::string& e) {
        std::cout << "Test "  << std::setw(30) << std::left << test_name  << std::right
        << " FAIL with string " << e << std::endl;
    } catch (const char *& e) {
        std::cout << "Test "  << std::setw(30) << std::left << test_name  << std::right
        << " FAIL with string " << e << std::endl;
    }
    return 0;
}