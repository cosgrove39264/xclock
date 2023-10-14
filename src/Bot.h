//
// Created by marcel on 14.10.23.
//

#ifndef XCLOCK_BOT_H
#define XCLOCK_BOT_H

#include <Arduino.h>
#include "AsyncTelegram2.h"
//#include "version.h"

class Bot {
protected:
    static bool started;
    static void task(void *pvParameters);

    static void send_welcome(AsyncTelegram2 bot,uint64_t chat_id);

public:
    static void start_bot();
};


#endif //XCLOCK_BOT_H
