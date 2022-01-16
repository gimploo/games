#include "./lib/basic.h"


int main(void)
{
    int value = randint(-255, 255);
    printf("%i\n", value);

    f32 fval = (f32 )randint(-255, 255)/ 255;
    printf("%f\n", fval);
    fval = (f32 )randint(-255, 255)/ 255;
    printf("%f\n", fval);
    fval = (f32 )randint(-255, 255)/ 255;
    printf("%f\n", fval);
    fval = (f32 )randint(-255, 255)/ 255;
    printf("%f\n", fval);

    return 0;
}
