#pragma once

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <chrono>

// Included to get access to colors inside the console
#ifdef _WIN32
    #include <windows.h>   
#endif 

/*!
 * @brief      Provides a framework to make test driven development easier
 * @details    Use the TEST macro to define your testing functions. 
 * The framework use this macro to register your tests inside the 
 * @ref _test_functions variable.
 *    
 * After defining a test function through the TEST macro,use the CHECK, CHECK_EQ, CHECK_NE
 * on conditions that can fail. If a check fails, an error will be automatically
 * notified and logged in the console through std::cerr
 *
 * To launch all the register test functions, use the @ref start function
*/
namespace corgi { namespace test {

    namespace detail
    {
        inline void log_failed_functions();
    }
    
/*!
 * @brief Base class used by user fixtures
 */
class Test
{

    friend void detail::log_failed_functions();

    friend Test* 
        
        init_fixture
        (
            Test* i,
            const std::string& class_name,
            const std::string& test_name
        );

    friend int run_all();
    friend void run_fixtures();

public:

    /*!
     * @brief Override this function to initialize the fixture resources
     */
    virtual void set_up(){}

    /*!
     * @brief Override this function to release the fixture resources
     */
    virtual void tear_down(){}

    virtual ~Test()=default;


private:

    std::string _class_name;
    std::string _test_name;

    /*!
     * @brief Overriden by the TEST_F macro
     */
    virtual void run(){}
};

template<class T>
class Equals 
{
public:

    Equals(const T v1) : _val1(v1){}

    template<class U>
    bool run( U val2)
    {
        return this->_val1 == val2;
    }

    T _val1;
};

template<class T>
class NonEquals 
{
public:
    
    NonEquals( T v1) : _val1(v1){}

    template<class U>
    bool run( U val2)
    {
        return this->_val1 != val2;
    }

    T _val1;
};

template<class T>
class AlmostEquals 
{
public:

    AlmostEquals( T v1, T precision) : _val1(v1), precision(precision){}
    bool run( T val2) 
    {
        return (this->_val1> (val2 - precision)) && (this->_val1 < (val2 + precision));
    }
    T precision;
    T _val1;
};


// Without this function I would have to write Equals<int>(4) instead of equals(4)
template<class T>
Equals<T>* equals(T val)
{
    return new Equals<T>(val);
}

// Without this function I would have to write NonEquals<int>(4) instead of equals(4)
template<class T>
NonEquals<T>* non_equals(T val)
{
    return new NonEquals<T>(val);
}

template<class T>
AlmostEquals<T>* almost_equals(T val, T precision)
{
    return new AlmostEquals<T>(val, precision);
}

namespace detail 
{
    // Typedef/Using/Classes
    using TestFunctionPointer  = void(*)( );

    class TestFunction
    {
    public:

        TestFunction(TestFunctionPointer ptr,
                        const std::string& name,
                        const std::string& group)
        {
            this->pointer   = ptr;
            this->name      = name;
            this->group     = group;
        }

        TestFunctionPointer pointer;
        std::string         name;
        std::string         group;
        std::string         file;
        int                 line;
    };


    using FunctionList = std::vector<TestFunction>;

// Variables 

    inline unsigned short get_path_color()     { return 6; }
    inline unsigned short get_value_color()    { return 5; }
    inline unsigned short get_error_color()    { return 4; }
    inline unsigned short get_log_color()      { return 3; }
    inline unsigned short get_success_color()  { return 2; }

    /*!
        @brief  Returns how many error has been reported by the tests
    */
    inline int& get_error()
    {
        static int error; 
        return error;
    }

    /*!
        @brief  Returns the original of failed test functions
    */
    inline std::vector<TestFunction>& get_failed_functions()
    {
        static std::vector<TestFunction> failed_functions;
        return failed_functions;
    }

    static void set_console_color(unsigned short color)
    {
        #ifdef _WIN32
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, color);  
        #endif
        std::cout<<"\033[1;33m";
    }
            
template<class T>
void log_test_error(const T val,
               const std::string& value_name,
               const std::string& expected_name,
               const char* file,
               const char* function,
               int line)
{
    set_console_color(get_error_color());
    std::cerr << "        ! Error :\n ";
    set_console_color(get_log_color());
    std::cout << "           * file :     ";

    set_console_color(get_path_color());
    std::cerr << file << "\n";

    set_console_color(get_log_color());
    std::cerr << "            * line :     ";

    set_console_color(get_value_color());
    std::cerr << line << "\n";

    set_console_color(get_log_color());
    std::cout << "            * Check if ";
    set_console_color(get_path_color());
    std::cout << "\"" << value_name << "\" ";
    set_console_color(get_log_color());
    std::cout << "== ";
    set_console_color(get_path_color());
    std::cout << "\"" << expected_name << "\"\n";
    set_console_color(get_path_color());

    set_console_color(get_log_color());
    std::cerr << "                * Expected : ";

    set_console_color(get_value_color());
    std::cerr << expected_name << std::endl;

    set_console_color(get_log_color());
    std::cerr << "                * Value is : ";

    set_console_color(get_value_color());
    std::cerr << val << std::endl << std::endl;
    get_error() += 1;
}

/*
 * @brief 
 */
template<class T, class U>
void assert_that_(  T val, 
                    U checker,
                    const std::string& value_name,
                    const std::string& expected_name,
                    const char* file,
                    const char* ff,
                    int line)
{
    if (checker->run(val) == false)
    {
        log_test_error(val, value_name, expected_name, file, ff, line);
    }
    delete checker;
}
    /*!
        @brief      Check if @ref value and @ref expected are equals
        @details    Use the == comparison operator. Logs an error if the 
        condition failed and increment the error counter
    */
    template <class T>
    void check_equals(  T value,
                        T expected,
                        const std::string& value_name,
                        const std::string& expected_name,
                        const char* file,
                        const char* ff,
                        int line)
    {
        if (value != expected)
        {
            log_test_error(value, value_name, expected_name, file, ff, line);
        }
    }

    template<class T>
    void check_non_equals(T value, T expected, const char* file, const char* function, int line)
    {
        if (value == expected)
        {
            set_console_color(12); // Red
            detail::set_console_color(11);  // Blue
            std::cerr << "Error in " << file << "::<<"<<function << " at line " << line << ":" << std::endl;
            std::cerr << "Non expeceted : " << expected << std::endl;
            std::cerr << "Value is : " << value << std::endl;
            std::cerr << std::endl;
            get_error() += 1;
        }
    }
            
    /*!
        @brief  Returns a map that link group_name to their original of TestFunction
        @detail Test functions that have something in common can be put inside groups.
        Thus, this map is used to know in which group a test belong.
    */

    inline std::map<std::string, std::vector<TestFunction>> map_test_functions;
    inline std::map<std::string, std::vector<Test*>> fixtures_map;


    inline std::vector<Test*>& get_failed_fixtures()
    {
        static std::vector<Test*> failed_fixtures;
        return failed_fixtures;
    }

    static void cleanup()
    {
        auto& fixtures = fixtures_map;

        for (auto& f : fixtures)
        {
            for (auto* ff : f.second)
            {
                delete ff;
            }
        }
    }

    /*!
        @brief      Register a test function
        Called by the TEST macro.  The TEST macro declares a function 
        that will be named from the combination of the @ref group_name and @ref
        function_name parameters.
        Then, it will create a unique variable whose only goal is to call
        the register_function with a pointer to the previously declared function, along
        with the function name and the group (parameters of the TEST macro)
        Finally, it will start the function definition.
        @param func             Pointer to the test function created by the TEST macro
        @param group_name       First parameter of the TEST macro. Correspond to the group
        in which the function belong
        @param function_name    Second parameter of the TEST macro. Correspond to the 
        function name.
    */
    inline int register_function(
        TestFunctionPointer func_ptr,
        const std::string&  function_name, 
        const std::string&  group_name)
    {
        // We first check if the group doesn't already exist
        if (map_test_functions.find(group_name) == map_test_functions.end())
        {
            map_test_functions.emplace(group_name, std::vector<TestFunction>());
        }
        // Append the function point to the group.
        map_test_functions[group_name].push_back(TestFunction( func_ptr,
                                                                function_name,
                                                                group_name));

        return 0; // We only return a value because of the affectation trick in the macro
    }

    /*!
        @brief register a fixture object
    */
    inline int register_fixture(Test* t, const std::string& fixture_name, const std::string& function_name)
    {
        if (fixtures_map.find(fixture_name) == fixtures_map.end())
        {
            fixtures_map.emplace(fixture_name, std::vector<Test*>());
        }

        fixtures_map[fixture_name].push_back(t);

        return 0; // We only return a value because of the affectation trick in the macro
    }
            
    /*!
        @ brief Just a shortcut so I don't have to write std::cout<< text << "\n" all the time
    */
    inline void write_line(const std::string& str)
    {
        std::cout << str.c_str() << "\n";
    }

    /*!
        @brief  Write a line of text and changes the console color
        @param line         Contains the text to be displayed
        @param color_code   Contains a code corresponding to a color. To know which 
        code to use, you should prefer to use the @ref get_error_color() @ref get_path_color()
        @ref get_log_color() @ref get_success_color() and @ get_value_color() 
        to get the code with the color you want.
    */
    inline void write_line(const std::string& line, unsigned short color_code)
    {
        set_console_color(color_code);
        write_line(line);
        std::cout<<"\033[0m";
    }

    /*!
        @brief  Logs every failed function inside the console as a summary report
    */
    inline void log_failed_functions()
    {
        for (const auto& t : detail::get_failed_functions())
        {
            set_console_color(get_error_color());
            std::cout << "      * Function ";
            set_console_color(get_path_color());
            std::cout << t.group << "::" << t.name;
            set_console_color(get_error_color());
            std::cout << " failed \n";
        }

        for(const auto& t : detail::get_failed_fixtures())
        {
            set_console_color(get_path_color());
            std::cout << "      * ";
            std::cout << t->_class_name << "::" << t->_test_name;
            set_console_color(get_error_color());
            std::cout << " failed \n";
        }
    }        

    /*!
        @brief Just log that we start the tests
    */
    inline void log_start()
    {
        write_line("RUN ALL THE THINGS!!!", get_success_color());
    }

    /*!
        @brief  Logs that we successfully pass every test
    */
    inline void log_success()
    {
        write_line("    * Every test passed",get_success_color());
    }

    /*!
        @brief  Logs that at least 1 test failed
    */
    inline void log_failure()
    {
        write_line("    Error : Some test failed to pass", get_error_color());
        write_line("    Logging the Functions that failed");
        log_failed_functions();
    }

    /*!
        @brief  Write an header in the console
        @detail Just write something in that way inside the console
        ***********
        *   Text  *  
        ***********
    */
    inline void write_title(const std::string& text)
    {
        const int max_column = 78;

        std::string line;
        for (int i = 0; i < max_column; ++i)
            line += '*';

        write_line(line, get_log_color());

        std::string title = "*    "+text;

        title.append(max_column-1 - title.size(), ' ');
        title += ( '*' );
        write_line(title);
        write_line(line);
    }

    /*!
        @brief  Logs that we're running tests that belongs to the @ref group_name group
        @param  group_name  Name of the tested group 
        @param  group_size  How many test are inside the group
    */
    inline void log_start_group(const std::string& group_name, int group_size)
    {
        write_title("Running " + std::to_string(group_size) + " tests grouped in " + group_name);
    }

    /*!
     * @brief Log that we're starting a test
     * @param test_name    Name of the test we're running
     * @param group_name   Name of the group the test belongs to
     * @param count        Counter used to know on which test from group we're (like
     * the first one, the second one ? etc
     */
    inline void log_start_test( const std::string& test_name,
                                const std::string& group_name,
                                int group_size, 
                                int count)
    {
        set_console_color(get_log_color());
        std::cout << "  * Running ";
        set_console_color(get_path_color());
        std::cout << group_name << "." << test_name;
        set_console_color(get_log_color());
        std::cout << " (" << count << "/" << group_size << ") \n";
    }

    /*!
     * @brief  Log that a test was successful
     */
    inline void log_test_success(long long time)
    {
        set_console_color(get_success_color());
        std::cout.precision(3);
        double d = static_cast<double>(time) / 1000.0;
        std::cout << "       Passed in " <<std::fixed<< d << " ms \n";
    }

    /*!
     * @brief  Logs all the results
     */
    inline void log_results()
    {
        write_title("Results");
        // Log success or failure depending on the error count
        ( detail::get_error() == 0 ) ? detail::log_success() : detail::log_failure();
    }
}

inline void run_fixtures()
{
    for (auto test : detail::fixtures_map)
    {
        int count = 1;

        detail::log_start_group(test.first, detail::fixtures_map[test.first].size());

        for (auto test_object : test.second)
        {
            // This is trash, I would like my test function to return a value directly
            // I'll have to check If I can do some macro magic to do that
            int error_value = detail::get_error();

            detail::log_start_test
            (
                test_object->_test_name,
                test_object->_class_name,
                detail::fixtures_map[test_object->_class_name].size(),
                count++
            );

            test_object->set_up();
            const auto timer = std::chrono::high_resolution_clock::now();
            test_object->run();
            const auto end_timer = std::chrono::high_resolution_clock::now();
            const long long time = std::chrono::duration_cast<std::chrono::microseconds>(end_timer - timer).count();
            test_object->tear_down();

            if (error_value == detail::get_error())
            {
                detail::log_test_success(time);
            }
            else
            {
                //detail::get_failed_fixtures().push_back(test_object);
            }
        }
    }
}

inline void run_functions()
{
    for (auto test : detail::map_test_functions)
    {
        // test count
        int count = 1;

        detail::log_start_group(test.first, detail::map_test_functions[test.first].size());

        for (auto test_function : test.second)
        {
            // This is trash, I would like my test function to return a value directly
            // I'll have to check If I can do some macro magic to do that
            int error_value = detail::get_error();

            detail::log_start_test
            (
                test_function.name,
                test_function.group,
                detail::map_test_functions[test_function.group].size(),
                count++
            );

            auto timer = std::chrono::high_resolution_clock::now();
            // Runs the function
            test_function.pointer();

            auto end_timer = std::chrono::high_resolution_clock::now();
            long long time = std::chrono::duration_cast<std::chrono::microseconds>(end_timer - timer).count();

            if (error_value == detail::get_error())
            {
                detail::log_test_success(time);
            }
            else
            {
                detail::get_failed_functions().push_back(test_function);
            }
        }
    }
}
/*!
    @brief      Run all the tests defined by the user 
    @details    Must be called from main. Will fire all the test the user defined
    with the TEST macro. Warning, this function returns a value that must be returned
    inside the main function!
*/
inline int run_all()
{
    detail::log_start();
    detail::get_error() = 0;
    run_fixtures();
    run_functions();
    detail::log_results();

    detail::cleanup();

    // A test run by ctest must return 0 to pass
    return detail::get_error();
}

inline Test* init_fixture(Test* t, const std::string& class_name, const std::string& test_name )
{
    t->_class_name = class_name;
    t->_test_name  = test_name;
    return t;
}

/*!
 *   @brief  Define a new Fixture
 *
 *   A fixture is simply a way to organize tests around a class.
 *   To create a fixture, the user first needs to create a class
 *   that inherits from @ref corgi::test::Test.
 *   This class comes with 2 virtual methods the user needs to override :
 *   set_up and tear_down.
 *   In set_up, the user is expected to initialize the data that will be
 *   used by all the tests of the current feature.
 *   In tear_down, the user is expected to clean the data used by the test.
 *
 *   Once this is done, to add a test to the fixture, the user should use
 *   the TEST_F macro, and define the test. What the framework will do is
 *   call the set_up function, then the code defined inside the TEST_F macro,
 *   and then the tear_down function.
 *
 *   Under the hood, the TEST_F macro will create a new class that inherits from
 *   @ref class_name, declare and define the virtual run method, and register
 *   itself to the framework
 */
#define TEST_F(class_name, test_name)                                                         \
class class_name##test_name : public class_name{ public : void run()override;};                     \
static int var##class_name##test_name = corgi::test::detail::register_fixture(init_fixture(new class_name##test_name(),#class_name, #test_name),#class_name,#test_name ); \
void class_name##test_name::run()

/*!
    @brief      Replace the TEST macro with a function registered by the framework
    @details    Ok so the trick is a little bit dirty, and the parameter should
    actually not be a string for it to works. The idea is to create an useless variable
    so I can call the register function. 
*/
#define TEST(group_name,function_name)\
void group_name##_##function_name();   \
static int var##group_name##function_name = corgi::test::detail::register_function(&group_name##_##function_name, #function_name,#group_name); \
void group_name##_##function_name()


#define ASSERT_EQ(value, expected)   corgi::test::detail::check_equals(value, expected, #value, #expected, __FILE__, __func__, __LINE__);
#define ASSERT_NE(value, expected)   corgi::test::detail::check_non_equals(value, expected, __FILE__, __func__, __LINE__);

#define assert_that(value, expected)  \
    corgi::test::detail::assert_that_(value, expected, #value, #expected, __FILE__, __func__, __LINE__);
}}