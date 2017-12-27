#include "pic.h"

namespace svm
{
    PIC::PIC()
        : isr_0([]()  { }),
          isr_1([]()  { }),
          isr_2([]()  { }),
          isr_3([]()  { }),
          isr_4([]()  { }),
          isr_5([]()  { }),
          isr_6([]()  { }),
          isr_7([]()  { }),
          isr_8([]()  { }),
          isr_9([]()  { }),
          isr_10([]() { }),
          isr_11([]() { }),
          isr_12([]() { }),
          isr_13([]() { }),
          isr_14([]() { }),
          isr_15([]() { }),
          isr_16([]() { }) { }

    PIC::~PIC() { }
}
