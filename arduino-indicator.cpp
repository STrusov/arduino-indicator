#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <locale>
#include <thread>

#include <sys/sysinfo.h>

using namespace std;
using namespace std::chrono;

milliseconds tick = 500ms;

const char* def_out_file = "/dev/ttyACM0";

const char* temperature_file = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";
const char* fan_file = "/sys/devices/platform/nct6775.656/hwmon/hwmon2/fan2_input";

const char* cpufreq_file = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";

int main(int n, char** c) {

    setlocale(LC_ALL,"");

    ofstream out;
    const char* out_file;
    switch (n) {
    default:
        cerr << "Допустимо передать единственным аргументом имя файла для вывода."
             << endl;
        return -1;
    case 1:
        out_file = def_out_file;
        break;
    case 2:
        out_file = c[1];
    }

    out.open(out_file);
    if (!out) {
        cerr << "Ошибка отрытия файла \"" << out_file << "\"." << endl;
        return -1;
    }

    for (unsigned step = 0; ; step = (step + 1) % 10) {

        if (!out) {
            out.close();
            out.open(out_file);
        }

        system_clock::time_point now = system_clock::now();

        time_t t = system_clock::to_time_t(now);
        char strtime[(16 + sizeof ("\n")) * sizeof(char32_t)];
        strftime(strtime, sizeof(strtime), "%H:%M:%S %a%d%b \n", localtime(&t));
        out << strtime << flush;

        if ( step > 6 ) {
            // Температура рапортуется в тысячных °C, три правые цифры незначащие
            unsigned temperature;
            ifstream(temperature_file) >> temperature;
            out.width(2);
            out << temperature / 1000 << char(1) << "C  ";

            unsigned fan;
            ifstream(fan_file) >> fan;
            out.width(4);
            out << fan << "об/мин ";

            out << '\n' << flush;
            this_thread::sleep_until(now + tick);
            continue;
        }

        unsigned cpufreq;
        ifstream(cpufreq_file) >> cpufreq;
        out.setf(ios_base::showpoint);
        double cf = cpufreq/1000000.0;
        out.precision(1 + (cf > 1.0));
        out << cf << "ГГц ";

        ifstream meminfo("/proc/meminfo");
        // Вычисления схожи с утилитой free (см. sysinfo.c из procps-ng)
        enum { 
            main_total, main_free, page_cache, slab_reclaimable, main_buffers,
            swap_total, swap_free
        };
        struct { const char* name; ssize_t val; } mi[] = {
            { "MemTotal", -1 },
            { "MemFree", -1 },
            { "Cached", -1 },
            { "SReclaimable", -1 },
            { "Buffers", -1 },
            { "SwapTotal", -1 },
            { "SwapFree", -1 },
        };
//      for (int found = 0; found < std::size(mi)/sizeof(*mi); ) {
        for (int found = 0; found < size(mi); ) {
            char c;
            // Проверяем строку на совпадение с каждым искомым
            for (auto& it : mi) {
                if (it.val != -1)
                    continue;
                for (int i = 0; ; ++i) {
                    if (!meminfo.get(c))
                        break;
                    // Формат строки
                    // MemFree:          123456 kB
                    // Совпало ли начало с именем поля?
                    if (!it.name[i] && c == ':') {
                        ++found;
                        meminfo >> it.val;
//                        cout << " found: " << it.name << ":" << it.val;
                        break;
                    }
                    if (it.name[i] != c) {
                        meminfo.seekg(-i - 1, ios::cur);
                        break;
                    }
                }
            }
            while (meminfo.get(c))
                if (c == '\n')
                    break;
            if (!meminfo)
                break;
        }
        // main_cached = page_cache + slab_reclaimable
        // mem_used = main_total - main_free - main_cached - main_buffers
        ssize_t ram_used  = mi[main_total].val - mi[main_free].val - 
                            mi[page_cache].val - mi[slab_reclaimable].val;
        ssize_t swap_used = mi[swap_total].val - mi[swap_free].val;
        out.width(4);
        out << (ram_used  + 512) / 1024 << '+'
            << (swap_used + 512) / 1024 << "МБ ";

        out << '\n' << flush;
        this_thread::sleep_until(now + tick);
    }
}
