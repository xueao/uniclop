#include <boost/test/included/unit_test.hpp>

#include <aligned.hpp>

using boost::unit_test::test_suite;
using namespace boost::gil;

//g++ aligned.tests.cpp -I /home/scott/project/boost_1_36_0 -I .

static const struct
{
    int  	 width;
    int  	 height;
    int  	 bytes_per_pixel; /* 3:RGB, 4:RGBA */
    char 	 pixel_data[16 * 16 * 3 + 1];
} gimp_image =
{
    16, 16, 3,
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\277\277\277\177\177\177"
    "\0\0\0\0\0\0\0\0\0\177\177\177\277\277\277\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\277\277"
    "\277\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\277\277\277\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\277\277\277\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\277\277\277\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\177\177\177\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\177\177\177\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\177\177\177\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\177\177\177\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\277\277\277\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\277\277\277\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\277\277\277\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\277\277\277\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\277\277"
    "\277\177\177\177\0\0\0\0\0\0\0\0\0\177\177\177\277\277\277\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377",
};

struct test1
{
    void operator()()
    {
        int width = 200;
        int height = 200;
        unsigned char* buffer = new unsigned char[width * height * 3];
        memset(buffer, 255, width * height * 3);
        rgb8_view_t view = interleaved_view(width,height,(rgb8_pixel_t*)buffer,width*3);

        rgb8_view_t view2 = interleaved_view(gimp_image.width,gimp_image.height,
                                             (rgb8_pixel_t*)gimp_image.pixel_data,gimp_image.width*gimp_image.bytes_per_pixel);

        typedef layer::aligned<rgb8_view_t> aligned_t;
        aligned_t aligned(view2,aligned_t::center|aligned_t::middle);
        aligned(view);

        aligned.align = aligned_t::left|aligned_t::top;
        aligned(view);

        FILE* fd = fopen("aligned.ppm", "wb");
        fprintf(fd, "P6\n# CREATOR: reportbase\n%d %d\n255\n", width, height);
        fwrite(buffer, 1, width*height*3, fd);
        fclose(fd);
        delete [] buffer;
    }
};

test_suite* init_unit_test_suite( int argc, char** argv)
{
    test_suite* test= BOOST_TEST_SUITE( "aligned tests" );
    test->add( BOOST_TEST_CASE(test1()), 0);
    return test;
}
