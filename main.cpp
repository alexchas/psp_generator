#include <QCoreApplication>
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

double getClock( ) {                        // ф-ция возвращает процессорное время из доступных POSIX-таймеров
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    {
        clockid_t id;
        struct timespec ts;
        id = CLOCK_PROCESS_CPUTIME_ID;      // clockID для Linux чтобы измерять процессорное время на исполнении
#endif
        if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
            return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
    return -1;
}

int main() {
    std::thread my_thread;
    double stT1, endT1, stT2, endT2, res1, res2;
    uint16_t c=0, poly=0x2358;

    stT1 = getClock();
    for(c=0; c<16; c++) {
        my_thread = std::thread([](){std::this_thread::sleep_for(std::chrono::microseconds(10)); });        // выравниваем итерации по 10 микросекунд
        uint16_t n  = ((poly >> 14) ^ (poly >> 3) ^ (poly >> 1)) & 0x01;     // полином f(x) = 1 + x^2 + x^4 + x^15
        poly =  (poly << 1) | (n);
        my_thread.join();
        printf("%i = 0x%08x\n", (c + 1), poly);
    }
    endT1 = getClock();
    res1 = endT1 - stT1;
    printf("CPU time used = %lf\n", res1);

    stT2 = getClock();
    c=0;
    while(c<16) {
            uint16_t n  = ((poly >> 14) ^ (poly >> 3) ^ (poly >> 1)) & 0x01;
            poly =  (poly << 1) | (n);
            printf("%i = 0x%08x\n", (c + 1), poly);
            c++;
        }
    endT2 = getClock();
    res2 = endT2 - stT2;
    printf("CPU time used = %lf\n", res2);
    printf("CPU time 1-st more used than 2-nd on = %lf\n", res1 - res2);

    return 0;
}
