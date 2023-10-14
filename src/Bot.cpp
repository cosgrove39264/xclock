//
// Created by marcel on 14.10.23.
//

#include <WiFiClientSecure.h>
#include "Bot.h"
#include "WLan.h"
#include "SetupMode.h"
#include "AsyncTelegram2.h"
#include "bot_config.h"
#include "Dice.h"


bool Bot::started = false;

void Bot::start_bot() {
    if (started) {
        return;
    }
    if (!WLan::connected) {
        return;
    }
    if (SetupMode::ota_started) {
        return;
    }
    TaskHandle_t taskHandle;
    xTaskCreate(Bot::task,
                "Bot",
                4096,
                nullptr,
                10,
                &taskHandle);
}

void Bot::task(void *pvParameters) {
    if (!WLan::connected) {
        Bot::started = false;

        vTaskDelete(NULL);
        return;
    }
    if (SetupMode::ota_started) {
        Bot::started = false;
        vTaskDelete(NULL);
        return;
    }

    Bot::started = true;

    WiFiClientSecure client;
    AsyncTelegram2 myBot(client);

    client.setCACert(telegram_cert);
    myBot.setUpdateTime(2000);
    myBot.setTelegramToken(bot_token);
    if (!myBot.begin()) {
        Serial.println("Bot nicht gestartet");
        Bot::started = false;
        vTaskDelete(NULL);
        return;
    }


    for (;;) {
        if (SetupMode::ota_started) {
            break;
        }
        if (!WLan::connected) {
            break;
        }
        TBMessage msg;
        if (myBot.getNewMessage(msg)) {
            String tgReply;
            static String document;
            switch (msg.messageType) {
                case MessageDocument : {

                    myBot.sendMessage(msg, "nicht möglich");

                }
                    break;
                case MessageReply: {
                    tgReply = msg.text;
                    myBot.sendMessage(msg, "You have entered wrong password");
                }
                    break;

                default: {
                    tgReply = msg.text;
                    if (tgReply.equalsIgnoreCase("/version")) {
                        String fw = "XClock Version: " + String(SetupMode::firmware_version);
                        myBot.sendMessage(msg, fw);
                    } else if (tgReply.equalsIgnoreCase("/dice")) {
                        Dice::stop=true;
                        while(Dice::running){
                            vTaskDelay(5 / portTICK_PERIOD_MS);
                        }
                        Dice::run(15);
                        myBot.sendMessage(msg, "Die Würfel sind gefallen");
                    } else if (tgReply.equalsIgnoreCase("/upgrade")) {
                        myBot.sendMessage(msg, "Firmware-Update wird gestartet");
                        SetupMode::start_http_ota();
                    }else if (tgReply.equalsIgnoreCase("/help")) {
                        myBot.sendMessage(msg, "Verfügbare Optionen");
                        myBot.sendMessage(msg, "/dice - würfeln");
                        myBot.sendMessage(msg, "/upgrade - Firmware-Update");
                        myBot.sendMessage(msg, "/version - XClock Version");
                    } else {
                        Bot::send_welcome(myBot, msg.chatId);
                    }
                    break;
                }
            }
        } else {
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void Bot::send_welcome(AsyncTelegram2 bot, uint64_t chat_id) {
    char welcome_msg[128];
    snprintf(welcome_msg, 128, "%s\n\n/help alle verfügbaren Optionen", bot.getBotName());
    bot.sendTo(chat_id, welcome_msg);

}
