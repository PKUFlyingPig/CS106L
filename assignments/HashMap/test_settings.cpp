// Test settings - use this file to change which tests are RUN_TEST_2A

// Use this to run on the test harness
// 0 = run student_main, 1 = run test harness
#define RUN_TEST_HARNESS 1

// Change the 0 to a 1 to run that test
// Note that the tests won't compile until their respective functions
// have a header in the .h, and implementations in the .cpp

// Milestone 1 test cases (starter code) will automatically be run.

// Milestone 2: range constructor (optional)
#define RUN_TEST_2A 1
#define RUN_TEST_2B 1
// Milestone 2: initializer list constructor (optional)
#define RUN_TEST_2C 1
#define RUN_TEST_2D 1

// Milestone 3: operator[]
#define RUN_TEST_3A 1
#define RUN_TEST_3B 1
// Milestone 3: operator<<
#define RUN_TEST_3C 1
#define RUN_TEST_3D 1
// Milestone 3: operator== and operator!=
#define RUN_TEST_3E 1
#define RUN_TEST_3F 1
// Milestone 4: copy operations
#define RUN_TEST_4A 1
#define RUN_TEST_4B 1
#define RUN_TEST_4C 1

// Milestone 4: move operations
// warning: these may pass even if you haven't implemented them
// - before implementing copy or move, 3AB will fail, 3CDEFGH will pass
// - after implementing copy but not move, 3GH will fail, 3ABCDEF will pass
// - after implementing copy & move, all of them should pass (you should aim for this)
#define RUN_TEST_4D 1
#define RUN_TEST_4E 1
#define RUN_TEST_4F 1
#define RUN_TEST_4G 1
#define RUN_TEST_4H 1
// Milestone 5: benchmark (optional)
#define RUN_BENCHMARK 1

